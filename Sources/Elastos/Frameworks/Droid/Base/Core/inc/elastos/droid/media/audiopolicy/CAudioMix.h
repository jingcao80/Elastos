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

#ifndef __ELASTOS_DROID_MEDIA_AUDIOPOLICY_CAUDIOMIX_H__
#define __ELASTOS_DROID_MEDIA_AUDIOPOLICY_CAUDIOMIX_H__

#include "_Elastos_Droid_Media_AudioPolicy_CAudioMix.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {
namespace AudioPolicy {

/**
 * @hide CANDIDATE FOR PUBLIC API
 */
CarClass(CAudioMix)
    , public Object
    , public IAudioMix
{
public:
    CAudioMix();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CAudioMix();

    /**
     * All parameters are guaranteed valid through the Builder.
     */
    CARAPI constructor(
        /* [in] */ IAudioMixingRule * rule,
        /* [in] */ IAudioFormat * format,
        /* [in] */ Int32 routeFlags);

private:
    Int32 GetRouteFlags();

    CARAPI_(AutoPtr<IAudioFormat>) GetFormat();

    CARAPI_(AutoPtr<IAudioMixingRule>) GetRule();

private:
    AutoPtr<IAudioMixingRule> mRule;
    AutoPtr<IAudioFormat> mFormat;
    Int32 mRouteFlags;

};

} // namespace AudioPolicy
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_AUDIOPOLICY_CAUDIOMIX_H__
