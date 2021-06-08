#!/bin/bash

CERT_SRC=/usr/local/etc/certificate/SynologyDrive/SynologyDrive/cert.pem
CERT_DST=/volume1/@appstore/mosquitto/var/cert.pem

CHAIN_SRC=/usr/local/etc/certificate/SynologyDrive/SynologyDrive/chain.pem
CHAIN_DST=/volume1/@appstore/mosquitto/var/chain.pem

PRIVKEY_SRC=/usr/local/etc/certificate/SynologyDrive/SynologyDrive/privkey.pem
PRIVKEY_DST=/volume1/@appstore/mosquitto/var/privkey.pem

# Check certificates changed
if ! cmp $CERT_SRC $CERT_DST > /dev/null 2>&1
then
    # Certificates are different.
    # Copy certificates to mosquitto working directory
    cp $CERT_SRC $CERT_DST
    cp $CHAIN_SRC $CHAIN_DST
    cp $PRIVKEY_SRC $PRIVKEY_DST

    # Change ownership
    chown sc-mosquitto:mosquitto $CERT_DST
    chown sc-mosquitto:mosquitto $CHAIN_DST
    chown sc-mosquitto:mosquitto $PRIVKEY_DST

    # Change mod
    chmod 0644 $CERT_DST
    chmod 0644 $CHAIN_DST
    chmod 0644 $PRIVKEY_DST
else
    # Certificates not changed
fi