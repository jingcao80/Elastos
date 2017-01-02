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

#ifndef __ELASTOS_DROID_INPUTMETHOD_PINYIN_SOUNDMANAGER_H__
#define __ELASTOS_DROID_INPUTMETHOD_PINYIN_SOUNDMANAGER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Media.h"
#include "_Elastos.Droid.InputMethod.Pinyin.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

/**
 * Class used to manage related sound resources.
 */
class SoundManager
    : public Object
{
public:
    SoundManager(
        /* [in] */ IContext* ctx);

    CARAPI_(void) UpdateRingerMode();

    static CARAPI_(AutoPtr<SoundManager>) GetInstance(
        /* [in] */ IContext* context);

    CARAPI_(void) PlayKeyDown();

private:
    static AutoPtr<SoundManager> sInstance;
    AutoPtr<IContext> mContext;
    AutoPtr<IAudioManager> mAudioManager;
    // Align sound effect volume on music volume
    const Float FX_VOLUME;
    Boolean mSilentMode;
};

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_INPUTMETHOD_PINYIN_SOUNDMANAGER_H__
