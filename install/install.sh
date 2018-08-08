#!/bin/bash

if [ -f ../build/LicenseManager ]
then
    cp ../build/LicenseManager /usr/local/bin

    if [[ x`systemctl is-enabled iuno_licensemanager.service` == xenabled ]]
    then
        systemctl disable iuno_licensemanager.service
        rm /etc/systemd/system/iuno_licensemanager.service
    fi

    cp iuno_licensemanager.service /etc/systemd/system
    systemctl enable iuno_licensemanager.service
    systemctl start iuno_licensemanager.service
else
    echo "LicenseManager executable does not exist in ../build . Call make in project folder first"
fi