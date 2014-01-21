################################################################################
#
# This script is to automate Uninstallaing Hyper-V Backup Essentials
#
################################################################################

# Determine kernel architecture version 
osbit=`uname -m`

#Selecting appropriate rpm, 64 bit rpm for x86_64 based VM
if [ "$osbit" == "x86_64" ]; then
       {
              msrpm=`ls microsoft-hyper-v-vss-rhel6*.x86_64.rpm | sed 's/.\{4\}$//'`
       }
elif [ "$osbit" == "i686" ]; then
       {
              msrpm=`ls microsoft-hyper-v-vss-rhel6*.i686.rpm | sed 's/.\{4\}$//'`
       }
fi

#Making sure both rpms are present

if [ "$msrpm" != ""  ]; then
         echo "UnInstalling  Hyper-V Backup Essentials"
              rpm -e --nodeps $msrpm
              msexit=$?
              if [ "$msexit" != 0 ]; then
                      echo "Uninstallaing Hyper-V Backup Essentials failed, Exiting."
                     exit 1;
              else
                     echo "Hyper-V Backup Essentials have been uninstalled. Please reboot your system."
              fi
else 
       echo "RPM's are missing"
fi

