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

#ifndef __ELASTOS_DROID_IMS_CIMSCALLPROFILE_H__
#define __ELASTOS_DROID_IMS_CIMSCALLPROFILE_H__

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Telecom.h"
#include "_Elastos_Droid_Ims_CImsCallProfile.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Ims {

CarClass(CImsCallProfile)
    , public Object
    , public IImsCallProfile
    , public IParcelable
{
public:
    CImsCallProfile();

    virtual ~CImsCallProfile();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 serviceType,
        /* [in] */ Int32 callType);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI GetCallExtra(
        /* [in] */ const String& name,
        /* [out] */ String* result);

    CARAPI GetCallExtra(
        /* [in] */ const String& name,
        /* [in] */ const String& defaultValue,
        /* [out] */ String* result);

    CARAPI GetCallExtraBoolean(
        /* [in] */ const String& name,
        /* [out] */ Boolean* result);

    CARAPI GetCallExtraBoolean(
        /* [in] */ const String& name,
        /* [in] */ Boolean defaultValue,
        /* [out] */ Boolean* result);

    CARAPI GetCallExtraInt32(
        /* [in] */ const String& name,
        /* [out] */ Int32* result);

    CARAPI GetCallExtraInt32(
        /* [in] */ const String& name,
        /* [in] */ Int32 defaultValue,
        /* [out] */ Int32* result);

    CARAPI SetCallExtra(
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    CARAPI SetCallExtraBoolean(
        /* [in] */ const String& name,
        /* [in] */ Boolean value);

    CARAPI SetCallExtraInt32(
        /* [in] */ const String& name,
        /* [in] */ Int32 value);

    CARAPI UpdateCallType(
        /* [in] */ IImsCallProfile* profile);

    CARAPI UpdateCallExtras(
        /* [in] */ IImsCallProfile* profile);

    /**
     * Converts from the call types defined in {@link com.android.ims.ImsCallProfile} to the
     * video state values defined in {@link VideoProfile}.
     *
     * @param callType The call type.
     * @return The video state.
     */
    static CARAPI GetVideoStateFromCallType(
        /* [in] */ Int32 callType,
        /* [out] */ Int32* result);

    /**
     * Converts from the video state values defined in {@link VideoProfile}
     * to the call types defined in {@link ImsCallProfile}.
     *
     * @param videoState The video state.
     * @return The call type.
     */
    static CARAPI GetCallTypeFromVideoState(
        /* [in] */ Int32 videoState,
        /* [out] */ Int32* result);

    /**
     * Translate presentation value to OIR value
     * @param presentation
     * @return OIR valuse
     */
    static CARAPI PresentationToOIR(
        /* [in] */ Int32 presentation,
        /* [out] */ Int32* result);

    /**
     * Translate OIR value to presentation value
     * @param oir value
     * @return presentation value
     */
    static CARAPI OIRToPresentation(
        /* [in] */ Int32 oir,
        /* [out] */ Int32* result);

private:
    static CARAPI_(Boolean) IsVideoStateSet(
        /* [in] */ Int32 videoState,
        /* [in] */ Int32 videoStateToCheck);

public:
    Int32 mServiceType;
    Int32 mCallType;
    AutoPtr<IBundle> mCallExtras;
    AutoPtr<IImsStreamMediaProfile> mMediaProfile;
};

} // namespace Ims
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_IMS_CIMSCALLPROFILE_H__
