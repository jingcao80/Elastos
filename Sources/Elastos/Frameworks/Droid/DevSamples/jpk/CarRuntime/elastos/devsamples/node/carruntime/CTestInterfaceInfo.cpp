
#include "CTestInterfaceInfo.h"

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CAR_INTERFACE_IMPL(CTestInterfaceInfo, Object, ITestInterfaceInfo)

CAR_OBJECT_IMPL(CTestInterfaceInfo)

ECode CTestInterfaceInfo::GetName(
    /* [out] */ String * pName)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestInterfaceInfo::GetSize(
    /* [out] */ MemorySize * pSize)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestInterfaceInfo::GetDataType(
    /* [out] */ CarDataType * pDataType)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestInterfaceInfo::GetNamespace(
    /* [out] */ String * pNs)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestInterfaceInfo::GetId(
    /* [out] */ InterfaceID * pIid)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestInterfaceInfo::GetModuleInfo(
    /* [out] */ ITestModuleInfo ** ppModuleInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestInterfaceInfo::IsLocal(
    /* [out] */ Boolean * pIsLocal)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestInterfaceInfo::HasBase(
    /* [out] */ Boolean * pHasBase)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestInterfaceInfo::GetBaseInfo(
    /* [out] */ IInterfaceInfo ** ppBaseInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestInterfaceInfo::GetMethodCount(
    /* [out] */ Int32 * pCount)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestInterfaceInfo::GetAllMethodInfos(
    /* [out] */ ArrayOf<ITestMethodInfo *> ** ppMethodInfos)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestInterfaceInfo::GetMethodInfo(
    /* [in] */ const String& name,
    /* [in] */ const String& signature,
    /* [out] */ ITestMethodInfo ** ppMethodInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestInterfaceInfo::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestInterfaceInfo::constructor(
    /* [in] */ IInterfaceInfo * pInterfaceInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}

