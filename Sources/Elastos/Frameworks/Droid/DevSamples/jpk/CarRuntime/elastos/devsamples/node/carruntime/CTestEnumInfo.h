
#ifndef __CTESTENUMINFO_H__
#define __CTESTENUMINFO_H__

#include "_Elastos_DevSamples_Node_CarRuntime_CTestEnumInfo.h"

#include <elastos/droid/ext/frameworkdef.h>
#include <elastos/core/Object.h>

#include <cutils/log.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CarClass(CTestEnumInfo)
    , public Object
    , public ITestEnumInfo
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

    CARAPI GetModuleInfo(
        /* [out] */ ITestModuleInfo ** ppModuleInfo);

    CARAPI GetItemCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllItemInfos(
        /* [out] */ ArrayOf<ITestEnumItemInfo *> ** ppItemInfos);

    CARAPI GetItemInfo(
        /* [in] */ const String& name,
        /* [out] */ ITestEnumItemInfo ** ppEnumItemInfo);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IEnumInfo * pEnumInfo);

    CARAPI GetInternalObject(
        /* [out] */ PInterface* ppObject);

private:
    AutoPtr<IEnumInfo> mEnumInfo;
};

}
}
}
}

#endif // __CTESTENUMINFO_H__
