# PlatformIO usage

As explained in the main readme, the Arduino code is organized in a PlatformIO project. This has a few advantages:
* PIO has library management. You don't need to search for and download libraries yourself, they are defined in the project file and PIO will download them for you.
* The same holds for board-specific code files. This is relevant when using something else than an Arduino.
* PIO works with the exact same libraries and Arduino platform code as the Arduino IDE itself.
* You can use a nice to use IDE, such as [VS Code](https://code.visualstudio.com/), [CLion](https://www.jetbrains.com/clion/), etc, that offers code completion and insight. This just makes programming easier.
* You can upload code to the Arduino from the command line. Useful when working remotely through SSH.
* With the right hardware, you can debug your code within your IDE through PIO.


## PIO installation

See the [installation instructions](https://docs.platformio.org/en/latest/core/installation/index.html). There are numerous options for installing PlatformIO. The easiest might be the [VS Code integration](https://docs.platformio.org/en/latest/integration/ide/vscode.html#ide-vscode).
Slightly more advanced is the [CLI usage](https://docs.platformio.org/en/latest/core/index.html#piocore), called PlatformIO Core.

## PIO commands

Most things can be done from within VS Code, but when using the command line, these commands are useful. For example when working through an SSH connection.

### Init
navigate to the folder that contains the pio init file (e.g. ```GitHub/RAS_TitoNeri/arduino_low_level``` )

Configure the project/environment based on the project file.
```shell
pio init
```

When using CLion, you can use `pio init --ide clion`.

### Compiling and uploading

Simply run

```shell
pio run
```

and PIO will compile and upload your code.

Or in Visual Studio Code
- Open the project
- Click the PIO alien icon on the left bar
- Under Project tasks/mega/general press upload. 

The terminal should show uploading progress in over several seconds.

### Device monitor

You can use the `device monitor` command to open a serial monitor.
```shell
pio device monitor -p /dev/ttyACM1 -b 115200
```
