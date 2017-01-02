//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
