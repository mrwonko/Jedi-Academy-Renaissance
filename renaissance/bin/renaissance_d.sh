#!/bin/bash

FILEDIR=$(dirname $0)
export LD_LIBRARY_PATH=$FILEDIR/
$FILEDIR/renaissance_d $@
