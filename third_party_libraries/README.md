# Assimp 6.0.6 Setup

## Download

Download Assimp v6.0.6:

https://github.com/assimp/assimp/archive/refs/tags/v6.0.6.zip

## Build

Generate the Visual Studio solution using CMake:

```bash
cmake -G "Visual Studio 18 2026" ^
      -DBUILD_SHARED_LIBS=OFF ^
      -DASSIMP_BUILD_TESTS=OFF ^
      -DASSIMP_INSTALL=OFF ^
      -S . ^
      -B build
```

## Runtime Library Configuration

Before building, configure all Assimp projects to use the same runtime library as the engine:

| Configuration | Runtime Library |
|---------------|-----------------|
| Debug         | Multi-threaded Debug (/MTd) |
| Release       | Multi-threaded (/MT) |

This should be applied to:

- assimp
- zlibstatic
- any other generated Assimp static library projects

## Directory Layout

Copy the required files into the following structure:

```text
third_party_libraries/
└── assimp-6.0.6/
    ├── include/
    │   └── assimp/
    └── lib/
        ├── assimp-vc145-mtd.lib
        └── zlibstaticd.lib
        ├── assimp-vc145-mt.lib
        └── zlibstatic.lib
```

## Visual Studio Configuration

### Include Directories

```text
third_party_libraries\assimp-6.0.6\include
```

### Library Directories

Release/Debug:

```text
third_party_libraries\assimp-6.0.6\lib\
```

### Additional Dependencies

Debug:

```text
assimp-vc145-mtd.lib
zlibstaticd.lib
```

Release:

```text
assimp-vc145-mt.lib
zlibstatic.lib
```

## Notes

Assimp depends on zlib for compressed formats. When linking Assimp statically, the corresponding zlib static library must also be linked. Failure to do so will result in linker errors similar to:

```text
unresolved external symbol inflate
unresolved external symbol inflateEnd
unresolved external symbol crc32
unresolved external symbol uncompress
```

No Assimp DLLs are required when using this configuration.