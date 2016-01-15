
#ifndef __ELASTOS_DROID_MEDIA_CAUDIOMANAGER_H__
#define __ELASTOS_DROID_MEDIA_CAUDIOMANAGER_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.View.h"
#include "_Elastos_Droid_Media_CAudioManager.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/AudioPortEventHandler.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::AudioPolicy::IAudioPolicy;
using Elastos::Droid::Media::AudioPortEventHandler;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Core::IInteger32;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * AudioManager provides access to volume and ringer mode control.
 * <p>
 * Use <code>Context.getSystemService(Context.AUDIO_SERVICE)</code> to get
 * an instance of this class.
 */
CarClass(CAudioManager)
    , public Object
    , public IAudioManager
{
friend class CAudioManagerAudioFocusDispatcher;

private:
    /**
     * Helper class to handle the forwarding of audio focus events to the appropriate listener
     */
    class FocusEventHandlerDelegate
         : public Object
    {
    private:
        class MyHandler
            : public Handler
        {
        public:
            MyHandler(
                /* [in] */ ILooper* looper,
                /* [in] */ FocusEventHandlerDelegate* host)
                : Handler(looper)
                , mHost(host)
            {}

            CARAPI HandleMessage(
                /* [in] */ IMessage* msg);

        private:
            FocusEventHandlerDelegate* mHost;
        };

    public:
        FocusEventHandlerDelegate(
            /* [in] */ CAudioManager* owner);

        CARAPI_(AutoPtr<IHandler>) GetHandler();

    private:
        AutoPtr<IHandler> mHandler;

        CAudioManager* mOwner;
    };

public:
    CAudioManager();

    virtual ~CAudioManager();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Sends a simulated key event for a media button.
     * To simulate a key press, you must first send a KeyEvent built with a
     * {@link KeyEvent#ACTION_DOWN} action, then another event with the {@link KeyEvent#ACTION_UP}
     * action.
     * <p>The key event will be sent to the current media key event consumer which registered with
     * {@link AudioManager#registerMediaButtonEventReceiver(PendingIntent)}.
     * @param keyEvent a {@link KeyEvent} instance whose key code is one of
     *     {@link KeyEvent#KEYCODE_MUTE},
     *     {@link KeyEvent#KEYCODE_HEADSETHOOK},
     *     {@link KeyEvent#KEYCODE_MEDIA_PLAY},
     *     {@link KeyEvent#KEYCODE_MEDIA_PAUSE},
     *     {@link KeyEvent#KEYCODE_MEDIA_PLAY_PAUSE},
     *     {@link KeyEvent#KEYCODE_MEDIA_STOP},
     *     {@link KeyEvent#KEYCODE_MEDIA_NEXT},
     *     {@link KeyEvent#KEYCODE_MEDIA_PREVIOUS},
     *     {@link KeyEvent#KEYCODE_MEDIA_REWIND},
     *     {@link KeyEvent#KEYCODE_MEDIA_RECORD},
     *     {@link KeyEvent#KEYCODE_MEDIA_FAST_FORWARD},
     *     {@link KeyEvent#KEYCODE_MEDIA_CLOSE},
     *     {@link KeyEvent#KEYCODE_MEDIA_EJECT},
     *     or {@link KeyEvent#KEYCODE_MEDIA_AUDIO_TRACK}.
     */
    CARAPI DispatchMediaKeyEvent(
        /* [in] */ IKeyEvent* event);

    /**
     * @hide
     */
    CARAPI PreDispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 stream);

    /**
     * @hide
     */
    CARAPI HandleKeyDown(
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 stream);

    /**
     * @hide
     */
    CARAPI HandleKeyUp(
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 stream);

    /**
     * Indicates if the device implements a fixed volume policy.
     * <p>Some devices may not have volume control and may operate at a fixed volume,
     * and may not enable muting or changing the volume of audio streams.
     * This method will return true on such devices.
     * <p>The following APIs have no effect when volume is fixed:
     * <ul>
     *   <li> {@link #adjustVolume(int, int)}
     *   <li> {@link #adjustSuggestedStreamVolume(int, int, int)}
     *   <li> {@link #adjustStreamVolume(int, int, int)}
     *   <li> {@link #setStreamVolume(int, int, int)}
     *   <li> {@link #setRingerMode(int)}
     *   <li> {@link #setStreamSolo(int, boolean)}
     *   <li> {@link #setStreamMute(int, boolean)}
     * </ul>
     */
    CARAPI IsVolumeFixed(
        /* [out] */ Boolean* result);

    /**
     * Adjusts the volume of a particular stream by one step in a direction.
     * <p>
     * This method should only be used by applications that replace the platform-wide
     * management of audio settings or the main telephony application.
     *
     * @param streamType The stream type to adjust. One of {@link #STREAM_VOICE_CALL},
     * {@link #STREAM_SYSTEM}, {@link #STREAM_RING}, {@link #STREAM_MUSIC} or
     * {@link #STREAM_ALARM}
     * @param direction The direction to adjust the volume. One of
     *            {@link #ADJUST_LOWER}, {@link #ADJUST_RAISE}, or
     *            {@link #ADJUST_SAME}.
     * @param flags One or more flags.
     * @see #adjustVolume(Int32, Int32)
     * @see #setStreamVolume(Int32, Int32, Int32)
     */
    CARAPI AdjustStreamVolume(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 direction,
        /* [in] */ Int32 flags);

    /**
     * Adjusts the volume of the most relevant stream. For example, if a call is
     * active, it will have the highest priority regardless of if the in-call
     * screen is showing. Another example, if music is playing in the background
     * and a call is not active, the music stream will be adjusted.
     * <p>
     * This method should only be used by applications that replace the platform-wide
     * management of audio settings or the main telephony application.
     *
     * @param direction The direction to adjust the volume. One of
     *            {@link #ADJUST_LOWER}, {@link #ADJUST_RAISE}, or
     *            {@link #ADJUST_SAME}.
     * @param flags One or more flags.
     * @see #adjustSuggestedStreamVolume(Int32, Int32, Int32)
     * @see #adjustStreamVolume(Int32, Int32, Int32)
     * @see #setStreamVolume(Int32, Int32, Int32)
     */
    CARAPI AdjustVolume(
        /* [in] */ Int32 direction,
        /* [in] */ Int32 flags);

    /**
     * Adjusts the volume of the most relevant stream, or the given fallback
     * stream.
     * <p>
     * This method should only be used by applications that replace the platform-wide
     * management of audio settings or the main telephony application.
     *
     * @param direction The direction to adjust the volume. One of
     *            {@link #ADJUST_LOWER}, {@link #ADJUST_RAISE}, or
     *            {@link #ADJUST_SAME}.
     * @param suggestedStreamType The stream type that will be used if there
     *            isn't a relevant stream. {@link #USE_DEFAULT_STREAM_TYPE} is valid here.
     * @param flags One or more flags.
     * @see #adjustVolume(Int32, Int32)
     * @see #adjustStreamVolume(Int32, Int32, Int32)
     * @see #setStreamVolume(Int32, Int32, Int32)
     */
    CARAPI AdjustSuggestedStreamVolume(
        /* [in] */ Int32 direction,
        /* [in] */ Int32 suggestedStreamType,
        /* [in] */ Int32 flags);

    /**
     * Adjusts the master volume for the device's audio amplifier.
     * <p>
     *
     * @param steps The number of volume steps to adjust. A positive
     *            value will raise the volume.
     * @param flags One or more flags.
     * @hide
     */
    CARAPI AdjustMasterVolume(
        /* [in] */ Int32 steps,
        /* [in] */ Int32 flags);

    /**
     * Returns the current ringtone mode.
     *
     * @return The current ringtone mode, one of {@link #RINGER_MODE_NORMAL},
     *         {@link #RINGER_MODE_SILENT}, or {@link #RINGER_MODE_VIBRATE}.
     * @see #setRingerMode(Int32)
     */
    CARAPI GetRingerMode(
        /* [out] */ Int32* result);

    /**
     * Returns the maximum volume index for a particular stream.
     *
     * @param streamType The stream type whose maximum volume index is returned.
     * @return The maximum valid volume index for the stream.
     * @see #getStreamVolume(Int32)
     */
    CARAPI GetStreamMaxVolume(
        /* [in] */ Int32 streamType,
        /* [out] */ Int32* result);

    /**
     * Returns the current volume index for a particular stream.
     *
     * @param streamType The stream type whose volume index is returned.
     * @return The current volume index for the stream.
     * @see #getStreamMaxVolume(Int32)
     * @see #setStreamVolume(Int32, Int32, Int32)
     */
    CARAPI GetStreamVolume(
        /* [in] */ Int32 streamType,
        /* [out] */ Int32* result);

    /**
     * Get last audible volume before stream was muted.
     *
     * @hide
     */
    CARAPI GetLastAudibleStreamVolume(
        /* [in] */ Int32 streamType,
        /* [out] */ Int32* result);

    /**
     * Get the stream type whose volume is driving the UI sounds volume.
     * UI sounds are screen lock/unlock, camera shutter, key clicks...
     * @hide
     */
    CARAPI GetMasterStreamType(
        /* [out] */ Int32* result);

    /**
     * Sets the ringer mode.
     * <p>
     * Silent mode will mute the volume and will not vibrate. Vibrate mode will
     * mute the volume and vibrate. Normal mode will be audible and may vibrate
     * according to user settings.
     *
     * @param ringerMode The ringer mode, one of {@link #RINGER_MODE_NORMAL},
     *            {@link #RINGER_MODE_SILENT}, or {@link #RINGER_MODE_VIBRATE}.
     * @see #getRingerMode()
     */
    CARAPI SetRingerMode(
        /* [in] */ Int32 ringerMode);

    /**
     * @see #setRingerMode(int)
     * @param checkZen  Update zen mode if necessary to compensate.
     * @hide
     */
    CARAPI SetRingerMode(
        /* [in] */ Int32 ringerMode,
        /* [in] */ Boolean checkZen);

    /**
     * Sets the volume index for a particular stream.
     *
     * @param streamType The stream whose volume index should be set.
     * @param index The volume index to set. See
     *            {@link #getStreamMaxVolume(Int32)} for the largest valid value.
     * @param flags One or more flags.
     * @see #getStreamMaxVolume(Int32)
     * @see #getStreamVolume(Int32)
     */
    CARAPI SetStreamVolume(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 index,
        /* [in] */ Int32 flags);

    /**
     * Returns the maximum volume index for master volume.
     *
     * @hide
     */
    CARAPI GetMasterMaxVolume(
        /* [out] */ Int32* result);

    /**
     * Returns the current volume index for master volume.
     *
     * @return The current volume index for master volume.
     * @hide
     */
    CARAPI GetMasterVolume(
        /* [out] */ Int32* result);

    /**
     * Get last audible volume before master volume was muted.
     *
     * @hide
     */
    CARAPI GetLastAudibleMasterVolume(
        /* [out] */ Int32* result);

    /**
     * Sets the volume index for master volume.
     *
     * @param index The volume index to set. See
     *            {@link #getMasterMaxVolume(Int32)} for the largest valid value.
     * @param flags One or more flags.
     * @see #getMasterMaxVolume(Int32)
     * @see #getMasterVolume(Int32)
     * @hide
     */
    CARAPI SetMasterVolume(
        /* [in] */ Int32 index,
        /* [in] */ Int32 flags);

    /**
     * Solo or unsolo a particular stream. All other streams are muted.
     * <p>
     * The solo command is protected against client process death: if a process
     * with an active solo request on a stream dies, all streams that were muted
     * because of this request will be unmuted automatically.
     * <p>
     * The solo requests for a given stream are cumulative: the AudioManager
     * can receive several solo requests from one or more clients and the stream
     * will be unsoloed only when the same number of unsolo requests are received.
     * <p>
     * For a better user experience, applications MUST unsolo a soloed stream
     * in onPause() and solo is again in onResume() if appropriate.
     *
     * @param streamType The stream to be soloed/unsoloed.
     * @param state The required solo state: TRUE for solo ON, FALSE for solo OFF
     */
    CARAPI SetStreamSolo(
        /* [in] */ Int32 streamType,
        /* [in] */ Boolean state);

    /**
     * Mute or unmute an audio stream.
     * <p>
     * The mute command is protected against client process death: if a process
     * with an active mute request on a stream dies, this stream will be unmuted
     * automatically.
     * <p>
     * The mute requests for a given stream are cumulative: the AudioManager
     * can receive several mute requests from one or more clients and the stream
     * will be unmuted only when the same number of unmute requests are received.
     * <p>
     * For a better user experience, applications MUST unmute a muted stream
     * in onPause() and mute is again in onResume() if appropriate.
     * <p>
     * This method should only be used by applications that replace the platform-wide
     * management of audio settings or the main telephony application.
     *
     * @param streamType The stream to be muted/unmuted.
     * @param state The required mute state: TRUE for mute ON, FALSE for mute OFF
     */
    CARAPI SetStreamMute(
        /* [in] */ Int32 streamType,
        /* [in] */ Boolean state);

    /**
     * get stream mute state.
     *
     * @hide
     */
    CARAPI IsStreamMute(
        /* [in] */ Int32 streamType,
        /* [out] */ Boolean* result);

    /**
     * set master mute state.
     *
     * @hide
     */
    CARAPI SetMasterMute(
        /* [in] */ Boolean state);

    /**
     * set master mute state with optional flags.
     *
     * @hide
     */
    CARAPI SetMasterMute(
        /* [in] */ Boolean state,
        /* [in] */ Int32 flags);

    /**
     * get master mute state.
     *
     * @hide
     */
    CARAPI IsMasterMute(
        /* [out] */ Boolean* result);

    /**
     * forces the stream controlled by hard volume keys
     * specifying streamType == -1 releases control to the
     * logic.
     *
     * @hide
     */
    CARAPI ForceVolumeControlStream(
        /* [in] */ Int32 streamType);

    /**
     * Returns whether a particular type should vibrate according to user
     * settings and the current ringer mode.
     * <p>
     * This shouldn't be needed by most clients that use notifications to
     * vibrate. The notification manager will not vibrate if the policy doesn't
     * allow it, so the client should always set a vibrate pattern and let the
     * notification manager control whether or not to actually vibrate.
     *
     * @param vibrateType The type of vibrate. One of
     *            {@link #VIBRATE_TYPE_NOTIFICATION} or
     *            {@link #VIBRATE_TYPE_RINGER}.
     * @return Whether the type should vibrate at the instant this method is
     *         called.
     * @see #setVibrateSetting(Int32, Int32)
     * @see #getVibrateSetting(Int32)
     */
    CARAPI ShouldVibrate(
        /* [in] */ Int32 vibrateType,
        /* [out] */ Boolean* result);

    /**
     * Returns whether the user's vibrate setting for a vibrate type.
     * <p>
     * This shouldn't be needed by most clients that want to vibrate, instead
     * see {@link #shouldVibrate(Int32)}.
     *
     * @param vibrateType The type of vibrate. One of
     *            {@link #VIBRATE_TYPE_NOTIFICATION} or
     *            {@link #VIBRATE_TYPE_RINGER}.
     * @return The vibrate setting, one of {@link #VIBRATE_SETTING_ON},
     *         {@link #VIBRATE_SETTING_OFF}, or
     *         {@link #VIBRATE_SETTING_ONLY_SILENT}.
     * @see #setVibrateSetting(Int32, Int32)
     * @see #shouldVibrate(Int32)
     */
    CARAPI GetVibrateSetting(
        /* [in] */ Int32 vibrateType,
        /* [out] */ Int32* result);

    /**
     * Sets the setting for when the vibrate type should vibrate.
     * <p>
     * This method should only be used by applications that replace the platform-wide
     * management of audio settings or the main telephony application.
     *
     * @param vibrateType The type of vibrate. One of
     *            {@link #VIBRATE_TYPE_NOTIFICATION} or
     *            {@link #VIBRATE_TYPE_RINGER}.
     * @param vibrateSetting The vibrate setting, one of
     *            {@link #VIBRATE_SETTING_ON},
     *            {@link #VIBRATE_SETTING_OFF}, or
     *            {@link #VIBRATE_SETTING_ONLY_SILENT}.
     * @see #getVibrateSetting(Int32)
     * @see #shouldVibrate(Int32)
     */
    CARAPI SetVibrateSetting(
        /* [in] */ Int32 vibrateType,
        /* [in] */ Int32 vibrateSetting);

    /**
     * Sets the speakerphone on or off.
     * <p>
     * This method should only be used by applications that replace the platform-wide
     * management of audio settings or the main telephony application.
     *
     * @param on set <var>TRUE</var> to turn on speakerphone;
     *           <var>FALSE</var> to turn it off
     */
    CARAPI SetSpeakerphoneOn(
        /* [in] */ Boolean on);

    /**
     * Checks whether the speakerphone is on or off.
     *
     * @return TRUE if speakerphone is on, FALSE if it's off
     */
    CARAPI IsSpeakerphoneOn(
        /* [out] */ Boolean* result);

    //====================================================================
    // Bluetooth SCO control
    /**
     * Indicates if current platform supports use of SCO for off call use cases.
     * Application wanted to use bluetooth SCO audio when the phone is not in call
     * must first call thsi method to make sure that the platform supports this
     * feature.
     * @return TRUE if bluetooth SCO can be used for audio when not in call
     *         FALSE otherwise
     * @see #startBluetoothSco()
     */
    CARAPI IsBluetoothScoAvailableOffCall(
        /* [out] */ Boolean* result);

    /**
     * Start bluetooth SCO audio connection.
     * <p>Requires Permission:
     *   {@link android.Manifest.permission#MODIFY_AUDIO_SETTINGS}.
     * <p>This method can be used by applications wanting to send and received audio
     * to/from a bluetooth SCO headset while the phone is not in call.
     * <p>As the SCO connection establishment can take several seconds,
     * applications should not rely on the connection to be available when the method
     * returns but instead register to receive the intent {@link #ACTION_SCO_AUDIO_STATE_CHANGED}
     * and wait for the state to be {@link #SCO_AUDIO_STATE_CONNECTED}.
     * <p>As the connection is not guaranteed to succeed, applications must wait for this intent with
     * a timeout.
     * <p>When finished with the SCO connection or if the establishment times out,
     * the application must call {@link #stopBluetoothSco()} to clear the request and turn
     * down the bluetooth connection.
     * <p>Even if a SCO connection is established, the following restrictions apply on audio
     * output streams so that they can be routed to SCO headset:
     * - the stream type must be {@link #STREAM_VOICE_CALL}
     * - the format must be mono
     * - the sampling must be 16kHz or 8kHz
     * <p>The following restrictions apply on input streams:
     * - the format must be mono
     * - the sampling must be 8kHz
     *
     * <p>Note that the phone application always has the priority on the usage of the SCO
     * connection for telephony. If this method is called while the phone is in call
     * it will be ignored. Similarly, if a call is received or sent while an application
     * is using the SCO connection, the connection will be lost for the application and NOT
     * returned automatically when the call ends.
     * @see #stopBluetoothSco()
     * @see #ACTION_SCO_AUDIO_STATE_CHANGED
     */
    CARAPI StartBluetoothSco();

    /**
     * @hide
     * Start bluetooth SCO audio connection in virtual call mode.
     * <p>Requires Permission:
     *   {@link android.Manifest.permission#MODIFY_AUDIO_SETTINGS}.
     * <p>Similar to {@link #startBluetoothSco()} with explicit selection of virtual call mode.
     * Telephony and communication applications (VoIP, Video Chat) should preferably select
     * virtual call mode.
     * Applications using voice input for search or commands should first try raw audio connection
     * with {@link #startBluetoothSco()} and fall back to startBluetoothScoVirtualCall() in case of
     * failure.
     * @see #startBluetoothSco()
     * @see #stopBluetoothSco()
     * @see #ACTION_SCO_AUDIO_STATE_UPDATED
     */
    CARAPI StartBluetoothScoVirtualCall();

    /**
     * Stop bluetooth SCO audio connection.
     * <p>Requires Permission:
     *   {@link android.Manifest.permission#MODIFY_AUDIO_SETTINGS}.
     * <p>This method must be called by applications having requested the use of
     * bluetooth SCO audio with {@link #startBluetoothSco()}
     * when finished with the SCO connection or if the establishment times out.
     * @see #startBluetoothSco()
     */
    CARAPI StopBluetoothSco();

    /**
     * Request use of Bluetooth SCO headset for communications.
     * <p>
     * This method should only be used by applications that replace the platform-wide
     * management of audio settings or the main telephony application.
     *
     * @param on set <var>TRUE</var> to use bluetooth SCO for communications;
     *               <var>FALSE</var> to not use bluetooth SCO for communications
     */
    CARAPI SetBluetoothScoOn(
        /* [in] */ Boolean on);

    /**
     * Checks whether communications use Bluetooth SCO.
     *
     * @return TRUE if SCO is used for communications;
     *         FALSE if otherwise
     */
    CARAPI IsBluetoothScoOn(
        /* [out] */ Boolean* result);

    /**
     * @param on set <var>TRUE</var> to route A2DP audio to/from Bluetooth
     *           headset; <var>FALSE</var> disable A2DP audio
     * @deprecated Do not use.
     */
    //@Deprecated
    CARAPI SetBluetoothA2dpOn(
        /* [in] */ Boolean on);

    /**
     * Checks whether A2DP audio routing to the Bluetooth headset is on or off.
     *
     * @return TRUE if A2DP audio is being routed to/from Bluetooth headset;
     *         FALSE if otherwise
     */
    CARAPI IsBluetoothA2dpOn(
        /* [out] */ Boolean* result);

    /**
     * Sets audio routing to the wired headset on or off.
     *
     * @param on set <var>TRUE</var> to route audio to/from wired
     *           headset; <var>FALSE</var> disable wired headset audio
     * @deprecated Do not use.
     */
    //@Deprecated
    CARAPI SetWiredHeadsetOn(
        /* [in] */ Boolean on);

    /**
     * Checks whether audio routing to the wired headset is on or off.
     *
     * @return TRUE if audio is being routed to/from wired headset;
     *         FALSE if otherwise
     */
    CARAPI IsWiredHeadsetOn(
        /* [out] */ Boolean* result);

    /**
     * Sets the microphone mute on or off.
     * <p>
     * This method should only be used by applications that replace the platform-wide
     * management of audio settings or the main telephony application.
     *
     * @param on set <var>TRUE</var> to mute the microphone;
     *           <var>FALSE</var> to turn mute off
     */
    CARAPI SetMicrophoneMute(
        /* [in] */ Boolean on);

    /**
     * Checks whether the microphone mute is on or off.
     *
     * @return TRUE if microphone is muted, FALSE if it's not
     */
    CARAPI IsMicrophoneMute(
        /* [out] */ Boolean* result);

    /**
     * Sets the audio mode.
     * <p>
     * The audio mode encompasses audio routing AND the behavior of
     * the telephony layer. Therefore this method should only be used by applications that
     * replace the platform-wide management of audio settings or the main telephony application.
     * In particular, the {@link #MODE_IN_CALL} mode should only be used by the telephony
     * application when it places a phone call, as it will cause signals from the radio layer
     * to feed the platform mixer.
     *
     * @param mode  the requested audio mode (NORMAL, RINGTONE, or IN_CALL).
     *              Informs the HAL about the current audio state so that
     *              it can route the audio appropriately.
     */
    CARAPI SetMode(
        /* [in] */ Int32 mode);

    /**
     * Returns the current audio mode.
     *
     * @return      the current audio mode (NORMAL, RINGTONE, or IN_CALL).
     *              Returns the current current audio state from the HAL.
     */
    CARAPI GetMode(
        /* [out] */ Int32* result);

    /**
     * Sets the audio routing for a specified mode
     *
     * @param mode   audio mode to change route. E.g., MODE_RINGTONE.
     * @param routes bit vector of routes requested, created from one or
     *               more of ROUTE_xxx types. Set bits indicate that route should be on
     * @param mask   bit vector of routes to change, created from one or more of
     * ROUTE_xxx types. Unset bits indicate the route should be left unchanged
     *
     * @deprecated   Do not set audio routing directly, use setSpeakerphoneOn(),
     * setBluetoothScoOn() methods instead.
     */
    //@Deprecated
    CARAPI SetRouting(
        /* [in] */ Int32 mode,
        /* [in] */ Int32 routes,
        /* [in] */ Int32 mask);

    /**
     * Returns the current audio routing bit vector for a specified mode.
     *
     * @param mode audio mode to get route (e.g., MODE_RINGTONE)
     * @return an audio route bit vector that can be compared with ROUTE_xxx
     * bits
     * @deprecated   Do not query audio routing directly, use isSpeakerphoneOn(),
     * isBluetoothScoOn(), isBluetoothA2dpOn() and isWiredHeadsetOn() methods instead.
     */
    //@Deprecated
    CARAPI GetRouting(
        /* [in] */ Int32 mode,
        /* [out] */ Int32* result);

    /**
     * Checks whether any music is active.
     *
     * @return TRUE if any music tracks are active.
     */
    CARAPI IsMusicActive(
        /* [out] */ Boolean* result);

    /**
     * @hide
     * Checks whether any music or media is actively playing on a remote device (e.g. wireless
     *   display). Note that BT audio sinks are not considered remote devices.
     * @return true if {@link AudioManager#STREAM_MUSIC} is active on a remote device
     */
    CARAPI IsMusicActiveRemotely(
        /* [out] */ Boolean* result);

    /**
     * @hide
     * Checks whether the current audio focus is exclusive.
     * @return true if the top of the audio focus stack requested focus
     *     with {@link #AUDIOFOCUS_GAIN_TRANSIENT_EXCLUSIVE}
     */
    CARAPI IsAudioFocusExclusive(
        /* [out] */ Boolean* result);

    /**
     * Return a new audio session identifier not associated with any player or effect.
     * An audio session identifier is a system wide unique identifier for a set of audio streams
     * (one or more mixed together).
     * <p>The primary use of the audio session ID is to associate audio effects to audio players,
     * such as {@link MediaPlayer} or {@link AudioTrack}: all audio effects sharing the same audio
     * session ID will be applied to the mixed audio content of the players that share the same
     * audio session.
     * <p>This method can for instance be used when creating one of the
     * {@link android.media.audiofx.AudioEffect} objects to define the audio session of the effect,
     * or to specify a session for a speech synthesis utterance
     * in {@link android.speech.tts.TextToSpeech.Engine}.
     * @return a new unclaimed and unused audio session identifier, or {@link #ERROR} when the
     *   system failed to generate a new session, a condition in which audio playback or recording
     *   will subsequently fail as well.
     */
    CARAPI GenerateAudioSessionId(
        /* [out] */ Int32* result);

    /*
     * Sets a generic audio configuration parameter. The use of these parameters
     * are platform dependant, see libaudio
     *
     * ** Temporary interface - DO NOT USE
     *
     * TODO: Replace with a more generic key:value get/set mechanism
     *
     * param key   name of parameter to set. Must not be NULL.
     * param value value of parameter. Must not be NULL.
     */
    /**
     * @hide
     * @deprecated Use {@link #setPrameters(String)} instead
     */
    //@Deprecated
    CARAPI SetParameter(
        /* [in] */ const String& key,
        /* [in] */ const String& value);

    /**
     * Sets a variable number of parameter values to audio hardware.
     *
     * @param keyValuePairs list of parameters key value pairs in the form:
     *    key1=value1;key2=value2;...
     *
     */
    CARAPI SetParameters(
        /* [in] */ const String& keyValuePairs);

    /**
     * Sets a varaible number of parameter values to audio hardware.
     *
     * @param keys list of parameters
     * @return list of parameters key value pairs in the form:
     *    key1=value1;key2=value2;...
     */
    CARAPI GetParameters(
        /* [in] */ const String& keys,
        /* [out] */ String* parameters);

    /**
     * Plays a sound effect (Key clicks, lid open/close...)
     * @param effectType The type of sound effect. One of
     *            {@link #FX_KEY_CLICK},
     *            {@link #FX_FOCUS_NAVIGATION_UP},
     *            {@link #FX_FOCUS_NAVIGATION_DOWN},
     *            {@link #FX_FOCUS_NAVIGATION_LEFT},
     *            {@link #FX_FOCUS_NAVIGATION_RIGHT},
     *            {@link #FX_KEYPRESS_STANDARD},
     *            {@link #FX_KEYPRESS_SPACEBAR},
     *            {@link #FX_KEYPRESS_DELETE},
     *            {@link #FX_KEYPRESS_RETURN},
     * NOTE: This version uses the UI settings to determine
     * whether sounds are heard or not.
     */
    CARAPI PlaySoundEffect(
        /* [in] */ Int32 effectType);

    /**
     * Plays a sound effect (Key clicks, lid open/close...)
     * @param effectType The type of sound effect. One of
     *            {@link #FX_KEY_CLICK},
     *            {@link #FX_FOCUS_NAVIGATION_UP},
     *            {@link #FX_FOCUS_NAVIGATION_DOWN},
     *            {@link #FX_FOCUS_NAVIGATION_LEFT},
     *            {@link #FX_FOCUS_NAVIGATION_RIGHT},
     *            {@link #FX_KEYPRESS_STANDARD},
     *            {@link #FX_KEYPRESS_SPACEBAR},
     *            {@link #FX_KEYPRESS_DELETE},
     *            {@link #FX_KEYPRESS_RETURN},
     *            {@link #FX_KEYPRESS_INVALID},
     * @param userId The current user to pull sound settings from
     * NOTE: This version uses the UI settings to determine
     * whether sounds are heard or not.
     * @hide
     */
    CARAPI PlaySoundEffect(
        /* [in] */ Int32 effectType,
        /* [in] */ Int32 userId);

    /**
     * Plays a sound effect (Key clicks, lid open/close...)
     * @param effectType The type of sound effect. One of
     *            {@link #FX_KEY_CLICK},
     *            {@link #FX_FOCUS_NAVIGATION_UP},
     *            {@link #FX_FOCUS_NAVIGATION_DOWN},
     *            {@link #FX_FOCUS_NAVIGATION_LEFT},
     *            {@link #FX_FOCUS_NAVIGATION_RIGHT},
     *            {@link #FX_KEYPRESS_STANDARD},
     *            {@link #FX_KEYPRESS_SPACEBAR},
     *            {@link #FX_KEYPRESS_DELETE},
     *            {@link #FX_KEYPRESS_RETURN},
     * @param volume Sound effect volume.
     * The volume value is a raw scalar so UI controls should be scaled logarithmically.
     * If a volume of -1 is specified, the AudioManager.STREAM_MUSIC stream volume minus 3dB will be used.
     * NOTE: This version is for applications that have their own
     * settings panel for enabling and controlling volume.
     */
    CARAPI PlaySoundEffect(
        /* [in] */ Int32 effectType,
        /* [in] */ Float volume);

    /**
     *  Load Sound effects.
     *  This method must be called when sound effects are enabled.
     */
    CARAPI LoadSoundEffects();

    /**
     *  Unload Sound effects.
     *  This method can be called to free some memory when
     *  sound effects are disabled.
     */
    CARAPI UnloadSoundEffects();

    /**
     * @hide
     * Registers a listener to be called when audio focus changes. Calling this method is optional
     * before calling {@link #requestAudioFocus(IAudioManagerOnAudioFocusChangeListener, Int32, Int32)}, as it
     * will register the listener as well if it wasn't registered already.
     * @param l the listener to be notified of audio focus changes.
     */
    CARAPI RegisterAudioFocusListener(
        /* [in] */ IAudioManagerOnAudioFocusChangeListener* l);

    /**
     * @hide
     * Causes the specified listener to not be called anymore when focus is gained or lost.
     * @param l the listener to unregister.
     */
    CARAPI UnregisterAudioFocusListener(
        /* [in] */ IAudioManagerOnAudioFocusChangeListener* l);

    /**
     *  Request audio focus.
     *  Send a request to obtain the audio focus
     *  @param l the listener to be notified of audio focus changes
     *  @param streamType the main audio stream type affected by the focus request
     *  @param durationHint use {@link #AUDIOFOCUS_GAIN_TRANSIENT} to indicate this focus request
     *      is temporary, and focus will be abandonned shortly. Examples of transient requests are
     *      for the playback of driving directions, or notifications sounds.
     *      Use {@link #AUDIOFOCUS_GAIN_TRANSIENT_MAY_DUCK} to indicate also that it's ok for
     *      the previous focus owner to keep playing if it ducks its audio output.
     *      Use {@link #AUDIOFOCUS_GAIN} for a focus request of unknown duration such
     *      as the playback of a song or a video.
     *  @return {@link #AUDIOFOCUS_REQUEST_FAILED} or {@link #AUDIOFOCUS_REQUEST_GRANTED}
     */
    CARAPI RequestAudioFocus(
        /* [in] */ IAudioManagerOnAudioFocusChangeListener* l,
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 durationHint,
        /* [out] */ Int32* result);

    /**
     * @hide
     * Used internally by telephony package to request audio focus. Will cause the focus request
     * to be associated with the "voice communication" identifier only used in AudioService
     * to identify this use case.
     * @param streamType use STREAM_RING for focus requests when ringing, VOICE_CALL for
     *    the establishment of the call
     * @param durationHint the type of focus request. AUDIOFOCUS_GAIN_TRANSIENT is recommended so
     *    media applications resume after a call
     */
    CARAPI RequestAudioFocusForCall(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 durationHint);

    /**
     * @hide
     * Used internally by telephony package to abandon audio focus, typically after a call or
     * when ringing ends and the call is rejected or not answered.
     * Should match one or more calls to {@link #requestAudioFocusForCall(Int32, Int32)}.
     */
    CARAPI AbandonAudioFocusForCall();

    /**
     *  Abandon audio focus. Causes the previous focus owner, if any, to receive focus.
     *  @param l the listener with which focus was requested.
     *  @return {@link #AUDIOFOCUS_REQUEST_FAILED} or {@link #AUDIOFOCUS_REQUEST_GRANTED}
     */
    CARAPI AbandonAudioFocus(
        /* [in] */ IAudioManagerOnAudioFocusChangeListener* l,
        /* [out] */ Int32* result);

    //====================================================================
    // Remote Control
    /**
     * Register a component to be the sole receiver of MEDIA_BUTTON intents.
     * @param eventReceiver identifier of a {@link android.content.BroadcastReceiver}
     *      that will receive the media button intent. This broadcast receiver must be declared
     *      in the application manifest.
     */
    CARAPI RegisterMediaButtonEventReceiver(
        /* [in] */ IComponentName* eventReceiver);

    /**
     * Register a component to be the sole receiver of MEDIA_BUTTON intents.  This is like
     * {@link #registerMediaButtonEventReceiver(android.content.ComponentName)}, but allows
     * the buttons to go to any PendingIntent.  Note that you should only use this form if
     * you know you will continue running for the full time until unregistering the
     * PendingIntent.
     * @param eventReceiver target that will receive media button intents.  The PendingIntent
     * will be sent an {@link Intent#ACTION_MEDIA_BUTTON} event when a media button action
     * occurs, with {@link Intent#EXTRA_KEY_EVENT} added and holding the key code of the
     * media button that was pressed.
     * @deprecated Use {@link MediaSession#setMediaButtonReceiver(PendingIntent)} instead.
     */
    // @Deprecated
    CARAPI RegisterMediaButtonEventReceiver(
        /* [in] */ IPendingIntent* eventReceiver);

    /**
     * @hide
     * no-op if (pi == null) or (eventReceiver == null)
     */
    CARAPI RegisterMediaButtonIntent(
        /* [in] */ IPendingIntent* pi,
        /* [in] */ IComponentName* eventReceiver);

    /**
     * Unregister the receiver of MEDIA_BUTTON intents.
     * @param eventReceiver identifier of a {@link android.content.BroadcastReceiver}
     *      that was registered with {@link #registerMediaButtonEventReceiver(ComponentName)}.
     */
    CARAPI UnregisterMediaButtonEventReceiver(
        /* [in] */ IComponentName* eventReceiver);

    /**
     * Unregister the receiver of MEDIA_BUTTON intents.
     * @param eventReceiver same PendingIntent that was registed with
     *      {@link #registerMediaButtonEventReceiver(PendingIntent)}.
     * @deprecated Use {@link MediaSession} instead.
     */
    // @Deprecated
    CARAPI UnregisterMediaButtonEventReceiver(
        /* [in] */ IPendingIntent* eventReceiver);

    /**
     * @hide
     */
    CARAPI UnregisterMediaButtonIntent(
        /* [in] */ IPendingIntent* pi);

    /**
     * Registers the remote control client for providing information to display on the remote
     * controls.
     * @param rcClient The remote control client from which remote controls will receive
     *      information to display.
     * @see RemoteControlClient
     */
    CARAPI RegisterRemoteControlClient(
        /* [in] */ IRemoteControlClient* rcClient);

    /**
     * Unregisters the remote control client that was providing information to display on the
     * remote controls.
     * @param rcClient The remote control client to unregister.
     * @see #registerRemoteControlClient(RemoteControlClient)
     */
    CARAPI UnregisterRemoteControlClient(
        /* [in] */ IRemoteControlClient* rcClient);

    /**
     * Registers a {@link RemoteController} instance for it to receive media
     * metadata updates and playback state information from applications using
     * {@link RemoteControlClient}, and control their playback.
     * <p>
     * Registration requires the {@link OnClientUpdateListener} listener to be
     * one of the enabled notification listeners (see
     * {@link android.service.notification.NotificationListenerService}).
     *
     * @param rctlr the object to register.
     * @return true if the {@link RemoteController} was successfully registered,
     *         false if an error occurred, due to an internal system error, or
     *         insufficient permissions.
     * @deprecated Use
     *             {@link MediaSessionManager#addOnActiveSessionsChangedListener(android.media.session.MediaSessionManager.OnActiveSessionsChangedListener, ComponentName)}
     *             and {@link MediaController} instead.
     */
    // @Deprecated
    CARAPI RegisterRemoteController(
        /* [in] */ IRemoteController* rctlr,
        /* [out] */ Boolean* result);

    /**
     * Unregisters a {@link RemoteController}, causing it to no longer receive
     * media metadata and playback state information, and no longer be capable
     * of controlling playback.
     *
     * @param rctlr the object to unregister.
     * @deprecated Use
     *             {@link MediaSessionManager#removeOnActiveSessionsChangedListener(android.media.session.MediaSessionManager.OnActiveSessionsChangedListener)}
     *             instead.
     */
    // @Deprecated
    CARAPI UnregisterRemoteController(
        /* [in] */ IRemoteController* rctlr);

    /**
     * @hide
     * Registers a remote control display that will be sent information by remote control clients.
     * Use this method if your IRemoteControlDisplay is not going to display artwork, otherwise
     * use {@link #registerRemoteControlDisplay(IRemoteControlDisplay, int, int)} to pass the
     * artwork size directly, or
     * {@link #remoteControlDisplayUsesBitmapSize(IRemoteControlDisplay, int, int)} later if artwork
     * is not yet needed.
     * <p>Registration requires the {@link Manifest.permission#MEDIA_CONTENT_CONTROL} permission.
     * @param rcd the IRemoteControlDisplay
     */
    CARAPI RegisterRemoteControlDisplay(
        /* [in] */ IIRemoteControlDisplay* rcd);

    /**
     * @hide
     * Registers a remote control display that will be sent information by remote control clients.
     * <p>Registration requires the {@link Manifest.permission#MEDIA_CONTENT_CONTROL} permission.
     * @param rcd
     * @param w the maximum width of the expected bitmap. Negative values indicate it is
     *   useless to send artwork.
     * @param h the maximum height of the expected bitmap. Negative values indicate it is
     *   useless to send artwork.
     */
    CARAPI RegisterRemoteControlDisplay(
        /* [in] */ IIRemoteControlDisplay* rcd,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h);

    /**
     * @hide
     * Unregisters a remote control display that was sent information by remote control clients.
     * @param rcd
     */
    CARAPI UnregisterRemoteControlDisplay(
        /* [in] */ IIRemoteControlDisplay* rcd);

    /**
     * @hide
     * Sets the artwork size a remote control display expects when receiving bitmaps.
     * @param rcd
     * @param w the maximum width of the expected bitmap. Negative values indicate it is
     *   useless to send artwork.
     * @param h the maximum height of the expected bitmap. Negative values indicate it is
     *   useless to send artwork.
     */
    CARAPI RemoteControlDisplayUsesBitmapSize(
        /* [in] */ IIRemoteControlDisplay* rcd,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h);

    /**
     * @hide
     * Controls whether a remote control display needs periodic checks of the RemoteControlClient
     * playback position to verify that the estimated position has not drifted from the actual
     * position. By default the check is not performed.
     * The IRemoteControlDisplay must have been previously registered for this to have any effect.
     * @param rcd the IRemoteControlDisplay for which the anti-drift mechanism will be enabled
     *     or disabled. No effect is null.
     * @param wantsSync if true, RemoteControlClient instances which expose their playback position
     *     to the framework will regularly compare the estimated playback position with the actual
     *     position, and will update the IRemoteControlDisplay implementation whenever a drift is
     *     detected.
     */
    CARAPI RemoteControlDisplayWantsPlaybackPositionSync(
        /* [in] */ IIRemoteControlDisplay* rcd,
        /* [in] */ Boolean wantsSync);

    /**
     * @hide
     * CANDIDATE FOR PUBLIC API
     * Register the given {@link AudioPolicy}.
     * This call is synchronous and blocks until the registration process successfully completed
     * or failed to complete.
     * @param policy the {@link AudioPolicy} to register.
     * @return {@link #ERROR} if there was an error communicating with the registration service
     *    or if the user doesn't have the required
     *    {@link android.Manifest.permission#MODIFY_AUDIO_ROUTING} permission,
     *    {@link #SUCCESS} otherwise.
     */
    CARAPI RegisterAudioPolicy(
        /* [in] */ IAudioPolicy* policy,
        /* [out] */ Int32* result);

    /**
     * @hide
     * CANDIDATE FOR PUBLIC API
     * @param policy the {@link AudioPolicy} to unregister.
     */
    CARAPI UnregisterAudioPolicyAsync(
        /* [in] */ IAudioPolicy* policy);

    /**
     *  @hide
     *  Reload audio settings. This method is called by Settings backup
     *  agent when audio settings are restored and causes the AudioService
     *  to read and apply restored settings.
     */
    CARAPI ReloadAudioSettings();

    /**
     * @hide
     * Notifies AudioService that it is connected to an A2DP device that supports absolute volume,
     * so that AudioService can send volume change events to the A2DP device, rather than handling
     * them.
     */
    CARAPI AvrcpSupportsAbsoluteVolume(
        /* [in] */ const String& address,
        /* [in] */ Boolean support);

    /**
     * Checks whether the phone is in silent mode, with or without vibrate.
     *
     * @return true if phone is in silent mode, with or without vibrate.
     *
     * @see #getRingerMode()
     *
     * @hide pending API Council approval
     */
    CARAPI IsSilentMode(
        /* [out] */ Boolean* result);

    /**
     * Return the enabled devices for the specified output stream type.
     *
     * @param streamType The stream type to query. One of
     *            {@link #STREAM_VOICE_CALL},
     *            {@link #STREAM_SYSTEM},
     *            {@link #STREAM_RING},
     *            {@link #STREAM_MUSIC},
     *            {@link #STREAM_ALARM},
     *            {@link #STREAM_NOTIFICATION},
     *            {@link #STREAM_DTMF}.
     *
     * @return The bit-mask "or" of audio output device codes for all enabled devices on this
     *         stream. Zero or more of
     *            {@link #DEVICE_OUT_EARPIECE},
     *            {@link #DEVICE_OUT_SPEAKER},
     *            {@link #DEVICE_OUT_WIRED_HEADSET},
     *            {@link #DEVICE_OUT_WIRED_HEADPHONE},
     *            {@link #DEVICE_OUT_BLUETOOTH_SCO},
     *            {@link #DEVICE_OUT_BLUETOOTH_SCO_HEADSET},
     *            {@link #DEVICE_OUT_BLUETOOTH_SCO_CARKIT},
     *            {@link #DEVICE_OUT_BLUETOOTH_A2DP},
     *            {@link #DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES},
     *            {@link #DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER},
     *            {@link #DEVICE_OUT_AUX_DIGITAL},
     *            {@link #DEVICE_OUT_ANLG_DOCK_HEADSET},
     *            {@link #DEVICE_OUT_DGTL_DOCK_HEADSET}.
     *            {@link #DEVICE_OUT_DEFAULT} is not used here.
     *
     * The implementation may support additional device codes beyond those listed, so
     * the application should ignore any bits which it does not recognize.
     * Note that the information may be imprecise when the implementation
     * cannot distinguish whether a particular device is enabled.
     *
     * {@hide}
     */
    CARAPI GetDevicesForStream(
        /* [in] */ Int32 streamType,
        /* [out] */ Int32* result);

     /**
     * Indicate wired accessory connection state change.
     * @param device type of device connected/disconnected (AudioManager.DEVICE_OUT_xxx)
     * @param state  new connection state: 1 connected, 0 disconnected
     * @param name   device name
     * {@hide}
     */
    CARAPI SetWiredDeviceConnectionState(
        /* [in] */ Int32 device,
        /* [in] */ Int32 state,
        /* [in] */ const String& name);

     /**
     * Indicate A2DP sink connection state change.
     * @param device Bluetooth device connected/disconnected
     * @param state  new connection state (BluetoothProfile.STATE_xxx)
     * @return a delay in ms that the caller should wait before broadcasting
     * BluetoothA2dp.ACTION_CONNECTION_STATE_CHANGED intent.
     * {@hide}
     */
    CARAPI SetBluetoothA2dpDeviceConnectionState(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ Int32 state,
        /* [in] */ Int32 profile,
        /* [out] */ Int32* result);

    /** {@hide} */
    CARAPI GetRingtonePlayer(
        /* [out] */ IIRingtonePlayer** result);

    /**
     * Returns the value of the property with the specified key.
     * @param key One of the strings corresponding to a property key: either
     *            {@link #PROPERTY_OUTPUT_SAMPLE_RATE} or
     *            {@link #PROPERTY_OUTPUT_FRAMES_PER_BUFFER}
     * @return A string representing the associated value for that property key,
     *         or null if there is no value for that key.
     */
    CARAPI GetProperty(
        /* [in] */ const String& key,
        /* [out] */ String* result);

    /**
     * Returns the estimated latency for the given stream type in milliseconds.
     *
     * DO NOT UNHIDE. The existing approach for doing A/V sync has too many problems. We need
     * a better solution.
     * @hide
     */
    CARAPI GetOutputLatency(
        /* [in] */ Int32 streamType,
        /* [out] */ Int32* result);

    /**
     * Registers a global volume controller interface.  Currently limited to SystemUI.
     *
     * @hide
     */
    CARAPI SetVolumeController(
        /* [in] */ IIVolumeController* controller);

    /**
     * Notify audio manager about volume controller visibility changes.
     * Currently limited to SystemUI.
     *
     * @hide
     */
    CARAPI NotifyVolumeControllerVisible(
        /* [in] */ IIVolumeController* controller,
        /* [in] */ Boolean visible);

    /**
     * Only useful for volume controllers.
     * @hide
     */
    CARAPI IsStreamAffectedByRingerMode(
        /* [in] */ Int32 streamType,
        /* [out] */ Boolean* result);

    /**
     * Only useful for volume controllers.
     * @hide
     */
    CARAPI DisableSafeMediaVolume();

    /**
     * Set Hdmi Cec system audio mode.
     *
     * @param on whether to be on system audio mode
     * @return output device type. 0 (DEVICE_NONE) if failed to set device.
     * @hide
     */
    CARAPI SetHdmiSystemAudioSupported(
        /* [in] */ Boolean on,
        /* [out] */ Int32* result);

    /**
     * Returns true if Hdmi Cec system audio mode is supported.
     *
     * @hide
     */
    CARAPI IsHdmiSystemAudioSupported(
        /* [out] */ Boolean* result);

    /**
     * Returns a list of descriptors for all audio ports managed by the audio framework.
     * Audio ports are nodes in the audio framework or audio hardware that can be configured
     * or connected and disconnected with createAudioPatch() or releaseAudioPatch().
     * See AudioPort for a list of attributes of each audio port.
     * @param ports An AudioPort ArrayList where the list will be returned.
     * @hide
     */
    CARAPI ListAudioPorts(
        /* [in] */ IArrayList* ports,
        /* [out] */ Int32* result);

    /**
     * Specialized version of listAudioPorts() listing only audio devices (AudioDevicePort)
     * @see listAudioPorts(ArrayList<AudioPort>)
     * @hide
     */
    CARAPI ListAudioDevicePorts(
        /* [in] */ IArrayList* devices,
        /* [out] */ Int32* result);

    /**
     * Create a connection between two or more devices. The framework will reject the request if
     * device types are not compatible or the implementation does not support the requested
     * configuration.
     * NOTE: current implementation is limited to one source and one sink per patch.
     * @param patch AudioPatch array where the newly created patch will be returned.
     *              As input, if patch[0] is not null, the specified patch will be replaced by the
     *              new patch created. This avoids calling releaseAudioPatch() when modifying a
     *              patch and allows the implementation to optimize transitions.
     * @param sources List of source audio ports. All must be AudioPort.ROLE_SOURCE.
     * @param sinks   List of sink audio ports. All must be AudioPort.ROLE_SINK.
     *
     * @return - {@link #SUCCESS} if connection is successful.
     *         - {@link #ERROR_BAD_VALUE} if incompatible device types are passed.
     *         - {@link #ERROR_INVALID_OPERATION} if the requested connection is not supported.
     *         - {@link #ERROR_PERMISSION_DENIED} if the client does not have permission to create
     *         a patch.
     *         - {@link #ERROR_DEAD_OBJECT} if the server process is dead
     *         - {@link #ERROR} if patch cannot be connected for any other reason.
     *
     *         patch[0] contains the newly created patch
     * @hide
     */
    CARAPI CreateAudioPatch(
        /* [in] */ ArrayOf<IAudioPatch*>* patch,
        /* [in] */ ArrayOf<IAudioPortConfig*>* sources,
        /* [in] */ ArrayOf<IAudioPortConfig*>* sinks,
        /* [out] */ Int32* result);

    /**
     * Releases an existing audio patch connection.
     * @param patch The audio patch to disconnect.
     * @return - {@link #SUCCESS} if disconnection is successful.
     *         - {@link #ERROR_BAD_VALUE} if the specified patch does not exist.
     *         - {@link #ERROR_PERMISSION_DENIED} if the client does not have permission to release
     *         a patch.
     *         - {@link #ERROR_DEAD_OBJECT} if the server process is dead
     *         - {@link #ERROR} if patch cannot be released for any other reason.
     * @hide
     */
    CARAPI ReleaseAudioPatch(
        /* [in] */ IAudioPatch* patch,
        /* [out] */ Int32* result);

    /**
     * List all existing connections between audio ports.
     * @param patches An AudioPatch array where the list will be returned.
     * @hide
     */
    CARAPI ListAudioPatches(
        /* [in] */ IArrayList* patches,
        /* [out] */ Int32* result);

    /**
     * Set the gain on the specified AudioPort. The AudioGainConfig config is build by
     * AudioGain.buildConfig()
     * @hide
     */
    CARAPI SetAudioPortGain(
        /* [in] */ IAudioPort* port,
        /* [in] */ IAudioGainConfig* gain,
        /* [out] */ Int32* result);

    /**
     * Register an audio port list update listener.
     * @hide
     */
    CARAPI RegisterAudioPortUpdateListener(
        /* [in] */ IAudioManagerOnAudioPortUpdateListener* l);

    /**
     * Unregister an audio port list update listener.
     * @hide
     */
    CARAPI UnregisterAudioPortUpdateListener(
        /* [in] */ IAudioManagerOnAudioPortUpdateListener* l);

    CARAPI ResetAudioPortGeneration(
        /* [out] */ Int32* result);

    CARAPI UpdateAudioPortCache(
        /* [in] */ IArrayList* ports,
        /* [in] */ IArrayList* patches,
        /* [out] */ Int32* result);

    CARAPI UpdatePortConfig(
        /* [in] */ IAudioPortConfig* portCfg,
        /* [in] */ IArrayList* ports,
        /* [out] */ IAudioPortConfig** result);

    /**
     * Checks valid ringer mode values.
     *
     * @return true if the ringer mode indicated is valid, false otherwise.
     *
     * @see #setRingerMode(Int32)
     * @hide
     */
    static CARAPI IsValidRingerMode(
        /* [in] */ Int32 ringerMode,
        /* [out] */ Boolean* result);

    /**
     * Return true if the device code corresponds to an output device.
     * @hide
     */
    static CARAPI IsOutputDevice(
        /* [in] */ Int32 device,
        /* [out] */ Boolean* result);

    /**
     * Return true if the device code corresponds to an input device.
     * @hide
     */
    static CARAPI IsInputDevice(
        /* [in] */ Int32 device,
        /* [out] */ Boolean* result);

private:
    static CARAPI_(AutoPtr<IIAudioService>) GetService();

    /**
     * Settings has an in memory cache, so this is fast.
     */
    CARAPI_(Boolean) QuerySoundEffectsEnabled(
            /* [in] */ Int32 user);

    CARAPI_(AutoPtr<IAudioManagerOnAudioFocusChangeListener>) FindFocusListener(
        /* [in] */ const String& id);

    CARAPI_(String) GetIdForAudioFocusListener(
        /* [in] */ IAudioManagerOnAudioFocusChangeListener* l);

public:
    /**  @hide Default volume index values for audio streams */
    static const Int32 DEFAULT_STREAM_VOLUME[];

    static const Int32 AUDIOPORT_GENERATION_INIT;

    AutoPtr<IInteger32> mAudioPortGeneration;
    AutoPtr<IArrayList> mAudioPortsCached;
    AutoPtr<IArrayList> mAudioPatchesCached;

private:
    AutoPtr<IContext> mContext;
    Int64 mVolumeKeyUpTime;
    Boolean mUseMasterVolume;
    Boolean mUseVolumeKeySounds;
    Boolean mUseFixedVolume;
    AutoPtr<IBinder> mToken;
    static const String TAG;
    AutoPtr<AudioPortEventHandler> mAudioPortEventHandler;

    // maximum valid ringer mode value. Values must start from 0 and be contiguous.
    static const Int32 RINGER_MODE_MAX;

    static AutoPtr<IIAudioService> sService;

    /**
     * Map to convert focus event listener IDs, as used in the AudioService audio focus stack,
     * to actual listener objects.
     */
    HashMap<String, AutoPtr<IAudioManagerOnAudioFocusChangeListener> > mAudioFocusIdListenerMap;

    /**
     * Lock to prevent concurrent changes to the list of focus listeners for this AudioManager
     * instance.
     */
    Object mFocusListenerLock;

    /**
     * Handler for audio focus events coming from the audio service.
     */
    AutoPtr<FocusEventHandlerDelegate> mAudioFocusEventHandlerDelegate;

    AutoPtr<IIAudioFocusDispatcher> mAudioFocusDispatcher;

    /**
     * {@hide}
     */
    AutoPtr<IBinder> mICallBack;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CAUDIOMANAGER_H__
