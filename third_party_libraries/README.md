# Third-Party Libraries

## Directory Layout

All third-party dependencies are organized into a common include and library structure:

```text
third_party_libraries/
├── include/
│   ├── assimp/
│   └── dxtex/
│
└── lib/
    ├── Debug/
    │   ├── assimp-vc145-mtd.lib
    │   ├── zlibstaticd.lib
    │   ├── DirectXTex.lib
    │   └── ...
    │
    └── Release/
        ├── assimp-vc145-mt.lib
        ├── zlibstatic.lib
        ├── DirectXTex.lib
        └── ...
```

---

# Assimp 6.0.6

## Download

https://github.com/assimp/assimp/archive/refs/tags/v6.0.6.zip

## Generate Visual Studio Solution

```bash
cmake -G "Visual Studio 18 2026" ^
      -DASSIMP_BUILD_TESTS=OFF ^
      -DASSIMP_INSTALL=OFF ^
      -S . ^
      -B build
```

## Runtime Library Configuration

Prior to building, update the runtime library settings for all generated projects to match the engine:

| Configuration | Runtime Library |
|---------------|-----------------|
| Debug         | Multi-threaded Debug (/MTd) |
| Release       | Multi-threaded (/MT) |

Apply this to:

- assimp
- zlibstatic
- Any other generated static library projects

## Required Libraries

Copy the generated libraries into:

```text
third_party_libraries/lib/Debug
third_party_libraries/lib/Release
```

Required libraries:

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

## Include Directory

Copy Assimp headers to:

```text
third_party_libraries/include/assimp
```

---

# DirectXTex

## Download

https://github.com/microsoft/DirectXTex/archive/refs/tags/may2026.zip

## Runtime Library Configuration

Update the DirectXTex project to use the same runtime library settings as the engine:

| Configuration | Runtime Library |
|---------------|-----------------|
| Debug         | Multi-threaded Debug (/MTd) |
| Release       | Multi-threaded (/MT) |

Rebuild the solution after changing these settings.

## Required Libraries

Copy the generated DirectXTex libraries into:

```text
third_party_libraries/lib/Debug
third_party_libraries/lib/Release
```

## Include Directory

Copy DirectXTex headers to:

```text
third_party_libraries/include/dxtex
```

---

# Visual Studio Configuration

## Additional Include Directories

```text
$(SolutionDir)third_party_libraries/include
```

## Additional Library Directories

Debug:

```text
$(SolutionDir)third_party_libraries/lib/Debug
```

Release:

```text
$(SolutionDir)third_party_libraries/lib/Release
```

## Additional Dependencies

Debug:

```text
assimp-vc145-mtd.lib
zlibstaticd.lib
DirectXTex.lib
```

Release:

```text
assimp-vc145-mt.lib
zlibstatic.lib
DirectXTex.lib
```

---

# Notes

- All third-party libraries are statically linked.
- All projects use the same runtime library configuration:
  - `/MTd` for Debug
  - `/MT` for Release
- No Assimp DLLs are required.
- No DirectXTex DLLs are required.
- Assimp requires `zlibstatic(.lib)` to be linked alongside the Assimp library. Missing zlib linkage will result in unresolved symbols such as:

```text
inflate
inflateEnd
inflateInit2_
crc32
uncompress
```