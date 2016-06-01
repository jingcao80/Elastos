
#include "CTestInterfaceInfo.h"

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CAR_INTERFACE_IMPL_2(CTestInterfaceInfo, Object, ITestDataTypeInfo, ITestInterfaceInfo)

CAR_OBJECT_IMPL(CTestInterfaceInfo)

ECode CTestInterfaceInfo::GetName(
    /* [out] */ String * pName)
{
    ECode ec = mInterfaceInfo->GetName(pName);

    if (FAILED(ec)) {
        //E_INVALID_ARGUMENT
        //0x0800a000    E_NO_INTERFACE
        ALOGD("CTestInterfaceInfo::\"%s\" failed! ECode:%x\n", "GetName", ec);
        return ec;
    }

    return ec;
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
    return mInterfaceInfo->GetNamespace(pNs);
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
    mInterfaceInfo = pInterfaceInfo;

    //interfaceInfo->AddRef();

    return NOERROR;
}

}
}
}
}

