################################################################################
#
# This script is to automate installation of VSS of Linux Integration Services for  Microsoft Hyper-V
#
################################################################################

# Determine kernel architecture version 
osbit=`uname -m`

#Selecting appropriate rpm, 64 bit rpm for x86_64 based VM
if [ "$osbit" == "x86_64" ]; then
       {
              kmodrpm=`ls kmod-microsoft-hyper-v-vss-rhel6*.x86_64.rpm | sed 's/.\{4\}$//' `
              msrpm=`ls microsoft-hyper-v-vss-rhel6*.x86_64.rpm | sed 's/.\{4\}$//'`
       }
elif [ "$osbit" == "i686" ]; then
       {
              kmodrpm=`ls kmod-microsoft-hyper-v-vss-rhel6*.i686.rpm | sed 's/.\{4\}$//'`
              msrpm=`ls microsoft-hyper-v-vss-rhel6*.i686.rpm | sed 's/.\{4\}$//'`
       }
fi

#Making sure both rpms are present

if [ "$kmodrpm" != "" ] && [ "$msrpm" != ""  ]; then
       echo "UnInstalling the VSS of Linux Integration Services for Microsoft Hyper-V..."
       rpm -e --nodeps $msrpm
       kmodexit=$?
       if [ "$kmodexit" == 0 ]; then
              rpm -e --nodeps $kmodrpm
              msexit=$?
              if [ "$msexit" != 0 ]; then
                     echo "Microsoft-Hyper-V VSS RPM Uninstallation failed, Exiting."
                     exit 1;
              else
                     echo " VSS of Linux Integration Services for Hyper-V has been uninstalled. Please reboot your system."
              fi
       else
              echo "Kmod RPM uninstallation failed, Exiting."
              exit 1
       fi
else 
       echo "RPM's are missing"
fi

