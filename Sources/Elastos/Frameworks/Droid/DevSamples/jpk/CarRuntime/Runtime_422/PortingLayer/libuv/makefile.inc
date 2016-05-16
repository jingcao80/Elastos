build_src:
	CFLAGS="$(shell echo $(C_FLAGS) | sed -e 's@-c@@g')" \
	LDFLAGS="$(shell echo $(DLL_FLAGS) | sed -e 's@-shared\|,-fpic@@g')" \
	CC="$(CC)" \
	"$(MAKEDIR)"/configure --libdir="$(XDK_TARGETS)" \
						   --includedir="$(XDK_USER_INC)" \
                           --disable-dtrace \
						   --enable-shared \
						   --disable-static
	make
	make install
