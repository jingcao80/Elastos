
#include "CTestClassInfo.h"

#include "CTestModuleInfo.h"
#include "CTestConstructorInfo.h"

#include "CTestCallbackMethodInfo.h"
#include "CTestMethodInfo.h"
#include "CTestInterfaceInfo.h"

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
    return mClassInfo->GetNamespace(pNs);
}

ECode CTestClassInfo::GetId(
    /* [out] */ ClassID * pClsid)
{
    return mClassInfo->GetId(pClsid);
}

ECode CTestClassInfo::GetModuleInfo(
    /* [out] */ ITestModuleInfo ** ppModuleInfo)
{
    ECode ec = NOERROR;

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

    return ec;
}

ECode CTestClassInfo::IsSingleton(
    /* [out] */ Boolean * pIsSingleton)
{
    return mClassInfo->IsSingleton(pIsSingleton);
}

ECode CTestClassInfo::GetThreadingModel(
    /* [out] */ ThreadingModel * pThreadingModel)
{
    return mClassInfo->GetThreadingModel(pThreadingModel);
}

ECode CTestClassInfo::IsPrivate(
    /* [out] */ Boolean * pIsPrivate)
{
    return mClassInfo->IsPrivate(pIsPrivate);
}

ECode CTestClassInfo::IsReturnValue(
    /* [out] */ Boolean * pReturnValue)
{
    return mClassInfo->IsReturnValue(pReturnValue);
}

ECode CTestClassInfo::IsBaseClass(
    /* [out] */ Boolean * pIsBaseClass)
{
    return mClassInfo->IsBaseClass(pIsBaseClass);
}

ECode CTestClassInfo::HasBaseClass(
    /* [out] */ Boolean * pHasBaseClass)
{
    return mClassInfo->HasBaseClass(pHasBaseClass);
}

ECode CTestClassInfo::GetBaseClassInfo(
    /* [out] */ ITestClassInfo ** ppBaseClassInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IClassInfo> classInfo;
    ec = mClassInfo->GetBaseClassInfo((IClassInfo**)&classInfo);
    if (FAILED(ec)) {
        ALOGD("CTestClassInfo::GetBaseClassInfo error: GetBaseClassInfo fail!");
        return ec;
    }

    AutoPtr<ITestClassInfo> testClassInfo;
    ec = CTestClassInfo::New(classInfo,(ITestClassInfo**)&testClassInfo);
    if (FAILED(ec)) {
        ALOGD("CTestClassInfo::GetBaseClassInfo error: CTestClassInfo::New fail!");
        return ec;
    }
    *ppBaseClassInfo = testClassInfo;

    classInfo->AddRef();
    testClassInfo->AddRef();

    return ec;
}

ECode CTestClassInfo::IsGeneric(
    /* [out] */ Boolean * pIsGeneric)
{
    return mClassInfo->IsGeneric(pIsGeneric);
}

ECode CTestClassInfo::HasGeneric(
    /* [out] */ Boolean * pHasGeneric)
{
    return mClassInfo->HasGeneric(pHasGeneric);
}

ECode CTestClassInfo::GetGenericInfo(
    /* [out] */ ITestClassInfo ** ppGenericInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IClassInfo> classInfo;
    ec = mClassInfo->GetGenericInfo((IClassInfo**)&classInfo);
    if (FAILED(ec)) {
        ALOGD("CTestClassInfo::GetBaseClassInfo error: GetBaseClassInfo fail!");
        return ec;
    }

    AutoPtr<ITestClassInfo> testClassInfo;
    ec = CTestClassInfo::New(classInfo,(ITestClassInfo**)&testClassInfo);
    if (FAILED(ec)) {
        ALOGD("CTestClassInfo::GetBaseClassInfo error: CTestClassInfo::New fail!");
        return ec;
    }
    *ppGenericInfo = testClassInfo;

    classInfo->AddRef();
    testClassInfo->AddRef();

    return ec;
}

ECode CTestClassInfo::IsRegime(
    /* [out] */ Boolean * pIsRegime)
{
    return mClassInfo->IsRegime(pIsRegime);
}

ECode CTestClassInfo::GetAspectCount(
    /* [out] */ Int32 * pCount)
{
    return mClassInfo->GetAspectCount(pCount);
}

