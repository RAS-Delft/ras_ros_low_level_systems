# ras_ros_low_level_systems
A bridge between vessels' low level systems and ros, running on the onboard computers. 


## Use:


## Setup
### Installation of OS on on-board computer
- Install a recent linux distribution. You can choose a more minimalistic OS, or one with conveniencies such as a desktop GUI. 
    - For Raspberry Pi's consider using the linux raspberry-pi-imager program from the software center. It's quite convenient. I used Ubuntu-Server-24.04LTS64bit image for RPI4B
    - Consider what this distro is compatible with. Docker can give you flexibility in deploying modules, but if you want to run something specific, check if it is supported (e.g. check support for your envisioned ros distibution).
- Configure the network to connect to RAS routers or VPN. For RPI4 server I changed the network-config on the sdcard to know the NETGEAR21 router. 
- Configure SSH connection. Make sure you can ssh with a terminal from another PC to the device using `ssh defaultusername@192.168.1.3` (default user is commonly 'ubuntu' or 'pi' depending on the OS you chose) [This guide](https://phoenixnap.com/kb/ssh-permission-denied-publickey) worked nice for me. 
- Set username to ras with new password. 
    - SSH into device using default user ssh ubuntu@192.168.1.3
    - add ras user `sudo adduser ras`
    - add ras user to sudo group `sudo adduser temporary sudo`
    - `exit` and relog as new user `ssh ras@192.168.1.3`
    - remove default user `sudo deluser ubuntu` and its homefolder `sudo rm -r /home/ubuntu`
- Change the device name to something sensible: `sudo nano /etc/hostname` and preferably unique (to allow login by name later)
- (optional) Enable login by hostname by `sudo nano /etc/systemd/resolved.conf` and setting `MulticastDNS=yes`
- (optional) Set access keys between device and your pc for easy access: `ssh-copy-id ras@192.168.1.3`

- snap install docker
- sudo groupadd docker
- sudo apt install net-tools

### Prepare modules 
All major components are stored in the ras homefolder (`/home/ras`).
- Add the configuration file for this drone, where we set the major settings for this device.
    - `sudo nano ~/ras_entrypoint.sh' and edit the fields as required
``` 
#!/bin/bash
export VESSEL_ID="RAS_TN_DB"
export RAS_GH_USERNAME="ras-delft-user"
export RAS_GH_KEY="<FILL IN ras-delft-user pull/write GITHUB KEY>"
export RMW_IMPLEMENTATION="rmw_fastrtps_cpp"
```
    - Make sure this gets called upon bash startup by adding it to .bashrc with `nano` or `echo "source /home/ras/ras_entrypoint.sh" >> ~/.bashrc`
- Get the repository with main dockerfiles
    - `git clone https://${RAS_GH_USERNAME}:${RAS_GH_KEY}@github.com/RAS-Delft/ras_ros_low_level_systems`
    - Update submodules (again fill in `$RAS_GH_USERNAME` and `$RAS_GH_KEY` if queried) `cd ras_ros_low_level_systems; git submodule update --init --recursive`

- Build and run the docker containers
from /ras_ros_low_level_systems:
```shell
docker compose build
```
```shell
docker compose up
```

### Microcontroller
This device is responsible for executing desired actuation. 

... Documentation W.I.P. ...
