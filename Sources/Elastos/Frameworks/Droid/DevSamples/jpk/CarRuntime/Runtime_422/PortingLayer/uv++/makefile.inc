build_src:
	CFLAGS="$(shell echo $(C_FLAGS) | sed -e 's@-c@@g')" \
	CXXFLAGS="$(shell echo $(C_FLAGS) | sed -e 's@-c@@g') $(shell echo $(CPP_FLAGS) | sed -e 's@-c@@g')" \
	LDFLAGS="$(shell echo $(DLL_FLAGS) | sed -e 's@-shared\|,-fpic@@g') -L$(XDK_TARGETS)" \
	CC="$(CC)" \
	CXX="$(CXX)" \
	cmake "$(MAKEDIR)"
	make
	cp "$(MAKEDIR)"/src/*h "$(XDK_USER_INC)"
	cp -a libuv++.so "$(XDK_TARGETS)"