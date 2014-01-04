/*
 * An implementation of host initiated guest snapshot.
 *
 *
 * Copyright (C) 2013, Microsoft, Inc.
 * Author : K. Y. Srinivasan <kys@microsoft.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, GOOD TITLE or
 * NON INFRINGEMENT.  See the GNU General Public License for more
 * details.
 *
 */
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/net.h>
#include <linux/nls.h>
#include <linux/connector.h>
#include <linux/workqueue.h>
#include <linux/hyperv.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sysctl.h>

#include "vss.h" 

/*
 * VSS (Backup/Restore) GUID
 */
#define HV_VSS_GUID \
	.guid = { \
			0x29, 0x2e, 0xfa, 0x35, 0x23, 0xea, 0x36, 0x42, \
			0x96, 0xae, 0x3a, 0x6e, 0xba, 0xcb, 0xa4,  0x40 \
		}

#define UTIL_FW_MINOR  0

#define UTIL_FW_MAJOR  3
#define UTIL_FW_VERSION     (UTIL_FW_MAJOR << 16 | UTIL_FW_MINOR)


#define VSS_MAJOR  5
#define VSS_MINOR  0
#define VSS_VERSION    (VSS_MAJOR << 16 | VSS_MINOR)


int hv_vss_init(struct hv_util_service *);
void hv_vss_deinit(void);
void hv_vss_onchannelcallback(void *);


/*
 * Global state maintained for transaction that is being processed.
 * Note that only one transaction can be active at any point in time.
 *
 * This state is set when we receive a request from the host; we
 * cleanup this state when the transaction is completed - when we respond
 * to the host with the key value.
 */

static struct {
	bool active; /* transaction status - active or not */
	int recv_len; /* number of bytes received. */
	struct vmbus_channel *recv_channel; /* chn we got the request */
	u64 recv_req_id; /* request ID. */
	struct hv_vss_msg  *msg; /* current message */
} vss_transaction;


static void vss_respond_to_host(int error);

static struct cb_id vss_id = { CN_VSS_IDX, CN_VSS_VAL };
static char vss_name[] = "vss_kernel_module";
static __u8 *recv_buffer;

static void vss_send_op(void *dummy);
static DECLARE_WORK(vss_send_op_work, vss_send_op, &vss_send_op_work);


/**
 * vmbus_prep_negotiate_resp() - Create default response for Hyper-V Negotiate message
 * @icmsghdrp: Pointer to msg header structure
 * @icmsg_negotiate: Pointer to negotiate message structure
 * @buf: Raw buffer channel data
 *
 * @icmsghdrp is of type &struct icmsg_hdr.
 * @negop is of type &struct icmsg_negotiate.
 * Set up and fill in default negotiate response message.
 *
 * The fw_version specifies the  framework version that
 * we can support and srv_version specifies the service
 * version we can support.
 *
 * Mainly used by Hyper-V drivers.
 */
bool vmbus_priv_negotiate_resp(struct icmsg_hdr *icmsghdrp,
				struct icmsg_negotiate *negop, u8 *buf,
				int fw_version, int srv_version)
{
        int icframe_major, icframe_minor;
        int icmsg_major, icmsg_minor;
        int fw_major, fw_minor;
        int srv_major, srv_minor;
        int i;
        bool found_match = false;

        icmsghdrp->icmsgsize = 0x10;
        fw_major = (fw_version >> 16);
        fw_minor = (fw_version & 0xFFFF);

        srv_major = (srv_version >> 16);
        srv_minor = (srv_version & 0xFFFF);

        negop = (struct icmsg_negotiate *)&buf[
                sizeof(struct vmbuspipe_hdr) +
                sizeof(struct icmsg_hdr)];

        icframe_major = negop->icframe_vercnt;
        icframe_minor = 0;

        icmsg_major = negop->icmsg_vercnt;
        icmsg_minor = 0;

        /*
         * Select the framework version number we will
         * support.
         */

        for (i = 0; i < negop->icframe_vercnt; i++) {
                if ((negop->icversion_data[i].major == fw_major) &&
                   (negop->icversion_data[i].minor == fw_minor)) {
                        icframe_major = negop->icversion_data[i].major;
                        icframe_minor = negop->icversion_data[i].minor;
                        found_match = true;
                }
        }

        if (!found_match)
                goto fw_error;

        found_match = false;

        for (i = negop->icframe_vercnt;
                 (i < negop->icframe_vercnt + negop->icmsg_vercnt); i++) {
                if ((negop->icversion_data[i].major == srv_major) &&
                   (negop->icversion_data[i].minor == srv_minor)) {
                        icmsg_major = negop->icversion_data[i].major;
                        icmsg_minor = negop->icversion_data[i].minor;
                        found_match = true;
                }
        }

        /*
         * Respond with the framework and service
         * version numbers we can support.
         */

fw_error:
        if (!found_match) {
                negop->icframe_vercnt = 0;
                negop->icmsg_vercnt = 0;
        } else {
                negop->icframe_vercnt = 1;
                negop->icmsg_vercnt = 1;
        }

        negop->icversion_data[0].major = icframe_major;
        negop->icversion_data[0].minor = icframe_minor;
        negop->icversion_data[1].major = icmsg_major;
        negop->icversion_data[1].minor = icmsg_minor;
        return found_match;
}


