#!/bin/sh

dirBuild="build"
dirProject=$(pwd)
dirSrcVM="src/emptyVM"
dirTests="test/tests"
dirSrcBuild="$dirProject/$dirBuild/$dirSrcVM"
dirTestBuild="$dirProject/$dirBuild/$dirTests"
vm="emptyVM"
testsOnMacros="tests"

mkdir $dirBuild &&
cd $dirBuild &&
cmake .. &&
make

$dirSrcBuild
$dirTestBuild

cd $dirProject 
rm -fr $dirBuild
