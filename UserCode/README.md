# UserCode

If writing game code for Shadow Engine, this is the place to do it.

Right now, only c++ is supported out of the box, but it should be really easy to reconfigure Meson to build C.

To build the code in this directory for use in Shadow Engine simply run:
```
meson setup ../bin
cd ../bin
ninja
```

Note that you don't have to build this at all for Shadow Engine to work, you'll get a warning from the engine at most
