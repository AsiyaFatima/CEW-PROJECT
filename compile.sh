#!/bin/bash
gcc main.c send_email.c -o example -lcurl -lcjson
./example

source sendEmail.sh
