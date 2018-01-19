
export XDK_TARGET_PRODUCT=elastos
export XDK_TARGET_PLATFORM=android
export XDK_TARGET_CPU=aarch64
export XDK_COMPILER=clang
export XDK_COMPILER_VERSION=4.0
export XDK_TARGET_FORMAT=elf
export XDK_TARGET_BOARD=victara
export XDK_COMPILER_PATH=$XDK_BUILD_PATH/Compilers/clang
export GCC_TOOLCHAIN=$XDK_BUILD_PATH/Compilers/gcc/aarch64/aarch64-linux-android-4.9/aarch64-linux-android
export EMULATOR_PATH=$XDK_BUILD_PATH/Emulator/android_sdk
export PATH=$XDK_COMPILER_PATH/bin:$GCC_TOOLCHAIN/bin:$EMULATOR_PATH/tools:$EMULATOR_PATH/platform-tools:$PATH:
