
#ifndef __CTESTINTERFACEINFO_H__
#define __CTESTINTERFACEINFO_H__

#include "_Elastos_DevSamples_Node_CarRuntime_CTestInterfaceInfo.h"

#include <elastos/droid/ext/frameworkdef.h>
#include <elastos/core/Object.h>

#include <cutils/log.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CarClass(CTestInterfaceInfo)
    , public Object
    , public ITestInterfaceInfo
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

    CARAPI GetNamespace(
        /* [out] */ String * pNs);

    CARAPI GetId(
        /* [out] */ InterfaceID * pIid);

    CARAPI GetModuleInfo(
        /* [out] */ ITestModuleInfo ** ppModuleInfo);

    CARAPI IsLocal(
        /* [out] */ Boolean * pIsLocal);

    CARAPI HasBase(
        /* [out] */ Boolean * pHasBase);

    CARAPI GetBaseInfo(
        /* [out] */ ITestInterfaceInfo ** ppBaseInfo);

    CARAPI GetMethodCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllMethodInfos(
        /* [out] */ ArrayOf<ITestMethodInfo *> ** ppMethodInfos);

    CARAPI GetMethodInfo(
        /* [in] */ const String& name,
        /* [in] */ const String& signature,
        /* [out] */ ITestMethodInfo ** ppMethodInfo);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IInterfaceInfo * pInterfaceInfo);

    CARAPI GetInternalObject(
        /* [out] */ PInterface* ppObject);

private:
    AutoPtr<IInterfaceInfo> mInterfaceInfo;
};

}
}
}
}

#endif // __CTESTINTERFACEINFO_H__
