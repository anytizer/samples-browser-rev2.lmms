# Samples Browser for LMMS (rev2)

Second version of the Samples Browser for LMMS (as a plugin) that lists all .wav samples in single window, and makes graphs per selected sample.

- Standalone version: complete
- Plugin version: Work in progress!!

# Compile as Standalone Application

```
mkdir build
cd build

cmake .. .
make

./samples2
```

# Compile As a Plugin for LMMS

- Link `plugin` folder as lmms/plugins/Samples2.
- Activate Samples2 plguin in CMakeLists.txt of lmms/plugins/.
- Compile.

```
ln -s /PATH/TO/samples-browser-rev2.lmms/plugin /PATH/TO/lmms/plugins/Samples2
```

# ToDo

- Sometimes (for some samples), graph has prominent extra vertical line(s)
    - at the begining
    - at end of the graph
- Optional autoplay
