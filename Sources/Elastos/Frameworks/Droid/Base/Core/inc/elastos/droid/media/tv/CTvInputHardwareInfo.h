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

#ifndef __ELASTOS_DROID_MEDIA_TV_CTVINPUTHARDWAREINFO_H__
#define __ELASTOS_DROID_MEDIA_TV_CTVINPUTHARDWAREINFO_H__

#include "_Elastos_Droid_Media_Tv_CTvInputHardwareInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

/**
 * Simple container for information about TV input hardware.
 * Not for third-party developers.
 *
 * @hide
 */
// @SystemApi
CarClass(CTvInputHardwareInfo)
    , public Object
    , public ITvInputHardwareInfo
    , public IParcelable
{
public:
    friend class CTvInputHardwareInfoBuilder;

    CTvInputHardwareInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CTvInputHardwareInfo();

    CARAPI constructor();

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetDeviceId(
        /* [out] */ Int32* result);

    CARAPI GetType(
        /* [out] */ Int32* result);

    CARAPI GetAudioType(
        /* [out] */ Int32* result);

    CARAPI GetAudioAddress(
        /* [out] */ String* result);

    CARAPI GetHdmiPortId(
        /* [out] */ Int32* result);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    static String TAG;

    Int32 mDeviceId;
    Int32 mType;
    Int32 mAudioType;
    Int32 mHdmiPortId;
    String mAudioAddress;
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_CTVINPUTHARDWAREINFO_H__
