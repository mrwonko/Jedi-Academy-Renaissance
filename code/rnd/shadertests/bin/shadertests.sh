#!/bin/sh
ScriptDir=`dirname "$0"`
echo $ScriptDir
#LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ScriptDir
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/Code/libraries/lib/
Binary=$ScriptDir/shadertests
$Binary
