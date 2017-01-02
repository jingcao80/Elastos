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

#ifndef __ELASTOS_DROID_MEDIA_SESSION_CMEDIASESSIONTOKEN_H__
#define __ELASTOS_DROID_MEDIA_SESSION_CMEDIASESSIONTOKEN_H__

#include "_Elastos_Droid_Media_Session_CMediaSessionToken.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

/**
 * Represents an ongoing session. This may be passed to apps by the session
 * owner to allow them to create a {@link MediaController} to communicate with
 * the session.
 */
CarClass(CMediaSessionToken)
    , public Object
    , public IMediaSessionToken
    , public IParcelable
{
public:
    CMediaSessionToken();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CMediaSessionToken();

    CARAPI constructor();

    /**
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IISessionController * binder);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean * result);

    CARAPI GetHashCode(
        /* [out] */ Int32 *hashCode);

    CARAPI GetBinder(
        /* [out] */ IISessionController ** result);

public:
    AutoPtr<IISessionController> mBinder;
};

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_SESSION_CMEDIASESSIONTOKEN_H__
