
#ifndef __CTESTDATATYPEINFO_H__
#define __CTESTDATATYPEINFO_H__

#include "_Elastos_DevSamples_Node_CarRuntime_CTestDataTypeInfo.h"

#include <elastos/droid/ext/frameworkdef.h>
#include <elastos/core/Object.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CarClass(CTestDataTypeInfo)
    , public Object
    , public ITestDataTypeInfo
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

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IDataTypeInfo * pDataTypeInfo);

private:
    // TODO: Add your private member variables here.
    IDataTypeInfo* mDataTypeInfo;
};

}
}
}
}

#endif // __CTESTDATATYPEINFO_H__
