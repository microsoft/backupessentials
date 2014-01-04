#ifndef _VSS_H
#define _VSS_H

#include <linux/types.h>

#define CN_VSS_IDX      0xA
#define CN_VSS_VAL      0x1

/*
 * Framework version for util services.
 */

#define UTIL_FW_MINOR  0

#define UTIL_WS2K8_FW_MAJOR  1
#define UTIL_WS2K8_FW_VERSION     (UTIL_WS2K8_FW_MAJOR << 16 | UTIL_FW_MINOR)

#define UTIL_FW_MAJOR  3
#define UTIL_FW_VERSION     (UTIL_FW_MAJOR << 16 | UTIL_FW_MINOR)

/*
 * Implementation of host controlled snapshot of the guest.
 */

#define VSS_OP_REGISTER 128

enum hv_vss_op {
        VSS_OP_CREATE = 0,
        VSS_OP_DELETE,
        VSS_OP_HOT_BACKUP,
        VSS_OP_GET_DM_INFO,
        VSS_OP_BU_COMPLETE,
        /*
         * Following operations are only supported with IC version >= 5.0
         */
        VSS_OP_FREEZE, /* Freeze the file systems in the VM */
        VSS_OP_THAW, /* Unfreeze the file systems */
        VSS_OP_AUTO_RECOVER,
        VSS_OP_COUNT /* Number of operations, must be last */
};

/*
 * Header for all VSS messages.
 */
struct hv_vss_hdr {
        __u8 operation;
        __u8 reserved[7];
} __attribute__((packed));

/*
 * Flag values for the hv_vss_check_feature. Linux supports only
 * one value.
 */
#define VSS_HBU_NO_AUTO_RECOVERY        0x00000005

struct hv_vss_check_feature {
        __u32 flags;
} __attribute__((packed));

struct hv_vss_check_dm_info {
        __u32 flags;
} __attribute__((packed));

struct hv_vss_msg {
        union {
                struct hv_vss_hdr vss_hdr;
                int error;
        };
        union {
                struct hv_vss_check_feature vss_cf;
                struct hv_vss_check_dm_info dm_info;
        };
} __attribute__((packed));

#endif

