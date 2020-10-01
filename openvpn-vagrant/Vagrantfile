# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure("2") do |config|

  config.vm.define "centos-7" do |box|
    box.vm.box = "centos/7"
    box.vm.box_version = "1710.01"
    box.vm.hostname = "centos-7.local"
    box.vm.network "private_network", ip: "192.168.48.101"
    box.vm.synced_folder ".", "/vagrant", type: "virtualbox"
    box.vm.provision "shell", path: "centos-7.sh"
    box.vm.provision "shell", path: "install-mbedtls.sh"
    box.vm.provider "virtualbox" do |vb|
      vb.gui = false
      vb.memory = 768
    end
  end

  config.vm.define "debian-9" do |box|
    box.vm.box = "debian/stretch64"
    box.vm.box_version = "9.2.0"
    box.vm.hostname = "debian-9.local"
    box.vm.network "private_network", ip: "192.168.48.102"
    box.vm.synced_folder ".", "/vagrant", type: "virtualbox"
    box.vm.provision "shell", path: "debian-9.sh"
    box.vm.provider "virtualbox" do |vb|
      vb.gui = false
      vb.memory = 768
    end
  end

  config.vm.define "freebsd-11" do |box|
    box.ssh.shell = "sh"
    box.vm.box = "openvpn/FreeBSD-11.1p3"
    box.vm.box_version = "1"
    box.vm.hostname = "freebsd-11.local"
    box.vm.network "private_network", ip: "192.168.48.103"
    box.vm.synced_folder ".", "/vagrant", type: "rsync"
    box.vm.provision "shell", path: "freebsd-11.sh"
    box.vm.provider "virtualbox" do |vb|
      vb.gui = false
      vb.memory = 768
      vb.customize ["modifyvm", :id, "--usb", "on"]
      vb.customize ["modifyvm", :id, "--usbehci", "off"]
    end
  end

  config.vm.define "openbsd-6" do |box|
    box.vm.box = "generic/openbsd6"
    box.vm.box_version = "1.2.38"
    box.vm.hostname = "openbsd-6.local"
    box.vm.network "private_network", ip: "192.168.48.104"
    box.vm.provision "shell", path: "openbsd-6.sh"
    box.vm.provider "virtualbox" do |vb|
      vb.gui = false
      vb.memory = 768
    end
  end

  config.vm.define "solaris-113" do |box|
    # we use a local box for licensing reasons, see recipes/Solaris113.txt
    box.vm.box = "solaris113-v1.box"
    #box.vm.box_version = "1"
    box.vm.hostname = "solaris113.local"
    box.vm.network "private_network", ip: "192.168.48.105"
    box.vm.provision "shell", path: "solaris-113.sh"
    box.vm.provider "virtualbox" do |vb|
      vb.gui = false
      vb.memory = 1024
    end
  end

  config.vm.define "ubuntu-1604" do |box|
    box.vm.box = "ubuntu/xenial64"
    box.vm.box_version = "20171118.0.0"
    box.vm.hostname = "ubuntu-1604.local"
    box.vm.network "private_network", ip: "192.168.48.106"
    box.vm.synced_folder ".", "/vagrant", type: "virtualbox"
    box.vm.provision "shell", path: "ubuntu-1604.sh"
    box.vm.provision "shell", path: "install-mbedtls.sh"
    box.vm.provider "virtualbox" do |vb|
      vb.gui = false
      vb.memory = 768
    end
  end

  config.vm.define "netbsd-7" do |box|
    box.vm.box = "netbsd/NetBSD-7.0"
    box.vm.box_version = "1.0.0"
    box.vm.hostname = "netbsd-7.local"
    box.vm.network "private_network", ip: "192.168.48.108"
    box.vm.provision "shell", path: "netbsd-7.sh"
    box.vm.synced_folder ".", "/vagrant", disabled: true
    box.vm.provider "virtualbox" do |vb|
      vb.gui = false
      vb.memory = 768
    end
  end

  config.vm.define "ubuntu-1804" do |box|
    box.vm.box = "ubuntu/bionic64"
    box.vm.box_version = "20180823.0.0"
    box.vm.hostname = "ubuntu-1804.local"
    box.vm.network "private_network", ip: "192.168.48.109"
    box.vm.provision "shell", path: "ubuntu-1604.sh"
    box.vm.provision "shell", path: "install-mbedtls.sh"
    box.vm.provider "virtualbox" do |vb|
      vb.gui = false
      vb.memory = 768
    end
  end

  config.vm.define "openvpn-build-bionic" do |box|
    box.vm.box = "ubuntu/bionic64"
    box.vm.box_version = "20180823.0.0"
    box.vm.hostname = "openvpn-build-bionic.local"
    box.vm.network "private_network", ip: "192.168.48.110"
    box.vm.synced_folder ".", "/vagrant", type: "virtualbox"
    box.vm.provision "shell" do |s|
      s.path = "setup-generic-buildsystem.sh"
      s.args = ["-f"]
    end
    box.vm.provision "shell", path: "setup-samba-share.sh"
    box.vm.provider "virtualbox" do |vb|
      vb.gui = false
      vb.memory = 1024
    end
  end

  config.vm.define "sbuild" do |box|
    box.vm.box = "ubuntu/bionic64"
    box.vm.box_version = "20180823.0.0"
    box.vm.hostname = "sbuild.local"
    box.vm.network "private_network", ip: "192.168.48.111"
    box.vm.provision "shell", path: "sbuild.sh"
    box.vm.provider "virtualbox" do |vb|
      vb.gui = false
      vb.memory = 1024
    end
  end

  config.vm.define "oas" do |box|
    box.vm.box = "ubuntu/bionic64"
    box.vm.box_version = "20191030.0.0"
    box.vm.hostname = "oas.local"
    box.vm.network "private_network", ip: "192.168.48.112"
    box.vm.provision "shell", path: "oas.sh"
    box.vm.provider "virtualbox" do |vb|
      vb.gui = false
      vb.memory = 2048
    end
  end

  config.vm.define "msibuilder" do |box|
    box.vm.box = "mwrock/Windows2016"
    box.vm.box_version = "0.3.0"
    box.vm.hostname = "msibuilder"
    box.vm.network "private_network", ip: "192.168.48.113"
    box.vm.synced_folder ".", "/vagrant", type: "virtualbox"
    box.vm.provision "shell", path: "msibuilder.ps1"
    box.vm.provider "virtualbox" do |vb|
      vb.gui = false
      vb.memory = 3072
    end
  end
end