ECode CTestClassInfo::GetAllAspectInfos(
    /* [out] */ ArrayOf<ITestClassInfo *> ** ppAspectInfos)
{
    ECode ec = NOERROR;

    Int32 count;
    ec = this->GetAspectCount(&count);

    ArrayOf<IClassInfo *> * infos;
    infos = ArrayOf<IClassInfo *>::Alloc(count);
    ec = mClassInfo->GetAllAspectInfos(infos);

    //Int32 used = infos->GetUsed();
    Int32 used = count;

    *ppAspectInfos = ArrayOf<ITestClassInfo *>::Alloc(used);

    for(Int32 i = 0; i < used; i++){
        AutoPtr<ITestClassInfo> info;
        ec = CTestClassInfo::New((*infos)[i],(ITestClassInfo**)&info);
        if (FAILED(ec)) {
            ALOGD("Create \"%s\" instance failed!\n", "CTestClassInfo");
            return ec;
        }
        (*ppAspectInfos)->Set(i,info);

        (*infos)[i]->AddRef();
        info->AddRef();
    }   //for

    return ec;
}

ECode CTestClassInfo::GetAspectInfo(
    /* [in] */ const String& name,
    /* [out] */ ITestClassInfo ** ppAspectInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IClassInfo> classInfo;
    ec = mClassInfo->GetAspectInfo(name, (IClassInfo**)&classInfo);
    if (FAILED(ec)) {
        ALOGD("CTestClassInfo::GetBaseClassInfo error: GetBaseClassInfo fail!");
        return ec;
    }

    AutoPtr<ITestClassInfo> testClassInfo;
    ec = CTestClassInfo::New(classInfo,(ITestClassInfo**)&testClassInfo);
    if (FAILED(ec)) {
        ALOGD("CTestClassInfo::GetBaseClassInfo error: CTestClassInfo::New fail!");
        return ec;
    }
    *ppAspectInfo = testClassInfo;

    classInfo->AddRef();
    testClassInfo->AddRef();

    return ec;
}

ECode CTestClassInfo::IsAspect(
    /* [out] */ Boolean * pIsAspect)
{
    return mClassInfo->IsAspect(pIsAspect);
}

ECode CTestClassInfo::GetAggregateeCount(
    /* [out] */ Int32 * pCount)
{
    return mClassInfo->GetAggregateeCount(pCount);
}

ECode CTestClassInfo::GetAllAggregateeInfos(
    /* [out] */ ArrayOf<ITestClassInfo *> ** ppAggregateeInfos)
{
    ECode ec = NOERROR;

    Int32 count;
    ec = this->GetAggregateeCount(&count);

    ArrayOf<IClassInfo *> * infos;
    infos = ArrayOf<IClassInfo *>::Alloc(count);
    ec = mClassInfo->GetAllAggregateeInfos(infos);

    //Int32 used = infos->GetUsed();
    Int32 used = count;

    *ppAggregateeInfos = ArrayOf<ITestClassInfo *>::Alloc(used);

    for(Int32 i = 0; i < used; i++){
        AutoPtr<ITestClassInfo> info;
        ec = CTestClassInfo::New((*infos)[i],(ITestClassInfo**)&info);
        if (FAILED(ec)) {
            ALOGD("Create \"%s\" instance failed!\n", "CTestClassInfo");
            return ec;
        }
        (*ppAggregateeInfos)->Set(i,info);

        (*infos)[i]->AddRef();
        info->AddRef();
    }   //for

    return ec;
}

ECode CTestClassInfo::GetAggregateeInfo(
    /* [in] */ const String& name,
    /* [out] */ ITestClassInfo ** ppAggregateeInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IClassInfo> classInfo;
    ec = mClassInfo->GetAggregateeInfo(name, (IClassInfo**)&classInfo);
    if (FAILED(ec)) {
        ALOGD("CTestClassInfo::GetBaseClassInfo error: GetBaseClassInfo fail!");
        return ec;
    }

    AutoPtr<ITestClassInfo> testClassInfo;
    ec = CTestClassInfo::New(classInfo,(ITestClassInfo**)&testClassInfo);
    if (FAILED(ec)) {
        ALOGD("CTestClassInfo::GetBaseClassInfo error: CTestClassInfo::New fail!");
        return ec;
    }
    *ppAggregateeInfo = testClassInfo;

    classInfo->AddRef();
    testClassInfo->AddRef();

    return ec;
}

ECode CTestClassInfo::GetConstructorCount(
    /* [out] */ Int32 * pCount)
{
    return mClassInfo->GetConstructorCount(pCount);
}

ECode CTestClassInfo::GetAllConstructorInfos(
    /* [out] */ ArrayOf<ITestConstructorInfo *> ** ppConstructorInfos)
{
    ECode ec = NOERROR;

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

    return ec;
}

