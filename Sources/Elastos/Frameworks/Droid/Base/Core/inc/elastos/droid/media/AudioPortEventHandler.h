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
            /* [in] */ AudioPortEventHandler* host,
            /* [in] */ ILooper* looper)
            : Handler(looper)
            , mHost(host)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        AudioPortEventHandler* mHost;
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
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_AUDIOPORTEVENTHANDLER_H__
