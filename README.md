Hyper-V BackupEssentials
========================

This repository contains installable binaries for Hyper-V backup 
components for certain Linux distributions. Since the Hyper-V backup
feature was only completed in late 2013, we could not include it
in several distributions that existed prior to that date. For such
distributions, we decided to deliver the backup feature through
the RPM installables in this repository.

What distributions are supported?
---------------------------------

As of now we support the following distributions:

a) RHEL 5.9<br>
b) RHEL 6.4<br>

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
other than these require Hyper-V backup essentials for utilizing the
Hyper-V backup feature.

Please refer to the feature maps mentioned in the following page
to discover Hyper-V backup feature availability for your distribution:

http://technet.microsoft.com/en-us/library/dn531030.aspx

Installation
------------
This section describes how you may install the Hyper-V backup 
components for different distributions.<br>

a) For RHEL 5.9:

Download the RHEL 5.9 installation script and the corresponding
RPMs as follows:
curl -l

Run the install.sh script as follows:
./install.sh

b) For RHEL 6.4:

Download the RHEL 5.9 installation script and the corresponding
RPMs as follows:
curl -l

Run the install.sh script as follows:
./install.sh
