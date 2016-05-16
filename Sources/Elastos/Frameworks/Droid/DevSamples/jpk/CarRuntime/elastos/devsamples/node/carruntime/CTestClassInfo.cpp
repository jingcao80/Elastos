
#include "CTestClassInfo.h"

#include "CTestModuleInfo.h"
#include "CTestConstructorInfo.h"

#include <cutils/log.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CAR_INTERFACE_IMPL(CTestClassInfo, Object, ITestClassInfo)

CAR_OBJECT_IMPL(CTestClassInfo)

ECode CTestClassInfo::GetName(
    /* [out] */ String * pName)
{
    return mClassInfo->GetName(pName);
}

ECode CTestClassInfo::GetNamespace(
    /* [out] */ String * pNs)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestClassInfo::GetId(
    /* [out] */ ClassID * pClsid)
{
    mClassInfo->GetId(pClsid);
    return NOERROR;
}

ECode CTestClassInfo::GetModuleInfo(
    /* [out] */ ITestModuleInfo ** ppModuleInfo)
{
    ECode ec;

    AutoPtr<IModuleInfo> moduleInfo;
    ec = mClassInfo->GetModuleInfo((IModuleInfo**)&moduleInfo);
    if (FAILED(ec)) {
        ALOGD("CTestClassInfo::GetModuleInfo error: GetModuleInfo fail!");
        return ec;
    }

    AutoPtr<ITestModuleInfo> testModuleInfo;
    ec = CTestModuleInfo::New(moduleInfo,(ITestModuleInfo**)&testModuleInfo);
    if (FAILED(ec)) {
        ALOGD("CTestClassInfo::GetModuleInfo error: CTestModlueInfo::New fail!");
        return ec;
    }
    *ppModuleInfo = testModuleInfo;

    moduleInfo->AddRef();
    testModuleInfo->AddRef();

    ec = NOERROR;

    return ec;
}

ECode CTestClassInfo::IsSingleton(
    /* [out] */ Boolean * pIsSingleton)
{
    mClassInfo->IsSingleton(pIsSingleton);
    return NOERROR;
}

ECode CTestClassInfo::GetThreadingModel(
    /* [out] */ ThreadingModel * pThreadingModel)
{
    mClassInfo->GetThreadingModel(pThreadingModel);
    return NOERROR;
}

ECode CTestClassInfo::IsPrivate(
    /* [out] */ Boolean * pIsPrivate)
{
    mClassInfo->IsPrivate(pIsPrivate);
    return NOERROR;
}

ECode CTestClassInfo::IsReturnValue(
    /* [out] */ Boolean * pReturnValue)
{
    mClassInfo->IsReturnValue(pReturnValue);
    return NOERROR;
}

ECode CTestClassInfo::IsBaseClass(
    /* [out] */ Boolean * pIsBaseClass)
{
    mClassInfo->IsBaseClass(pIsBaseClass);
    return NOERROR;
}

ECode CTestClassInfo::HasBaseClass(
    /* [out] */ Boolean * pHasBaseClass)
{
    mClassInfo->HasBaseClass(pHasBaseClass);
    return NOERROR;
}

