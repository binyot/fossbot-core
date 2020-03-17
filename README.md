# fossbot-core

## Building

```
$ cd fossbot-core
$ mkdir build
$ cd build
$ cmake ..
```

After this you can run ```ccmake .``` to configure this build.

## Running

Example usage:
```
$ bin/core -vvv --channel 1
```

## Bluetooth

User running this program has to have bluetooth/rfcomm-specific permissions. Service should be advertized as SP with UUID 0x1101.
