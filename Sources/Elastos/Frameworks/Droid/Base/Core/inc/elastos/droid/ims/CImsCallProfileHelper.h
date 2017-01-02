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

#ifndef __ELASTOS_DROID_IMS_CIMSCALLPROFILEHELPER_H__
#define __ELASTOS_DROID_IMS_CIMSCALLPROFILEHELPER_H__

#include "_Elastos_Droid_Ims_CImsCallProfileHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Ims {

CarClass(CImsCallProfileHelper)
    , public Singleton
    , public IImsCallProfileHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Converts from the call types defined in {@link com.android.ims.ImsCallProfile} to the
     * video state values defined in {@link VideoProfile}.
     *
     * @param callType The call type.
     * @return The video state.
     */
    CARAPI GetVideoStateFromCallType(
        /* [in] */ Int32 callType,
        /* [out] */ Int32* result);

    /**
     * Converts from the video state values defined in {@link VideoProfile}
     * to the call types defined in {@link ImsCallProfile}.
     *
     * @param videoState The video state.
     * @return The call type.
     */
    CARAPI GetCallTypeFromVideoState(
        /* [in] */ Int32 videoState,
        /* [out] */ Int32* result);

    /**
     * Translate presentation value to OIR value
     * @param presentation
     * @return OIR valuse
     */
    CARAPI PresentationToOIR(
        /* [in] */ Int32 presentation,
        /* [out] */ Int32* result);

    /**
     * Translate OIR value to presentation value
     * @param oir value
     * @return presentation value
     */
    CARAPI OIRToPresentation(
        /* [in] */ Int32 oir,
        /* [out] */ Int32* result);
};

} //namespace Ims
} //namespace Droid
} //namespace Elastos

#endif // __ELASTOS_DROID_IMS_CIMSCALLPROFILEHELPER_H__
