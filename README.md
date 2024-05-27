# ras_ros_low_level_systems
A bridge between vessels' low level systems and ros, running on the onboard computers. 


## Use:
Manually turning on the core on-board modules is done from a terminal within the network:
Connect to a ship:
```
ssh ras@titoneri-darkblue.local
``` 
where titoneri-darkblue.local can also be replaced by its IP address. 

Starting the main modules is done by:
```
cd ras_ros_low_level_systems; docker compose up
```

## Setup
### Installation of OS on on-board computer
Install a recent linux distribution. You can choose a more minimalistic OS, or one with conveniencies such as a desktop GUI. 

For Raspberry Pi's consider using the linux raspberry-pi-imager program from the software center. It's quite convenient. I used Ubuntu-Server-24.04LTS64bit image for RPI4B

Consider what this distro is compatible with. Docker can give you flexibility in deploying modules, but if you want to run something specific, check if it is supported (e.g. check support for your envisioned ros distibution).

Configure the network to connect to lab routers or VPN.

Configure SSH connection. Make sure you can ssh with a terminal from another PC to the device using `ssh defaultusername@192.168.1.3` (default user is commonly 'ubuntu' or 'pi' depending on the OS you chose) (a helpful [guide](https://phoenixnap.com/kb/ssh-permission-denied-publickey) )

Set username to `ras` with new password. For example with:
```shell
ssh ubuntu@192.168.1.3
```
make `ras` user and add to sudo group
```shell
sudo adduser ras; sudo adduser ras sudo
```
exit and relog as ras
```shell
ssh ras@192.168.1.3
```
```shell
sudo deluser --remove-home ubuntu
```

(optional) Change the device name to something sensible and preferably unique (to allow login by name later)
```shell
sudo nano /etc/hostname
```

(optional) Enable access by hostname (e.g. ssh ras@titoneri-green.local) by 
```shell
sudo nano /etc/systemd/resolved.conf
```
and setting `MulticastDNS=yes`


(optional) Set access keys between device and your pc for easy access. From your host pc that wants access:
```shell
ssh-copy-id ras@192.168.1.3
```

Install Docker
```shell
snap install docker
sudo groupadd docker # Give docker sudo access. Might need relog to apply.
```

(optional) install network debug tools
```shell
sudo apt install net-tools
```

### Prepare modules 
All major components are stored in the ras homefolder (`/home/ras`).
Add the configuration file for this drone, where we set the major settings for this device.
```shell
sudo nano ~/ras_entrypoint.sh
```
Fill in the following configuration:
```shell
#!/bin/bash
source vessel_id.env
export RAS_GH_USERNAME="ras-delft-user"
export RAS_GH_KEY="<FILL IN ras-delft-user pull/write GITHUB KEY>"
export RMW_IMPLEMENTATION="rmw_fastrtps_cpp"
```
 Make sure this gets called upon bash startup by adding it to .bashrc with `nano` or 
```shell
echo "source /home/ras/ras_entrypoint.sh" >> ~/.bashrc
```

Get the repository with main dockerfiles in the home folder ('cd ~')
```shell
git clone https://github.com/RAS-Delft/ras_ros_low_level_systems
```
Navigate to folder and update submodules
 ```shell
 cd ras_ros_low_level_systems; git submodule update --init --recursive
 ```

Build and run the docker containers
```shell
docker compose build
```

### Microcontroller
This device is responsible for executing desired actuation. 

... Documentation W.I.P. ...
