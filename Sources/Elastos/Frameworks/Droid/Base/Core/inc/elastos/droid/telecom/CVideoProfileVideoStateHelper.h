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

#ifndef __ELASTOS_DROID_TELECOM_CVIDEOPROFILEVIDEOSTATEHELPER_H__
#define __ELASTOS_DROID_TELECOM_CVIDEOPROFILEVIDEOSTATEHELPER_H__

#include "_Elastos_Droid_Telecom_CVideoProfileVideoStateHelper.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CVideoProfileVideoStateHelper)
    , public Singleton
    , public IVideoProfileVideoStateHelper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    /**
     * Whether the video state is audio only.
     * @param videoState The video state.
     * @return Returns true if the video state is audio only.
     */
    CARAPI IsAudioOnly(
        /* [in] */ Int32 videoState,
        /* [out] */ Boolean* result);

    /**
     * Whether the video state is any of the video type
     * @param videoState The video state.
     * @return Returns true if the video state TX or RX or Bidirectional
     */
    CARAPI IsVideo(
        /* [in] */ Int32 videoState,
        /* [out] */ Boolean* result);

    /**
     * Whether the video transmission is enabled.
     * @param videoState The video state.
     * @return Returns true if the video transmission is enabled.
     */
    CARAPI IsTransmissionEnabled(
        /* [in] */ Int32 videoState,
        /* [out] */ Boolean* result);

    /**
     * Whether the video reception is enabled.
     * @param videoState The video state.
     * @return Returns true if the video transmission is enabled.
     */
    CARAPI IsReceptionEnabled(
        /* [in] */ Int32 videoState,
        /* [out] */ Boolean* result);

    /**
     * Whether the video signal is bi-directional.
     * @param videoState
     * @return Returns true if the video signal is bi-directional.
     */
    CARAPI IsBidirectional(
        /* [in] */ Int32 videoState,
        /* [out] */ Boolean* result);

    /**
     * Whether the video is paused.
     * @param videoState The video state.
     * @return Returns true if the video is paused.
     */
    CARAPI IsPaused(
        /* [in] */ Int32 videoState,
        /* [out] */ Boolean* result);

    /**
     * Determines if a specified state is set in a videoState bit-mask.
     *
     * @param videoState The video state bit-mask.
     * @param state The state to check.
     * @return {@code True} if the state is set.
     * {@hide}
     */
    CARAPI HasState(
        /* [in] */ Int32 videoState,
        /* [in] */ Int32 state,
        /* [out] */ Boolean* result);

};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELECOM_CVIDEOPROFILEVIDEOSTATEHELPER_H__
