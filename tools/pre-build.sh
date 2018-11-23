#!/bin/sh

#download boost library archive
if ! test -e ./../deps/boost.tar.gz
then mkdir ./../deps && curl -Lo ./../deps/boost.tar.gz https://dl.bintray.com/boostorg/release/1.66.0/source/boost_1_66_0.tar.gz
fi

#extract boost library sources
if ! test -d ./../deps/boost
then mkdir ./../deps/boost && tar -xzf ./../deps/boost.tar.gz -C ./../deps/boost --strip-components=1
fi

#download catch2 header
if ! test -e ./../deps/catch2/catch.hpp
then mkdir ./../deps/catch2 && curl -Lo ./../deps/catch2/catch.hpp https://github.com/catchorg/Catch2/releases/download/v2.2.3/catch.hpp
fi