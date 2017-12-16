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

#ifndef __ELASTOS_DROID_MEDIA_AUDIOPORTEVENTHANDLER_H__
#define __ELASTOS_DROID_MEDIA_AUDIOPORTEVENTHANDLER_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Media.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * The AudioPortEventHandler handles AudioManager.OnAudioPortUpdateListener callbacks
 * posted from JNI
 * @hide
 */
class AudioPortEventHandler
    : public Object
{
private:
    class EventHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("AudioPortEventHandler::EventHandler")

        EventHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ AudioPortEventHandler* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

public:
    AudioPortEventHandler();

    virtual ~AudioPortEventHandler();

    CARAPI constructor(
        /* [in] */ IAudioManager* audioManager);

    CARAPI RegisterListener(
        /* [in] */ IAudioManagerOnAudioPortUpdateListener* l);

    CARAPI UnregisterListener(
        /* [in] */ IAudioManagerOnAudioPortUpdateListener* l);

    CARAPI Handler(
        /* [out] */ IHandler** result);

    static CARAPI PostEventFromNative(
        /* [in] */ IWeakReference* module_ref,
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ IInterface* obj);

private:
    CARAPI NativeSetup(
        /* [in] */ IWeakReference* module_this);

    CARAPI NativeFinalize();

public:
    const static Int32 AUDIOPORT_EVENT_PORT_LIST_UPDATED;
    const static Int32 AUDIOPORT_EVENT_PATCH_LIST_UPDATED;
    const static Int32 AUDIOPORT_EVENT_SERVICE_DIED;
    const static Int32 AUDIOPORT_EVENT_NEW_LISTENER;

private:
    AutoPtr<IHandler> mHandler;
    AutoPtr<IArrayList> mListeners;
    IAudioManager* mAudioManager; // AudioManager holder this

    Int64 mNativeCallback;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_AUDIOPORTEVENTHANDLER_H__
