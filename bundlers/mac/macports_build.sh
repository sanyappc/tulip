#!/bin/bash

# set colums in terminal, required by curl
export COLUMNS=80

# first install MacPorts in order to easily retrieve Tulip dependencies
curl -LO https://raw.githubusercontent.com/GiovanniBussi/macports-ci/master/macports-ci
bash ./macports-ci install

# configure PATH with macports binaries installation dir
export PATH=/opt/local/bin:$PATH

# configure according clang compiler version (9.0, 10, ...)
if [ "$CLANG_COMPILER_VERSION" != "" ]; then
  CLANG_COMPILER_PKG=clang-${CLANG_COMPILER_VERSION}
  CLANG_COMPILER_DEFINES="-DCMAKE_C_COMPILER=/opt/local/bin/clang-mp-${CLANG_COMPILER_VERSION} -DCMAKE_CXX_COMPILER=/opt/local/bin/clang++-mp-${CLANG_COMPILER_VERSION}"
fi

# check if ccache is needed
if [ "$TULIP_USE_CCACHE" == "ON" ]; then
  CCACHE_PKG=ccache
fi

# install Tulip core build dependencies
sudo port -N install cmake $CLANG_COMPILER_PKG $CCACHE_PKG qhull

# check for more dependencies
if [ "$TRAVIS_BUILD_THIRDPARTY_ONLY" != "ON" ]; then
# install Tulip tests build dependencies
  if [ "$TULIP_BUILD_TESTS" == "ON" ]; then
    sudo port -N install pkgconfig cppunit
  fi
  PYTHON_DEFINE=-DPYTHON_EXECUTABLE=/usr/bin/python2.7
# install Tulip complete build dependencies
  if [ "$TULIP_BUILD_CORE_ONLY" != "ON" ]; then
    sudo port -N install qt5-qtbase qt5-qttools qt5-qtwebkit quazip freetype glew
    curl -O https://bootstrap.pypa.io/get-pip.py
    sudo python get-pip.py
    pip install --user sphinx==1.7.9
    TULIP_GUI_DEFINES="-DZLIB_INCLUDE_DIR=/opt/local/include -DZLIB_LIBRARY_RELEASE=/opt/local/lib/libz.dylib -DGLEW_SHARED_LIBRARY_RELEASE=/opt/local/lib/libGLEW.dylib"
  fi
fi

# create build directory
mkdir build && cd build

# configure Tulip build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$PWD/install ${CLANG_COMPILER_DEFINES} ${PYTHON_DEFINE} -DTRAVIS_BUILD_THIRDPARTY_ONLY=${TRAVIS_BUILD_THIRDPARTY_ONLY} -DTULIP_BUILD_CORE_ONLY=${TULIP_BUILD_CORE_ONLY} -DTULIP_BUILD_DOC=${TULIP_BUILD_DOC} -DTULIP_BUILD_TESTS=${TULIP_BUILD_TESTS} -DTULIP_USE_CCACHE=$TULIP_USE_CCACHE ${TULIP_GUI_DEFINES}
# compile Tulip
make -j$(getconf _NPROCESSORS_ONLN) install

if [ "$TRAVIS_BUILD_THIRDPARTY_ONLY" != "ON" ]; then
# run Tulip unit tests
  if [ "$TULIP_BUILD_TESTS" == "ON" ]; then
    make test
  fi
# build Tulip bundle
  if [ "$TULIP_MAKE_BUNDLE" == "ON" ]; then
    make bundle
  fi
fi
