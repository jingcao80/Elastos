
#ifndef __CANNOTATION_H__
#define __CANNOTATION_H__

#include "_CAnnotation.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

CarClass(CAnnotation)
    , public Object
    , public IAnnotation
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 data);

    CARAPI M1();

    CARAPI M2();

    CARAPI M3();

    CARAPI M4();

    CARAPI M5();

private:
    // TODO: Add your private member variables here.
};


#endif // __CANNOTATION_H__
