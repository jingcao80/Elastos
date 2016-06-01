
#include "CTestStructInfo.h"

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CAR_INTERFACE_IMPL_2(CTestStructInfo, Object, ITestDataTypeInfo, ITestStructInfo)

CAR_OBJECT_IMPL(CTestStructInfo)

ECode CTestStructInfo::GetName(
    /* [out] */ String * pName)
{
    return mStructInfo->GetName(pName);
}

ECode CTestStructInfo::GetSize(
    /* [out] */ MemorySize * pSize)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestStructInfo::GetDataType(
    /* [out] */ CarDataType * pDataType)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestStructInfo::GetModuleInfo(
    /* [out] */ ITestModuleInfo ** ppModuleInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestStructInfo::GetFieldCount(
    /* [out] */ Int32 * pCount)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestStructInfo::GetAllFieldInfos(
    /* [out] */ ArrayOf<ITestFieldInfo *> ** ppFieldInfos)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestStructInfo::GetFieldInfo(
    /* [in] */ const String& name,
    /* [out] */ ITestFieldInfo ** ppFieldInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestStructInfo::CreateVariable(
    /* [out] */ ITestVariableOfStruct ** ppVariableBox)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestStructInfo::CreateVariableBox(
    /* [in] */ PCarQuintet pVariableDescriptor,
    /* [out] */ ITestVariableOfStruct ** ppVariableBox)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestStructInfo::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestStructInfo::constructor(
    /* [in] */ IStructInfo * pStructInfo)
{
    mStructInfo = pStructInfo;
    return NOERROR;
}

}
}
}
}

