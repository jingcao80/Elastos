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

#ifndef __ELASTOS_DROID_MEDIA_AUDIOFX_AUDIOEFFECT_H__
#define __ELASTOS_DROID_MEDIA_AUDIOFX_AUDIOEFFECT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Media.h"
#include <elastos/core/Object.h>
#include "elastos/droid/os/Handler.h"

using Elastos::Utility::IUUID;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Handler;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

/**
 * AudioEffect is the base class for controlling audio effects provided by the android audio
 * framework.
 * <p>Applications should not use the AudioEffect class directly but one of its derived classes to
 * control specific effects:
 * <ul>
 *   <li> {@link android.media.audiofx.Equalizer}</li>
 *   <li> {@link android.media.audiofx.Virtualizer}</li>
 *   <li> {@link android.media.audiofx.BassBoost}</li>
 *   <li> {@link android.media.audiofx.PresetReverb}</li>
 *   <li> {@link android.media.audiofx.EnvironmentalReverb}</li>
 * </ul>
 * <p>If the audio effect is to be applied to a specific AudioTrack or MediaPlayer instance,
 * the application must specify the audio session ID of that instance when creating the AudioEffect.
 * (see {@link android.media.MediaPlayer#getAudioSessionId()} for details on audio sessions).
 * To apply an effect to the global audio output mix, session 0 must be specified when creating the
 * AudioEffect.
 * <p>Creating an effect on the output mix (audio session 0) requires permission
 * {@link android.Manifest.permission#MODIFY_AUDIO_SETTINGS}
 * <p>Creating an AudioEffect object will create the corresponding effect engine in the audio
 * framework if no instance of the same effect type exists in the specified audio session.
 * If one exists, this instance will be used.
 * <p>The application creating the AudioEffect object (or a derived class) will either receive
 * control of the effect engine or not depending on the priority parameter. If priority is higher
 * than the priority used by the current effect engine owner, the control will be transfered to the
 * new object. Otherwise control will remain with the previous object. In this case, the new
 * application will be notified of changes in effect engine state or control ownership by the
 * appropiate listener.
 */

class AudioEffect
    : public Object
    , public IAudioEffect
{
private:
    /**
     * Helper class to handle the forwarding of native events to the appropriate
     * listeners
     */
    class NativeEventHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("AudioEffect::NativeEventHandler")

        NativeEventHandler(
            /* [in] */ AudioEffect* ae,
            /* [in] */ ILooper* looper);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        AudioEffect* mAudioEffect;
    };

