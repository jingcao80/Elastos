
#include "CTestLocalTypeInfo.h"

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CAR_INTERFACE_IMPL(CTestLocalTypeInfo, Object, ITestLocalTypeInfo)

CAR_OBJECT_IMPL(CTestLocalTypeInfo)

ECode CTestLocalTypeInfo::GetName(
    /* [out] */ String * pName)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestLocalTypeInfo::GetSize(
    /* [out] */ MemorySize * pSize)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestLocalTypeInfo::GetDataType(
    /* [out] */ CarDataType * pDataType)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestLocalTypeInfo::DoNothing()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestLocalTypeInfo::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestLocalTypeInfo::constructor(
    /* [in] */ ILocalTypeInfo * pLocalTypeInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}

