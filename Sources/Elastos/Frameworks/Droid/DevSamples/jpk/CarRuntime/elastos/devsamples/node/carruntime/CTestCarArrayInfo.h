
#ifndef __CTESTCARARRAYINFO_H__
#define __CTESTCARARRAYINFO_H__

#include "_Elastos_DevSamples_Node_CarRuntime_CTestCarArrayInfo.h"

#include <elastos/droid/ext/frameworkdef.h>
#include <elastos/core/Object.h>

#include <cutils/log.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CarClass(CTestCarArrayInfo)
    , public Object
    , public ITestCarArrayInfo
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

    CARAPI GetElementTypeInfo(
        /* [out] */ ITestDataTypeInfo ** ppElementTypeInfo);

    CARAPI CreateVariable(
        /* [in] */ Int32 capacity,
        /* [out] */ ITestVariableOfCarArray ** ppVariableBox);

    CARAPI CreateVariableBox(
        /* [in] */ PCarQuintet pVariableDescriptor,
        /* [out] */ ITestVariableOfCarArray ** ppVariableBox);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ICarArrayInfo * pCarArrayInfo);

    CARAPI GetInternalObject(
        /* [out] */ PInterface* ppObject);

private:
    AutoPtr<ICarArrayInfo> mCarArrayInfo;
};

}
}
}
}

#endif // __CTESTCARARRAYINFO_H__
