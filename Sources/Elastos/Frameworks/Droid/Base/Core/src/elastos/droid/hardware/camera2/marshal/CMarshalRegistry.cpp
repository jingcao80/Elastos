
#include "elastos/droid/hardware/camera2/marshal/CMarshalRegistry.h"
#include "elastos/droid/hardware/camera2/marshal/MarshalRegistry.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {

CAR_INTERFACE_IMPL(CMarshalRegistry, Singleton, IMarshalRegistryHelper)

CAR_SINGLETON_IMPL(CMarshalRegistry)

ECode CMarshalRegistry::RegisterMarshalQueryable(
    /* [in] */ IMarshalQueryable* queryable)
{
    return MarshalRegistry::RegisterMarshalQueryable(queryable);
}

ECode CMarshalRegistry::GetMarshaler(
    /* [in] */ ITypeReference* typeToken,
    /* [in] */ Int32 nativeType,
    /* [out] */ IMarshaler** outmar)
{
    VALIDATE_NOT_NULL(outmar);

    return MarshalRegistry::GetMarshaler(typeToken, nativeType, outmar);
}

} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
