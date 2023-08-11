# Shadow-Native

C++ Shadow Engine

Add to vscode settings for syntax highlighting on shaders
`"files.associations": { "*.sc": "hlsl" }`

# Building

You need Deno, Ninja, and optionally Make in order to build Shadow

Running `./Horde` in the root of the project directory will generate the build
files needed for compilation. Then run `make` somewhere in "bin/shadow..."

# TODO

- Horde Build system
  - Shaders
  - HDoc
  - compile_commands.json generation
- Hot Reloading with jet-live or custom solution -
  https://slembcke.github.io/HotLoadC
- Physics
- PBR Rendering
- Proper Editor Interface (seems like a pain to write)
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
