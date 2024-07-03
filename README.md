Fledermaus
==========

Flying mouse. Get it?

Use hand tracking to control your mouse cursor.

Controls
--------

- Pinch to click. Supports buttondown on pinch and buttonup on release
- Rotate hand so your thumb is pointing up to right click
- V-sign to scroll. Make a v-sign, palm down, pointing forwards. Tilt your hand up to scroll up, down to scroll down.
- Fist to ignore. Equivalent of lifting your mouse off the desk, good to reach further or avoid accidental interactions.

The above is all configurable through the config file.

Dependencies
------------

$: git clone git@github.com:Tencent/rapidjson.git


To build
--------

$: mkdir build && cd build
$: cmake  ..
$: cmake --build . --config [Debug|Release]

Then copy the LeapC.dll into the same folder as the executable.

To run
------

$: ./Fledermaus.exe speed [a float] scrolling [a float]