/*
 * Callback when data is received from user mode.
 */

static void
vss_cn_callback(void *cb_data)
{
	struct hv_vss_msg *vss_msg;
	struct cn_msg *msg = (struct cn_msg *)cb_data;

	vss_msg = (struct hv_vss_msg *)msg->data;

	if (vss_msg->vss_hdr.operation == VSS_OP_REGISTER) {
		pr_info("VSS daemon registered\n");
		vss_transaction.active = false;
		if (vss_transaction.recv_channel != NULL)
			hv_vss_onchannelcallback(vss_transaction.recv_channel);
		return;

	}
	vss_respond_to_host(vss_msg->error);
}


static void vss_send_op(void *dummy)
{
	int op = vss_transaction.msg->vss_hdr.operation;
	struct cn_msg *msg;
	struct hv_vss_msg *vss_msg;

	msg = kzalloc(sizeof(*msg) + sizeof(*vss_msg), GFP_ATOMIC);
	if (!msg)
		return;

	vss_msg = (struct hv_vss_msg *)msg->data;

	msg->id.idx =  CN_VSS_IDX;
	msg->id.val = CN_VSS_VAL;

	vss_msg->vss_hdr.operation = op;
	msg->len = sizeof(struct hv_vss_msg);

	cn_netlink_send(msg, 0, GFP_ATOMIC);
	kfree(msg);

	return;
}

/*
 * Send a response back to the host.
 */

static void
vss_respond_to_host(int error)
{
	struct icmsg_hdr *icmsghdrp;
	u32	buf_len;
	struct vmbus_channel *channel;
	u64	req_id;

	/*
	 * If a transaction is not active; log and return.
	 */

	if (!vss_transaction.active) {
		/*
		 * This is a spurious call!
		 */
		pr_warn("VSS: Transaction not active\n");
		return;
	}
	/*
	 * Copy the global state for completing the transaction. Note that
	 * only one transaction can be active at a time.
	 */

	buf_len = vss_transaction.recv_len;
	channel = vss_transaction.recv_channel;
	req_id = vss_transaction.recv_req_id;
	vss_transaction.active = false;

	icmsghdrp = (struct icmsg_hdr *)
			&recv_buffer[sizeof(struct vmbuspipe_hdr)];

	if (channel->onchannel_callback == NULL)
		/*
		 * We have raced with util driver being unloaded;
		 * silently return.
		 */
		return;

	icmsghdrp->status = error;

	icmsghdrp->icflags = ICMSGHDRFLAG_TRANSACTION | ICMSGHDRFLAG_RESPONSE;

	vmbus_sendpacket(channel, recv_buffer, buf_len, req_id,
				VM_PKT_DATA_INBAND, 0);

}

/*
 * This callback is invoked when we get a VSS message from the host.
 * The host ensures that only one VSS transaction can be active at a time.
 */

