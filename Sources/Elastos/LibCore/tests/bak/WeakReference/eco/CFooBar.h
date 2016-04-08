
#ifndef __CFOOBAR_H__
#define __CFOOBAR_H__

#include "_CFooBar.h"
#include "FooBarImpl.h"
#include <elautoptr.h>

CarClass(CFooBar)
{
public:
    CARAPI Foo();

    CARAPI Bar();

    CARAPI FooBar();
};

#endif //__CFOOBAR_H__
