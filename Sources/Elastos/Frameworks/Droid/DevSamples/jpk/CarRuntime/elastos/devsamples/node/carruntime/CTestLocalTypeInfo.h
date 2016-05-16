
#ifndef __CTESTLOCALTYPEINFO_H__
#define __CTESTLOCALTYPEINFO_H__

#include "_Elastos_DevSamples_Node_CarRuntime_CTestLocalTypeInfo.h"

#include <elastos/droid/ext/frameworkdef.h>
#include <elastos/core/Object.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CarClass(CTestLocalTypeInfo)
    , public Object
    , public ITestLocalTypeInfo
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
        /* [in] */ ILocalTypeInfo * pLocalTypeInfo);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}

#endif // __CTESTLOCALTYPEINFO_H__
