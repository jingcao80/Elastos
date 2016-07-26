
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/os/CParcelUuid.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::CUUID;
using Elastos::Utility::IUUIDHelper;
using Elastos::Utility::CUUIDHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL_2(CParcelUuid, Object, IParcelUuid, IParcelable)

CAR_OBJECT_IMPL(CParcelUuid)

CParcelUuid::CParcelUuid()
{
}

ECode CParcelUuid::constructor()
{
    return NOERROR;
}

ECode CParcelUuid::constructor(
    /* [in] */ IUUID* uuid)
{
    mUuid = uuid;
    return NOERROR;
}

ECode CParcelUuid::FromString(
    /* [in] */ const String& uuid,
    /* [out] */ IParcelUuid** id)
{
    VALIDATE_NOT_NULL(id);

    AutoPtr<IUUIDHelper> helper;
    CUUIDHelper::AcquireSingleton((IUUIDHelper**)&helper);
    AutoPtr<IUUID> uuidObj;
    helper->FromString(uuid, (IUUID**)&uuidObj);
    AutoPtr<IParcelUuid> cpu;
    CParcelUuid::New(uuidObj, (IParcelUuid**)&cpu);
    *id = cpu;
    REFCOUNT_ADD(*id);
    return NOERROR;
}

ECode CParcelUuid::GetUuid(
    /* [out] */ IUUID** uuid)
{
    VALIDATE_NOT_NULL(uuid)
    *uuid = mUuid;
    REFCOUNT_ADD(*uuid)
    return NOERROR;
}

ECode CParcelUuid::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = Object::ToString(mUuid);
    return NOERROR;
}

ECode CParcelUuid::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash)
    *hash = Object::GetHashCode(mUuid);
    return NOERROR;
}

ECode CParcelUuid::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (object == NULL) {
        return NOERROR;
    }

    if (TO_IINTERFACE(this) == object) {
        *result = TRUE;
        return NOERROR;
    }

    IParcelUuid* pu = IParcelUuid::Probe(object);
    if (pu == NULL) {
        return NOERROR;
    }

    CParcelUuid* that = (CParcelUuid*) pu;
    *result = Object::Equals(mUuid, that->mUuid);
    return NOERROR;
}

ECode CParcelUuid::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    Int64 most, least;
    source->ReadInt64(&most);
    source->ReadInt64(&least);
    mUuid = NULL;
    CUUID::New(most, least, (IUUID**)&mUuid);
    return NOERROR;
}

ECode CParcelUuid::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    Int64 most = 0, least = 0;
    if (mUuid != NULL) {
        mUuid->GetMostSignificantBits(&most);
        mUuid->GetLeastSignificantBits(&least);
    }
    else {
        Logger::E("CParcelUuid", "TODO mUuid is null");
    }

    dest->WriteInt64(most);
    dest->WriteInt64(least);
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
