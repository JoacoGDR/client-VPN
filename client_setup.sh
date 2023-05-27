#!/bin/bash
mkdir keys
(umask 077 && $ wg genkey > privatekey)
wg pubkey < privatekey > publickey