ECode CTestClassInfo::GetBaseClassInfo(
    /* [out] */ ITestClassInfo ** ppBaseClassInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestClassInfo::IsGeneric(
    /* [out] */ Boolean * pIsGeneric)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestClassInfo::HasGeneric(
    /* [out] */ Boolean * pHasGeneric)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestClassInfo::GetGenericInfo(
    /* [out] */ ITestClassInfo ** ppGenericInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestClassInfo::IsRegime(
    /* [out] */ Boolean * pIsRegime)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestClassInfo::GetAspectCount(
    /* [out] */ Int32 * pCount)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestClassInfo::GetAllAspectInfos(
    /* [out] */ ArrayOf<ITestClassInfo *> ** ppAspectInfos)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestClassInfo::GetAspectInfo(
    /* [in] */ const String& name,
    /* [out] */ ITestClassInfo ** ppAspectInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestClassInfo::IsAspect(
    /* [out] */ Boolean * pIsAspect)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestClassInfo::GetAggregateeCount(
    /* [out] */ Int32 * pCount)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestClassInfo::GetAllAggregateeInfos(
    /* [out] */ ArrayOf<ITestClassInfo *> ** ppAggregateeInfos)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestClassInfo::GetAggregateeInfo(
    /* [in] */ const String& name,
    /* [out] */ ITestClassInfo ** ppAggregateeInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestClassInfo::GetConstructorCount(
    /* [out] */ Int32 * pCount)
{
    mClassInfo->GetConstructorCount(pCount);
    return NOERROR;
}

ECode CTestClassInfo::GetAllConstructorInfos(
    /* [out] */ ArrayOf<ITestConstructorInfo *> ** ppConstructorInfos)
{
    ECode ec;

    Int32 constructorCount;
    ec = this->GetConstructorCount(&constructorCount);

    ArrayOf<IConstructorInfo *> * constructorInfos;
    constructorInfos = ArrayOf<IConstructorInfo *>::Alloc(constructorCount);
    ec = mClassInfo->GetAllConstructorInfos(constructorInfos);

    //Int32 used = constructorInfos->GetUsed();
    Int32 used = constructorCount;

    *ppConstructorInfos = ArrayOf<ITestConstructorInfo *>::Alloc(used);

    for(Int32 i = 0; i < used; i++){
        AutoPtr<ITestConstructorInfo> testConstructorInfo;
        ec = CTestConstructorInfo::New((*constructorInfos)[i],(ITestConstructorInfo**)&testConstructorInfo);
        if (FAILED(ec)) {
            ALOGD("Create \"%s\" instance failed!\n", "CTestConstructorInfo");
            return ec;
        }
        (*ppConstructorInfos)->Set(i,testConstructorInfo);

        (*constructorInfos)[i]->AddRef();
        testConstructorInfo->AddRef();
    }   //for

    ec = NOERROR;

    return ec;
}

ECode CTestClassInfo::GetConstructorInfoByParamNames(
    /* [in] */ const String& name,
    /* [out] */ ITestConstructorInfo ** ppConstructorInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestClassInfo::GetConstructorInfoByParamCount(
    /* [in] */ Int32 count,
    /* [out] */ ITestConstructorInfo ** ppConstructorInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestClassInfo::GetInterfaceCount(
    /* [out] */ Int32 * pCount)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestClassInfo::GetAllInterfaceInfos(
    /* [out] */ ArrayOf<ITestInterfaceInfo *> ** ppInterfaceInfos)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestClassInfo::GetInterfaceInfo(
    /* [in] */ const String& fullName,
    /* [out] */ ITestInterfaceInfo ** ppInterfaceInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestClassInfo::GetCallbackInterfaceCount(
    /* [out] */ Int32 * pCount)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestClassInfo::GetAllCallbackInterfaceInfos(
    /* [out] */ ArrayOf<ITestInterfaceInfo *> ** ppCallbackInterfaceInfos)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestClassInfo::GetCallbackInterfaceInfo(
    /* [in] */ const String& name,
    /* [out] */ ITestInterfaceInfo ** ppCallbackInterfaceInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestClassInfo::GetMethodCount(
    /* [out] */ Int32 * pCount)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestClassInfo::GetAllMethodInfos(
    /* [out] */ ArrayOf<ITestMethodInfo *> ** ppMethodInfos)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestClassInfo::GetMethodInfo(
    /* [in] */ const String& name,
    /* [in] */ const String& signature,
    /* [out] */ ITestMethodInfo ** ppMethodInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestClassInfo::GetCallbackMethodCount(
    /* [out] */ Int32 * pCount)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestClassInfo::GetAllCallbackMethodInfos(
    /* [out] */ ArrayOf<ITestCallbackMethodInfo *> ** ppCallbackMethodInfos)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestClassInfo::GetCallbackMethodInfo(
    /* [in] */ const String& name,
    /* [out] */ ITestCallbackMethodInfo ** ppCallbackMethodInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestClassInfo::RemoveAllCallbackHandlers(
    /* [in] */ PInterface pServer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestClassInfo::CreateObject(
    /* [out] */ PInterface * ppObject)
{
    //ECode ec =  mClassInfo->CreateObject((IInterface**)&ppObject);
    ECode ec =  mClassInfo->CreateObject(ppObject);

    if (FAILED(ec)) {
        ALOGD("========CTestClassInfo::CreateObject========fail========");
    }
    else {
        ALOGD("========CTestClassInfo::CreateObject========success========");
    }

    return ec;
}

ECode CTestClassInfo::CreateObjectInRegime(
    /* [in] */ PRegime pRgm,
    /* [out] */ PInterface * ppObject)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestClassInfo::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestClassInfo::constructor(
    /* [in] */ IClassInfo * pClassInfo)
{
    mClassInfo = pClassInfo;

    //moduleInfo->AddRef();

    return NOERROR;
}

}
}
}
}