ECode CTestClassInfo::GetConstructorInfoByParamNames(
    /* [in] */ const String& name,
    /* [out] */ ITestConstructorInfo ** ppConstructorInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IConstructorInfo> constructorInfo;
    ec = mClassInfo->GetConstructorInfoByParamNames(name, (IConstructorInfo**)&constructorInfo);
    if (FAILED(ec)) {
        ALOGD("CTestConstructorInfo::GetConstructorInfoByParamNames error: GetConstructorInfoByParamNames fail!");
        return ec;
    }

    AutoPtr<ITestConstructorInfo> testConstructorInfo;
    ec = CTestConstructorInfo::New(constructorInfo,(ITestConstructorInfo**)&testConstructorInfo);
    if (FAILED(ec)) {
        ALOGD("CTestConstructorInfo::GetConstructorInfoByParamNames error: CTestClassInfo::New fail!");
        return ec;
    }
    *ppConstructorInfo = testConstructorInfo;

    constructorInfo->AddRef();
    testConstructorInfo->AddRef();

    return ec;
}

ECode CTestClassInfo::GetConstructorInfoByParamCount(
    /* [in] */ Int32 count,
    /* [out] */ ITestConstructorInfo ** ppConstructorInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IConstructorInfo> constructorInfo;
    ec = mClassInfo->GetConstructorInfoByParamCount(count, (IConstructorInfo**)&constructorInfo);
    if (FAILED(ec)) {
        ALOGD("CTestConstructorInfo::GetConstructorInfoByParamCount error: GetConstructorInfoByParamCount fail!");
        return ec;
    }

    AutoPtr<ITestConstructorInfo> testConstructorInfo;
    ec = CTestConstructorInfo::New(constructorInfo,(ITestConstructorInfo**)&testConstructorInfo);
    if (FAILED(ec)) {
        ALOGD("CTestConstructorInfo::GetConstructorInfoByParamCount error: CTestClassInfo::New fail!");
        return ec;
    }
    *ppConstructorInfo = testConstructorInfo;

    constructorInfo->AddRef();
    testConstructorInfo->AddRef();

    return ec;
}

ECode CTestClassInfo::GetInterfaceCount(
    /* [out] */ Int32 * pCount)
{
    return mClassInfo->GetInterfaceCount(pCount);
}

ECode CTestClassInfo::GetAllInterfaceInfos(
    /* [out] */ ArrayOf<ITestInterfaceInfo *> ** ppInterfaceInfos)
{
    ECode ec = NOERROR;

    Int32 count;
    ec = this->GetInterfaceCount(&count);

    ArrayOf<IInterfaceInfo *> * infos;
    infos = ArrayOf<IInterfaceInfo *>::Alloc(count);
    ec = mClassInfo->GetAllInterfaceInfos(infos);

    //Int32 used = infos->GetUsed();
    Int32 used = count;

    *ppInterfaceInfos = ArrayOf<ITestInterfaceInfo *>::Alloc(used);

    for(Int32 i = 0; i < used; i++){
        AutoPtr<ITestInterfaceInfo> info;
        ec = CTestInterfaceInfo::New((*infos)[i],(ITestInterfaceInfo**)&info);
        if (FAILED(ec)) {
            ALOGD("Create \"%s\" instance failed!\n", "CTestInterfaceInfo");
            return ec;
        }
        (*ppInterfaceInfos)->Set(i,info);

        (*infos)[i]->AddRef();
        info->AddRef();
    }   //for

    return ec;
}

ECode CTestClassInfo::GetInterfaceInfo(
    /* [in] */ const String& fullName,
    /* [out] */ ITestInterfaceInfo ** ppInterfaceInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IInterfaceInfo> interfaceInfo;
    ec = mClassInfo->GetInterfaceInfo(fullName, (IInterfaceInfo**)&interfaceInfo);
    if (FAILED(ec)) {
        ALOGD("CTestClassInfo::GetInterfaceInfo error: GetInterfaceInfo fail!");
        return ec;
    }

    AutoPtr<ITestInterfaceInfo> testInterfaceInfo;
    ec = CTestInterfaceInfo::New(interfaceInfo,(ITestInterfaceInfo**)&testInterfaceInfo);
    if (FAILED(ec)) {
        ALOGD("CTestClassInfo::GetInterfaceInfo error: CTestClassInfo::New fail!");
        return ec;
    }
    *ppInterfaceInfo = testInterfaceInfo;

    interfaceInfo->AddRef();
    testInterfaceInfo->AddRef();

    return ec;
}

