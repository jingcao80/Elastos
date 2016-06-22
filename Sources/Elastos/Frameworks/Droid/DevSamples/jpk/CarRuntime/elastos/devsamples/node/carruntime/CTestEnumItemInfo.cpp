
#include "CTestEnumItemInfo.h"

#include "CTestEnumInfo.h"

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CAR_INTERFACE_IMPL_2(CTestEnumItemInfo, Object, ITestDataTypeInfo, ITestEnumItemInfo)

CAR_OBJECT_IMPL(CTestEnumItemInfo)

ECode CTestEnumItemInfo::GetName(
    /* [out] */ String * pName)
{
    return mEnumItemInfo->GetName(pName);
}

ECode CTestEnumItemInfo::GetEnumInfo(
    /* [out] */ ITestEnumInfo ** ppEnumInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IEnumInfo> info;
    ec = mEnumItemInfo->GetEnumInfo((IEnumInfo**)&info);
    if (FAILED(ec)) {
        ALOGD("CTestEnumItemInfo::GetEnumInfo error: GetEnumInfo fail!");
        return ec;
    }

    AutoPtr<ITestEnumInfo> testInfo;
    ec = CTestEnumInfo::New(info,(ITestEnumInfo**)&testInfo);
    if (FAILED(ec)) {
        ALOGD("CTestEnumItemInfo::GetEnumInfo error: CTestGetEnumInfo::New fail!");
        return ec;
    }
    *ppEnumInfo = testInfo;

    info->AddRef();
    testInfo->AddRef();

    return ec;
}

ECode CTestEnumItemInfo::GetValue(
    /* [out] */ Int32 * pValue)
{
    return mEnumItemInfo->GetValue(pValue);
}

ECode CTestEnumItemInfo::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestEnumItemInfo::constructor(
    /* [in] */ IEnumItemInfo * pEnumItemInfo)
{
    mEnumItemInfo = pEnumItemInfo;
    return NOERROR;
}

}
}
}
}

