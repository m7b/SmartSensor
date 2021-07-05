This directory is intended for the mosquitto configuration file.
Mosquitto is running on Synology DSM 7.0

mosquitto.conf
==============

This file should be placed in
`/volume1/@appdata/mosquitto/mosquitto.conf`


job_copy_certs_mosquitto.sh
===========================

This file should be placed in
`/volume1/@appdata/mosquitto/job_copy_certs_mosquitto.sh`

Then create a job in Synology DSM for running this script as root before shut down.


users.txt
=========

This file is for users and their passwords.
Place this file also in
`/volume1/@appdata/mosquitto/users.txt`

Then run the following on it:
`mosquitto_passwd -U users.txt`