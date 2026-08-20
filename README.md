# Samples Browser for LMMS (rev2)

Second version of the Samples Browser for LMMS (as a plugin) that lists all .wav samples in single window, and makes graphs per selected sample.

- Standalone version: complete
- Plugin version: Work in progress!!

<img width="818" height="654" alt="screenshot" src="https://github.com/user-attachments/assets/0607f1cc-6a90-4755-98e9-cc3bc362b4d6" />

## Compile as Standalone Application

```
mkdir build
cd build

cmake .. .
make

./samples2
```

## Compile As a Plugin for LMMS

- Link `plugin` folder as `[LMMS]/plugins/Samples2`.
- Activate `Samples2` plguin in `[LMMS]/cmake/modules/PluginList.cmake`.
- Compile LMMS.
- Go to `Tools` > `Samples Browser`.

```
cd samples-browser-rev2.lmms
ln -s plugin /PATH/TO/[LMMS]/plugins/Samples2
```

## ToDo

- Sometimes (for some samples), graph has prominent extra vertical line(s)
    - at the begining
    - at end of the graph
- Optional autoplay on row activation.
- Upgrade to Qt6.
