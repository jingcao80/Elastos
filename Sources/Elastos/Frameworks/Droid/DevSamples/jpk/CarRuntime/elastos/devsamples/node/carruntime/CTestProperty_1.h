
#ifndef __CTESTPROPERTY_H__
#define __CTESTPROPERTY_H__

#include "_Elastos_DevSamples_Node_CarRuntime_CTestProperty.h"

#include <elastos/droid/ext/frameworkdef.h>
#include <elastos/core/Object.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CarClass(CTestProperty)
    , public Object
    , public IProperty
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetProperty(
        /* [out] */ IInterface ** ppProperty);

    CARAPI SetProperty(
        /* [in] */ IInterface * pProperty);

    CARAPI TestMethod_00();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IInterface * pProperty);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}

#endif // __CTESTPROPERTY_H__
