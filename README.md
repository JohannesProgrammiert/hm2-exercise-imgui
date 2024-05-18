# hm2-exercise-imgui

University math exercise with ImGui frontend for Linux.

Last tested on EndavourOS 6.8.9-arch1-2

## Requirements

- `opengl`
- `glfw`
- `cmake`
- `make` or `ninja`
- c++20 compiler, e.g. `gcc`

## Compile Instructions

1. Clone repository and submodules

```sh
git clone https://github.com/JohannesProgrammiert/hm2-exercise-imgui.git
cd hm2-exercise-imgui
git submodule update --init --recursive
```

2. Build

For example, `make` or `ninja` may be used:

### With Ninja

```sh
mkdir build
cd build
cmake -GNinja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
ninja
```

### With Make

```sh
mkdir build
cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
make
```

3. Run

The executable is in directory `build`:

```sh
./build/plottings
````
