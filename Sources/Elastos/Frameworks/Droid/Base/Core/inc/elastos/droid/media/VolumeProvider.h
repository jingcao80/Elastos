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

#ifndef __ELASTOS_DROID_MEDIA_VOLUMEPROVIDER_H__
#define __ELASTOS_DROID_MEDIA_VOLUMEPROVIDER_H__

#include "Elastos.Droid.Media.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {

class ECO_PUBLIC VolumeProvider
    : public Object
    , public IVolumeProvider
{
public:
    VolumeProvider();

    virtual ~VolumeProvider();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ Int32 volumeControl,
        /* [in] */ Int32 maxVolume,
        /* [in] */ Int32 currentVolume);

    CARAPI GetVolumeControl(
        /* [out] */ Int32 * result);

    CARAPI GetMaxVolume(
        /* [out] */ Int32 * result);

    CARAPI GetCurrentVolume(
        /* [out] */ Int32 * result);

    CARAPI SetCurrentVolume(
        /* [in] */ Int32 currentVolume);

    CARAPI OnSetVolumeTo(
        /* [in] */ Int32 volume);

    CARAPI OnAdjustVolume(
        /* [in] */ Int32 direction);

    CARAPI SetCallback(
        /* [in] */ IVolumeProviderCallback * cb);

private:
    Int32 mControlType;
    Int32 mMaxVolume;
    Int32 mCurrentVolume;
    AutoPtr<IVolumeProviderCallback> mCallback;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_VOLUMEPROVIDER_H__