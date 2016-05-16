
#ifndef __CTESTCPPVECTORINFO_H__
#define __CTESTCPPVECTORINFO_H__

#include "_Elastos_DevSamples_Node_CarRuntime_CTestCppVectorInfo.h"

#include <elastos/droid/ext/frameworkdef.h>
#include <elastos/core/Object.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CarClass(CTestCppVectorInfo)
    , public Object
    , public ITestCppVectorInfo
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

    CARAPI GetLength(
        /* [out] */ Int32 * pLength);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ICppVectorInfo * pCppVectorInfo);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}

#endif // __CTESTCPPVECTORINFO_H__
