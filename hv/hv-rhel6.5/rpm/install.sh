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
              msrpm=`ls microsoft-hyper-v-vss-rhel6*.x86_64.rpm`
       }
elif [ "$osbit" == "i686" ]; then
       {
              msrpm=`ls microsoft-hyper-v-vss-rhel6*.i686.rpm`
       }
fi

#Making sure both rpms are present

if [ "$msrpm" != ""  ]; then
       echo "Installing the VSS of Linux Integration Services for Microsoft Hyper-V..."
              rpm -ivh --nodeps $msrpm
              msexit=$?
              if [ "$msexit" != 0 ]; then
                     echo "Microsoft-Hyper-V VSS RPM installation failed, Exiting."
                     exit 1;
              else
                     echo " VSS of Linux Integration Services for Hyper-V has been installed. Please reboot your system."
              fi
else 
       echo "RPM's are missing"
fi

