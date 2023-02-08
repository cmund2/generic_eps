# Generic EPS - NOS3 Component
This repository contains the NOS3 Generic Electrical Power System Component.
This includes flight software (FSW), ground software (GSW), simulation, and support directories.

## Overview
This generic eps component is an I2C device that accepts commands via writes to specific addresses in memory and returns status in every response.
The available FSW is for use in the core Flight System (cFS) while the GSW supports COSMOS.
A NOS3 simulation is available which includes both generic_eps and 42 data providers.


# Device Communications
The protocol, commands, and responses of the component are captured below.
The EPS has 8 switches to turn components on and off.
Information for voltage, current, and status of each are provided. 
Additionally battery voltage, battery temperature and solar array voltage, solar array temperature.

## Protocol
The protocol in use is I2C at 1MHz with 7-bit address 0x2B. 

## Command Address
Device commands are all formatted in the same manner and are fixed in size.
Commands are confirmed only by verifying the expected switch state change in telemetry.
* uint8, Slave Address [7:1] and Read/Write [0]
* uint8, Command
* uint8, Data
* uint8, CRC
  - CRC8 of command and data

Command Table
* 0x00, Switch 0 State
  - Data of 0x00 for OFF, 0xAA for ON for all switches
* 0x01, Switch 1 State
* ...
* 0x07, Switch 7 State
* 0x70, Telemetry Request
  - Data field unused
* 0xAA, Reset
  - Data of 0xAA to trigger power cycle

## Telemetry
Telemetry reported is in big endian format (MSB x, LSB x+1) and is updated at a 1Hz frequency.
* Battery
  - uint16, voltage
  - uint16, temperature
* EPS
  - uint16, 3.3 voltage
  - uint16, 5.0 voltage
  - uint16, 12.0 voltage
  - uint16, temperature
* Solar Array
  - uint16, voltage
  - uint16, temperature 
* Switch [0-7]
  - uint16, voltage
  - uint16, current
  - uint16, status
    * Bits [15:8] - Error flags, 0x00 is healthy
    * Bits [7:0]  - 0x00 is off, 0xAA is on
* uint8, CRC
  - CRC8 of the previous data

Conversion table
* Voltages = (uint16 value * 0.001V)
* Currents = (uint16 value * 0.001A)
* Temperatures = (uint16 value * 0.01C) - 60C


# Configuration
The various configuration parameters available for each portion of the component are captured below.

## FSW
Refer to the file [fsw/platform_inc/generic_eps_platform_cfg.h](fsw/platform_inc/generic_eps_platform_cfg.h) for the default configuration settings, as well as a summary on overriding parameters in mission-specific repositories.

## Simulation
The default configuration returns data that mimics standard laboratory conditions after conversions:
```
<simulator>
    <name>generic_eps_sim</name>
    <active>true</active>
    <library>libgeneric_eps_sim.so</library>
    <hardware-model>
        <type>GENERIC_EPS</type>
        <connections>
            <connection>
                <type>command</type>
                <bus-name>command</bus-name>
                <node-name>generic_eps-sim-command-node</node-name>
            </connection>
            <connection>
                <type>i2c</type>
                <bus-name>i2c_1</bus-name>
                <bus-address>0x2B</bus-address>
                <node-port>10</node-port>
            </connection>
        </connections>
        <data-provider>
            <type>GENERIC_EPS_PROVIDER</type>
        </data-provider>
        <physical>
            <bus>
                <battery-voltage>24.0</battery-voltage>
                <battery-temperature>30.0</battery-temperature>
                <solar-array-voltage>32.0</solar-array-voltage>
                <solar-array-temperature>80.0</solar-array-temperature>
            </bus>
            <switch-0>
                <node-name>unknown-sim-command-node</node-name>
                <voltage>3.3</voltage>
                <current>0.25</current>
                <hex-status>0000</hex-status>
            </switch-0>
            ...
            <switch-7>
                <node-name>unknown-sim-command-node</node-name>
                <voltage>12.0</voltage>
                <current>1.23</current>
                <hex-status>00AA</hex-status>
            </switch-7>
        </physical>
    </hardware-model>
</simulator>
```

## 42
Optionally the 42 data provider can be configured in the `nos3-simulator.xml`:
```
        <data-provider>
            <type>GENERIC_EPS_42_PROVIDER</type>
            <hostname>localhost</hostname>
            <port>4242</port>
            <max-connection-attempts>5</max-connection-attempts>
            <retry-wait-seconds>5</retry-wait-seconds>
            <spacecraft>0</spacecraft>
        </data-provider>
```


# Documentation
If this generic_eps application had an ICD and/or test procedure, they would be linked here.

## Releases
We use [SemVer](http://semver.org/) for versioning. For the versions available, see the tags on this repository.
* v1.0.0 - X/Y/Z 
  - Updated to be a component repository including FSW, GSW, Sim, and Standalone checkout
* v0.1.0 - 10/9/2021 
  - Initial release with version tagging
