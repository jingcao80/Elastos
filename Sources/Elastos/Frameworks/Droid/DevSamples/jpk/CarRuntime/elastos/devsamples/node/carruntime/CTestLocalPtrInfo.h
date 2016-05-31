
#ifndef __CTESTLOCALPTRINFO_H__
#define __CTESTLOCALPTRINFO_H__

#include "_Elastos_DevSamples_Node_CarRuntime_CTestLocalPtrInfo.h"

#include <elastos/droid/ext/frameworkdef.h>
#include <elastos/core/Object.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CarClass(CTestLocalPtrInfo)
    , public Object
    , public ITestLocalPtrInfo
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetName(
        /* [out] */ String * pName);

    CARAPI GetSize(
        /* [out] */ MemorySize * pSize);

    CARAPI GetDataType(
        /* [out] */ CarDataType * pDataType);

    CARAPI GetTargetTypeInfo(
        /* [out] */ ITestDataTypeInfo ** ppDateTypeInfo);

    CARAPI GetPtrLevel(
        /* [out] */ Int32 * pLevel);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ILocalPtrInfo * pLocalPtrInfo);

private:
    ILocalPtrInfo* mLocalPtrInfo;
};

}
}
}
}

#endif // __CTESTLOCALPTRINFO_H__
