#!/bin/bash
echo "Server Public key: $1"
echo "server private network: $2"
echo "Server Public ip: $3"
echo "Assigned address: $4"
echo "Private Key"



#!/bin/bash

sudo wg-quick down wg0
private_key=$(cat privatekey)

# Create the WireGuard configuration file
cat > /etc/wireguard/wg0.conf << EOF
[Interface]
PrivateKey = $private_key
Address = $4/32
ListenPort = 51821

[Peer]
Endpoint = $3:51820
PublicKey = $1
AllowedIPs = $2
EOF

sudo wg-quick up wg0

# if [[ $(uname) == "Darwin" ]]; then
#     # Run macOS-specific commands
#     echo "Running macOS commands..."
#     # Place your macOS commands here
#     private_key=$(cat privatekey)

#     sudo wg-quick down wg0
#     # Create the WireGuard configuration file
#     cat > /etc/wireguard/wg0.conf << EOF
#     [Interface]
#     PrivateKey = $private_key
#     Address = $4/32
#     ListenPort = 51821

#     [Peer]
#     Endpoint = $3:51820
#     PublicKey = $1
#     AllowedIPs = $2
#     EOF
    
#     sudo wg-quick up wg0

# else
#     # Run commands for other operating systems
#     echo "Running commands for other operating systems..."
#     # Place your commands for other OS here
#     sudo ip link delete dev wg0 > /dev/null
#     sudo ip link add dev wg0 type wireguard
#     sudo wg set wg0 private-key privatekey peer $1 allowed-ips $2 endpoint $3:51820
#     sudo ip link set dev wg0 up
#     sudo ip route add $2 dev wg0 scope link

#     sudo ip address add dev wg0 $4/32

# fi



