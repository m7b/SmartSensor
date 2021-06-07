This directory is intended for the mosquitto configuration file.

mosquitto.conf
==============

This file should be placed in
/volume1/@appstore/mosquitto/var/mosquitto.conf


job_copy_certs_mosquitto.sh
===========================

This file should be placed in
/volume1/@appstore/mosquitto/var/job_copy_certs_mosquitto.sh

Then create a job in Synology DSM for running this script as root before shut down.


users.txt
=========

This file is for users and their passwords.
Place this file also in
/volume1/@appstore/mosquitto/var/users.txt

Then run the following on it:
mosquitto_passwd -U users.txt