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

#ifndef __ELASTOS_DROID_MEDIA_CJETPLAYER_H__
#define __ELASTOS_DROID_MEDIA_CJETPLAYER_H__

#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Content.h"
#include "_Elastos_Droid_Media_CJetPlayer.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::Res::IAssetFileDescriptor;
using Elastos::Droid::Os::Handler;
using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Media {
/**
 * JetPlayer provides access to JET content playback and control.
 *
 * <p>Please refer to the JET Creator User Manual for a presentation of the JET interactive
 * music concept and how to use the JetCreator tool to create content to be player by JetPlayer.
 *
 * <p>Use of the JetPlayer class is based around the playback of a number of JET segments
 * sequentially added to a playback FIFO queue. The rendering of the MIDI content stored in each
 * segment can be dynamically affected by two mechanisms:
 * <ul>
 * <li>tracks in a segment can be muted or unmuted at any moment, individually or through
 *    a mask (to change the mute state of multiple tracks at once)</li>
 * <li>parts of tracks in a segment can be played at predefined points in the segment, in order
 *    to maintain synchronization with the other tracks in the segment. This is achieved through
 *    the notion of "clips", which can be triggered at any time, but that will play only at the
 *    right time, as authored in the corresponding JET file.</li>
 * </ul>
 * As a result of the rendering and playback of the JET segments, the user of the JetPlayer instance
 * can receive notifications from the JET engine relative to:
 * <ul>
 * <li>the playback state,</li>
 * <li>the number of segments left to play in the queue,</li>
 * <li>application controller events (CC80-83) to mark points in the MIDI segments.</li>
 * </ul>
 *  Use {@link #getJetPlayer()} to construct a JetPlayer instance. JetPlayer is a singleton class.
 *
 */
CarClass(CJetPlayer)
    , public Object
    , public IJetPlayer
{
private:
    class NativeEventHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CJetPlayer::NativeEventHandler")

        NativeEventHandler(
            /* [in] */ IWeakReference* jet,
            /* [in] */ ILooper* looper);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

public:
    CJetPlayer();

    ~CJetPlayer();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /**
     * Cloning a JetPlayer instance is not supported. Calling clone() will generate an exception.
     */
    CARAPI Clone(
        /* [out] */ IJetPlayer** jetPlayer);

    /**
     * Stops the current JET playback, and releases all associated native resources.
     * The object can no longer be used and the reference should be set to null
     * after a call to release().
     */
    CARAPI ReleaseResources();

    /**
     * Loads a .jet file from a given path.
     * @param path the path to the .jet file, for instance "/sdcard/mygame/music.jet".
     * @return true if loading the .jet file was successful, false if loading failed.
     */
    CARAPI LoadJetFile(
        /* [in] */ const String& path,
        /* [out] */ Boolean* result);

    /**
     * Loads a .jet file from an asset file descriptor.
     * @param afd the asset file descriptor.
     * @return true if loading the .jet file was successful, false if loading failed.
     */
    CARAPI LoadJetFile(
        /* [in] */ IAssetFileDescriptor* afd,
        /* [out] */ Boolean* result);

    /**
     * Closes the resource containing the JET content.
     * @return true if successfully closed, false otherwise.
     */
    CARAPI CloseJetFile(
        /* [out] */ Boolean* result);

    /**
     * Starts playing the JET segment queue.
     * @return true if rendering and playback is successfully started, false otherwise.
     */
    CARAPI Play(
        /* [out] */ Boolean* result);

    /**
     * Pauses the playback of the JET segment queue.
     * @return true if rendering and playback is successfully paused, false otherwise.
     */
    CARAPI Pause(
        /* [out] */ Boolean* result);

    /**
     * Queues the specified segment in the JET queue.
     * @param segmentNum the identifier of the segment.
     * @param libNum the index of the sound bank associated with the segment. Use -1 to indicate
     *    that no sound bank (DLS file) is associated with this segment, in which case JET will use
     *    the General MIDI library.
     * @param repeatCount the number of times the segment will be repeated. 0 means the segment will
     *    only play once. -1 means the segment will repeat indefinitely.
     * @param transpose the amount of pitch transposition. Set to 0 for normal playback.
     *    Range is -12 to +12.
     * @param muteFlags a bitmask to specify which MIDI tracks will be muted during playback. Bit 0
     *    affects track 0, bit 1 affects track 1 etc.
     * @param userID a value specified by the application that uniquely identifies the segment.
     *    this value is received in the
     *    {@link OnJetEventListener#onJetUserIdUpdate(JetPlayer, int, int)} event listener method.
     *    Normally, the application will keep a byte value that is incremented each time a new
     *    segment is queued up. This can be used to look up any special characteristics of that
     *    track including trigger clips and mute flags.
     * @return true if the segment was successfully queued, false if the queue is full or if the
     *    parameters are invalid.
     */
    CARAPI QueueJetSegment(
        /* [in] */ Int32 segmentNum,
        /* [in] */ Int32 libNum,
        /* [in] */ Int32 repeatCount,
        /* [in] */ Int32 transpose,
        /* [in] */ Int32 muteFlags,
        /* [in] */ Byte userID,
        /* [out] */ Boolean* result);

    /**
     * Queues the specified segment in the JET queue.
     * @param segmentNum the identifier of the segment.
     * @param libNum the index of the soundbank associated with the segment. Use -1 to indicate that
     *    no sound bank (DLS file) is associated with this segment, in which case JET will use
     *    the General MIDI library.
     * @param repeatCount the number of times the segment will be repeated. 0 means the segment will
     *    only play once. -1 means the segment will repeat indefinitely.
     * @param transpose the amount of pitch transposition. Set to 0 for normal playback.
     *    Range is -12 to +12.
     * @param muteArray an array of booleans to specify which MIDI tracks will be muted during
     *    playback. The value at index 0 affects track 0, value at index 1 affects track 1 etc.
     *    The length of the array must be {@link #getMaxTracks()} for the call to succeed.
     * @param userID a value specified by the application that uniquely identifies the segment.
     *    this value is received in the
     *    {@link OnJetEventListener#onJetUserIdUpdate(JetPlayer, int, int)} event listener method.
     *    Normally, the application will keep a byte value that is incremented each time a new
     *    segment is queued up. This can be used to look up any special characteristics of that
     *    track including trigger clips and mute flags.
     * @return true if the segment was successfully queued, false if the queue is full or if the
     *    parameters are invalid.
     */
    CARAPI QueueJetSegmentMuteArray(
        /* [in] */ Int32 segmentNum,
        /* [in] */ Int32 libNum,
        /* [in] */ Int32 repeatCount,
        /* [in] */ Int32 transpose,
        /* [in] */ ArrayOf<Boolean>* muteArray,
        /* [in] */ Byte userID,
        /* [out] */ Boolean* result);

    /**
     * Modifies the mute flags.
     * @param muteFlags a bitmask to specify which MIDI tracks are muted. Bit 0 affects track 0,
     *    bit 1 affects track 1 etc.
     * @param sync if false, the new mute flags will be applied as soon as possible by the JET
     *    render and playback engine. If true, the mute flags will be updated at the start of the
     *    next segment. If the segment is repeated, the flags will take effect the next time
     *    segment is repeated.
     * @return true if the mute flags were successfully updated, false otherwise.
     */
    CARAPI SetMuteFlags(
        /* [in] */ Int32 muteFlags,
        /* [in] */ Boolean sync,
        /* [out] */ Boolean* result);

    /**
     * Modifies the mute flags for the current active segment.
     * @param muteArray an array of booleans to specify which MIDI tracks are muted. The value at
     *    index 0 affects track 0, value at index 1 affects track 1 etc.
     *    The length of the array must be {@link #getMaxTracks()} for the call to succeed.
     * @param sync if false, the new mute flags will be applied as soon as possible by the JET
     *    render and playback engine. If true, the mute flags will be updated at the start of the
     *    next segment. If the segment is repeated, the flags will take effect the next time
     *    segment is repeated.
     * @return true if the mute flags were successfully updated, false otherwise.
     */
    CARAPI SetMuteArray(
        /* [in] */ ArrayOf<Boolean>* muteArray,
        /* [in] */ Boolean sync,
        /* [out] */ Boolean* result);

    /**
     * Mutes or unmutes a single track.
     * @param trackId the index of the track to mute.
     * @param muteFlag set to true to mute, false to unmute.
     * @param sync if false, the new mute flags will be applied as soon as possible by the JET
     *    render and playback engine. If true, the mute flag will be updated at the start of the
     *    next segment. If the segment is repeated, the flag will take effect the next time
     *    segment is repeated.
     * @return true if the mute flag was successfully updated, false otherwise.
     */
    CARAPI SetMuteFlag(
        /* [in] */ Int32 trackId,
        /* [in] */ Boolean muteFlag,
        /* [in] */ Boolean sync,
        /* [out] */ Boolean* result);

    /**
     * Schedules the playback of a clip.
     * This will automatically update the mute flags in sync with the JET Clip Marker (controller
     * 103). The parameter clipID must be in the range of 0-63. After the call to triggerClip, when
     * JET next encounters a controller event 103 with bits 0-5 of the value equal to clipID and
     * bit 6 set to 1, it will automatically unmute the track containing the controller event.
     * When JET encounters the complementary controller event 103 with bits 0-5 of the value equal
     * to clipID and bit 6 set to 0, it will mute the track again.
     * @param clipId the identifier of the clip to trigger.
     * @return true if the clip was successfully triggered, false otherwise.
     */
    CARAPI TriggerClip(
        /* [in] */ Int32 clipId,
        /* [out] */ Boolean* result);

    /**
     * Empties the segment queue, and clears all clips that are scheduled for playback.
     * @return true if the queue was successfully cleared, false otherwise.
     */
    CARAPI ClearQueue(
        /* [out] */ Boolean* result);

    CARAPI SetEventListener(
        /* [in] */ IOnJetEventListener* listener);

    CARAPI SetEventListener(
        /* [in] */ IOnJetEventListener* listener,
        /* [in] */ IHandler* handler);

public:
    static CARAPI_(AutoPtr<IJetPlayer>) GetJetPlayer();

    static CARAPI_(Int32) GetMaxTracks();

    static CARAPI_(void) PostEventFromNative(
            /* [in] */ IWeakReference* jetplayer_ref,
            /* [in] */ Int32 what,
            /* [in] */ Int32 arg1,
            /* [in] */ Int32 arg2);

private:
    CARAPI_(Boolean) NativeSetup(
        /* [in] */ Int32 maxTracks,
        /* [in] */ Int32 trackBufferSize);

    CARAPI_(void) NativeFinalize();

    CARAPI_(void) NativeRelease();

    CARAPI NativeLoadJetFromFile(
        /* [in] */ const String& pathToJetFile,
        /* [out] */ Boolean* result);

    CARAPI NativeLoadJetFromFileD(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int64 offset,
        /* [in] */ Int64 len,
        /* [out] */ Boolean* result);

    CARAPI NativeCloseJetFile(
        /* [out] */ Boolean* result);

    CARAPI NativePlayJet(
        /* [out] */ Boolean* result);

    CARAPI NativePauseJet(
        /* [out] */ Boolean* result);

    CARAPI NativeQueueJetSegment(
        /* [in] */ Int32 segmentNum,
        /* [in] */ Int32 libNum,
        /* [in] */ Int32 repeatCount,
        /* [in] */ Int32 transpose,
        /* [in] */ Int32 muteFlags,
        /* [in] */ Byte userID,
        /* [out] */ Boolean* result);

    CARAPI NativeQueueJetSegmentMuteArray(
        /* [in] */ Int32 segmentNum,
        /* [in] */ Int32 libNum,
        /* [in] */ Int32 repeatCount,
        /* [in] */ Int32 transpose,
        /* [in] */ ArrayOf<Boolean>* muteArray,
        /* [in] */ Byte userID,
        /* [out] */ Boolean* result);

    CARAPI NativeSetMuteFlags(
        /* [in] */ Int32 muteFlags,
        /* [in] */ Boolean sync,
        /* [out] */ Boolean* result);

    CARAPI NativeSetMuteArray(
        /* [in] */ ArrayOf<Boolean>* muteArray,
        /* [in] */ Boolean sync,
        /* [out] */ Boolean* result);

    CARAPI NativeSetMuteFlag(
        /* [in] */ Int32 trackId,
        /* [in] */ Boolean muteFlag,
        /* [in] */ Boolean sync,
        /* [out] */ Boolean* result);

    CARAPI NativeTriggerClip(
        /* [in] */ Int32 clipId,
        /* [out] */ Boolean* result);

    CARAPI NativeClearQueue(
        /* [out] */ Boolean* result);

private:
    static CARAPI_(void) Logd(
        /* [in] */ const String& msg);

    static CARAPI_(void) Loge(
        /* [in] */ const String& msg);

private:
    static const String TAG;

    /**
     * The maximum number of simultaneous tracks. Use {@link #getMaxTracks()} to
     * access this value.
     */
    static const Int32 MAXTRACKS;

    /**
     * to keep in sync with the JetPlayer class constants
     * defined in frameworks/base/include/media/JetPlayer.h
     */
    static const Int32 JET_EVENT;
    static const Int32 JET_USERID_UPDATE;
    static const Int32 JET_NUMQUEUEDSEGMENT_UPDATE;
    static const Int32 JET_PAUSE_UPDATE;

    /**
     * to keep in sync with external/sonivox/arm-wt-22k/lib_src/jet_data.h
     * Encoding of event information on 32 bits
     */
    static const Int32 JET_EVENT_VAL_MASK;        // mask for value
    static const Int32 JET_EVENT_CTRL_MASK;       // mask for controller
    static const Int32 JET_EVENT_CHAN_MASK;       // mask for channel
    static const Int32 JET_EVENT_TRACK_MASK;      // mask for track number
    static const Int32 JET_EVENT_SEG_MASK;        // mask for segment ID

    static const Int32 JET_EVENT_CTRL_SHIFT;      // shift to get controller number to bit 0
    static const Int32 JET_EVENT_CHAN_SHIFT;      // shift to get MIDI channel to bit 0
    static const Int32 JET_EVENT_TRACK_SHIFT;     // shift to get track ID to bit 0
    static const Int32 JET_EVENT_SEG_SHIFT;       // shift to get segment ID to bit 0

    /**
     * to keep in sync with values used in external/sonivox/arm-wt-22k/Android.mk
     * Jet rendering audio parameters
     */
    static const Int32 JET_OUTPUT_RATE;           // _SAMPLE_RATE_22050 in Android.mk
    static const Int32 JET_OUTPUT_CHANNEL_CONFIG; // NUM_OUTPUT_CHANNELS=2 in Android.mk

private:
    friend class NativeEventHandler;

    // JetPlayer is a singleton class.
    static AutoPtr<IJetPlayer> singletonRef;

    /**
     * Handler for jet events and status updates coming from the native code
     */
    AutoPtr<IHandler> mEventHandler;

    /**
     * Looper associated with the thread that creates the AudioTrack instance
     */
    AutoPtr<ILooper> mInitializationLooper;

    /**
     * Lock to protect the event listener updates against event notifications
     */
    Object mEventListenerLock;

    AutoPtr<IOnJetEventListener> mJetEventListener;

    /**
     * Accessed by native methods: provides access to C++ JetPlayer object
     */
    Int64 mNativePlayer;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_MEDIA_CJETPLAYER_H__
