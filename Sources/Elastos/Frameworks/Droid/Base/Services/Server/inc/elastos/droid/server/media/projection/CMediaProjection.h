
#ifndef __ELASTOS_DROID_SERVER_MEDIA_PROJECTION_CMEDIAPROJECTION_H__
#define __ELASTOS_DROID_SERVER_MEDIA_PROJECTION_CMEDIAPROJECTION_H__

#include "_Elastos_Droid_Server_Media_Projection_CMediaProjection.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Media::Projection::IIMediaProjectionCallback;
using Elastos::Droid::Media::Projection::IIMediaProjection;
using Elastos::Droid::Media::Projection::IMediaProjectionInfo;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Media {
namespace Projection {

class MediaProjectionManagerService;

CarClass(CMediaProjection)
    , public Object
    , public IIMediaProjection
    , public IBinder
{
private:
    class DeathEater
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        DeathEater(
            /* [in] */ IIMediaProjectionCallback* cb,
            /* [in] */ CMediaProjection* host)
            : mCallback(cb)
            , mHost(host)
        {}

        //@Override
        CARAPI ProxyDied();

    private:
        AutoPtr<IIMediaProjectionCallback> mCallback;
        CMediaProjection* mHost;
    };

public:
    CMediaProjection();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Int32 type,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName,
        /* [in] */ Handle64 host);

    CARAPI CanProjectVideo(
        /* [out] */ Boolean* result);

    CARAPI CanProjectSecureVideo(
        /* [out] */ Boolean* result);

    CARAPI CanProjectAudio(
        /* [out] */ Boolean* result);

    CARAPI ApplyVirtualDisplayFlags(
        /* [in] */ Int32 flags,
        /* [out] */ Int32* result);

    CARAPI Start(
        /* [in] */ IIMediaProjectionCallback* cb);

    CARAPI Stop();

    CARAPI RegisterCallback(
        /* [in] */ IIMediaProjectionCallback* cb);

    CARAPI UnregisterCallback(
        /* [in] */ IIMediaProjectionCallback* cb);

    CARAPI_(AutoPtr<IMediaProjectionInfo>) GetProjectionInfo();

    CARAPI ToString(
        /* [out] */ String* str);

public:
    Int32 mUid;
    String mPackageName;
    AutoPtr<IUserHandle> mUserHandle;

private:
    MediaProjectionManagerService* mHost;

    AutoPtr<IBinder> mToken;
    AutoPtr<IProxyDeathRecipient> mDeathEater;
    Int32 mType;
};

} // namespace Projection
} // namespace Media
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_MEDIA_PROJECTION_CMEDIAPROJECTION_H__
