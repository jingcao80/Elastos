
export XDK_TARGET_PRODUCT=elastos
export XDK_TARGET_PLATFORM=android
export XDK_TARGET_CPU=arm
export XDK_COMPILER=gcc
export XDK_COMPILER_VERSION=4.8
export XDK_TARGET_FORMAT=elf
export XDK_TARGET_BOARD=victara
export CROSS_COMPILE=arm-linux-androideabi-
export XDK_COMPILER_PATH=$XDK_BUILD_PATH/Compilers/gcc
export EMULATOR_PATH=$XDK_BUILD_PATH/Emulator/android_sdk
export PATH=$XDK_COMPILER_PATH/bin:$EMULATOR_PATH/tools:$EMULATOR_PATH/platform-tools:$PATH:
