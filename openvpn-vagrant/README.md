# openvpn-vagrant

Vagrantfile and support scripts for use in OpenVPN development and testing.

# Prequisites

You need to install Virtualbox and Vagrant.

# List of VMs

The following Vagrant VM are provisioned to include everything needed to build 
OpenVPN on them. Both OpenSSL and MbedTLS builds are supported out of the box:

* centos-7
* debian-9
* freebsd-11
* netbsd-7
* openbsd-6
* solaris-113
* ubuntu-1604
* ubuntu-1804

Note that it is not possible to legally distribute the base box for solaris-113; 
please refer to [recipes/Solaris113.txt](recipes/Solaris113.txt) for details.

These VMs are special-purpose:

* openvpn-build: cross-compile OpenVPN using [openvpn-build](https://github.com/OpenVPN/openvpn-build) (Ubuntu 16.04)
* openvpn-build-bionic: cross-compile OpenVPN using [openvpn-build](https://github.com/OpenVPN/openvpn-build) (Ubuntu 18.04)
* sbuild: build OpenVPN 2.x Debian/Ubuntu packages using [sbuild_wrapper](https://github.com/OpenVPN/sbuild_wrapper) (Ubuntu 18.04)
* oas: [OpenVPN Access Server](https://openvpn.net/faq/what-is-openvpn-access-server/) for testing / experimentation (Ubuntu 18.04)
* msibuilder: package OpenVPN as MSI using the [WiX toolset](http://wixtoolset.org) (Windows Server 2016)

# Building MSI packages

Building MSI packages is a two-phase process which requires using
openvpn-build-bionic (Ubuntu 18.04) and msibuilder (Windows Server 2016) VMs.
The former is used to produce Windows binaries and the latter to produce MSI
packages. The openvpn-build directory on openvpn-build-bionic is shared with
Samba so that msibuilder can access the build artifacts. The setup process is
almost completely automated:

    $ vagrant up openvpn-build-bionic
    $ vagrant up msibuilder

If you're running Vagrant on Linux you're almost certainly using
[FreeRDP](https://www.freerdp.com/). That means you have to accept msibuilder
VM's host key before attempting to "vagrant rdp" into it:

    $ xfreerdp /v:127.0.0.1:3389

Once the host key is in FreeRDP's cache you can connect to the instance:

    $ vagrant rdp msibuilder

Once there, launch a Powershell session and mount the openvpn-build directory
shared by openvpn-build-bionic:

    PS> net use O: /USER:vagrant /PERSISTENT:YES \\192.168.48.110\openvpn-build vagrant

Now SSH in to openvpn-build-bionic and build both 32-bit and 64-bit binaries.
Until OpenVPN 2.5 is officially out you need to point the build system to an
OpenVPN tarball that has MSI support:

    $ vagrant ssh openvpn-build-bionic
    $ cd openvpn-build/generic
    $ OPENVPN_URL=http://build.openvpn.net/downloads/temp/msi/openvpn-2.5_git.tar.gz IMAGEROOT=`pwd`/image-win32 CHOST=i686-w64-mingw32 CBUILD=x86_64-pc-linux-gnu ./build
    $ OPENVPN_URL=http://build.openvpn.net/downloads/temp/msi/openvpn-2.5_git.tar.gz IMAGEROOT=`pwd`/image-win64 CHOST=x86_64-w64-mingw32 CBUILD=x86_64-pc-linux-gnu ./build

This produces Windows binaries which the msibuilder VM can access via the Samba
share mounted to O:. To produce MSI installers login to msibuilder:

    $ vagrant rdp msibuilder

Then from a Powershell session produce the MSI packages:

    PS> cd O:\windows-msi
    PS> cscript build.wsf msi

If you want to code-sign the files in the MSI installer append something like this to the ./build command-line:

    --sign --sign-pkcs12=digicert-user-mode-2022.pfx --sign-pkcs12-pass=secret \
    --sign-timestamp=http://timestamp.verisign.com/scripts/timstamp.dll

You also probably want to sign the MSI files. You can do this with
osslsigncode on openvpn-build-bionic:

    $ cd openvpn-build/generic
    $ osslsigncode sign -h sha2 -pkcs12 digicert-user-mode-2022.pfx -pass secret \
      -t http://timestamp.verisign.com/scripts/timstamp.dll \
      -in ../windows-msi/image/OpenVPN-2.5-beta1-x86.msi \
      -out ../windows-msi/image/OpenVPN-2.5-beta1-x86.msi.signed

Repeat the process for x86 and amd64.

# TODO

* Add t_client style test server with multiple server instances with differing configurations
* Merge [testing: Add vagrant based integration tests](https://github.com/OpenVPN/openvpn/pull/45) into this repository
