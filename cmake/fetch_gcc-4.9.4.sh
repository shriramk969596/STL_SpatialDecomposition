#!/bin/bash


#DISTRO=Ubuntu14.04_x86_64 
DISTRO=$1_x86_64 
VERSION=4.9.4

PACKAGES_HOME=$ESS_THIRDPARTY_DIR/$DISTRO

fetch_package (){
    
    PACKAGE=$1
    VERSION=$2

    echo "fetching $PACKAGE $VERSION"
    
    if [ ! -d $PACKAGES_HOME/$PACKAGE-$VERSION ]
    then
        echo "  downloading..."
        mkdir -p $PACKAGES_HOME/$PACKAGE-$VERSION
        cd $PACKAGES_HOME/$PACKAGE-$VERSION
        git archive --format=tar.gz --remote ssh://git@gitlab.essteyr.com:2222/Dependencies/$PACKAGE HEAD:$VERSION/$DISTRO | tar xzBf - 
        cd - > /dev/null
    fi


    export LD_LIBRARY_PATH=$PACKAGES_HOME/$PACKAGE-$VERSION/lib:$LD_LIBRARY_PATH
    export LD_LIBRARY_PATH=$PACKAGES_HOME/$PACKAGE-$VERSION/lib64:$LD_LIBRARY_PATH
}


fetch_package gmp  6.1.2
fetch_package mpfr 4.0.0
fetch_package mpc  1.1.0
fetch_package gcc  4.9.4


export PATH=$PACKAGES_HOME/gcc-4.9.4/bin:$PATH

gcc --version

echo done!
