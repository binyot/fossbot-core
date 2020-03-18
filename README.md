# fossbot-core

## Building

```
$ cd fossbot-core
$ mkdir build
$ cd build
$ cmake ..
```

After this you can run ```ccmake .``` to configure this build.

To build just the core, run
```
$ make core
```

Binaries should be in the ```bin/core``` path of your ```build``` directory.

## Running

Example usage:
```
$ bin/core -vvv --servo-file /dev/custom_leds --channel 1
```

## Bluetooth

User running this program has to have bluetooth/rfcomm-specific permissions. Service should be advertized as SP with UUID 0x1101.

## Syntax \[WIP\]

```
<time offset [ms]>: <servo id> -> <angle [deg]f> ... <servo id> -> <angle>
push
```

Example:
```
0000: neck -> 90.0 l_hand->30.0
0250: neck   -> 135.0
      r_hand -> 90.0
1000: r_hand -> .5
0250: l_hand -> 0
```

is interpreted as

```
(0, neck, 90.0),
(0, l_hand, 30.0),
(250, neck, 135.0),
(250, r_hand, 90.0),
(250, l_hand, 0.0),
(1000, r_hand, 0.5)
```
