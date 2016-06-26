
#include "CTestEnumInfo.h"

#include "CTestEnumItemInfo.h"

#include "CTestModuleInfo.h"

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CAR_INTERFACE_IMPL_2(CTestEnumInfo, Object, ITestDataTypeInfo, ITestEnumInfo)

CAR_OBJECT_IMPL(CTestEnumInfo)

ECode CTestEnumInfo::GetName(
    /* [out] */ String * pName)
{
    return mEnumInfo->GetName(pName);
}

ECode CTestEnumInfo::GetSize(
    /* [out] */ MemorySize * pSize)
{
    return mEnumInfo->GetSize(pSize);
}

ECode CTestEnumInfo::GetDataType(
    /* [out] */ CarDataType * pDataType)
{
    return mEnumInfo->GetDataType(pDataType);
}

ECode CTestEnumInfo::GetNamespace(
    /* [out] */ String * pNs)
{
    return mEnumInfo->GetNamespace(pNs);
}

ECode CTestEnumInfo::GetModuleInfo(
    /* [out] */ ITestModuleInfo ** ppModuleInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IModuleInfo> info;
    ec = mEnumInfo->GetModuleInfo((IModuleInfo**)&info);
    if (FAILED(ec)) {
        ALOGD("CTestClassInfo::GetModuleInfo error: GetModuleInfo fail!");
        return ec;
    }

    AutoPtr<ITestModuleInfo> testInfo;
    ec = CTestModuleInfo::New(info,(ITestModuleInfo**)&testInfo);
    if (FAILED(ec)) {
        ALOGD("CTestEnumInfo::GetModuleInfo error: CTestModlueInfo::New fail!");
        return ec;
    }
    *ppModuleInfo = testInfo;

    info->AddRef();
    testInfo->AddRef();

    return ec;
}

ECode CTestEnumInfo::GetItemCount(
    /* [out] */ Int32 * pCount)
{
    return mEnumInfo->GetItemCount(pCount);
}

ECode CTestEnumInfo::GetAllItemInfos(
    /* [out] */ ArrayOf<ITestEnumItemInfo *> ** ppItemInfos)
{
    ECode ec = NOERROR;

    Int32 count;
    ec = this->GetItemCount(&count);

    ArrayOf<IEnumItemInfo *> * infos;
    infos = ArrayOf<IEnumItemInfo *>::Alloc(count);
    ec = mEnumInfo->GetAllItemInfos(infos);

    //Int32 used = infos->GetUsed();
    Int32 used = count;

    *ppItemInfos = ArrayOf<ITestEnumItemInfo *>::Alloc(used);

    for(Int32 i = 0; i < used; i++){
        AutoPtr<ITestEnumItemInfo> info;
        ec = CTestEnumItemInfo::New((*infos)[i],(ITestEnumItemInfo**)&info);
        if (FAILED(ec)) {
            ALOGD("Create \"%s\" instance failed!\n", "CTestEnumItemInfo");
            return ec;
        }
        (*ppItemInfos)->Set(i,info);

        (*infos)[i]->AddRef();
        info->AddRef();
    }   //for

    return ec;
}

ECode CTestEnumInfo::GetItemInfo(
    /* [in] */ const String& name,
    /* [out] */ ITestEnumItemInfo ** ppEnumItemInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IEnumItemInfo> info;
    ec = mEnumInfo->GetItemInfo(name, (IEnumItemInfo**)&info);
    if (FAILED(ec)) {
        ALOGD("CTestEnumInfo::GetItemInfo error: GetModuleInfo fail!");
        return ec;
    }

    AutoPtr<ITestEnumItemInfo> testInfo;
    ec = CTestEnumItemInfo::New(info,(ITestEnumItemInfo**)&testInfo);
    if (FAILED(ec)) {
        ALOGD("CTestEnumInfo::GetItemInfo error: CTestEnumItemInfo::New fail!");
        return ec;
    }
    *ppEnumItemInfo = testInfo;

    info->AddRef();
    testInfo->AddRef();

    return ec;
}

ECode CTestEnumInfo::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestEnumInfo::constructor(
    /* [in] */ IEnumInfo * pEnumInfo)
{
    mEnumInfo = pEnumInfo;
    return NOERROR;
}

ECode CTestEnumInfo::GetInternalObject(
    /* [out] */ PInterface* ppObject){
    *ppObject = mEnumInfo;
    return NOERROR;
}

}
}
}
}

