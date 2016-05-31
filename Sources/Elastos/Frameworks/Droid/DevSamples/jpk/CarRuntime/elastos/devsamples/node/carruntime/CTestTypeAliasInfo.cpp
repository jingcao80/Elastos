
#include "CTestTypeAliasInfo.h"

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CAR_INTERFACE_IMPL(CTestTypeAliasInfo, Object, ITestTypeAliasInfo)

CAR_OBJECT_IMPL(CTestTypeAliasInfo)

ECode CTestTypeAliasInfo::GetName(
    /* [out] */ String * pName)
{
    return mTypeAliasInfo->GetName(pName);
}

ECode CTestTypeAliasInfo::GetTypeInfo(
    /* [out] */ ITestDataTypeInfo ** ppTypeInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestTypeAliasInfo::GetModuleInfo(
    /* [out] */ ITestModuleInfo ** ppModuleInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestTypeAliasInfo::IsDummy(
    /* [out] */ Boolean * pIsDummy)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestTypeAliasInfo::GetPtrLevel(
    /* [out] */ Int32 * pLevel)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestTypeAliasInfo::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestTypeAliasInfo::constructor(
    /* [in] */ ITypeAliasInfo * pTypeAliasInfo)
{
    mTypeAliasInfo = pTypeAliasInfo;
    return NOERROR;
}

}
}
}
}

