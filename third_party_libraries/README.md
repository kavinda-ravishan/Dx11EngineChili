# Assimp 6.0.6 Setup

## Download

Download Assimp v6.0.6:

https://github.com/assimp/assimp/archive/refs/tags/v6.0.6.zip

## Build

Generate the Visual Studio solution using CMake:

```bash
cmake -G "Visual Studio 18 2026" ^
      -DASSIMP_BUILD_TESTS=OFF ^
      -DASSIMP_INSTALL=OFF ^
      -S . ^
      -B build
```

## Directory Layout

After building, place the generated files in the following structure:

```text
third_party_libraries/
└── assimp-6.0.6/
    ├── include/
    │   └── assimp/
    ├── lib/
    └── bin/
```

## Runtime Configuration

Add the Assimp `bin` directory to your Visual Studio project's PATH environment:

```text
PATH=$(SolutionDir)third_party_libraries\assimp-6.0.6\bin;%PATH%
```

This ensures the required Assimp DLLs are available when running the application from Visual Studio.