public:
    CAR_INTERFACE_DECL()

    AudioEffect();

    virtual ~AudioEffect();

    // virtual CARAPI_(PInterface) Probe(
    //     /* [in] */ REIID riid) = 0;

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IUUID* type,
        /* [in] */ IUUID* uuid,
        /* [in] */ Int32 priority,
        /* [in] */ Int32 audioSession);

    /**
     * Releases the native AudioEffect resources. It is a good practice to
     * release the effect engine when not in use as control can be returned to
     * other applications or the native resources released.
     */
    CARAPI ReleaseResources();

    /**
     * Get the effect descriptor.
     *
     * @see android.media.audiofx.AudioEffect.Descriptor
     * @throws IllegalStateException
     */
    CARAPI GetDescriptor(
        /* [out] */ IAudioEffectDescriptor** descriptor);

    // --------------------------------------------------------------------------
    // Effects Enumeration
    // --------------------

    /**
    * Query all effects available on the platform. Returns an array of
    * {@link android.media.audiofx.AudioEffect.Descriptor} objects
    *
    * @throws IllegalStateException
    */

     static  CARAPI QueryEffects(
        /* [out, callee] */ ArrayOf<IAudioEffectDescriptor*>** descriptor);

    /**
     * Query all audio pre processing effects applied to the AudioRecord with the supplied
     * audio session ID. Returns an array of {@link android.media.audiofx.AudioEffect.Descriptor}
     * objects.
     * @param audioSession system wide unique audio session identifier.
     * @throws IllegalStateException
     * @hide
     */

    static CARAPI QueryPreProcessings(
        /* [in] */ Int32 audioSession,
        /* [out, callee] */ ArrayOf<IAudioEffectDescriptor*>** descriptor);

    /**
     * Checks if the device implements the specified effect type.
     * @param type the requested effect type.
     * @return true if the device implements the specified effect type, false otherwise.
     * @hide
     */
    static CARAPI IsEffectTypeAvailable(
        /* [in] */ IUUID* type,
        /* [out] */ Boolean* result);

    /**
     * Enable or disable the effect.
     * Creating an audio effect does not automatically apply this effect on the audio source. It
     * creates the resources necessary to process this effect but the audio signal is still bypassed
     * through the effect engine. Calling this method will make that the effect is actually applied
     * or not to the audio content being played in the corresponding audio session.
     *
     * @param enabled the requested enable state
     * @return {@link #SUCCESS} in case of success, {@link #ERROR_INVALID_OPERATION}
     *         or {@link #ERROR_DEAD_OBJECT} in case of failure.
     * @throws IllegalStateException
     */
    CARAPI SetEnabled(
        /* [in]  */ Boolean enabled,
        /* [out] */ Int32* result);

    /**
     * Set effect parameter. The setParameter method is provided in several
     * forms addressing most common parameter formats. This form is the most
     * generic one where the parameter and its value are both specified as an
     * array of bytes. The parameter and value type and length are therefore
     * totally free. For standard effect defined by OpenSL ES, the parameter
     * format and values must match the definitions in the corresponding OpenSL
     * ES interface.
     *
     * @param param the identifier of the parameter to set
     * @param value the new value for the specified parameter
     * @return {@link #SUCCESS} in case of success, {@link #ERROR_BAD_VALUE},
     *         {@link #ERROR_NO_MEMORY}, {@link #ERROR_INVALID_OPERATION} or
     *         {@link #ERROR_DEAD_OBJECT} in case of failure
     * @throws IllegalStateException
     * @hide
     */
    CARAPI SetParameter(
        /* [in] */ ArrayOf<Byte>* param,
        /* [in] */ ArrayOf<Byte>* value,
        /* [out] */ Int32* result);

    /**
     * Set effect parameter. The parameter and its value are integers.
     *
     * @see #setParameter(byte[], byte[])
     * @hide
     */
    CARAPI SetParameter(
        /* [in] */ Int32 param,
        /* [in] */ Int32 value,
        /* [out] */ Int32* result);

    /**
     * Set effect parameter. The parameter is an integer and the value is a
     * short integer.
     *
     * @see #setParameter(byte[], byte[])
     * @hide
     */
    CARAPI SetParameter(
        /* [in] */ Int32 param,
        /* [in] */ Int16 value,
        /* [out] */ Int32* result);

    /**
     * Set effect parameter. The parameter is an integer and the value is an
     * array of bytes.
     *
     * @see #setParameter(byte[], byte[])
     * @hide
     */
    CARAPI SetParameter(
        /* [in] */ Int32 param,
        /* [in] */ ArrayOf<Byte>* value,
        /* [out] */ Int32* result);

    /**
     * Set effect parameter. The parameter is an array of 1 or 2 integers and
     * the value is also an array of 1 or 2 integers
     *
     * @see #setParameter(byte[], byte[])
     * @hide
     */
    CARAPI SetParameter(
        /* [in] */ ArrayOf<Int32>* param,
        /* [in] */ ArrayOf<Int32>* value,
        /* [out] */ Int32* result);

    /**
     * Set effect parameter. The parameter is an array of 1 or 2 integers and
     * the value is an array of 1 or 2 short integers
     *
     * @see #setParameter(byte[], byte[])
     * @hide
     */
    CARAPI SetParameter(
        /* [in] */ ArrayOf<Int32>* param,
        /* [in] */ ArrayOf<Int16>* value,
        /* [out] */ Int32* result);

    /**
     * Set effect parameter. The parameter is an array of 1 or 2 integers and
     * the value is an array of bytes
     *
     * @see #setParameter(byte[], byte[])
     * @hide
     */
    CARAPI SetParameter(
        /* [in] */ ArrayOf<Int32>* param,
        /* [in] */ ArrayOf<Byte>* value,
        /* [out] */ Int32* result);

    /**
     * Get effect parameter. The getParameter method is provided in several
     * forms addressing most common parameter formats. This form is the most
     * generic one where the parameter and its value are both specified as an
     * array of bytes. The parameter and value type and length are therefore
     * totally free.
     *
     * @param param the identifier of the parameter to set
     * @param value the new value for the specified parameter
     * @return {@link #SUCCESS} in case of success, {@link #ERROR_BAD_VALUE},
     *         {@link #ERROR_NO_MEMORY}, {@link #ERROR_INVALID_OPERATION} or
     *         {@link #ERROR_DEAD_OBJECT} in case of failure When called, value.length
     *         indicates the maximum size of the returned parameters value. When
     *         returning, value.length is updated with the actual size of the
     *         returned value.
     * @throws IllegalStateException
     * @hide
     */
    CARAPI GetParameter(
        /* [in] */ ArrayOf<Byte>* param,
        /* [out] */ ArrayOf<Byte>* value,
        /* [out] */ Int32* status);

    /**
     * Get effect parameter. The parameter is an integer and the value is an
     * array of bytes.
     *
     * @see #getParameter(byte[], byte[])
     * @hide
     */
    CARAPI GetParameter(
        /* [in] */ Int32 param,
        /* [out] */ ArrayOf<Byte>* value,
        /* [out] */ Int32* status);

    /**
     * Get effect parameter. The parameter is an integer and the value is an
     * array of 1 or 2 integers
     *
     * @see #getParameter(byte[], byte[])
     * @hide
     */
    CARAPI GetParameter(
        /* [in] */ Int32 param,
        /* [out] */ ArrayOf<Int32>* value,
        /* [out] */ Int32* status);

    /**
     * Get effect parameter. The parameter is an integer and the value is an
     * array of 1 or 2 short integers
     *
     * @see #getParameter(byte[], byte[])
     * @hide
     */
    CARAPI GetParameter(
        /* [in] */ Int32 param,
        /* [out] */ ArrayOf<Int16>* value,
        /* [out] */ Int32* status);

    /**
     * Get effect parameter. The parameter is an array of 1 or 2 integers and
     * the value is also an array of 1 or 2 integers
     *
     * @see #getParameter(byte[], byte[])
     * @hide
     */
    CARAPI GetParameter(
        /* [in] */ ArrayOf<Int32>* param,
        /* [out] */ ArrayOf<Int32>* value,
        /* [out] */ Int32* status);

    /**
     * Get effect parameter. The parameter is an array of 1 or 2 integers and
     * the value is an array of 1 or 2 short integers
     *
     * @see #getParameter(byte[], byte[])
     * @hide
     */
    CARAPI GetParameter(
        /* [in] */ ArrayOf<Int32>* param,
        /* [out] */ ArrayOf<Int16>* value,
        /* [out] */ Int32* status);

    /**
     * Get effect parameter. The parameter is an array of 1 or 2 integers and
     * the value is an array of bytes
     *
     * @see #getParameter(byte[], byte[])
     * @hide
     */
    CARAPI GetParameter(
        /* [in] */ ArrayOf<Int32>* param,
        /* [in] */ ArrayOf<Byte>* value,
        /* [out] */ Int32* status);

    /**
     * Send a command to the effect engine. This method is intended to send
     * proprietary commands to a particular effect implementation.
     *
     * @hide
     */
    CARAPI Command(
        /* [in] */ Int32 cmdCode,
        /* [in] */ ArrayOf<Byte>* command,
        /* [out] */ ArrayOf<Byte>* reply,
        /* [out] */ Int32* result);

    /**
     * Returns effect unique identifier. This system wide unique identifier can
     * be used to attach this effect to a MediaPlayer or an AudioTrack when the
     * effect is an auxiliary effect (Reverb)
     *
     * @return the effect identifier.
     * @throws IllegalStateException
     */
    CARAPI GetId(
        /* [out] */ Int32* Id);

    /**
     * Returns effect enabled state
     *
     * @return true if the effect is enabled, false otherwise.
     * @throws IllegalStateException
     */
    CARAPI GetEnabled(
        /* [out] */ Boolean* enabled);

    /**
     * Checks if this AudioEffect object is controlling the effect engine.
     *
     * @return true if this instance has control of effect engine, false
     *         otherwise.
     * @throws IllegalStateException
     */
    CARAPI HasControl(
        /* [out] */ Boolean* control);

    // --------------------------------------------------------------------------
    // Initialization / configuration
    // --------------------
    /**
     * Sets the listener AudioEffect notifies when the effect engine is enabled
     * or disabled.
     *
     * @param listener
     */
    CARAPI SetEnableStatusListener(
        /* [in] */ IAudioEffectOnEnableStatusChangeListener* listener);

     /**
     * Sets the listener AudioEffect notifies when the effect engine control is
     * taken or returned.
     *
     * @param listener
     */
    CARAPI SetControlStatusListener(
        /* [in] */ IAudioEffectOnControlStatusChangeListener* listener);

    /**
    * Sets the listener AudioEffect notifies when a parameter is changed.
    *
    * @param listener
    * @hide
    */
    CARAPI SetParameterListener(
        /* [in] */ IAudioEffectOnParameterChangeListener* listener);

    // ---------------------------------------------------------
    // Utility methods
    // ------------------

    /**
    * @hide
    */
    CARAPI CheckState(
        /* [in] */ const String& methodName);

    /**
     * @hide
     */
    CARAPI CheckStatus(
        /* [in] */ Int32 status);

    /**
     * @hide
     */
    static CARAPI IsError(
        /* [in] */ Int32 status,
        /* [out] */ Boolean* result);

    /**
     * @hide
     */
    static CARAPI ByteArrayToInt32(
        /* [in] */ ArrayOf<Byte>* valueBuf,
        /* [out] */ Int32* result);

    /**
     * @hide
     */
    static CARAPI ByteArrayToInt32(
        /* [in] */ ArrayOf<Byte>* valueBuf,
        /* [in] */ Int32 offset,
        /* [out] */ Int32* result);

    /**
     * @hide
     */
    static CARAPI Int32ToByteArray(
        /* [in] */ Int32 value,
        /* [out, callee] */ ArrayOf<Byte>** result);

    /**
     * @hide
     */
    static CARAPI ByteArrayToInt16(
        /* [in] */ ArrayOf<Byte>* valueBuf,
        /* [out] */ Int16* result);

    /**
     * @hide
     */
    static CARAPI ByteArrayToInt16(
        /* [in] */ ArrayOf<Byte>* valueBuf,
        /* [in] */ Int32 offset,
        /* [out] */ Int16* result);

    /**
     * @hide
     */
    static CARAPI Int16ToByteArray(
        /* [in] */ Int16 value,
        /* [out, callee] */ ArrayOf<Byte>** result);

    /**
     * @hide
     */
    static CARAPI ConcatArrays(
        /* [in] */ ArrayOf<Byte>* array1,
        /* [in] */ ArrayOf<Byte>* array2,
        /* [out, callee] */ ArrayOf<Byte>** result);

