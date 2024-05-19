# Container for flashing microcontrollers

This container flashes the default RAS micro control system on a connected compatible board (commonly arduino with RAS). 

If you call from this folder:
```shell
docker compose build
docker compose up
```
Then it should start a container, install flash tools in the container and flash the software in ras_main_micro_driver onto the device. 

Be sure that the serial line is free during this process. 