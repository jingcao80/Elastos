
#ifndef __CTESTCLASSINFO_H__
#define __CTESTCLASSINFO_H__

#include "_CTestClassInfo.h"

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace JSTestObject {

CarClass(CTestClassInfo)
{
public:
    CARAPI GetName(
        /* [out] */ StringBuf * pName);

    CARAPI GetId(
        /* [out] */ ClassID * pClsid);

    CARAPI GetModuleInfo(
        /* [out] */ IModuleInfo ** ppModuleInfo);

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
        /* [out] */ IClassInfo ** ppBaseClassInfo);

    CARAPI IsGeneric(
        /* [out] */ Boolean * pIsGeneric);

    CARAPI HasGeneric(
        /* [out] */ Boolean * pHasGeneric);

    CARAPI GetGenericInfo(
        /* [out] */ IClassInfo ** ppGenericInfo);

    CARAPI IsRegime(
        /* [out] */ Boolean * pIsRegime);

    CARAPI GetAspectCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllAspectInfos(
        /* [out] */ BufferOf<IClassInfo *> * pAspectInfos);

    CARAPI GetAspectInfo(
        /* [in] */ CString name,
        /* [out] */ IClassInfo ** ppAspectInfo);

    CARAPI IsAspect(
        /* [out] */ Boolean * pIsAspect);

    CARAPI GetAggregateeCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllAggregateeInfos(
        /* [out] */ BufferOf<IClassInfo *> * pAggregateeInfos);

    CARAPI GetAggregateeInfo(
        /* [in] */ CString name,
        /* [out] */ IClassInfo ** ppAggregateeInfo);

    CARAPI GetConstructorCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllConstructorInfos(
        /* [out] */ BufferOf<IConstructorInfo *> * pConstructorInfos);

    CARAPI GetConstructorInfoByParamNames(
        /* [in] */ CString name,
        /* [out] */ IConstructorInfo ** ppConstructorInfo);

    CARAPI GetConstructorInfoByParamCount(
        /* [in] */ Int32 count,
        /* [out] */ IConstructorInfo ** ppConstructorInfo);

    CARAPI GetInterfaceCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllInterfaceInfos(
        /* [out] */ BufferOf<IInterfaceInfo *> * pInterfaceInfos);

    CARAPI GetInterfaceInfo(
        /* [in] */ CString name,
        /* [out] */ IInterfaceInfo ** ppInterfaceInfo);

    CARAPI GetCallbackInterfaceCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllCallbackInterfaceInfos(
        /* [out] */ BufferOf<IInterfaceInfo *> * pCallbackInterfaceInfos);

    CARAPI GetCallbackInterfaceInfo(
        /* [in] */ CString name,
        /* [out] */ IInterfaceInfo ** ppCallbackInterfaceInfo);

    CARAPI GetMethodCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllMethodInfos(
        /* [out] */ BufferOf<IMethodInfo *> * pMethodInfos);

    CARAPI GetMethodInfo(
        /* [in] */ CString name,
        /* [out] */ IMethodInfo ** ppMethodInfo);

    CARAPI GetCallbackMethodCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllCallbackMethodInfos(
        /* [out] */ BufferOf<ICallbackMethodInfo *> * pCallbackMethodInfos);

    CARAPI GetCallbackMethodInfo(
        /* [in] */ CString name,
        /* [out] */ ICallbackMethodInfo ** ppCallbackMethodInfo);

    CARAPI RemoveAllCallbackHandlers(
        /* [in] */ PInterface pServer);

    CARAPI CreateObject(
        /* [out] */ PInterface * ppObject);

    CARAPI CreateObjectInRegime(
        /* [in] */ PRegime pRgm,
        /* [out] */ PInterface * ppObject);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IClassInfo * pPClassInfo);

private:
    IClassInfo* classInfo;
};

}
}
}
}

#endif // __CTESTCLASSINFO_H__
