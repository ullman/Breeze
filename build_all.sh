#!/bin/bash
if [ -v TIZEN_CERTIFICATE ]
then
    echo "Certificate name: $TIZEN_CERTIFICATE"
else
    echo "Please define certificate name in TIZEN_CERTIFICATE"
fi


./build_lib.sh
./build_app.sh
