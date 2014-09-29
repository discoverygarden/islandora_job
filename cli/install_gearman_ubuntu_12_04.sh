apt-get install python-software-properties
add-apt-repository ppa:gearman-developers/ppa
apt-get update

apt-get install libgearman-dev gearman-job-server gearman-tools
pecl install gearman

# Add extension=gearman.so to all applicable php.ini files.
