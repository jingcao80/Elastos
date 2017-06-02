
#ifndef __CTESTCONSTRUCTORINFO_H__
#define __CTESTCONSTRUCTORINFO_H__

#include "_Elastos_DevSamples_Node_CarRuntime_CTestConstructorInfo.h"

#include <elastos/droid/ext/frameworkdef.h>
#include <elastos/core/Object.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CarClass(CTestConstructorInfo)
    , public Object
    , public ITestConstructorInfo
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetName(
        /* [out] */ String * pName);

    //CARAPI GetAnnotation(
    //    /* [out] */ String * pAnnotation);

    CARAPI GetParamCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllParamInfos(
        /* [out] */ ArrayOf<ITestParamInfo *> ** ppParamInfos);

    CARAPI GetParamInfoByIndex(
        /* [in] */ Int32 index,
        /* [out] */ ITestParamInfo ** ppParamInfo);

    CARAPI GetParamInfoByName(
        /* [in] */ const String& name,
        /* [out] */ ITestParamInfo ** ppParamInfo);

    CARAPI CreateArgumentList(
        /* [out] */ ITestArgumentList ** ppArgumentList);

    CARAPI CreateObject(
        /* [in] */ ITestArgumentList * pArgumentList,
        /* [out] */ PInterface * ppObject);

    CARAPI LocalCreateObject(
        /* [in] */ ITestArgumentList * pArgumentList,
        /* [out] */ PInterface * ppObject);

    CARAPI RemoteCreateObject(
        /* [in] */ ITestArgumentList * pArgumentList,
        /* [out] */ PInterface * ppObject);

    CARAPI CreateObjectInRegime(
        /* [in] */ PRegime pRgm,
        /* [in] */ ITestArgumentList * pArgumentList,
        /* [out] */ PInterface * ppObject);

    CARAPI GetClassInfo(
        /* [out] */ ITestClassInfo ** ppClassInfo);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IConstructorInfo * pConstructorInfo);

    CARAPI GetInternalObject(
        /* [out] */ PInterface* ppObject);

private:
    AutoPtr<IConstructorInfo> mConstructorInfo;
};

}
}
}
}

#endif // __CTESTCONSTRUCTORINFO_H__
