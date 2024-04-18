Fledermaus
==========

Flying mouse. Get it?

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