protected:
    /**
     * Class constructor.
     *
     * @param type type of effect engine created. See {@link #EFFECT_TYPE_ENV_REVERB},
     *            {@link #EFFECT_TYPE_EQUALIZER} ... Types corresponding to
     *            built-in effects are defined by AudioEffect class. Other types
     *            can be specified provided they correspond an existing OpenSL
     *            ES interface ID and the corresponsing effect is available on
     *            the platform. If an unspecified effect type is requested, the
     *            constructor with throw the IllegalArgumentException. This
     *            parameter can be set to {@link #EFFECT_TYPE_NULL} in which
     *            case only the uuid will be used to select the effect.
     * @param uuid unique identifier of a particular effect implementation.
     *            Must be specified if the caller wants to use a particular
     *            implementation of an effect type. This parameter can be set to
     *            {@link #EFFECT_TYPE_NULL} in which case only the type will
     *            be used to select the effect.
     * @param priority the priority level requested by the application for
     *            controlling the effect engine. As the same effect engine can
     *            be shared by several applications, this parameter indicates
     *            how much the requesting application needs control of effect
     *            parameters. The normal priority is 0, above normal is a
     *            positive number, below normal a negative number.
     * @param audioSession system wide unique audio session identifier. If audioSession
     *            is not 0, the effect will be attached to the MediaPlayer or
     *            AudioTrack in the same audio session. Otherwise, the effect
     *            will apply to the output mix.
     *
     * @throws java.lang.IllegalArgumentException
     * @throws java.lang.UnsupportedOperationException
     * @throws java.lang.RuntimeException
     * @hide
     */
    CARAPI Init(
        /* [in] */ IUUID* type,
        /* [in] */ IUUID* uuid,
        /* [in] */ Int32 priority,
        /* [in] */ Int32 audioSession);