ECode CTestClassInfo::GetCallbackInterfaceCount(
    /* [out] */ Int32 * pCount)
{
    return mClassInfo->GetCallbackInterfaceCount(pCount);
}

ECode CTestClassInfo::GetAllCallbackInterfaceInfos(
    /* [out] */ ArrayOf<ITestInterfaceInfo *> ** ppCallbackInterfaceInfos)
{
    ECode ec = NOERROR;

    Int32 count;
    ec = this->GetCallbackInterfaceCount(&count);

    ArrayOf<IInterfaceInfo *> * infos;
    infos = ArrayOf<IInterfaceInfo *>::Alloc(count);
    ec = mClassInfo->GetAllInterfaceInfos(infos);

    //Int32 used = infos->GetUsed();
    Int32 used = count;

    *ppCallbackInterfaceInfos = ArrayOf<ITestInterfaceInfo *>::Alloc(used);

    for(Int32 i = 0; i < used; i++){
        AutoPtr<ITestInterfaceInfo> info;
        ec = CTestInterfaceInfo::New((*infos)[i],(ITestInterfaceInfo**)&info);
        if (FAILED(ec)) {
            ALOGD("Create \"%s\" instance failed!\n", "CTestInterfaceInfo");
            return ec;
        }
        (*ppCallbackInterfaceInfos)->Set(i,info);

        (*infos)[i]->AddRef();
        info->AddRef();
    }   //for

    return ec;
}

ECode CTestClassInfo::GetCallbackInterfaceInfo(
    /* [in] */ const String& name,
    /* [out] */ ITestInterfaceInfo ** ppCallbackInterfaceInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IInterfaceInfo> interfaceInfo;
    ec = mClassInfo->GetCallbackInterfaceInfo(name, (IInterfaceInfo**)&interfaceInfo);
    if (FAILED(ec)) {
        ALOGD("CTestClassInfo::GetInterfaceInfo error: GetInterfaceInfo fail!");
        return ec;
    }

    AutoPtr<ITestInterfaceInfo> testInterfaceInfo;
    ec = CTestInterfaceInfo::New(interfaceInfo,(ITestInterfaceInfo**)&testInterfaceInfo);
    if (FAILED(ec)) {
        ALOGD("CTestClassInfo::GetInterfaceInfo error: CTestClassInfo::New fail!");
        return ec;
    }
    *ppCallbackInterfaceInfo = testInterfaceInfo;

    interfaceInfo->AddRef();
    testInterfaceInfo->AddRef();

    return ec;
}

ECode CTestClassInfo::GetMethodCount(
    /* [out] */ Int32 * pCount)
{
    return mClassInfo->GetMethodCount(pCount);
}

ECode CTestClassInfo::GetAllMethodInfos(
    /* [out] */ ArrayOf<ITestMethodInfo *> ** ppMethodInfos)
{
    ECode ec = NOERROR;

    Int32 count;
    ec = this->GetMethodCount(&count);

    ArrayOf<IMethodInfo *> * infos;
    infos = ArrayOf<IMethodInfo *>::Alloc(count);
    ec = mClassInfo->GetAllMethodInfos(infos);

    //Int32 used = infos->GetUsed();
    Int32 used = count;

    *ppMethodInfos = ArrayOf<ITestMethodInfo *>::Alloc(used);

    for(Int32 i = 0; i < used; i++){
        AutoPtr<ITestMethodInfo> info;
        ec = CTestMethodInfo::New((*infos)[i],(ITestMethodInfo**)&info);
        if (FAILED(ec)) {
            ALOGD("Create \"%s\" instance failed!\n", "CTestMethodInfo");
            return ec;
        }
        (*ppMethodInfos)->Set(i,info);

        (*infos)[i]->AddRef();
        info->AddRef();
    }   //for

    return ec;
}

ECode CTestClassInfo::GetMethodInfo(
    /* [in] */ const String& name,
    /* [in] */ const String& signature,
    /* [out] */ ITestMethodInfo ** ppMethodInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IMethodInfo> methodInfo;
    ec = mClassInfo->GetMethodInfo(name, signature, (IMethodInfo**)&methodInfo);
    if (FAILED(ec)) {
        ALOGD("CTestClassInfo::GetMethodInfo error: GetMethodInfo fail!");
        return ec;
    }

    AutoPtr<ITestMethodInfo> testMethodInfo;
    ec = CTestMethodInfo::New(methodInfo,(ITestMethodInfo**)&testMethodInfo);
    if (FAILED(ec)) {
        ALOGD("CTestClassInfo::GetMethodInfo error: CTestClassInfo::New fail!");
        return ec;
    }
    *ppMethodInfo = testMethodInfo;

    methodInfo->AddRef();
    testMethodInfo->AddRef();

    return ec;
}

