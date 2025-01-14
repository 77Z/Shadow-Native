# Shadow-Native

Shadow Engine

Add to vscode settings for syntax highlighting on shaders
`"files.associations": { "*.sc": "hlsl" }`

# Building

Shadow Engine binaries are built with a custom build system called Horde.

Horde requries the Deno runtime which can be downloaded for just for Shadow
by `cd`ing into `scripts` then executing `get-deno.sh` (linux only)

You need Deno, Ninja, and optionally Make in order to build Shadow

Running `./Horde` in the root of the project directory will generate the build
files needed for compilation. Then run `make` somewhere in "bin/sdw..."

The Makefile only serves to run all the Ninja file subdirectories and will
probably be replaced in the future. This also means that running make with
`-j` won't have an effect as Ninja automatically utilizes all available CPUs

## Windows

You will need Visual Studio 2022 with the *Desktop Development with C++*
package installed.

Windows support is worked on occasionally and can be done by running Horde
as `./Horde --config winbuild.json5`

This expects a gcc/clang type compiler and isn't going to work with MSVC

### WinGet Packages to install
```
Git.Git
LLVM.LLVM
Ninja-build.Ninja
Microsoft.VCRedist.2015+.x64
```

# Dependencies on Arch Linux
```bash
pacman -S base-devel glm vulkan-devel ninja wget unzip 
```

# TODO

- Horde Build system
  - compile_commands.json generation (bear is the current solution)
- Hot Reloading with jet-live or custom solution -
  https://slembcke.github.io/HotLoadC
- Physics
- PBR Rendering
- Script files to run static analysis, valgrind, etc.

Should have the ability to load any kind of resource (shader, model, texture,
font, etc.) from a Chunker file, embedded header file, or raw file. During
production it would be smart to convert chunks to embedded header files and
store them in the output binary.

When shipping a release build, the engine should optimize all models, convert
all textures to a common format depending on render backend, build all
dependencies in release mode, -O3 everything, and strip everything

Difference between editor and production build is the form of entry. Entry for
the editor is through the standard int main and goes through Runtime.cpp like we
see now. The production version will not load Dear ImGui for example, and
instead deserialize a scene file and go through that.

# Attributions

- BGFX - bkaradzic
- BX - bkaradzic
- BIMG - bkaradzic
- GLFW
- OpenVR - Valve
- Dear ImGui - ocornut
- Lohmann, N. (2022). JSON for Modern C++ (Version 3.10.5) [Computer software].
  https://github.com/nlohmann
- ImGui Node Editor - thedmd
- Miniaudio - mackron
- Termcolor - ikalnytskyi
- Snappy - Google
- LevelDB - Google
- EnTT - Michele Caini
- meshoptimizer - Arseny Kapoulkine
- stduuid - Marius Băncilă
- cubic_bezier.cc - The Chromium Project
