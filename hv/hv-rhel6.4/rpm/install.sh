################################################################################
#
# This script is to automate Installaing Hyper-V Backup Essentials
#
################################################################################

# Determine kernel architecture version 
osbit=`uname -m`

#Selecting appropriate rpm, 64 bit rpm for x86_64 based VM
if [ "$osbit" == "x86_64" ]; then
       {
              kmodrpm=`ls kmod-microsoft-hyper-v-vss-rhel6*.x86_64.rpm`
              msrpm=`ls microsoft-hyper-v-vss-rhel6*.x86_64.rpm`
       }
elif [ "$osbit" == "i686" ]; then
       {
              kmodrpm=`ls kmod-microsoft-hyper-v-vss-rhel6*.i686.rpm`
              msrpm=`ls microsoft-hyper-v-vss-rhel6*.i686.rpm`
       }
fi

#Making sure both rpms are present

if [ "$kmodrpm" != "" ] && [ "$msrpm" != ""  ]; then
       echo "Installing Hyper-v Backup Essentials"
       rpm -ivh --nodeps $kmodrpm
       kmodexit=$?
       if [ "$kmodexit" == 0 ]; then
              rpm -ivh --nodeps $msrpm
              msexit=$?
              if [ "$msexit" != 0 ]; then
                     echo "Installing Hyper-V Backup Essentials failed, Exiting."
                     exit 1;
              else
                     echo "Hyper-V Backup Essentials have been installed. Please reboot your system."
              fi
       else
              echo "Kmod RPM installation failed, Exiting."
              exit 1
       fi
else 
       echo "RPM's are missing"
fi

