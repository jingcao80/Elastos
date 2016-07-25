#include "CTestConstantInfo.h"

#include "CTestModuleInfo.h"

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CAR_INTERFACE_IMPL(CTestConstantInfo, Object, ITestConstantInfo)

CAR_OBJECT_IMPL(CTestConstantInfo)

ECode CTestConstantInfo::GetName(
    /* [out] */ String * pName)
{
    return mConstantInfo->GetName(pName);
}

ECode CTestConstantInfo::GetValue(
    /* [out] */ Int32 * pValue)
{
    return mConstantInfo->GetValue(pValue);
}

ECode CTestConstantInfo::GetModuleInfo(
    /* [out] */ ITestModuleInfo ** ppModuleInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IModuleInfo> info;
    ec = mConstantInfo->GetModuleInfo((IModuleInfo**)&info);
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
    REFCOUNT_ADD(*ppModuleInfo)

    info->AddRef();

    return ec;
}

ECode CTestConstantInfo::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestConstantInfo::constructor(
    /* [in] */ IConstantInfo * pConstantInfo)
{
    mConstantInfo = pConstantInfo;
    return NOERROR;
}

ECode CTestConstantInfo::GetInternalObject(
    /* [out] */ PInterface* ppObject){
    *ppObject = mConstantInfo;
    return NOERROR;
}

}
}
}
}

