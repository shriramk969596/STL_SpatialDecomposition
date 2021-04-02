#!/bin/bash


# this script is a different method to fetch dependecies stored in the dependecies repository
# no cmake just bash



TO=$1
PACKAGE_NAME=$2
PACKAGE_VER=$3
PLATFORM=$4

TMPDIR=$(dirname $(mktemp -u))
REPO=ssh://git@gitlab.essteyr.com:2222/Dependencies 


git archive --format=tar.gz --remote $REPO/$PACKAGE_NAME HEAD:$PACKAGE_VER/$PLATFORM -o $TMPDIR/$PACKAGE_NAME-$PACKAGE_VER.tar.gz

mkdir -p $TO
cd $TO
tar xf $TMPDIR/$PACKAGE_NAME-$PACKAGE_VER.tar.gz
rm $TMPDIR/$PACKAGE_NAME-$PACKAGE_VER.tar.gz


