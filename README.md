# LightDisplay
Music to fancy lightshow. <br>
Currently WIP

## TODO:
- [ ] Get ESP SDK setup for CMake
- [ ] Implement bluetooth server on ESP32
- [ ] Wire lights to microcontroller
- [ ] Improve the colorization algorithm
- [ ] Implement lightstrip control on microcontroller based on data received from bluetooth

# Overview
Audio is streamed from an audio source. Either directly loading a wav file or loopback recording via WASAPI.
The FFT is used to convert each "chunk" of audio from a time domain to a frequency domain. From here, this data can be converted to a visual representation by the Signal Visualizer.
As of this moment, the current algorithm is simply to find a multiple of each audio frequency within the visible light spectrum, and convert that band to its associated color in the
visual light spectrum. The alpha channel correlates to the intensity of the sound.
From here, the visual data (RGB and XY information) is transmitted via a SignalTransmitter and received by an endpoint with a SignalReceiver.
Finally, a SignalViewer displays that signal. The only working implementation so far uses OpenGL to create a 2D rendering on the local machine.

# Design:
The design of the applications rests heavily on Uncle Bob's philosophy that "everything is a detail". To that end, it might be a little curious why GLSL is being used.
Well, it's simply so the visuals can be tested without having the complete boundaries in all their formal glory setup first. Which, as of this moment, is still currently a todo.
<br>
DIP is employed to keep the flow of control from low-level to high level and keep dependencies going from high-level to low level components. 
The following is a mock-up I drew up before starting the project
![Dependency Diagram](https://dl.dropboxusercontent.com/s/cujdyymbja37j31/light%20display%20diagram.jpg?dl=0)
