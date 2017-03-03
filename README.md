# DeepGTAV v2

*Documentation under construction*.

## Installation
1. Download, install and update GTAV to the last version
2. Copy-paste the contents of *bin/Release* under your GTAV installation directory
3. Replace your saved game data in *Documents/Rockstar Games/GTA V/Profiles/* with the contents of *bin/SaveGame*
4. Download *[paths.xml](https://drive.google.com/open?id=0Bzh5djJlCOmMOTA1RVlOXzZ5dEk)* and store it also in the GTAV installation directory. 

## How it works

If installation was successful, GTAV will automatically load the DeepGTAV plugin. Once the game starts DeepGTAV will wait for TCP clients to connect on port 8000.

Clients connected to DeepGTAV are allowed to send messages to GTAV to start and configure the research environment (*Start* and *Config* messages), send driving commands to control the vehicle (*Commands* message) and to stop the environment to fallback to normal gameplay (*Stop* message).

When the environment has been started with the *Start* message, DeepGTAV will start sending the data gathered from the game back to the client in JSON format, so the client can use it to store a dataset, run it through a self-driving agent...

The data sent back to the client and intitial conditions will depend on the parameters sent by the client with the *Start* or *Config* messages. Things that can be controlled for instance are: rate of transmission, frame width and height, weather, time, type of vehicle, driving style, wether to get surrounding vehicles or peds, type of reward function and a large etc...

[VPilot](https://github.com/ai-tor/VPilot) provides a nice interface and examples written in Python to use DeepGTAV for your self-driving car research.

The following chapters describe the purpose and contents of each message.

## Messages from the client to DeepGTAV

### Start

This is the message that needs to be sent to start DeepGTAV, any other message sent prior to this won't make any effect. Along with this message, several configuration parameters can be set to start DeepGTAV with the desired initial conditions and requested data transmission.

Here follows an example of the Start message:
```json
{"start":
  "scenario": {
  
  },
  "dataset": {
  }
}
```


### Config
```json
{"config":
  "scenario": {
  
  },
  "dataset": {
  }
}
```
### Commands
```json
{"commands":
  "throttle": 1.0,
  "brake": 0.0,
  "steering": -0.5
}
```
### Stop
```json
{"stop": {}}
```
## Messages from DeepGTAV to the client

### Data
