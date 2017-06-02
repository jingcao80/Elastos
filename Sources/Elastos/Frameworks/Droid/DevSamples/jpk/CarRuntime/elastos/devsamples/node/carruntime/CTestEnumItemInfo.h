
#ifndef __CTESTENUMITEMINFO_H__
#define __CTESTENUMITEMINFO_H__

#include "_Elastos_DevSamples_Node_CarRuntime_CTestEnumItemInfo.h"

#include <elastos/droid/ext/frameworkdef.h>
#include <elastos/core/Object.h>

#include <cutils/log.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CarClass(CTestEnumItemInfo)
    , public Object
    , public ITestEnumItemInfo
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetName(
        /* [out] */ String * pName);

    CARAPI GetEnumInfo(
        /* [out] */ ITestEnumInfo ** ppEnumInfo);

    CARAPI GetValue(
        /* [out] */ Int32 * pValue);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IEnumItemInfo * pEnumItemInfo);

    CARAPI GetInternalObject(
        /* [out] */ PInterface* ppObject);

private:
    AutoPtr<IEnumItemInfo> mEnumItemInfo;
};

}
}
}
}

#endif // __CTESTENUMITEMINFO_H__
