#!/bin/bash

echo "Server Public key: $1"
echo "server private network: $2"
echo "Server Public ip: $3"

sudo ip link add dev wg0 type wireguard
# $1 -> server pub key - $2 -> server private network - $3 -> server pub ip
sudo wg set wg0 private-key privatekey peer $1 allowed-ips $2 endpoint $3:51820
sudo wg setconf wg0 wg0.conf
sudo ip link set up dev wg0
sudo wg-quick up wg0
