#include "elastos/droid/content/CUriPermission.h"
#include "elastos/droid/net/Uri.h"

using Elastos::Droid::Net::Uri;

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL_2(CUriPermission, Object, IUriPermission, IParcelable)

CAR_OBJECT_IMPL(CUriPermission)

CUriPermission::CUriPermission()
    : mModeFlags(0)
    , mPersistedTime(0)
{
}

CUriPermission::~CUriPermission()
{
}

ECode CUriPermission::constructor()
{
    return NOERROR;
}

ECode CUriPermission::constructor(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags,
    /* [in] */ Int64 persistedTime)
{
    mUri = uri;
    mModeFlags = modeFlags;
    mPersistedTime = persistedTime;
    return NOERROR;
}

ECode CUriPermission::GetUri(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)
    *uri = mUri;
    REFCOUNT_ADD(*uri)
    return NOERROR;
}

ECode CUriPermission::IsReadPermission(
    /* [out] */ Boolean* permission)
{
    VALIDATE_NOT_NULL(permission)
    *permission = (mModeFlags & IIntent::FLAG_GRANT_READ_URI_PERMISSION) != 0;
    return NOERROR;
}

ECode CUriPermission::IsWritePermission(
    /* [out] */ Boolean* permission)
{
    VALIDATE_NOT_NULL(permission)
    *permission = (mModeFlags & IIntent::FLAG_GRANT_WRITE_URI_PERMISSION) != 0;
    return NOERROR;
}

ECode CUriPermission::GetPersistedTime(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time)
    *time = mPersistedTime;
    return NOERROR;
}

ECode CUriPermission::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    IParcelable::Probe(mUri)->ReadFromParcel(in);
    in->ReadInt32(&mModeFlags);
    in->ReadInt64(&mPersistedTime);
    return NOERROR;
}

ECode CUriPermission::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    IParcelable::Probe(mUri)->WriteToParcel(dest);
    dest->WriteInt32(mModeFlags);
    dest->WriteInt64(mPersistedTime);
    return NOERROR;
}


} // Content
} // Droid
} // Elastos