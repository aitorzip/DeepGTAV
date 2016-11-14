# DeepGTAV
ScriptHook mod to build a vision-based deep learning environment using GTAV. It supports two main operation modes, Dataset generation or Reinforcement Learning environment (under development)

## Installation instructions
Update GTAV to the last version and just copy-paste the contents of bin/Release under your GTAV installation directory. Then set the correct Dataset store directory in the config.ini file and run the game. 

**It will work out-of-the-box, it doesn't require any extra dependencies!**

## Configuration options
The configuration options of the environment are set through the *config.ini* file:

* mode: 0 for dataset generation, 1 for reinforcement learning (TODO)
* imageWidth: Width in pixels of the images to store during dataset generation
* imageHeight: Height in pixels of the images to store during dataset generation
* captureFreq: Frequency of image and labels capture in Hz (FPS)
* datasetDir: Absolute path with trailing slash to directory to save the dataset (must exist)
* maxDuration: Duration in hours of the dataset generation
* setSpeed: Set speed of the vehicle in m/s during dataset generation
* drivingStyle: Driving style of the vehicle driver during dataset generation
* car: Vehicle to use during dataset generation. For now only blista (Toyota Prius) is supported (0)
* weatherChangeDelay: Time in seconds between weather changes
* initialWeather: Initial weather type
* initialHour: Initial in-game hour
* initialMinute: Initial in-game minute
* initialPosX: Initial x position of the vehicle (will be set to closest road)
* initialPosY: Initial y position of the vehicle (will be set to closest road)

## TODO
* Better describe the config options in this README file
* Finish randomizing some events, add more cars
* Find a way to automatically close/restart the game once dataset generation has finished/vehicle is stuck.
* Implement RL mode

## Acknowledgements
Many thanks to @crizCraig for pioneering this work with DeepDrive.io and for his valuable advice!
