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

#ifndef __ELASTOS_DROID_TELECOM_AUDIOSTATE_H__
#define __ELASTOS_DROID_TELECOM_AUDIOSTATE_H__

#include "Elastos.Droid.Telecom.h"

#include <elastos/core/Object.h>
#include <elastos/core/StringBuffer.h>

using Elastos::Core::Object;
using Elastos::Core::StringBuffer;

namespace Elastos {
namespace Droid {
namespace Telecom {

class AudioState
    : public Object
    , public IAudioState
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    AudioState();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Boolean isMuted,
        /* [in] */ Int32 route,
        /* [in] */ Int32 supportedRouteMask);

    CARAPI constructor(
        /* [in] */ IAudioState* state);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* res);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /** True if the call is muted, false otherwise. */
    CARAPI GetIsMuted(
        /* [out] */ Boolean* result);

    /** The route to use for the audio stream. */
    CARAPI GetRoute(
        /* [out] */ Int32* result);

    /** Bit vector of all routes supported by this call. */
    CARAPI GetSupportedRouteMask(
        /* [out] */ Int32* result);

    static CARAPI_(String) AudioRouteToString(
        /* [in] */ Int32 route);

private:
    static CARAPI_(void) ListAppend(
        /* [in] */ StringBuffer* buffer,
        /* [in] */ const String& str);

public:
    /** True if the call is muted, false otherwise. */
    Boolean mIsMuted;

    /** The route to use for the audio stream. */
    Int32 mRoute;

    /** Bit vector of all routes supported by this call. */
    Int32 mSupportedRouteMask;
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_AUDIOSTATE_H__
