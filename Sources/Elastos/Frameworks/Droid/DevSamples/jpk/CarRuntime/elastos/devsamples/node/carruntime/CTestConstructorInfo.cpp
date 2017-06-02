
#include "CTestConstructorInfo.h"
#include "CConstructorInfo.h"

#include "CTestParamInfo.h"
#include "CTestArgumentList.h"
#include "CTestClassInfo.h"
#include "CTestModuleInfo.h"

#include <elastos/droid/ext/frameworkdef.h>

#include <cutils/log.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CAR_INTERFACE_IMPL_2(CTestConstructorInfo, Object, ITestFunctionInfo, ITestConstructorInfo)

CAR_OBJECT_IMPL(CTestConstructorInfo)

ECode CTestConstructorInfo::GetName(
    /* [out] */ String * pName)
{
    return mConstructorInfo->GetName(pName);
}

// ECode CTestConstructorInfo::GetAnnotation(
//     /* [out] */ String * pName)
// {
//     return mConstructorInfo->GetAnnotation(pName);
// }

ECode CTestConstructorInfo::GetParamCount(
    /* [out] */ Int32 * pCount)
{
    return mConstructorInfo->GetParamCount(pCount);

    //CConstructorInfo* oConstructorInfo = (CConstructorInfo*)mConstructorInfo;
    //*pCount = oConstructorInfo->mMethodInfo->mMethodDescriptor->mParamCount;

    //return NOERROR;
}

ECode CTestConstructorInfo::GetAllParamInfos(
    /* [out] */ ArrayOf<ITestParamInfo *> ** ppParamInfos)
{
    ECode ec = NOERROR;

    Int32 paramCount;
    ec = this->GetParamCount(&paramCount);

    ArrayOf<IParamInfo *> * paramInfos;
    paramInfos = ArrayOf<IParamInfo *>::Alloc(paramCount);
    ec = mConstructorInfo->GetAllParamInfos(paramInfos);

    //Int32 used = paramInfos->GetUsed();
    Int32 used = paramCount;

    *ppParamInfos = ArrayOf<ITestParamInfo *>::Alloc(used);

    for(Int32 i = 0; i < used; i++){
        AutoPtr<ITestParamInfo> testParamInfo;
        ec = CTestParamInfo::New((*paramInfos)[i],(ITestParamInfo**)&testParamInfo);
        if (FAILED(ec)) {
            ALOGD("Create \"%s\" instance failed!\n", "CTestParamInfo");
            return ec;
        }
        (*ppParamInfos)->Set(i,testParamInfo);

        (*paramInfos)[i]->AddRef();
        testParamInfo->AddRef();
    }   //for

    return ec;
}

ECode CTestConstructorInfo::GetParamInfoByIndex(
    /* [in] */ Int32 index,
    /* [out] */ ITestParamInfo ** ppParamInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IParamInfo> info;
    ec = mConstructorInfo->GetParamInfoByIndex(index,(IParamInfo**)&info);
    if (FAILED(ec)) {
        ALOGD("CConstructorInfo::GetParamInfoByIndex error: GetParamInfoByIndex fail!");
        return ec;
    }

    AutoPtr<ITestParamInfo> testInfo;
    ec = CTestParamInfo::New(info,(ITestParamInfo**)&testInfo);
    if (FAILED(ec)) {
        ALOGD("CConstructorInfo::GetParamInfoByIndex error: CTestParamInfo::New fail!");
        return ec;
    }
    *ppParamInfo = testInfo;
    REFCOUNT_ADD(*ppParamInfo)

    info->AddRef();

    return ec;
}

ECode CTestConstructorInfo::GetParamInfoByName(
    /* [in] */ const String& name,
    /* [out] */ ITestParamInfo ** ppParamInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IParamInfo> info;
    ec = mConstructorInfo->GetParamInfoByName(name, (IParamInfo**)&info);
    if (FAILED(ec)) {
        ALOGD("CTestConstructorInfo::GetParamInfoByName error: GetParamInfoByName fail!");
        return ec;
    }

    AutoPtr<ITestParamInfo> testInfo;
    ec = CTestParamInfo::New(info,(ITestParamInfo**)&testInfo);
    if (FAILED(ec)) {
        ALOGD("CTestConstructorInfo::GetParamInfoByName error: CTestParamInfo::New fail!");
        return ec;
    }
    *ppParamInfo = testInfo;
    REFCOUNT_ADD(*ppParamInfo)

    info->AddRef();

    return ec;
}

ECode CTestConstructorInfo::CreateArgumentList(
    /* [out] */ ITestArgumentList ** ppArgumentList)
{
    ECode ec = NOERROR;

    AutoPtr<IArgumentList> argumentList;
    ec = mConstructorInfo->CreateArgumentList((IArgumentList**)&argumentList);
    if (FAILED(ec)) {
        ALOGD("CTestConstructorInfo::CreateArgumentList error: CreateArgumentList fail!");
        return ec;
    }

    AutoPtr<ITestArgumentList> testArgumentList;
    ec = CTestArgumentList::New(argumentList,(ITestArgumentList**)&testArgumentList);
    if (FAILED(ec)) {
        ALOGD("CTestConstructorInfo::CreateArgumentList error: CTestArgumentList::New fail!");
        return ec;
    }
    *ppArgumentList = testArgumentList;
    REFCOUNT_ADD(*ppArgumentList)

    argumentList->AddRef();

    return ec;
}

