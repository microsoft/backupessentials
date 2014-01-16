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

Installation
------------
This section describes how you may install the Hyper-V backup 
components for different distributions.<br>

a) Please use the following instructions for RHEL 5.9:

Download the RHEL 5.9 installation script and the corresponding
RPMs as follows:
curl -l

Run the install.sh script as follows:
./install.sh

b) Please use the following instructions for RHEL 6.4:

Download the RHEL 5.9 installation script and the corresponding
RPMs as follows:
curl -l

Run the install.sh script as follows:
./install.sh
