# Renderer3D

A simple C++ OpenGL rendering and physics demo built with CMake.

## Project structure

- `CMakeLists.txt` — project build configuration
- `README.md` — project overview and instructions
- `LICENSE` — project license
- `include/` — public headers
- `src/` — application source files
- `tests/` — test sources
- `build/` — generated build files (ignored by Git)

## Build

```bash
cd '/home/judegant/C++ 3D rendering'
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/Renderer3D
```

## Dependencies

- CMake 3.10 or newer
- C++ compiler with C++23 support
- OpenGL
- GLUT

## Notes

Keep generated files like `build/` out of Git and keep implementation files under `src/` with headers under `include/`.
