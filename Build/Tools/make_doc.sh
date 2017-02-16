#!/bin/bash

WORKING_DIR=$XDK_USER_OBJ/$XDK_BUILD_KIND/doc

if [ -d "$WORKING_DIR" ]; then rm -rf $WORKING_DIR; fi
mkdir -p $WORKING_DIR

#generate libcore api reference document
LIBCORE_SOURCE_PATH=$XDK_ROOT/Sources/Elastos/LibCore

