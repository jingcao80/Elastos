
#include "CTestConstructorInfo.h"

#include "CTestParamInfo.h"
#include "CTestArgumentList.h"
#include "CTestClassInfo.h"

//#include "ext/frameworkdef.h"
#include <elastos/droid/ext/frameworkdef.h>

#include <cutils/log.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CAR_INTERFACE_IMPL(CTestConstructorInfo, Object, ITestConstructorInfo)

CAR_OBJECT_IMPL(CTestConstructorInfo)

ECode CTestConstructorInfo::GetName(
    /* [out] */ String * pName)
{
    return mConstructorInfo->GetName(pName);
}

ECode CTestConstructorInfo::GetAnnotation(
    /* [out] */ String * pName)
{
    mConstructorInfo->GetAnnotation(pName);
    return NOERROR;
}

ECode CTestConstructorInfo::GetParamCount(
    /* [out] */ Int32 * pCount)
{
    mConstructorInfo->GetParamCount(pCount);
    return NOERROR;
}

ECode CTestConstructorInfo::GetAllParamInfos(
    /* [out] */ ArrayOf<ITestParamInfo *> ** ppParamInfos)
{
    ECode ec;

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

    ec = NOERROR;

    return ec;
}

ECode CTestConstructorInfo::GetParamInfoByIndex(
    /* [in] */ Int32 index,
    /* [out] */ ITestParamInfo ** ppParamInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestConstructorInfo::GetParamInfoByName(
    /* [in] */ const String& name,
    /* [out] */ ITestParamInfo ** ppParamInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestConstructorInfo::CreateArgumentList(
    /* [out] */ ITestArgumentList ** ppArgumentList)
{
    ECode ec;

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

    argumentList->AddRef();
    testArgumentList->AddRef();

    ec = NOERROR;

    return ec;
}

ECode CTestConstructorInfo::CreateObject(
    /* [in] */ ITestArgumentList * pArgumentList,
    /* [out] */ PInterface * ppObject)
{
    // TODO: Add your code here
    //return E_NOT_IMPLEMENTED;

    ECode ec = NOERROR;

    IArgumentList* argumentList;
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

    (*ppObject)->AddRef();

    return ec;
}

ECode CTestConstructorInfo::CreateObjectInRegime(
    /* [in] */ PRegime pRgm,
    /* [in] */ ITestArgumentList * pArgumentList,
    /* [out] */ PInterface * ppObject)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestConstructorInfo::GetClassInfo(
    /* [out] */ ITestClassInfo ** ppClassInfo)
{
    ECode ec;

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

    ALOGD("CTestConstructorInfo::GetClassInfo=================4==========================");

    //classInfo->AddRef();
    REFCOUNT_ADD(classInfo.Get());
    ALOGD("CTestConstructorInfo::GetClassInfo=================5==========================");
    testClassInfo->AddRef();
    ALOGD("CTestConstructorInfo::GetClassInfo=================6==========================");

    ec = NOERROR;

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

}
}
}
}

