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
Device command are all formatted in the same manner and are fixed in size.
Commands are confirmed by reading the commanded address and confirming previous input is returned.
* uint8, Slave Address [7:1] and Read/Write [0]
* uint8, Address
* uint8, Data
* uint8, CRC
  - CRC8 of address and data

Command Address Table
* 0x00, Switch 0 State
  - Data of 0x00 for OFF, 0xAA for ON for all switches
* 0x01, Switch 1 State
* ...
* 0x07, Switch 7 State
* 0xA0, Reset
  - Data of 0xAA to trigger power cycle

## Telemetry
The telemetry can be read individually or in sequential groups.
Telemetry registers are read only.
Telemetry reported is in big endian format (MSB x, LSB x+1).
All telemetry register reads are followed by a single uint8 CRC8 of the previous data.
* 0xB0, Battery
  - uint16, voltage
  - uint16, temperature
* 0xB4, EPS
  - uint16, temperature
* 0xB6, Solar Array
  - uint16, voltage
  - uint16, temperature
* 0xBA, Switch [0-7]
  - uint16, voltage
  - uint16, current
  - uint16, status

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
            <connection><type>command</type>
                <bus-name>command</bus-name>
                <node-name>generic_eps-sim-command-node</node-name>
            </connection>
            <connection><type>i2c</type>
                <bus-name>i2c_1</bus-name>
                <node-port>10</node-port>
            </connection>
        </connections>
        <data-provider>
            <type>GENERIC_EPS_PROVIDER</type>
        </data-provider>
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
