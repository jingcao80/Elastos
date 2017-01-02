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

#ifndef __ELASTOS_DROID_MEDIA_CAUDIOMANAGERHELPER_H__
#define __ELASTOS_DROID_MEDIA_CAUDIOMANAGERHELPER_H__

#include "_Elastos_Droid_Media_CAudioManagerHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CAudioManagerHelper)
    , public Singleton
    , public IAudioManagerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Checks valid ringer mode values.
     *
     * @return true if the ringer mode indicated is valid, false otherwise.
     *
     * @see #setRingerMode(int)
     * @hide
     */
    CARAPI IsValidRingerMode(
        /* [in] */ Int32 rightMode,
        /* [out] */ Boolean* result);

    /**
     * Return true if the device code corresponds to an output device.
     * @hide
     */
    CARAPI IsOutputDevice(
        /* [in] */ Int32 device,
        /* [out] */ Boolean* result);

    /**
     * Return true if the device code corresponds to an input device.
     * @hide
     */
    CARAPI IsInputDevice(
        /* [in] */ Int32 device,
        /* [out] */ Boolean* result);
};

} // namespace Media
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CAUDIOMANAGERHELPER_H__
