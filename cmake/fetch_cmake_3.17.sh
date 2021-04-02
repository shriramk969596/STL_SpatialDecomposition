#!/bin/bash

mkdir -p $ESS_THIRDPARTY_DIR
if [ ! -d $ESS_THIRDPARTY_DIR/cmake-3.17.2 ]
then
    cd $ESS_THIRDPARTY_DIR
    git archive --format=tar.gz --remote ssh://git@gitlab.essteyr.com:2222/Dependencies/cmake HEAD:cmake-3.17.2-Linux-x86_64 | tar xzBf - 
    cd -
fi

export PATH=$ESS_THIRDPARTY_DIR/cmake-3.17.2/bin:$PATH 

