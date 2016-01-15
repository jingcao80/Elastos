
#include "CTestClassInfo.h"

#include <cutils/log.h>

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace JSTestObject {


ECode CTestClassInfo::GetName(
    /* [out] */ StringBuf * pName)
{
    return classInfo->GetName(pName);
}

ECode CTestClassInfo::GetId(
    /* [out] */ ClassID * pClsid)
{
    return classInfo->GetId(pClsid);
}

ECode CTestClassInfo::GetModuleInfo(
    /* [out] */ IModuleInfo ** ppModuleInfo)
{
    return classInfo->GetModuleInfo(ppModuleInfo);
}

ECode CTestClassInfo::IsSingleton(
    /* [out] */ Boolean * pIsSingleton)
{
    return classInfo->IsSingleton(pIsSingleton);
}

ECode CTestClassInfo::GetThreadingModel(
    /* [out] */ ThreadingModel * pThreadingModel)
{
    return classInfo->GetThreadingModel(pThreadingModel);
}

ECode CTestClassInfo::IsPrivate(
    /* [out] */ Boolean * pIsPrivate)
{
    return classInfo->IsPrivate(pIsPrivate);
}

ECode CTestClassInfo::IsReturnValue(
    /* [out] */ Boolean * pReturnValue)
{
    return classInfo->IsReturnValue(pReturnValue);
}

ECode CTestClassInfo::IsBaseClass(
    /* [out] */ Boolean * pIsBaseClass)
{
    return classInfo->IsBaseClass(pIsBaseClass);
}

ECode CTestClassInfo::HasBaseClass(
    /* [out] */ Boolean * pHasBaseClass)
{
    return classInfo->HasBaseClass(pHasBaseClass);
}

ECode CTestClassInfo::GetBaseClassInfo(
    /* [out] */ IClassInfo ** ppBaseClassInfo)
{
    return classInfo->GetBaseClassInfo(ppBaseClassInfo);
}

ECode CTestClassInfo::IsGeneric(
    /* [out] */ Boolean * pIsGeneric)
{
    return classInfo->IsGeneric(pIsGeneric);
}

ECode CTestClassInfo::HasGeneric(
    /* [out] */ Boolean * pHasGeneric)
{
    return classInfo->HasGeneric(pHasGeneric);
}

ECode CTestClassInfo::GetGenericInfo(
    /* [out] */ IClassInfo ** ppGenericInfo)
{
    return classInfo->GetGenericInfo(ppGenericInfo);
}

ECode CTestClassInfo::IsRegime(
    /* [out] */ Boolean * pIsRegime)
{
    return classInfo->IsRegime(pIsRegime);
}

ECode CTestClassInfo::GetAspectCount(
    /* [out] */ Int32 * pCount)
{
    return classInfo->GetAspectCount(pCount);
}

ECode CTestClassInfo::GetAllAspectInfos(
    /* [out] */ BufferOf<IClassInfo *> * pAspectInfos)
{
    return classInfo->GetAllAspectInfos(pAspectInfos);
}

ECode CTestClassInfo::GetAspectInfo(
    /* [in] */ CString name,
    /* [out] */ IClassInfo ** ppAspectInfo)
{
    return classInfo->GetAspectInfo(name,ppAspectInfo);
}

ECode CTestClassInfo::IsAspect(
    /* [out] */ Boolean * pIsAspect)
{
    return classInfo->IsAspect(pIsAspect);
}

ECode CTestClassInfo::GetAggregateeCount(
    /* [out] */ Int32 * pCount)
{
    return classInfo->GetAggregateeCount(pCount);
}

ECode CTestClassInfo::GetAllAggregateeInfos(
    /* [out] */ BufferOf<IClassInfo *> * pAggregateeInfos)
{
    return classInfo->GetAllAggregateeInfos(pAggregateeInfos);
}

ECode CTestClassInfo::GetAggregateeInfo(
    /* [in] */ CString name,
    /* [out] */ IClassInfo ** ppAggregateeInfo)
{
    return classInfo->GetAggregateeInfo(name,ppAggregateeInfo);
}

ECode CTestClassInfo::GetConstructorCount(
    /* [out] */ Int32 * pCount)
{
    return classInfo->GetConstructorCount(pCount);
}

ECode CTestClassInfo::GetAllConstructorInfos(
    /* [out] */ BufferOf<IConstructorInfo *> * pConstructorInfos)
{
    return classInfo->GetAllConstructorInfos(pConstructorInfos);
}

ECode CTestClassInfo::GetConstructorInfoByParamNames(
    /* [in] */ CString name,
    /* [out] */ IConstructorInfo ** ppConstructorInfo)
{
    return classInfo->GetConstructorInfoByParamNames(name,ppConstructorInfo);
}

ECode CTestClassInfo::GetConstructorInfoByParamCount(
    /* [in] */ Int32 count,
    /* [out] */ IConstructorInfo ** ppConstructorInfo)
{
    return classInfo->GetConstructorInfoByParamCount(count,ppConstructorInfo);
}

ECode CTestClassInfo::GetInterfaceCount(
    /* [out] */ Int32 * pCount)
{
    return classInfo->GetInterfaceCount(pCount);
}

ECode CTestClassInfo::GetAllInterfaceInfos(
    /* [out] */ BufferOf<IInterfaceInfo *> * pInterfaceInfos)
{
    return classInfo->GetAllInterfaceInfos(pInterfaceInfos);
}

ECode CTestClassInfo::GetInterfaceInfo(
    /* [in] */ CString name,
    /* [out] */ IInterfaceInfo ** ppInterfaceInfo)
{
    return classInfo->GetInterfaceInfo(name,ppInterfaceInfo);
}

ECode CTestClassInfo::GetCallbackInterfaceCount(
    /* [out] */ Int32 * pCount)
{
    return classInfo->GetCallbackInterfaceCount(pCount);
}

ECode CTestClassInfo::GetAllCallbackInterfaceInfos(
    /* [out] */ BufferOf<IInterfaceInfo *> * pCallbackInterfaceInfos)
{
    return classInfo->GetAllCallbackInterfaceInfos(pCallbackInterfaceInfos);
}

ECode CTestClassInfo::GetCallbackInterfaceInfo(
    /* [in] */ CString name,
    /* [out] */ IInterfaceInfo ** ppCallbackInterfaceInfo)
{
    return classInfo->GetCallbackInterfaceInfo(name,ppCallbackInterfaceInfo);
}

ECode CTestClassInfo::GetMethodCount(
    /* [out] */ Int32 * pCount)
{
    return classInfo->GetMethodCount(pCount);
}

ECode CTestClassInfo::GetAllMethodInfos(
    /* [out] */ BufferOf<IMethodInfo *> * pMethodInfos)
{
    return classInfo->GetAllMethodInfos(pMethodInfos);
}

ECode CTestClassInfo::GetMethodInfo(
    /* [in] */ CString name,
    /* [out] */ IMethodInfo ** ppMethodInfo)
{
    return classInfo->GetMethodInfo(name,ppMethodInfo);
}

ECode CTestClassInfo::GetCallbackMethodCount(
    /* [out] */ Int32 * pCount)
{
    return classInfo->GetCallbackMethodCount(pCount);
}

ECode CTestClassInfo::GetAllCallbackMethodInfos(
    /* [out] */ BufferOf<ICallbackMethodInfo *> * pCallbackMethodInfos)
{
    return classInfo->GetAllCallbackMethodInfos(pCallbackMethodInfos);
}

ECode CTestClassInfo::GetCallbackMethodInfo(
    /* [in] */ CString name,
    /* [out] */ ICallbackMethodInfo ** ppCallbackMethodInfo)
{
    return classInfo->GetCallbackMethodInfo(name,ppCallbackMethodInfo);
}

ECode CTestClassInfo::RemoveAllCallbackHandlers(
    /* [in] */ PInterface pServer)
{
    return classInfo->RemoveAllCallbackHandlers(pServer);
}

ECode CTestClassInfo::CreateObject(
    /* [out] */ PInterface * ppObject)
{
    return classInfo->CreateObject(ppObject);
}

ECode CTestClassInfo::CreateObjectInRegime(
    /* [in] */ PRegime pRgm,
    /* [out] */ PInterface * ppObject)
{
    return classInfo->CreateObjectInRegime(pRgm,ppObject);
}

ECode CTestClassInfo::constructor()
{
    return NOERROR;
}

ECode CTestClassInfo::constructor(
    /* [in] */ IClassInfo * pPClassInfo)
{
    classInfo = pPClassInfo;
    return NOERROR;
}

}
}
}
}

