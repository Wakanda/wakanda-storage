#!/bin/sh

#create dependencies dest folder
if ! test -e ./../deps
then mkdir ./../deps
fi

#download boost library archive
if ! test -e ./../deps/boost.tar.gz
then echo "Downloading boost library archive" && curl -sLo ./../deps/boost.tar.gz https://dl.bintray.com/boostorg/release/1.68.0/source/boost_1_68_0.tar.gz
fi

#create boost dest folder
if ! test -e ./../deps/boost
then mkdir ./../deps/boost
fi

#extract boost library sources
if ! test -e ./../deps/boost/boost && test -e ./../deps/boost.tar.gz
then echo "Extracting boost library" && tar -xzf ./../deps/boost.tar.gz -C ./../deps/boost --strip-components=1
fi

#create catch2 dest folder
if ! test -e ./../deps/catch2
then mkdir ./../deps/catch2
fi

#download catch2 header
if ! test -e ./../deps/catch2/catch.hpp
then echo "Downloading catch2 header" && curl -sLo ./../deps/catch2/catch.hpp https://github.com/catchorg/Catch2/releases/download/v2.4.2/catch.hpp
fi