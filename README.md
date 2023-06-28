# Switchfin

<img src="resources/icon/icon.svg" alt="icon" height="128" width="128" align="left">

Switchfin is third-party PC player for Jellyfin that provides a native user interface to browse and play movies and series.
<br>

[![build](https://github.com/dragonflylee/switchfin/actions/workflows/build.yaml/badge.svg)](https://github.com/dragonflylee/switchfin/actions/workflows/build.yaml) [![release](https://img.shields.io/github/release/dragonflylee/switchfin)](https://github.com/dragonflylee/switchfin/releases) ![download](https://img.shields.io/github/downloads/dragonflylee/switchfin/total) ![NS](https://img.shields.io/badge/-Nintendo%20Switch-e4000f?style=flat&logo=Nintendo%20Switch) ![MS](https://img.shields.io/badge/-Windows%2010-357ec7?style=flat&logo=Windows) ![mac](https://img.shields.io/badge/-macOS%2010.15-black?style=flat&logo=Apple) ![Linux](https://img.shields.io/badge/-Linux-lightgrey?style=flat&logo=Linux)

**This project is in its early stages so expect bugs.**


## Features
- Completely native interface
- Supported media items: movies, series, seasons, episodes 
  - Direct play only, (no transcoding)
- Base on MPV Player
  - Container formats: mkv, mov, mp4, avi
  - Video codecs: H.264, H.265, VP8, VP9, AV1
  - Audio codecs: Opus, FLAC, MP3, AAC, AC-3, E-AC-3, TrueHD, DTS, DTS-HD
  - Subtitle codecs: SRT, VTT, SSA/ASS, DVDSUB
  - Optionally force software decoding when hardware decoding has issues.

## TODO list

- [ ] Movie view
- [ ] Series detail
- [ ] Search page
- [ ] Websocket connection (Syncplay)
- [ ] [danmu plugin](https://github.com/cxfksword/jellyfin-plugin-danmu) integration

## Develop

```shell
git clone https://github.com/dragonflylee/switchfin
cd switchfin
git submodule update --init --recursive --depth 1
```

### Building for Switch

To build for Switch, a standard development environment must first be set up. In order to do so, [refer to the Getting Started guide](https://devkitpro.org/wiki/Getting_Started).

```bash
sudo dkp-pacman -S switch-glfw switch-libwebp switch-cmake switch-curl devkitA64
cmake -B build_switch -DPLATFORM_SWITCH=ON
make -C build_switch Switchfin.nro -j$(nproc)
```

### Building for MinGW64

```bash
cmake -B build_pc -G 'MinGW Makefiles' -DPLATFORM_DESKTOP=ON
mingw32-make -C build_pc -j$(nproc)
```

## Thanks to

- devkitpro and switchbrew for [libnx](https://github.com/switchbrew/libnx)
- natinusala and XITRIX for [borealis](https://github.com/natinusala/borealis)
- xfangfang for [wiliwili](https://github.com/xfangfang/wiliwili)