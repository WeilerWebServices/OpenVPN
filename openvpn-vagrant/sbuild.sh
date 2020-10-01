echo "Updating package cache"
apt-get update
echo
echo "Installing sbuild_wrapper dependencies"
apt-get -y install sbuild git quilt debhelper
echo
echo "Cloning sbuild_wrapper if not present"
test -d sbuild_wrapper || git clone --branch fixes https://github.com/mattock/sbuild_wrapper.git
echo
echo "Setting up sbuild_wrapper and schroots"
cd sbuild_wrapper
scripts/setup.sh
scripts/setup_chroots.sh
scripts/install-build-deps.sh
scripts/update-all.sh
scripts/prepare-all.sh