private:
    // Convenience method for the creation of the native event handler
    // It is called only when a non-null event listener is set.
    // precondition:
    // mNativeEventHandler is null
    CARAPI_(void) CreateNativeEventHandler();

    // ---------------------------------------------------------
    // Java methods called from the native side
    // --------------------
    static CARAPI_(void) PostEventFromNative(
        /* [in] */ AudioEffect* effect_ref,
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ IInterface* obj);

    // ---------------------------------------------------------
    // Native methods called from the Java side
    // --------------------
    static CARAPI_(void) effectCallback(
        /* [in] */ int event,
        /* [in] */ void* user,
        /* [in] */ void *info);

    CARAPI_(void) NativeInit();

    CARAPI_(Int32) NativeSetup(
        /* [in] */ const String& type,
        /* [in] */ const String& uuid,
        /* [in] */ Int32 priority,
        /* [in] */ Int32 audioSession,
        /* [in] */ ArrayOf<Int32>* id,
        /* [in] */ ArrayOf<IAudioEffectDescriptor*>* desc);

    CARAPI_(void) NativeFinalize();

    CARAPI_(void) NativeRelease();

    CARAPI_(Int32) NativeSetEnabled(
        /* [in] */ Boolean enabled);

    CARAPI_(Boolean) NativeGetEnabled();

    CARAPI_(Boolean) NativeHasControl();

    CARAPI_(Int32) NativeSetParameter(
        /* [in] */ Int32 psize,
        /* [in] */ ArrayOf<Byte>* param,
        /* [in] */ Int32 vsize,
        /* [in] */ ArrayOf<Byte>* value);

    CARAPI_(Int32) NativeGetParameter(
        /* [in] */ Int32 psize,
        /* [in] */ ArrayOf<Byte>* param,
        /* [in] */ Int32 vsize,
        /* [in] */ ArrayOf<Byte>* value);

    CARAPI_(Int32) NativeCommand(
        /* [in] */ Int32 cmdCode,
        /* [in] */ Int32 cmdSize,
        /* [in] */ ArrayOf<Byte>* cmdData,
        /* [in] */ Int32 repSize,
        /* [in] */ ArrayOf<Byte>* repData);

    static CARAPI_(AutoPtr<ArrayOf<IAudioEffectDescriptor* > >) NativeQueryEffects();

    static CARAPI_(AutoPtr<ArrayOf<IAudioEffectDescriptor* > >) NativeQueryPreProcessing(
        /* [in] */ Int32 audioSession);

