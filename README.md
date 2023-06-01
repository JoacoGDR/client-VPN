# How-to explicativo:

En el siguiente documento se explica como instalar un VPN WireGuard ya sea cliente-stitio como sitio-sitio/multisitio:
Para todos los escenarios se va a requerir instalar WireGuard.
En caso de que tenga linux, puede correr el siguiente comando: sudo apt-install wireguard
En caso de poseer otro sistema operativo, puede fijarse la guía de wireguard en como instalarlo: https://www.wireguard.com/install/

Para todos los escenarios, si uno quiere probar la implementación, se van a requerir más de dos máquinas (ya sean virtuales o físicas). Si no posee dos dispositivos, puede utilizar una EC2 en AWS o una máquina virtual con acceso a la red local. En el caso de los servidores, siempre es necesario que la maquina tenga un subsistema linux debido a que se usan comandos que son específicamente de este sistema operativo. 

**Como crear una interfaz WireGuard:**
1. Primero hay que generar un par clave privada y pública con los siguientes comandos:
```
(umask 077 && wg genkey > privatekey)
wg pubkey < privatekey > publickey
```


2. En el directorio /etc/wireguard crear un archivo que se llame [nombre de la interfaz].conf
por ejemplo ```touch wg0.conf```
El archivo tiene que contener la siguientes líneas:

```
[Interface]
Address = [IP privada con la que quieras identificar tu servidor dentro de la VPN]
PreUp = sysctl -w net.ipv4.ip_forward=1 # --> Para habilitar el forwarding de IPv4.
ListenPort = 51820 # --> (Puerto que se utiliza por convención en WireGuard)
PrivateKey = [clave privada generada]
```

3. Si se quiere levantar la interfaz, correr 
```
sudo wg-quick up wg0
```
4. Verificar con ```ifconfig``` si la interfaz está levantada. También puede usar el comando ```sudo wg show``` para ver las interfaces de WireGuard que se encuentran activas. 


## Cliente-Sitio
1. Clonar el siguiente repositorio en ambas máquinas (la del cliente y la del servidor): 
```
	git clone https://github.com/luciatorrusio/client-VPN
```
2. Del lado del servidor: 
	1. Levantar primero un servidor WireGuard. Para ello seguir los pasos mencionados anteriormente.
	2. Pararse en el directorio del repositorio y correr ```gcc server.c -o server```
	3. Correr ```./server [clave publica] [nombre de interfaz]``` con la clave pública que se generó para el servidor WireGuard y el nombre de la interfaz correspondientemente.
	4. Verificar que se esté haciendo el port forwarding de los puertos 51820 y 51822 del router a la máquina donde estas corriendo los servidores, ya que son los puertos que utilizan los dos procesos. El port forwarding depende del router o ISP de cada uno, por lo tanto no podemos proveer pasos para realizarlo. En caso de que su ISP no permita esta funcionalidad, puede correr un servidor en AWS. 
 
3. Del lado del cliente:
	1. Pararse en el directorio del repositorio
	2. Correr ```gcc client.c -o client```
	3. Correr ```./client [ip publica del servidor] [allowedIPs]``` donde allowedIPs son todas las IPs que queres redirigir por le VPN. Por ejemplo, si se quiere usar VPN para anonimato, se podría usar un allowedIPs = 0.0.0.0/0. La ideas es que acá pongas la red del sitio por ejemplo un 10.10.10.0/24

Con esos pasos, se debería establecer una conexión cliente-sitio. Puede probarlo haciendo ```ping``` a una de las IPs de la red del sitio desde el cliente. Puede verificar con el comando ```sudo wg show``` a ver si se agregó como Peer el cliente o el servidor y verificar si se realizo un handshake entre los dos y se intercambió datos. 


## Sitio a Sitio / Multisitio

Para la configuración de este tipo de VPNs, todas las máquinas van a tener que ponerse de acuerdo cuanto a sus ips privadas dentro de la VPN e intercambiarse de alguna forma sus claves públicas y endpoints.

Esto se debe a que para este punto no se realizó ningun tipo de automatización para el intercambio de esta información. 

Por lo tanto, cada máquina va a tener que configurar sus archivos .conf como si fueran servidores y agregar a todo el resto como Peers de esta forma.

Suponiendo que tenemos 3 hosts:

Cada uno va a tener que generar sus claves con los siguientes comandos:
```
(umask 077 && wg genkey > privatekey)
wg pubkey < privatekey > publickey
```

Para el host A:
```
[Interface]
Address = [IP privada de la VPN para host A]
PreUp = sysctl -w net.ipv4.ip_forward=1 # --> Para habilitar el forwarding de IPv4.
ListenPort = 51820 # --> (Puerto que se utiliza por convención en WireGuard)
PrivateKey = [clave privada de A generada]

#host B como Peer
[Peer]
Endpoint = [IP publica de B]:51820
PublicKey = [PublicKey de B]
AllowedIPs = [Red privada de B], [Address de B en la VPN]


#host C como Peer
[Peer]
Endpoint = [IP publica de C]:51820
PublicKey = [PublicKey de C]
AllowedIPs = [Red privada de C], [Address de C en la VPN]
```

Para el host B y C, sería análogo..

Cada uno tiene que levantar su propio .conf y lo puede hacer con el siguiente comando:

```
sudo wg-quick up [nombre de interfaz]
```


Con eso debería estar establecida la VPN sitio a sitio o multisitio. 