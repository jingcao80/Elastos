#ifndef _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_MEDIATHUMBREQUEST_H__
#define _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_MEDIATHUMBREQUEST_H__

#include <elastos/core/Object.h>
#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Providers.MediaProvider.Media.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Net::IUri;
using Elastos::Utility::IRandom;
using Elastos::Core::IComparator;

namespace Elastos {
namespace Providers {
namespace MediaProvider {
namespace Media {

class MediaThumbRequest
    : public Object
    , public IMediaThumbRequest
{
    friend class MediaProvider;
    friend class CMediaThumbRequestHelper;

private:
    class MediaThumbRequestComparator
        : public Object
        , public IComparator
    {
    public:
        MediaThumbRequestComparator(
            /* [in] */ IMediaThumbRequest* r1,
            /* [in] */ IMediaThumbRequest* r2);

        ~MediaThumbRequestComparator();

        CAR_INTERFACE_DECL()

        CARAPI Compare(
            /* [in] */ IInterface* _r1,
            /* [in] */ IInterface* _r2,
            /* [out] */ Int32* result);

    private:
        AutoPtr<IMediaThumbRequest> mR1;
        AutoPtr<IMediaThumbRequest> mR2;
    };

public:
    MediaThumbRequest();

    virtual ~MediaThumbRequest();

    CAR_INTERFACE_DECL();

    CARAPI constructor(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& path,
        /* [in] */ IUri* uri,
        /* [in] */ Int32 priority,
        /* [in] */ Int64 magic);

    CARAPI UpdateDatabase(
        /* [in] */ IBitmap* thumbnail,
        /* [out] */ IUri** result);

    CARAPI Execute();

    CARAPI GetPriority(
        /* [out] */ Int32* result);

    CARAPI GetRequestTime(
        /* [out] */ Int32* time);

    CARAPI GetCallingPid(
        /* [out] */ Int32* result);

public:
    AutoPtr<IContentResolver> mCr;
    String mPath;
    Int64 mRequestTime;
    Int32 mCallingPid;
    Int64 mGroupId;
    Int32 mPriority;
    AutoPtr<IUri> mUri;
    AutoPtr<IUri> mThumbUri;
    String mOrigColumnName;
    Boolean mIsVideo;
    Int64 mOrigId;
    State mState;
    Int64 mMagic;

private:
    static const String TAG;
    static const Int32 PRIORITY_LOW;
    static const Int32 PRIORITY_NORMAL;
    static const Int32 PRIORITY_HIGH;
    static const Int32 PRIORITY_CRITICAL;

    static AutoPtr<ArrayOf<String> > THUMB_PROJECTION;

    static AutoPtr<IRandom> sRandom;
};

} // namespace Media
} // namespace MediaProvider
} // namespace Providers
} // namespace Elastos

#endif // _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_MEDIATHUMBREQUEST_H__