
#ifndef __ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_SOUNDTRIGGERMODULE_H__
#define __ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_SOUNDTRIGGERMODULE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::Handler;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Soundtrigger {

class SoundTriggerModule
    : public Object
    , public ISoundTriggerModule
{
private:
    class MyHandler : public Handler
    {
    public:
        TO_STRING_IMPL("SoundTriggerModule::MyHandler")

        MyHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ ISoundTriggerStatusListener* mdst);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        AutoPtr<ISoundTriggerStatusListener> mListener;

    };

    class NativeEventHandlerDelegate : public Object
    {
    public:
        NativeEventHandlerDelegate(
            /* [in] */ ISoundTriggerStatusListener* listener,
            /* [in] */ IHandler* handler);

        CARAPI GetHandler(
            /* [out] */ IHandler** handler);
    private:
        AutoPtr<IHandler> mHandler;
    };
public:
    CAR_INTERFACE_DECL()

    SoundTriggerModule();

    virtual ~SoundTriggerModule();

    CARAPI constructor(
        /* [in] */ Int32 moduleId,
        /* [in] */ ISoundTriggerStatusListener* listener,
        /* [in] */ IHandler* handler);

    /**
     * Detach from this module. The {@link SoundTrigger.StatusListener} callback will not be called
     * anymore and associated resources will be released.
     * */
    CARAPI NativeDetach();

    /**
     * Load a {@link SoundTrigger.SoundModel} to the hardware. A sound model must be loaded in
     * order to start listening to a key phrase in this model.
     * @param model The sound model to load.
     * @param soundModelHandle an array of int where the sound model handle will be returned.
     * @return - {@link SoundTrigger#STATUS_OK} in case of success
     *         - {@link SoundTrigger#STATUS_ERROR} in case of unspecified error
     *         - {@link SoundTrigger#STATUS_PERMISSION_DENIED} if the caller does not have
     *         system permission
     *         - {@link SoundTrigger#STATUS_NO_INIT} if the native service cannot be reached
     *         - {@link SoundTrigger#STATUS_BAD_VALUE} if parameters are invalid
     *         - {@link SoundTrigger#STATUS_DEAD_OBJECT} if the binder transaction to the native
     *         service fails
     *         - {@link SoundTrigger#STATUS_INVALID_OPERATION} if the call is out of sequence
     */
    CARAPI NativeLoadSoundModel(
        /* [in] */ ISoundTriggerSoundModel* model,
        /* [in] */ ArrayOf<Int32>* soundModelHandle,
        /* [out] */ Int32* result);

    /**
     * Unload a {@link SoundTrigger.SoundModel} and abort any pendiong recognition
     * @param soundModelHandle The sound model handle
     * @return - {@link SoundTrigger#STATUS_OK} in case of success
     *         - {@link SoundTrigger#STATUS_ERROR} in case of unspecified error
     *         - {@link SoundTrigger#STATUS_PERMISSION_DENIED} if the caller does not have
     *         system permission
     *         - {@link SoundTrigger#STATUS_NO_INIT} if the native service cannot be reached
     *         - {@link SoundTrigger#STATUS_BAD_VALUE} if the sound model handle is invalid
     *         - {@link SoundTrigger#STATUS_DEAD_OBJECT} if the binder transaction to the native
     *         service fails
     */
    CARAPI NativeUnloadSoundModel(
        /* [in] */ Int32 soundModelHandle,
        /* [out] */ Int32* result);

    /**
     * Start listening to all key phrases in a {@link SoundTrigger.SoundModel}.
     * Recognition must be restarted after each callback (success or failure) received on
     * the {@link SoundTrigger.StatusListener}.
     * @param soundModelHandle The sound model handle to start listening to
     * @param config contains configuration information for this recognition request:
     *  recognition mode, keyphrases, users, minimum confidence levels...
     * @return - {@link SoundTrigger#STATUS_OK} in case of success
     *         - {@link SoundTrigger#STATUS_ERROR} in case of unspecified error
     *         - {@link SoundTrigger#STATUS_PERMISSION_DENIED} if the caller does not have
     *         system permission
     *         - {@link SoundTrigger#STATUS_NO_INIT} if the native service cannot be reached
     *         - {@link SoundTrigger#STATUS_BAD_VALUE} if the sound model handle is invalid
     *         - {@link SoundTrigger#STATUS_DEAD_OBJECT} if the binder transaction to the native
     *         service fails
     *         - {@link SoundTrigger#STATUS_INVALID_OPERATION} if the call is out of sequence
     */
    CARAPI NativeStartRecognition(
        /* [in] */ Int32 soundModelHandle,
        /* [in] */ ISoundTriggerRecognitionConfig* config,
        /* [out] */ Int32* result);

    /**
     * Stop listening to all key phrases in a {@link SoundTrigger.SoundModel}
     * @param soundModelHandle The sound model handle to stop listening to
     * @return - {@link SoundTrigger#STATUS_OK} in case of success
     *         - {@link SoundTrigger#STATUS_ERROR} in case of unspecified error
     *         - {@link SoundTrigger#STATUS_PERMISSION_DENIED} if the caller does not have
     *         system permission
     *         - {@link SoundTrigger#STATUS_NO_INIT} if the native service cannot be reached
     *         - {@link SoundTrigger#STATUS_BAD_VALUE} if the sound model handle is invalid
     *         - {@link SoundTrigger#STATUS_DEAD_OBJECT} if the binder transaction to the native
     *         service fails
     *         - {@link SoundTrigger#STATUS_INVALID_OPERATION} if the call is out of sequence
     */
    CARAPI NativeStopRecognition(
        /* [in] */ Int32 soundModelHandle,
        /* [out] */ Int32* result);

    CARAPI GetNativeContext(
        /* [out] */ Int64* context);

    CARAPI GetId(
        /* [out] */ Int32* id);

    CARAPI SetNativeContext(
        /* [in] */ Int64 context);

private:
    friend class JNISoundTriggerCallback;
    CARAPI_(void) native_setup(
        /* [in] */ IInterface* module_this);

    CARAPI_(void) native_finalize();

    //@SuppressWarnings("unused")
    static CARAPI PostEventFromNative(
        /* [in] */ IInterface* module_ref,
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ IInterface* obj);
private:
    Int64 mNativeContext;

    Int32 mId;
    AutoPtr<NativeEventHandlerDelegate> mEventHandlerDelegate;

    // to be kept in sync with core/jni/android_hardware_SoundTrigger.cpp
    static const Int32 EVENT_RECOGNITION;
    static const Int32 EVENT_SERVICE_DIED;
    static const Int32 EVENT_SOUNDMODEL;
    static const Int32 EVENT_SERVICE_STATE_CHANGE;
};

} // namespace Soundtrigger
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_SOUNDTRIGGERMODULE_H__
