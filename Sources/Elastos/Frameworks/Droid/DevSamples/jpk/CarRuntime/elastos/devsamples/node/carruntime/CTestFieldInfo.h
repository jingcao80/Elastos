
#ifndef __CTESTFIELDINFO_H__
#define __CTESTFIELDINFO_H__

#include "_Elastos_DevSamples_Node_CarRuntime_CTestFieldInfo.h"

#include <elastos/droid/ext/frameworkdef.h>
#include <elastos/core/Object.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CarClass(CTestFieldInfo)
    , public Object
    , public ITestFieldInfo
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetName(
        /* [out] */ String * pName);

    CARAPI GetTypeInfo(
        /* [out] */ ITestDataTypeInfo ** ppTypeInfo);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IFieldInfo * pFieldInfo);

    CARAPI GetInternalObject(
        /* [out] */ PInterface* ppObject);

private:
    AutoPtr<IFieldInfo> mFieldInfo;
};

}
}
}
}

#endif // __CTESTFIELDINFO_H__
