
#ifndef __CTESTINTRINSICINFO_H__
#define __CTESTINTRINSICINFO_H__

#include "_Elastos_DevSamples_Node_CarRuntime_CTestIntrinsicInfo.h"

#include <elastos/droid/ext/frameworkdef.h>
#include <elastos/core/Object.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CarClass(CTestIntrinsicInfo)
    , public Object
    , public ITestIntrinsicInfo
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

    CARAPI DoNothing();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IIntrinsicInfo * pIntrinsicInfo);

private:
    IIntrinsicInfo* mIntrinsicInfo;
};

}
}
}
}

#endif // __CTESTINTRINSICINFO_H__
