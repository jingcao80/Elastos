#include "Elastos.Droid.Os.h"
#include "elastos/droid/media/projection/CMediaProjectionInfo.h"
#include <elastos/utility/Objects.h>

using Elastos::Utility::Objects;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Projection {

CAR_INTERFACE_IMPL_2(CMediaProjectionInfo, Object, IMediaProjectionInfo, IParcelable)

CAR_OBJECT_IMPL(CMediaProjectionInfo)

CMediaProjectionInfo::CMediaProjectionInfo()
    : mPackageName(String(NULL))
{
}

CMediaProjectionInfo::~CMediaProjectionInfo()
{
}

ECode CMediaProjectionInfo::constructor()
{
    return NOERROR;
}

ECode CMediaProjectionInfo::constructor(
    /* [in] */ const String& packageName,
    /* [in] */ IUserHandle * handle)
{
    mPackageName = packageName;
    mUserHandle = handle;
    return NOERROR;
}

ECode CMediaProjectionInfo::GetPackageName(
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPackageName;
    return NOERROR;
}

ECode CMediaProjectionInfo::GetUserHandle(
    /* [out] */ IUserHandle ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mUserHandle;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaProjectionInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    AutoPtr<IInterface> obj;
    source->ReadString(&mPackageName);
    source->ReadInterfacePtr((Handle32*)(IInterface**)&obj);
    mUserHandle = IUserHandle::Probe(obj);
    return NOERROR;
}

ECode CMediaProjectionInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mPackageName);
    dest->WriteInterfacePtr(mUserHandle);
    return NOERROR;
}

ECode CMediaProjectionInfo::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result)
    IMediaProjectionInfo* o = IMediaProjectionInfo::Probe(obj);
    if (o == (IMediaProjectionInfo*)this) {
        CMediaProjectionInfo* other = (CMediaProjectionInfo*)o;
        *result = mPackageName.Equals(other->mPackageName)
            && Objects::Equals(mUserHandle, other->mUserHandle);
    }
    else {
        *result = FALSE;
    }
    return NOERROR;
}

ECode CMediaProjectionInfo::GetHashCode(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)
    // return Objects.hash(mPackageName, mUserHandle);
    return E_NOT_IMPLEMENTED;
}

ECode CMediaProjectionInfo::ToString(
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("MediaProjectionInfo{mPackageName=") + mPackageName;
         // + ", mUserHandle=" + mUserHandle + "}";
    return NOERROR;
}

} // namespace Projection
} // namespace Media
} // namepsace Droid
} // namespace Elastos
