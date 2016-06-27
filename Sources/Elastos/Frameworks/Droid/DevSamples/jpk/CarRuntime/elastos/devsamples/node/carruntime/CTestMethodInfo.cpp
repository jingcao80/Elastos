
#include "CTestMethodInfo.h"

#include "CTestParamInfo.h"
#include "CTestArgumentList.h"

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CAR_INTERFACE_IMPL_2(CTestMethodInfo, Object, ITestFunctionInfo, ITestMethodInfo)

CAR_OBJECT_IMPL(CTestMethodInfo)

ECode CTestMethodInfo::GetName(
    /* [out] */ String * pName)
{
    return mMethodInfo->GetName(pName);
}

ECode CTestMethodInfo::GetAnnotation(
    /* [out] */ String * pName)
{
    return mMethodInfo->GetAnnotation(pName);
}

ECode CTestMethodInfo::GetParamCount(
    /* [out] */ Int32 * pCount)
{
    return mMethodInfo->GetParamCount(pCount);
}

ECode CTestMethodInfo::GetAllParamInfos(
    /* [out] */ ArrayOf<ITestParamInfo *> ** ppParamInfos)
{
    ECode ec = NOERROR;

    Int32 count;
    ec = this->GetParamCount(&count);

    ArrayOf<IParamInfo *> * infos;
    infos = ArrayOf<IParamInfo *>::Alloc(count);
    ec = mMethodInfo->GetAllParamInfos(infos);

    //Int32 used = infos->GetUsed();
    Int32 used = count;

    *ppParamInfos = ArrayOf<ITestParamInfo *>::Alloc(used);

    for(Int32 i = 0; i < used; i++){
        AutoPtr<ITestParamInfo> info;
        ec = CTestParamInfo::New((*infos)[i],(ITestParamInfo**)&info);
        if (FAILED(ec)) {
            ALOGD("Create \"%s\" instance failed!\n", "CTestParamInfo");
            return ec;
        }
        (*ppParamInfos)->Set(i,info);

        (*infos)[i]->AddRef();
        info->AddRef();
    }   //for

    return ec;
}

ECode CTestMethodInfo::GetParamInfoByIndex(
    /* [in] */ Int32 index,
    /* [out] */ ITestParamInfo ** ppParamInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IParamInfo> info;
    ec = mMethodInfo->GetParamInfoByIndex(index, (IParamInfo**)&info);
    if (FAILED(ec)) {
        ALOGD("CTestParamInfo::GetParamInfoByIndex error: GetParamInfoByIndex fail!");
        return ec;
    }

    AutoPtr<ITestParamInfo> testInfo;
    ec = CTestParamInfo::New(info,(ITestParamInfo**)&testInfo);
    if (FAILED(ec)) {
        ALOGD("CTestParamInfo::GetParamInfoByIndex error: CTestParamInfo::New fail!");
        return ec;
    }
    *ppParamInfo = testInfo;

    info->AddRef();
    testInfo->AddRef();

    return ec;
}

ECode CTestMethodInfo::GetParamInfoByName(
    /* [in] */ const String& name,
    /* [out] */ ITestParamInfo ** ppParamInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IParamInfo> info;
    ec = mMethodInfo->GetParamInfoByName(name, (IParamInfo**)&info);
    if (FAILED(ec)) {
        ALOGD("CTestParamInfo::GetParamInfoByName error: GetParamInfoByName fail!");
        return ec;
    }

    AutoPtr<ITestParamInfo> testInfo;
    ec = CTestParamInfo::New(info,(ITestParamInfo**)&testInfo);
    if (FAILED(ec)) {
        ALOGD("CTestParamInfo::GetParamInfoByName error: CTestParamInfo::New fail!");
        return ec;
    }
    *ppParamInfo = testInfo;

    info->AddRef();
    testInfo->AddRef();

    return ec;
}

ECode CTestMethodInfo::CreateArgumentList(
    /* [out] */ ITestArgumentList ** ppArgumentList)
{
    ECode ec = NOERROR;

    AutoPtr<IArgumentList> info;
    ec = mMethodInfo->CreateArgumentList((IArgumentList**)&info);
    if (FAILED(ec)) {
        ALOGD("CTestMethodInfo::CreateArgumentList error: CreateArgumentList fail!");
        return ec;
    }

    AutoPtr<ITestArgumentList> testInfo;
    ec = CTestArgumentList::New(info,(ITestArgumentList**)&testInfo);
    if (FAILED(ec)) {
        ALOGD("CTestMethodInfo::CreateArgumentList error: CTestArgumentList::New fail!");
        return ec;
    }
    *ppArgumentList = testInfo;

    info->AddRef();
    testInfo->AddRef();

    return ec;
}

ECode CTestMethodInfo::Invoke(
    /* [in] */ PInterface pTarget,
    /* [in] */ ITestArgumentList * pArgumentList)
{
    IArgumentList* mArgumentList;
    ECode ec = pArgumentList->GetInternalObject((PInterface*)&mArgumentList);

    return mMethodInfo->Invoke(pTarget, mArgumentList);
}

ECode CTestMethodInfo::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestMethodInfo::constructor(
    /* [in] */ IMethodInfo * pMethodInfo)
{
    mMethodInfo = pMethodInfo;
    return NOERROR;
}

ECode CTestMethodInfo::GetInternalObject(
    /* [out] */ PInterface* ppObject){
    *ppObject = mMethodInfo;
    return NOERROR;
}

}
}
}
}

