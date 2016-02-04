
#ifndef __CDUMMY_H__
#define __CDUMMY_H__

#include "_CDummy.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

CarClass(CDummy)
    , public Object
    , public IConstsDef
{
public:
    CAR_INTERFACE_DECL()

    CARAPI Read();

private:
    // TODO: Add your private member variables here.
};


#endif // __CDUMMY_H__