void hv_vss_onchannelcallback(void *context)
{
	struct vmbus_channel *channel = context;
	u32 recvlen;
	u64 requestid;
	struct hv_vss_msg *vss_msg;


	struct icmsg_hdr *icmsghdrp;
	struct icmsg_negotiate *negop = NULL;

	if (vss_transaction.active) {
		/*
		 * We will defer processing this callback once
		 * the current transaction is complete.
		 */
		vss_transaction.recv_channel = channel;
		return;
	}

	vmbus_recvpacket(channel, recv_buffer, PAGE_SIZE * 2, &recvlen,
			 &requestid);

	if (recvlen > 0) {
		icmsghdrp = (struct icmsg_hdr *)&recv_buffer[
			sizeof(struct vmbuspipe_hdr)];

		if (icmsghdrp->icmsgtype == ICMSGTYPE_NEGOTIATE) {
			vmbus_priv_negotiate_resp(icmsghdrp, negop,
				 recv_buffer, UTIL_FW_VERSION,
				 VSS_VERSION);
		} else {
			vss_msg = (struct hv_vss_msg *)&recv_buffer[
				sizeof(struct vmbuspipe_hdr) +
				sizeof(struct icmsg_hdr)];

			/*
			 * Stash away this global state for completing the
			 * transaction; note transactions are serialized.
			 */

			vss_transaction.recv_len = recvlen;
			vss_transaction.recv_channel = channel;
			vss_transaction.recv_req_id = requestid;
			vss_transaction.active = true;
			vss_transaction.msg = (struct hv_vss_msg *)vss_msg;

			switch (vss_msg->vss_hdr.operation) {
				/*
				 * Initiate a "freeze/thaw"
				 * operation in the guest.
				 * We respond to the host once
				 * the operation is complete.
				 *
				 * We send the message to the
				 * user space daemon and the
				 * operation is performed in
				 * the daemon.
				 */
			case VSS_OP_FREEZE:
			case VSS_OP_THAW:
				schedule_work(&vss_send_op_work);
				return;

			case VSS_OP_HOT_BACKUP:
				vss_msg->vss_cf.flags =
					 VSS_HBU_NO_AUTO_RECOVERY;
				vss_respond_to_host(0);
				return;

			case VSS_OP_GET_DM_INFO:
				vss_msg->dm_info.flags = 0;
				vss_respond_to_host(0);
				return;

			default:
				vss_respond_to_host(0);
				return;

			}

		}

		icmsghdrp->icflags = ICMSGHDRFLAG_TRANSACTION
			| ICMSGHDRFLAG_RESPONSE;

		vmbus_sendpacket(channel, recv_buffer,
				       recvlen, requestid,
				       VM_PKT_DATA_INBAND, 0);
	}

}

int
hv_vss_init(struct hv_util_service *srv)
{
	int err;

	err = cn_add_callback(&vss_id, vss_name, vss_cn_callback);
	if (err)
		return err;
	recv_buffer = srv->recv_buffer;

	/*
	 * When this driver loads, the user level daemon that
	 * processes the host requests may not yet be running.
	 * Defer processing channel callbacks until the daemon
	 * has registered.
	 */
	vss_transaction.active = true;
	return 0;
}

void hv_vss_deinit(void)
{
	cn_del_callback(&vss_id);
	cancel_work_sync(&vss_send_op_work);
}


static struct hv_util_service util_vss = {
	.util_cb = hv_vss_onchannelcallback,
	.util_init = hv_vss_init,
	.util_deinit = hv_vss_deinit,
};

static int vss_probe(struct hv_device *dev,
			const struct hv_vmbus_device_id *dev_id)
{
	struct hv_util_service *srv =
		(struct hv_util_service *)dev_id->driver_data;
	int ret;

	srv->recv_buffer = kmalloc(PAGE_SIZE * 2, GFP_KERNEL);
	if (!srv->recv_buffer)
		return -ENOMEM;
	if (srv->util_init) {
		ret = srv->util_init(srv);
		if (ret) {
			ret = -ENODEV;
			goto error1;
		}
	}

	ret = vmbus_open(dev->channel, 4 * PAGE_SIZE, 4 * PAGE_SIZE, NULL, 0,
			srv->util_cb, dev->channel);
	if (ret)
		goto error;

	hv_set_drvdata(dev, srv);
	return 0;

error:
	if (srv->util_deinit)
		srv->util_deinit();
error1:
	kfree(srv->recv_buffer);
	return ret;
}

static int vss_remove(struct hv_device *dev)
{
	struct hv_util_service *srv = hv_get_drvdata(dev);

	vmbus_close(dev->channel);
	if (srv->util_deinit)
		srv->util_deinit();
	kfree(srv->recv_buffer);

	return 0;
}

static const struct hv_vmbus_device_id id_table[] = {
	/* VSS GUID */
	{ HV_VSS_GUID,
	  .driver_data = (unsigned long)&util_vss
	},
	{ },
};

MODULE_DEVICE_TABLE(vmbus, id_table);

/* The one and only one */
static  struct hv_driver vss_drv = {
	.name = "hv_vss",
	.id_table = id_table,
	.probe =  vss_probe,
	.remove =  vss_remove,
};

static int __init init_hyperv_vss(void)
{
	pr_info("Registering HyperV VSS Driver\n");

	return vmbus_driver_register(&vss_drv);
}

static void exit_hyperv_vss(void)
{
	pr_info("De-Registered HyperV VSS Driver\n");

	vmbus_driver_unregister(&vss_drv);
}

module_init(init_hyperv_vss);
module_exit(exit_hyperv_vss);

MODULE_DESCRIPTION("Hyper-V VSS");
MODULE_VERSION(HV_DRV_VERSION);
MODULE_LICENSE("GPL");
MODULE_ALIAS("vmbus:292efa3523ea364296ae3a6ebacba440");