ECode CTestConstructorInfo::CreateObject(
    /* [in] */ ITestArgumentList * pArgumentList,
    /* [out] */ PInterface * ppObject)
{
    ECode ec = NOERROR;

    AutoPtr<IArgumentList> argumentList;
    ec = pArgumentList->GetInternalObject((PInterface*)&argumentList);
    if (FAILED(ec)) {
        ALOGD("CTestConstructorInfo::CreateObject: pArgumentList->GetInternalObject fail!");
        return ec;
    }
    else {
        ALOGD("CTestConstructorInfo::CreateObject: pArgumentList->GetInternalObject success!");
    }

    ec = mConstructorInfo->CreateObject(argumentList, ppObject);
    if (FAILED(ec)) {
        ALOGD("CTestConstructorInfo::CreateObject: mConstructorInfo->CreateObject fail!");
        return ec;
    }
    else {
        ALOGD("CTestConstructorInfo::CreateObject: mConstructorInfo->CreateObject success!");
    }

    return ec;
}

ECode CTestConstructorInfo::LocalCreateObject(
    /* [in] */ ITestArgumentList * pArgumentList,
    /* [out] */ PInterface * ppObject)
{
    ECode ec = NOERROR;

    AutoPtr<IArgumentList> argumentList;
    ec = pArgumentList->GetInternalObject((PInterface*)&argumentList);
    if (FAILED(ec)) {
        ALOGD("CTestConstructorInfo::CreateObject: pArgumentList->GetInternalObject fail!");
        return ec;
    }
    else {
        ALOGD("CTestConstructorInfo::CreateObject: pArgumentList->GetInternalObject success!");
    }

    ec = mConstructorInfo->CreateObject(argumentList.Get(), ppObject);
    if (FAILED(ec)) {
        ALOGD("CTestConstructorInfo::CreateObject: mConstructorInfo->CreateObject fail!");
        return ec;
    }
    else {
        ALOGD("CTestConstructorInfo::CreateObject: mConstructorInfo->CreateObject success!");
    }

    return ec;
}

ECode CTestConstructorInfo::RemoteCreateObject(
    /* [in] */ ITestArgumentList * pArgumentList,
    /* [out] */ PInterface * ppObject)
{
    ECode ec = NOERROR;

    AutoPtr<IArgumentList> argumentList;
    ec = pArgumentList->GetInternalObject((PInterface*)&argumentList);
    if (FAILED(ec)) {
        ALOGD("CTestConstructorInfo::CreateObject: pArgumentList->GetInternalObject fail!");
        return ec;
    }
    else {
        ALOGD("CTestConstructorInfo::CreateObject: pArgumentList->GetInternalObject success!");
    }

    ec = mConstructorInfo->CreateObject(argumentList,ppObject);
    if (FAILED(ec)) {
        ALOGD("CTestConstructorInfo::CreateObject: mConstructorInfo->CreateObject fail!");
        return ec;
    }
    else {
        ALOGD("CTestConstructorInfo::CreateObject: mConstructorInfo->CreateObject success!");
    }

    return ec;
}

ECode CTestConstructorInfo::CreateObjectInRegime(
    /* [in] */ PRegime pRgm,
    /* [in] */ ITestArgumentList * pArgumentList,
    /* [out] */ PInterface * ppObject)
{
    //ERROR: PRegime
    //return mConstructorInfo->CreateObjectInRegime(pRgm, pArgumentList, ppObject);

    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestConstructorInfo::GetClassInfo(
    /* [out] */ ITestClassInfo ** ppClassInfo)
{
    ECode ec = NOERROR;

    ALOGD("CTestConstructorInfo::GetClassInfo=================1==========================");

    AutoPtr<IClassInfo> classInfo;

    //TODO:
    //ec = mConstructorInfo->GetClassInfo((IClassInfo**)&classInfo);

    if (FAILED(ec)) {
        ALOGD("CTestConstructorInfo::GetClassObject error: GetClassObject fail!");
        return ec;
    }

    ALOGD("CTestConstructorInfo::GetClassInfo=================2==========================");

    AutoPtr<ITestClassInfo> testClassInfo;
    ec = CTestClassInfo::New(classInfo,(ITestClassInfo**)&testClassInfo);
    if (FAILED(ec)) {
        ALOGD("CTestConstructorInfo::GetClassObject error: CTestClassInfo::New fail!");
        return ec;
    }

    ALOGD("CTestConstructorInfo::GetClassInfo=================3==========================");
    *ppClassInfo = testClassInfo;
    REFCOUNT_ADD(testClassInfo)

    ALOGD("CTestConstructorInfo::GetClassInfo=================4==========================");

    //classInfo->AddRef();
    REFCOUNT_ADD(classInfo.Get());
    ALOGD("CTestConstructorInfo::GetClassInfo=================5==========================");

    return ec;
}

ECode CTestConstructorInfo::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestConstructorInfo::constructor(
    /* [in] */ IConstructorInfo * pConstructorInfo)
{
    mConstructorInfo = pConstructorInfo;
    return NOERROR;
}

ECode CTestConstructorInfo::GetInternalObject(
    /* [out] */ PInterface* ppObject)
{
    *ppObject = mConstructorInfo.Get();
    REFCOUNT_ADD(*ppObject)
    return NOERROR;
}

}
}
}
}

