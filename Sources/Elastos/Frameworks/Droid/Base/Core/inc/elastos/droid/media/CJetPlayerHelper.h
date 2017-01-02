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

#ifndef __ELASTOS_DROID_MEDIA_CJETPLAYERHELPER_H__
#define __ELASTOS_DROID_MEDIA_CJETPLAYERHELPER_H__

#include "_Elastos_Droid_Media_CJetPlayerHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CJetPlayerHelper)
    , public Singleton
    , public IJetPlayerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Factory method for the JetPlayer class.
     * @return the singleton JetPlayer instance
     */
    CARAPI GetJetPlayer(
        /* [out] */ IJetPlayer** player);

    /**
     * Returns the maximum number of simultaneous MIDI tracks supported by JetPlayer
     */
    CARAPI GetMaxTracks(
        /* [out] */ Int32* tracks);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_MEDIA_CJETPLAYERHELPER_H__
