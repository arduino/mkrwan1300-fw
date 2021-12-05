# MKRWAN1300 FIRMWARE

This package contains the firmware for Murata CMWX1ZZABZ-078 module.

It is derived from [I-CUBE-LRWAN](http://www.st.com/en/embedded-software/i-cube-lrwan.html), with some new APIs (like the ability to switch band at runtime, not only at compile time).

[Releases](https://github.com/bcmi-labs/mkrwan1300-fw/releases) contains the precompiled firmware that can be uploaded either using FWUpdaterBridge or MKRWANFWUpdate_standalone examples from in https://github.com/arduino-libraries/MKRWAN

All the code maintains its original license.

## Compiling the code 
The project is provided a [System Workbench for STM32](http://www.openstm32.org/System%2BWorkbench%2Bfor%2BSTM32) project, but can also be compiled in Eclipse after following [this](http://www.openstm32.org/Installing%2BSystem%2BWorkbench%2Bfor%2BSTM32%2Bfrom%2BEclipse#Important_note_about_your_MAC_OSX_host_version) guide. A third alternative is using Docker containers. Enter the following in a file called `Dockerfile`

```
FROM stronglytyped/arm-none-eabi-gcc:latest

#Buildkit for hex file
RUN apt-get update && \
	apt install -y xxd \
	&& rm -rf /var/lib/apt/lists/*

WORKDIR /home
CMD ["make", "-B"]
```
Then you create the new container image by running the build in the directory with `Dockerfile`. Let's name the image `arm-eabi-mkr`.
```
docker build . -t arm-eabi-mkr
```
Once you built the Docker image you can use the _Docker tooling_ add-on in Eclipse to build the firmware. Open the source code as CDT project and set the build container to `arm-eabi-mkr` in _Properties>C/C++ Build>Settings_.
Alternatively, enter the source directory and run a container mapping the source directory to home as follows:
```
docker run -rm -v "$PWD":/home arm-eabi-mkr 
```
The `-rm` option removes the container after execution, while `-v` sets the directory mapping from `$PWD`, i.e., local directory, to `/home` inside the container.

## AT Command List

| Command      | Description
| ------------ | ------------- |
| AT+ADR       | Get or Set the Adaptive Data Rate setting. (0: off, 1: on) |
| AT+APPEUI    | Get or Set the Application EUI |
| AT+APPKEY    | Get or Set the Application Key |
| AT+APPSKEY   | Get or Set the Application Session Key |
| AT+BAND      | Get or Set the Regional Band |
| AT+BAT       | Get the battery level |
| AT+CERTIF    | Set the module in LoraWan Certification Mode |
| AT+CFM       | Get or Set the confirmation mode (0-1) |
| AT+CFS       | Get confirmation status of the last AT+SEND (0-1) |
| AT+CHANMASK  | Gets the current region's channel mask, note this is reset when changing regions |
| AT+CHANDEFMASK | Gets the current region's default mask, note this is reset when changing regions |  |
| AT+CLASS     | Get or Set the Device Class |
| AT+CTX       | send with confirmation |
| AT+DEV       | Get the version of the AT_Slave FW |
| AT+DEVADDR   | Get or Set the Device address |
| AT+DEVEUI    | Get the Device EUI |
| AT+DFORMAT   | select hex or binary format |
| AT+DR        | Get or Set the Data Rate. (0-7 corresponding to DR_X). If ADR is off, it sets also the default data rate |
| AT+DUTYCYCLE | Get or Set the ETSI Duty Cycle setting - 0=disable, 1=enable - Only for testing |
| AT+FCD       | Get or Set the Frame Counter Downlink |
| AT+FCU       | Get or Set the Frame Counter Uplink |
| AT+IDNWK     | Get or Set the Network ID |
| AT+JN1DL     | Get or Set the Join Accept Delay between the end of the Tx and the Join Rx Window 1 in ms |
| AT+JN2DL     | Get or Set the Join Accept Delay between the end of the Tx and the Join Rx Window 2 in ms |
| AT+JOIN      | Join network |
| AT+MODE      | Get or Set the Network Join Mode. (0: ABP, 1: OTAA) |
| AT+MSIZE     | Get the maximum send/receive size for the actual data rate |
| AT+NJS       | Get the join status |
| AT+NWK       | Get or Set the public network mode. (0: off, 1: on) |
| AT+NWKSKEY   | Get or Set the Network Session Key |
| AT+PORT      | set application port |
| AT+REBOOT    | Trig a reset of the MCU |
| AT+RECV      | print last received data in raw format |
| AT+RECVB     | print last received data in binary format (with hexadecimal values) |
| AT+RFPOWER   | Get or Set the Transmit Power (0-5) |
| AT+RSSI      | Get the RSSI of the last received packet |
| AT+RX1DL     | Get or Set the delay between the end of the Tx and the Rx Window 1 in ms |
| AT+RX2DL     | Get or Set the delay between the end of the Tx and the Rx Window 2 in ms |
| AT+RX2DR     | Get or Set the Rx2 window data rate (0-7 corresponding to DR_X) |
| AT+RX2FQ     | Get or Set the Rx2 window frequency |
| AT+SEND      | Send text data along with the application port |
| AT+SENDB     | Send hexadecimal data along with the application port |
| AT+SNR       | Get the SNR of the last received packet |
| AT+TCONF     | Config LORA RF test |
| AT+TOFF      | Stops on-going RF test |
| AT+TRLRA     | Starts RF Rx LORA test |
| AT+TRSSI     | Starts RF RSSI tone test |
| AT+TTLRA     | Starts RF Tx LORA test |
| AT+TTONE     | Starts RF Tone test |
| AT+UTX       | send without confirmation |
| AT+VER       | Get the version of the AT_Slave FW|
