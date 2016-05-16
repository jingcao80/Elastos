
#include "CTestEnumInfo.h"

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CAR_INTERFACE_IMPL(CTestEnumInfo, Object, ITestEnumInfo)

CAR_OBJECT_IMPL(CTestEnumInfo)

ECode CTestEnumInfo::GetName(
    /* [out] */ String * pName)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestEnumInfo::GetSize(
    /* [out] */ MemorySize * pSize)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestEnumInfo::GetDataType(
    /* [out] */ CarDataType * pDataType)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestEnumInfo::GetNamespace(
    /* [out] */ String * pNs)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestEnumInfo::GetModuleInfo(
    /* [out] */ ITestModuleInfo ** ppModuleInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestEnumInfo::GetItemCount(
    /* [out] */ Int32 * pCount)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestEnumInfo::GetAllItemInfos(
    /* [out] */ ArrayOf<ITestEnumItemInfo *> ** ppItemInfos)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestEnumInfo::GetItemInfo(
    /* [in] */ const String& name,
    /* [out] */ ITestEnumItemInfo ** ppEnumItemInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestEnumInfo::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestEnumInfo::constructor(
    /* [in] */ IEnumInfo * pEnumInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}

