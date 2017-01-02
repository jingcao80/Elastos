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

#ifndef _ELASTOS_DROID_PROVIDERS_MEDIA_MEDIATHUMBREQUEST_H__
#define _ELASTOS_DROID_PROVIDERS_MEDIA_MEDIATHUMBREQUEST_H__

#include <elastos/core/Object.h>
#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Providers.Media.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Net::IUri;
using Elastos::Utility::IRandom;
using Elastos::Core::IComparator;

namespace Elastos {
namespace Droid {
namespace Providers {
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
        CAR_INTERFACE_DECL()

        MediaThumbRequestComparator();

        ~MediaThumbRequestComparator();

        CARAPI Compare(
            /* [in] */ IInterface* _r1,
            /* [in] */ IInterface* _r2,
            /* [out] */ Int32* result);
    };

public:
    CAR_INTERFACE_DECL();

    MediaThumbRequest();

    virtual ~MediaThumbRequest();

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

protected:
    static CARAPI GetComparator(
        /* [out] */ IComparator** result);

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
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_PROVIDERS_MEDIA_MEDIATHUMBREQUEST_H__