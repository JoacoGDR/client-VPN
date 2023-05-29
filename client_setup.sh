#!/bin/bash
(umask 077 && wg genkey > privatekey)
wg pubkey < privatekey > publickey