public:
    /**
     * Lock to protect listeners updates against event notifications
     * @hide
     */

    Object mListenerLock;

    /**
     * Handler for events coming from the native code
     * @hide
     */
     AutoPtr<NativeEventHandler> mNativeEventHandler;

private:
    static const String TAG;

    // --------------------------------------------------------------------------
    // Member variables
    // --------------------
    /**
     * Indicates the state of the AudioEffect instance
     */
    Int32 mState;
    /**
     * Lock to synchronize access to mState
     */
    Object mStateLock;
    /**
     * System wide unique effect ID
     */
    Int32 mId;

    // accessed by native methods
    Int64 mNativeAudioEffect;
    Int64 mJniData;

    /**
     * Effect descriptor
     */
    AutoPtr<IAudioEffectDescriptor> mDescriptor;

    /**
     * Listener for effect engine state change notifications.
     *
     * @see #setEnableStatusListener(OnEnableStatusChangeListener)
     */
    AutoPtr<IAudioEffectOnEnableStatusChangeListener> mEnableStatusChangeListener;
    /**
     * Listener for effect engine control ownership change notifications.
     *
     * @see #setControlStatusListener(OnControlStatusChangeListener)
     */
    AutoPtr<IAudioEffectOnControlStatusChangeListener> mControlChangeStatusListener;
    /**
     * Listener for effect engine control ownership change notifications.
     *
     * @see #setParameterListener(OnParameterChangeListener)
     */
    AutoPtr<IAudioEffectOnParameterChangeListener> mParameterChangeListener;

};

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_AUDIOFX_AUDIOEFFECT_H__
