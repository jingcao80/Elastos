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

#ifndef __ELASTOS_DROID_MEDIA_CMEDIACODECINFOCODECCAPABILITIESHELPER_H__
#define __ELASTOS_DROID_MEDIA_CMEDIACODECINFOCODECCAPABILITIESHELPER_H__

#include "_Elastos_Droid_Media_CMediaCodecInfoCodecCapabilitiesHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaCodecInfoCodecCapabilitiesHelper)
    , public Singleton
    , public IMediaCodecInfoCodecCapabilitiesHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Retrieve the codec capabilities for a certain {@code mime type}, {@code
     * profile} and {@code level}.  If the type, or profile-level combination
     * is not understood by the framework, it returns null.
     */
    CARAPI CreateFromProfileLevel(
        /* [in] */ const String& mime,
        /* [in] */ Int32 profile,
        /* [in] */ Int32 level,
        /* [out] */ IMediaCodecInfoCodecCapabilities** result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CMEDIACODECINFOCODECCAPABILITIESHELPER_H__
