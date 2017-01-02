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

#ifndef __ELASTOS_DROID_IMS_CIMSSTREAMMEDIAPROFILE_H__
#define __ELASTOS_DROID_IMS_CIMSSTREAMMEDIAPROFILE_H__

#include "_Elastos_Droid_Ims_CImsStreamMediaProfile.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Ims {

CarClass(CImsStreamMediaProfile)
    , public Object
    , public IImsStreamMediaProfile
    , public IParcelable
{
public:
    CImsStreamMediaProfile();

    virtual ~CImsStreamMediaProfile();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 audioQuality,
        /* [in] */ Int32 audioDirection,
        /* [in] */ Int32 videoQuality,
        /* [in] */ Int32 videoDirection);

    CARAPI CopyFrom(
        /* [in] */ IImsStreamMediaProfile* profile);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ToString(
        /* [out] */ String* result);

public:
    // Audio related information
    Int32 mAudioQuality;
    Int32 mAudioDirection;
    // Video related information
    Int32 mVideoQuality;
    Int32 mVideoDirection;

private:
    static const String TAG;
};

} // namespace Ims
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_IMS_CIMSSTREAMMEDIAPROFILE_H__
