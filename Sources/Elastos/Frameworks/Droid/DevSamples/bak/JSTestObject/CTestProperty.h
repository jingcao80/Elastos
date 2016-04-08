
#ifndef __CTESTPROPERTY_H__
#define __CTESTPROPERTY_H__

#include "_CTestProperty.h"

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace JSTestObject {

CarClass(CTestProperty)
{
public:
    CARAPI GetProperty(
        /* [out] */ IInterface ** ppProperty);

    CARAPI SetProperty(
        /* [in] */ IInterface * pProperty);

    CARAPI TestMethod_00();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IInterface * pProperty);

private:
    StructTestProperty* mProperty;
};

}
}
}
}

#endif // __CTESTPROPERTY_H__
