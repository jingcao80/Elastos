
export XDK_TARGET_PRODUCT=elastos
export XDK_TARGET_PLATFORM=android
export XDK_TARGET_CPU=arm
export XDK_COMPILER=clang
export XDK_COMPILER_VERSION=4.0
export XDK_TARGET_FORMAT=elf
export XDK_TARGET_BOARD=victara
export XDK_COMPILER_PATH=$XDK_BUILD_PATH/Compilers/clang
export GCC_TOOLCHAIN=$XDK_BUILD_PATH/Compilers/gcc/arm-linux-androideabi
export EMULATOR_PATH=$XDK_BUILD_PATH/Emulator/android_sdk
export PATH=$XDK_COMPILER_PATH/bin:$GCC_TOOLCHAIN/bin:$EMULATOR_PATH/tools:$EMULATOR_PATH/platform-tools:$PATH:
