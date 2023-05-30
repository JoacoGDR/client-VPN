#!/bin/bash

echo "Server Public key: $1"

echo "server private network: $2"

echo "Server Public ip: $3"



sudo ip link delete dev wg0

sudo ip link add dev wg0 type wireguard

sudo wg set wg0 private-key privatekey peer $1 allowed-ips $2 endpoint $3:51820

sudo ip link set dev wg0 up

sudo ip route add $2 dev wg0 scope link