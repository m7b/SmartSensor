#!/bin/bash

# Copy certificates to mosquitto working directory
cp /usr/local/etc/certificate/SynologyDrive/SynologyDrive/cert.pem /volume1/@appstore/mosquitto/var
cp /usr/local/etc/certificate/SynologyDrive/SynologyDrive/chain.pem /volume1/@appstore/mosquitto/var
cp /usr/local/etc/certificate/SynologyDrive/SynologyDrive/privkey.pem /volume1/@appstore/mosquitto/var

# Change ownership
chown sc-mosquitto:mosquitto /volume1/@appstore/mosquitto/var/*.pem

# Change mod
chmod 0644 /volume1/@appstore/mosquitto/var/*.pem