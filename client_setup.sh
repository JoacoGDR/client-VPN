#!/bin/bash
mkdir keys
(umask 077 && $ wg genkey > ./keys/privatekey)
wg pubkey < ./keys/privatekey > ./keys/publickey
