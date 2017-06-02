
#ifndef __CTESTSTRUCTINFO_H__
#define __CTESTSTRUCTINFO_H__

#include "_Elastos_DevSamples_Node_CarRuntime_CTestStructInfo.h"

#include <elastos/droid/ext/frameworkdef.h>
#include <elastos/core/Object.h>

#include <cutils/log.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CarClass(CTestStructInfo)
    , public Object
    , public ITestStructInfo
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

    CARAPI GetModuleInfo(
        /* [out] */ ITestModuleInfo ** ppModuleInfo);

    CARAPI GetFieldCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllFieldInfos(
        /* [out] */ ArrayOf<ITestFieldInfo *> ** ppFieldInfos);

    CARAPI GetFieldInfo(
        /* [in] */ const String& name,
        /* [out] */ ITestFieldInfo ** ppFieldInfo);

    CARAPI CreateVariable(
        /* [out] */ ITestVariableOfStruct ** ppVariableBox);

    CARAPI CreateVariableBox(
        /* [in] */ PCarQuintet pVariableDescriptor,
        /* [out] */ ITestVariableOfStruct ** ppVariableBox);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IStructInfo * pStructInfo);

    CARAPI GetInternalObject(
        /* [out] */ PInterface* ppObject);

private:
    AutoPtr<IStructInfo> mStructInfo;
};

}
}
}
}

#endif // __CTESTSTRUCTINFO_H__