ECode CTestClassInfo::GetCallbackMethodCount(
    /* [out] */ Int32 * pCount)
{
    return mClassInfo->GetCallbackMethodCount(pCount);
}

ECode CTestClassInfo::GetAllCallbackMethodInfos(
    /* [out] */ ArrayOf<ITestCallbackMethodInfo *> ** ppCallbackMethodInfos)
{
    ECode ec = NOERROR;

    Int32 count;
    ec = this->GetCallbackMethodCount(&count);

    ArrayOf<ICallbackMethodInfo *> * infos;
    infos = ArrayOf<ICallbackMethodInfo *>::Alloc(count);
    ec = mClassInfo->GetAllCallbackMethodInfos(infos);

    //Int32 used = infos->GetUsed();
    Int32 used = count;

    *ppCallbackMethodInfos = ArrayOf<ITestCallbackMethodInfo *>::Alloc(used);

    for(Int32 i = 0; i < used; i++){
        AutoPtr<ITestCallbackMethodInfo> info;
        ec = CTestCallbackMethodInfo::New((*infos)[i],(ITestCallbackMethodInfo**)&info);
        if (FAILED(ec)) {
            ALOGD("Create \"%s\" instance failed!\n", "CTestCallbackMethodInfo");
            return ec;
        }
        (*ppCallbackMethodInfos)->Set(i,info);

        (*infos)[i]->AddRef();
        info->AddRef();
    }   //for

    return ec;
}

ECode CTestClassInfo::GetCallbackMethodInfo(
    /* [in] */ const String& name,
    /* [out] */ ITestCallbackMethodInfo ** ppCallbackMethodInfo)
{
    ECode ec = NOERROR;

    AutoPtr<ICallbackMethodInfo> callbackMethodInfo;
    ec = mClassInfo->GetCallbackMethodInfo(name, (ICallbackMethodInfo**)&callbackMethodInfo);
    if (FAILED(ec)) {
        ALOGD("CTestClassInfo::GetCallbackMethodInfo error: GetCallbackMethodInfo fail!");
        return ec;
    }

    AutoPtr<ITestCallbackMethodInfo> testCallbackMethodInfo;
    ec = CTestCallbackMethodInfo::New(callbackMethodInfo,(ITestCallbackMethodInfo**)&testCallbackMethodInfo);
    if (FAILED(ec)) {
        ALOGD("CTestClassInfo::GetCallbackMethodInfo error: CTestClassInfo::New fail!");
        return ec;
    }
    *ppCallbackMethodInfo = testCallbackMethodInfo;

    callbackMethodInfo->AddRef();
    testCallbackMethodInfo->AddRef();

    return ec;
}

ECode CTestClassInfo::RemoveAllCallbackHandlers(
    /* [in] */ PInterface pServer)
{
    return mClassInfo->RemoveAllCallbackHandlers(pServer);
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

ECode CTestClassInfo::LocalCreateObject(
    /* [out] */ PInterface * ppObject)
{
    ALOGD("========CTestClassInfo::LocalCreateObject========begin========");

    //ECode ec =  mClassInfo->CreateObject((IInterface**)&ppObject);
    ECode ec =  mClassInfo->CreateObject(ppObject);

    if (FAILED(ec)) {
        ALOGD("========CTestClassInfo::LocalCreateObject========fail========");
    }
    else {
        ALOGD("========CTestClassInfo::LocalCreateObject========success========");
    }

    return ec;
}

ECode CTestClassInfo::RemoteCreateObject(
    /* [out] */ PInterface * ppObject)
{
    ALOGD("========CTestClassInfo::RemoteCreateObject========begin========");

    //ECode ec =  mClassInfo->CreateObject((IInterface**)&ppObject);
    ECode ec =  mClassInfo->CreateObject(ppObject);

    if (FAILED(ec)) {
        ALOGD("========CTestClassInfo::RemoteCreateObject========fail========");
    }
    else {
        ALOGD("========CTestClassInfo::RemoteCreateObject========success========");
    }

    return ec;
}

ECode CTestClassInfo::CreateObjectInRegime(
    /* [in] */ PRegime pRgm,
    /* [out] */ PInterface * ppObject)
{
    return mClassInfo->CreateObjectInRegime(pRgm, ppObject);
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
    return NOERROR;
}

}
}
}
}

