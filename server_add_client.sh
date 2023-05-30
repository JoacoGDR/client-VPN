#!/bin/bash
sudo wg set $1 peer $2 allowed-ips $3/32
sudo ip route add $3 dev $1 scope link
