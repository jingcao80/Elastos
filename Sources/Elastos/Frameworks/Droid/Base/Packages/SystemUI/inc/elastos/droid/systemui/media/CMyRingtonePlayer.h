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

#ifndef __ELASTOS_DROID_SYSTEMUI_MEDIA_CMYRINGTONEPLAYER_H__
#define __ELASTOS_DROID_SYSTEMUI_MEDIA_CMYRINGTONEPLAYER_H__

#include "_Elastos_Droid_SystemUI_Media_CMyRingtonePlayer.h"
#include "elastos/droid/systemui/media/CRingtonePlayer.h"

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Media{

class CRingtonePlayer;

CarClass(CMyRingtonePlayer)
    , public Object
    , public IIRingtonePlayer
    , public IBinder
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ ISystemUI* host);

    //@Override
    CARAPI Play(
        /* [in] */ IBinder* token,
        /* [in] */ IUri* uri,
        /* [in] */ IAudioAttributes* aa);

    //@Override
    CARAPI Stop(
        /* [in] */ IBinder* token);

    //@Override
    CARAPI IsPlaying(
        /* [in] */ IBinder* token,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI PlayAsync(
        /* [in] */ IUri* uri,
        /* [in] */ IUserHandle* user,
        /* [in] */ Boolean looping,
        /* [in] */ IAudioAttributes* aa);

    //@Override
    CARAPI StopAsync();

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI SetVolume(
        /* [in] */ IBinder* token,
        /* [in] */ Float volume);

private:
    CRingtonePlayer* mHost;
};


} // namespace Media
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_MEDIA_CMYRINGTONEPLAYER_H__
