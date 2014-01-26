Hyper-V Backup Essentials
=========================

This repository contains installable binaries for Hyper-V backup 
components for certain Linux distributions. The Hyper-V backup 
components allow a system administrator to perform uninterrupted
backup of a live Linux virtual machine. Since the Hyper-V backup
feature was only completed in late 2013, we could not include it
in several distributions that existed prior to that date. For such
distributions, we decided to deliver the backup feature through
the RPM installables in this repository.

What distributions are supported?
---------------------------------

As of now Hyper-V Backup Essentials are available for the following 
distributions:

* Red Hat Enterprise Linux (RHEL)/CentOS 5.9
* RHEL/CentOS 6.4
* RHEL/CentOS 6.5

What Windows Server version will this run on?
---------------------------------------------
The Hyper-V backup feature is only present in Windows Server 2012 R2 
release and its derivatives.

How is this different from Linux Integration Services (LIS) releases?
---------------------------------------------------------------------
The Hyper-V backup essentials have been released for Linux distributions
that do not have Hyper-V backup components either prebuilt in to them
or through an LIS release such as LIS 3.5.

Newer distributions starting SLES 11 SP3, Ubuntu 13.10 and RHEL 7
come prepackaged with Hyper-V backup components. Furthermore, LIS 3.5
provides backup components for RHEL 5.5-5.8/6.0-6.3. Distributions
other than these may require Hyper-V backup essentials for utilizing the
Hyper-V backup feature.

Please refer to the feature maps mentioned in the following page
to discover Hyper-V backup feature availability for your distribution:

http://technet.microsoft.com/en-us/library/dn531030.aspx

Installation
------------
This section describes how you may install the Hyper-V backup 
components for different distributions.<br>

* First of all download the Hyper-V backup components using the following
command:
```
# curl -L -o backupessentials.tar.gz https://github.com/LIS/backupessentials/tarball/1.0
```

* Next untar the downloaded tar file using the following command:
```
# tar -xvzf backupessentials.tar.gz
```

* After untar operation, you should see a folder similar to the following:
```
# drwxrwxr-x. 3 root root   4096 Jan 15 20:58 LIS-backupessentials-f5f542e
```

* For RHEL/CentOS 5.9, change directory as follows:
```
# cd LIS-backupessentials-f5f542e/hv/hv-rhel5.9/rpm/
```

* For RHEL/CentOS 6.4, change directory as follows:
```
# cd LIS-backupessentials-f5f542e/hv/hv-rhel6.4/rpm/
```

* For RHEL/CentOS 6.5, change directory as follows:
```
# cd LIS-backupessentials-f5f542e/hv/hv-rhel6.5/rpm/
```

* After changing your directory appropriately, run install.sh and reboot the virtual machine.
```
# ./install.sh
# reboot
```

Uninstallation
--------------
To uninstall Hyper-V backup components, switch to the directory corresponding
to your distribution and run the uninstall.sh script as outlined in the 
following steps:

* For RHEL/CentOS 5.9, change directory as follows:
```
# cd LIS-backupessentials-f5f542e/hv/hv-rhel5.9/rpm/
```

* For RHEL/CentOS 6.4, change directory as follows:
```
# cd LIS-backupessentials-f5f542e/hv/hv-rhel6.4/rpm/
```

* For RHEL/CentOS 6.5, change directory as follows:
```
# cd LIS-backupessentials-f5f542e/hv/hv-rhel6.5/rpm/
```

* After changing your directory appropriately, run uninstall.sh and reboot the virtual machine.
```
# ./uninstall.sh
# reboot
```

Customer support
----------------
The Hyper-V Backup Essentials are developed and maintained by Microsoft's Linux Integration Services team. Customers can obtain formal support through all standard Microsoft support channels. Furthermore, support is also available through the Linux Integration Services for Microsoft Hyper-V forum located at http://social.technet.microsoft.com/Forums/windowsserver/en-us/home?forum=linuxintegrationservices
