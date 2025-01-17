#!/bin/bash

set -e

: "${CMAKE_PREFIX_PATH:="/opt/switchfin"}"
export PKG_CONFIG_PATH=$CMAKE_PREFIX_PATH/lib/pkgconfig
export LD_LIBRARY_PATH=$CMAKE_PREFIX_PATH/lib

wget -qO- https://curl.se/download/curl-8.7.1.tar.xz | tar Jxf - -C /tmp
wget -qO- https://downloads.videolan.org/pub/videolan/dav1d/1.4.1/dav1d-1.4.1.tar.xz | tar Jxf - -C /tmp
wget -qO- https://ffmpeg.org/releases/ffmpeg-6.1.1.tar.xz | tar Jxf - -C /tmp
wget -qO- https://github.com/mpv-player/mpv/archive/v0.36.0.tar.gz | tar zxf - -C /tmp
git clone https://github.com/dragonflylee/glfw.git -b switchfin --depth=1 /tmp/glfw
git clone https://git.videolan.org/git/ffmpeg/nv-codec-headers.git -b n12.1.14.0 --depth=1 /tmp/nv-codec-headers

cd /tmp/curl-8.7.1
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$CMAKE_PREFIX_PATH \
  -DCMAKE_INSTALL_RPATH=$CMAKE_PREFIX_PATH/lib -DBUILD_SHARED_LIBS=ON -DCURL_USE_OPENSSL=ON \
  -DHTTP_ONLY=ON -DCURL_DISABLE_PROGRESS_METER=ON -DBUILD_CURL_EXE=OFF -DBUILD_TESTING=OFF \
  -DUSE_LIBIDN2=OFF -DCURL_USE_LIBSSH2=OFF -DCURL_USE_LIBPSL=OFF -DBUILD_LIBCURL_DOCS=OFF
cmake --build build
cmake --install build --strip

cd /tmp/dav1d-1.4.1
meson setup build --prefix=$CMAKE_PREFIX_PATH --libdir=lib --buildtype=release --default-library=shared \
  -Denable_tools=false -Denable_examples=false -Denable_tests=false -Denable_docs=false
meson compile -C build
meson install -C build

make PREFIX=$CMAKE_PREFIX_PATH -C /tmp/nv-codec-headers install

cd /tmp/ffmpeg-6.1.1
./configure --prefix=$CMAKE_PREFIX_PATH --enable-shared --disable-static \
  --ld=g++ --enable-nonfree --enable-openssl --enable-libv4l2 \
  --enable-opengl --enable-libass --disable-doc --enable-asm --enable-rpath \
  --disable-protocols --enable-protocol=file,http,tcp,udp,hls,https,tls,httpproxy \
  --disable-filters --enable-filter=hflip,vflip,transpose --disable-avdevice \
  --disable-muxers --disable-encoders --disable-programs --disable-debug
make -j$(nproc)
make install

cd /tmp/mpv-0.36.0
meson setup build --prefix=$CMAKE_PREFIX_PATH --libdir=lib --buildtype=release --default-library=shared \
  -Dlibmpv=true -Dcplayer=false -Dtests=false -Dlibarchive=disabled -Dlua=disabled
meson compile -C build
meson install -C build

cd /tmp/glfw
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$CMAKE_PREFIX_PATH \
  -DCMAKE_INSTALL_RPATH=$CMAKE_PREFIX_PATH/lib -DBUILD_SHARED_LIBS=ON -DGLFW_BUILD_WAYLAND=OFF \
  -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_DOCS=OFF
cmake --build build
cmake --install build --strip