#!/bin/sh
#
# Setup a Samba share so that msibuilder VM can access build artefacts

share_dir="/home/vagrant/openvpn-build"
share_name="openvpn-build"
samba_user="vagrant"
samba_password="vagrant"
smb_conf_tmpl_src="/vagrant/smb.conf.tmpl"
smb_conf_tmpl_dst="/etc/samba/smb.conf.tmpl"

echo "Installing Samba server and client"
apt-get -y install samba smbclient

echo "Enabling Samba share"
test -d $share_dir || mkdir -p $share_dir
chmod 755 $share_dir
mkdir -p /etc/samba
touch /etc/samba/smb.conf
test -f /etc/samba/smb.conf.dist || cp -v /etc/samba/smb.conf /etc/samba/smb.conf.dist
cp -v $smb_conf_tmpl_src $smb_conf_tmpl_dst

# Sed will get confused about the slashes in the share directory path unless
# we use something else as a separator
sed -i "s+==SHARE_DIR==+$share_dir+g" $smb_conf_tmpl_dst
sed -i "s/==SHARE_NAME==/$share_name/g" $smb_conf_tmpl_dst
sed -i "s/==SAMBA_USER==/$samba_user/g" $smb_conf_tmpl_dst
cmp $smb_conf_tmpl_dst /etc/samba/smb.conf
if [ $? -ne 0 ]; then
    mv $smb_conf_tmpl_dst /etc/samba/smb.conf
else
    rm -f $smb_conf_tmpl_dst
fi

pdbedit -L|grep "^${samba_user}:" > /dev/null
if [ $? -ne 0 ]; then
    echo "Adding samba user \"${samba_user}\" with password \"${samba_password}\""
    yes $samba_password|head -n 2|smbpasswd -a -s $samba_user
fi

systemctl restart smbd
