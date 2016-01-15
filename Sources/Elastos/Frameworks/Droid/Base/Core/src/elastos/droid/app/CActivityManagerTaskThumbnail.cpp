
#include "elastos/droid/app/CActivityManagerTaskThumbnail.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/CParcelFileDescriptor.h"
#include "elastos/droid/graphics/CBitmap.h"

using Elastos::Droid::Os::CParcelFileDescriptor;
using Elastos::Droid::Graphics::CBitmap;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL_2(CActivityManagerTaskThumbnail, Object, IActivityManagerTaskThumbnail, IParcelable)

CAR_OBJECT_IMPL(CActivityManagerTaskThumbnail)

CActivityManagerTaskThumbnail::CActivityManagerTaskThumbnail()
{
}

ECode CActivityManagerTaskThumbnail::constructor()
{
    return NOERROR;
}

ECode CActivityManagerTaskThumbnail::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    if (mMainThumbnail != NULL) {
        FAIL_RETURN(dest->WriteInt32(1));
        IParcelable::Probe(mMainThumbnail)->WriteToParcel(dest);
    } else {
        FAIL_RETURN(dest->WriteInt32(0));
    }

    if (mThumbnailFileDescriptor != NULL) {
        FAIL_RETURN(dest->WriteInt32(1));
        IParcelable::Probe(mThumbnailFileDescriptor)->WriteToParcel(dest);
    } else {
        FAIL_RETURN(dest->WriteInt32(0));
    }

    return NOERROR;
}

ECode CActivityManagerTaskThumbnail::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);

    Int32 value;
    FAIL_RETURN(source->ReadInt32(&value));
    if (value != 0) {
        CBitmap::New((IBitmap**)&mMainThumbnail);
        IParcelable::Probe(mMainThumbnail)->ReadFromParcel(source);
    } else {
        mMainThumbnail = NULL;
    }

    FAIL_RETURN(source->ReadInt32(&value));
    if (value != 0)  {
        CParcelFileDescriptor::New((IParcelFileDescriptor**)&mThumbnailFileDescriptor);
        IParcelable::Probe(mThumbnailFileDescriptor)->ReadFromParcel(source);
    } else {
        mThumbnailFileDescriptor = NULL;
    }

    return NOERROR;
}

ECode CActivityManagerTaskThumbnail::GetMainThumbnail(
    /* [out] */ IBitmap** thumbnail)
{
    VALIDATE_NOT_NULL(thumbnail);
    *thumbnail = mMainThumbnail;
    REFCOUNT_ADD(*thumbnail)
    return NOERROR;
}

ECode CActivityManagerTaskThumbnail::SetMainThumbnail(
    /* [in] */ IBitmap* thumbnail)
{
    mMainThumbnail = thumbnail;
    return NOERROR;
}

ECode CActivityManagerTaskThumbnail::GetThumbnailFileDescriptor(
    /* [out] */ IParcelFileDescriptor** pfd)
{
    VALIDATE_NOT_NULL(pfd);
    *pfd = mThumbnailFileDescriptor;
    REFCOUNT_ADD(*pfd)
    return NOERROR;
}

ECode CActivityManagerTaskThumbnail::SetThumbnailFileDescriptor(
    /* [in] */ IParcelFileDescriptor* pfd)
{
    mThumbnailFileDescriptor = pfd;
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos

