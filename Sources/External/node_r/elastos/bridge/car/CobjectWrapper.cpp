
#include "CobjectWrapper.h"

namespace Elastos {
namespace Node {

CobjectWrapper::CobjectWrapper(
    /* [in] */ IInterface* instance,
    /* [in] */ IDataTypeInfo* typeInfo)
    : mInstance(instance)
    , mDataTypeInfo(typeInfo)
{}

AutoPtr<IInterface> CobjectWrapper::GetInstance()
{
    return mInstance;
}

AutoPtr<IDataTypeInfo> CobjectWrapper::GetDataTypeInfo()
{
    return mDataTypeInfo;
}

} // namespace Node
} // namespace Elastos
