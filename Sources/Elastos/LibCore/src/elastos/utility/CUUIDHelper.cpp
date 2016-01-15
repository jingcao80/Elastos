#include "CUUIDHelper.h"
#include "CUUID.h"

namespace Elastos {
namespace Utility {

CAR_INTERFACE_IMPL(CUUIDHelper, Singleton, IUUIDHelper)

CAR_SINGLETON_IMPL(CUUIDHelper)

ECode CUUIDHelper::RandomUUID(
    /* [out] */ IUUID** uuid)
{
    return CUUID::RandomUUID(uuid);
}

ECode CUUIDHelper::NameUUIDFromBytes(
    /* [in] */ ArrayOf<Byte>* name,
    /* [out] */ IUUID** uuid)
{
    return CUUID::NameUUIDFromBytes(name, uuid);
}

ECode CUUIDHelper::FromString(
    /* [in] */ const String& name,
    /* [out] */ IUUID** uuid)
{
    return CUUID::FromString(name, uuid);
}

} // namespace Utility
} // namespace Elastos
