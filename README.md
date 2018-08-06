# LicenseManager
The component at the juice mixing machine, responsible for importing and managing licenses

# Build for Ultimaker 3

1. get a Raspberry Pi running Raspbian.
2. rsync the Ultimaker root filesystem to the raspberry pi `mkdir /tmp/ultimaker; cd /tmp/ultimaker; rsync -avPx root@<address-of-ultimaker:/ .`
3. mount _dev_, _proc_ and sys into chroot environment: `mount -t proc proc proc/; mount -t sysfs sys sys/; mount -o bind /dev dev/`
4. add /CodeMeter.h/ to /LicenseManager/src/ `cp CodeMeter.h /tmp/ultimaker/tmp/LicenseManager`
5. change into ultimaker environment `chroot ultimaker`
6. set reachable dns servers in _/etc/resolve.conf_ e.g. `8.8.8.8`
7. install packets required for build `apt-get update; apt-get install dropbear-scp- git g++ binutils autoconf asio-dev`
8. clone LicenseManager fcrom github `cd /tmp; git clone https://github.com/IUNO-TDM/LicenseManager.git`
9. compile Licensemanager `cd LicenseManager; make`
