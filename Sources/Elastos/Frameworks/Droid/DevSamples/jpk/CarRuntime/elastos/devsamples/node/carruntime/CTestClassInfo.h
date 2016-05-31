
#ifndef __CTESTCLASSINFO_H__
#define __CTESTCLASSINFO_H__

#include "_Elastos_DevSamples_Node_CarRuntime_CTestClassInfo.h"

#include <elastos/droid/ext/frameworkdef.h>
#include <elastos/core/Object.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CarClass(CTestClassInfo)
    , public Object
    , public ITestClassInfo
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetName(
        /* [out] */ String * pName);

    CARAPI GetNamespace(
        /* [out] */ String * pNs);

    CARAPI GetId(
        /* [out] */ ClassID * pClsid);

    CARAPI GetModuleInfo(
        /* [out] */ ITestModuleInfo ** ppModuleInfo);

    CARAPI IsSingleton(
        /* [out] */ Boolean * pIsSingleton);

    CARAPI GetThreadingModel(
        /* [out] */ ThreadingModel * pThreadingModel);

    CARAPI IsPrivate(
        /* [out] */ Boolean * pIsPrivate);

    CARAPI IsReturnValue(
        /* [out] */ Boolean * pReturnValue);

    CARAPI IsBaseClass(
        /* [out] */ Boolean * pIsBaseClass);

    CARAPI HasBaseClass(
        /* [out] */ Boolean * pHasBaseClass);

    CARAPI GetBaseClassInfo(
        /* [out] */ ITestClassInfo ** ppBaseClassInfo);

    CARAPI IsGeneric(
        /* [out] */ Boolean * pIsGeneric);

    CARAPI HasGeneric(
        /* [out] */ Boolean * pHasGeneric);

    CARAPI GetGenericInfo(
        /* [out] */ ITestClassInfo ** ppGenericInfo);

    CARAPI IsRegime(
        /* [out] */ Boolean * pIsRegime);

    CARAPI GetAspectCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllAspectInfos(
        /* [out] */ ArrayOf<ITestClassInfo *> ** ppAspectInfos);

    CARAPI GetAspectInfo(
        /* [in] */ const String& name,
        /* [out] */ ITestClassInfo ** ppAspectInfo);

    CARAPI IsAspect(
        /* [out] */ Boolean * pIsAspect);

    CARAPI GetAggregateeCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllAggregateeInfos(
        /* [out] */ ArrayOf<ITestClassInfo *> ** ppAggregateeInfos);

    CARAPI GetAggregateeInfo(
        /* [in] */ const String& name,
        /* [out] */ ITestClassInfo ** ppAggregateeInfo);

    CARAPI GetConstructorCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllConstructorInfos(
        /* [out] */ ArrayOf<ITestConstructorInfo *> ** ppConstructorInfos);

    CARAPI GetConstructorInfoByParamNames(
        /* [in] */ const String& name,
        /* [out] */ ITestConstructorInfo ** ppConstructorInfo);

    CARAPI GetConstructorInfoByParamCount(
        /* [in] */ Int32 count,
        /* [out] */ ITestConstructorInfo ** ppConstructorInfo);

    CARAPI GetInterfaceCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllInterfaceInfos(
        /* [out] */ ArrayOf<ITestInterfaceInfo *> ** ppInterfaceInfos);

    CARAPI GetInterfaceInfo(
        /* [in] */ const String& fullName,
        /* [out] */ ITestInterfaceInfo ** ppInterfaceInfo);

    CARAPI GetCallbackInterfaceCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllCallbackInterfaceInfos(
        /* [out] */ ArrayOf<ITestInterfaceInfo *> ** ppCallbackInterfaceInfos);

    CARAPI GetCallbackInterfaceInfo(
        /* [in] */ const String& name,
        /* [out] */ ITestInterfaceInfo ** ppCallbackInterfaceInfo);

    CARAPI GetMethodCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllMethodInfos(
        /* [out] */ ArrayOf<ITestMethodInfo *> ** ppMethodInfos);

    CARAPI GetMethodInfo(
        /* [in] */ const String& name,
        /* [in] */ const String& signature,
        /* [out] */ ITestMethodInfo ** ppMethodInfo);

    CARAPI GetCallbackMethodCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllCallbackMethodInfos(
        /* [out] */ ArrayOf<ITestCallbackMethodInfo *> ** ppCallbackMethodInfos);

    CARAPI GetCallbackMethodInfo(
        /* [in] */ const String& name,
        /* [out] */ ITestCallbackMethodInfo ** ppCallbackMethodInfo);

    CARAPI RemoveAllCallbackHandlers(
        /* [in] */ PInterface pServer);

    CARAPI CreateObject(
        /* [out] */ PInterface * ppObject);

    CARAPI CreateObjectInRegime(
        /* [in] */ PRegime pRgm,
        /* [out] */ PInterface * ppObject);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IClassInfo * pClassInfo);

private:
    AutoPtr<IClassInfo> mClassInfo;
};

}
}
}
}

#endif // __CTESTCLASSINFO_H__
