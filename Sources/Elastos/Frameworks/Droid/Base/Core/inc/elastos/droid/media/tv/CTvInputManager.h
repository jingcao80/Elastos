#ifndef __ELASTOS_DROID_MEDIA_TV_CTVINPUTMANAGER_H__
#define __ELASTOS_DROID_MEDIA_TV_CTVINPUTMANAGER_H__

#include "_Elastos_Droid_Media_Tv_CTvInputManager.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Runnable.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::View::IInputChannel;
using Elastos::Droid::View::ISurface;
using Elastos::Utility::ILinkedList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

/**
 * Central system API to the overall TV input framework (TIF) architecture, which arbitrates
 * interaction between applications and the selected TV inputs.
 */
CarClass(CTvInputManager)
    , public Object
    , public ITvInputManager
{
private:
    class SessionCallbackRecord
        : public Object
    {
    public:
        SessionCallbackRecord(
            /* [in] */ ITvInputManagerSessionCallback* sessionCallback,
            /* [in] */ IHandler* handler,
            /* [in] */ CTvInputManager* host);

        CARAPI PostSessionCreated(
            /* [in] */ ITvInputManagerSession* session);

        CARAPI PostSessionReleased();

        CARAPI PostChannelRetuned(
            /* [in] */ IUri* channelUri);

        CARAPI PostTracksChanged(
            /* [in] */ IList* tracks);

        CARAPI PostTrackSelected(
            /* [in] */ Int32 type,
            /* [in] */ const String& trackId);

        CARAPI PostVideoAvailable();

        CARAPI PostVideoUnavailable(
            /* [in] */ Int32 reason);

        CARAPI PostContentAllowed();

        CARAPI PostContentBlocked(
            /* [in] */ ITvContentRating* rating);

        CARAPI PostLayoutSurface(
            /* [in] */ Int32 left,
            /* [in] */ Int32 top,
            /* [in] */ Int32 right,
            /* [in] */ Int32 bottom);

        CARAPI PostSessionEvent(
            /* [in] */ const String& eventType,
            /* [in] */ IBundle* eventArgs);

    public:
        AutoPtr<ITvInputManagerSessionCallback> mSessionCallback;
        AutoPtr<IHandler> mHandler;
        AutoPtr<ITvInputManagerSession> mSession;
        CTvInputManager* mHost;
    };

    class TvInputCallbackRecord
        : public Object
    {
    public:
        TvInputCallbackRecord(
            /* [in] */ ITvInputManagerTvInputCallback* callback,
            /* [in] */ IHandler* handler,
            /* [in] */ CTvInputManager* host);

        CARAPI GetCallback(
            /* [out] */ ITvInputManagerTvInputCallback** result);

        CARAPI PostInputStateChanged(
            /* [in] */ const String& inputId,
            /* [in] */ Int32 state);

        CARAPI PostInputAdded(
            /* [in] */ const String& inputId);

        CARAPI PostInputRemoved(
            /* [in] */ const String& inputId);

        CARAPI PostInputUpdated(
            /* [in] */ const String& inputId);

    public:
        AutoPtr<ITvInputManagerTvInputCallback> mCallback;
        AutoPtr<IHandler> mHandler;
        CTvInputManager* mHost;
    };

    class PostSessionCreatedRunnable
        : public Runnable
    {
    public:
        PostSessionCreatedRunnable(
            /* [in] */ SessionCallbackRecord* host,
            /* [in] */ ITvInputManagerSession* session)
            : mHost(host)
            , mSession(session)
        {}

        CARAPI Run();

    private:
        SessionCallbackRecord* mHost;
        AutoPtr<ITvInputManagerSession> mSession;
    };

    class PostSessionReleasedRunnable
        : public Runnable
    {
    public:
        PostSessionReleasedRunnable(
            /* [in] */ SessionCallbackRecord* host)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        SessionCallbackRecord* mHost;
    };

    class PostChannelRetunedRunnable
        : public Runnable
    {
    public:
        PostChannelRetunedRunnable(
            /* [in] */ SessionCallbackRecord* host,
            /* [in] */ IUri* channelUri)
            : mHost(host)
            , mChannelUri(channelUri)
        {}

        CARAPI Run();

    private:
        SessionCallbackRecord* mHost;
        AutoPtr<IUri> mChannelUri;
    };

    class PostTracksChangedRunnable
        : public Runnable
    {
    public:
        PostTracksChangedRunnable(
            /* [in] */ SessionCallbackRecord* host,
            /* [in] */ IList* tracks)
            : mHost(host)
            , mTracks(tracks)
        {}

        CARAPI Run();

    private:
        SessionCallbackRecord* mHost;
        AutoPtr<IList> mTracks;
    };

    class PostTrackSelectedRunnable
        : public Runnable
    {
    public:
        PostTrackSelectedRunnable(
            /* [in] */ SessionCallbackRecord* host,
            /* [in] */ Int32 type,
            /* [in] */ const String& trackId)
            : mHost(host)
            , mType(type)
            , mTrackId(trackId)
        {}

        CARAPI Run();

    private:
        SessionCallbackRecord* mHost;
        Int32 mType;
        String mTrackId;
    };

    class PostVideoAvailableRunnable
        : public Runnable
    {
    public:
        PostVideoAvailableRunnable(
            /* [in] */ SessionCallbackRecord* host)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        SessionCallbackRecord* mHost;
    };

    class PostVideoUnavailableRunnable
        : public Runnable
    {
    public:
        PostVideoUnavailableRunnable(
            /* [in] */ SessionCallbackRecord* host,
            /* [in] */ Int32 reason)
            : mHost(host)
            , mReason(reason)
        {}

        CARAPI Run();

    private:
        SessionCallbackRecord* mHost;
        Int32 mReason;
    };

    class PostContentAllowedRunnable
        : public Runnable
    {
    public:
        PostContentAllowedRunnable(
            /* [in] */ SessionCallbackRecord* host)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        SessionCallbackRecord* mHost;
    };

    class PostContentBlockedRunnable
        : public Runnable
    {
    public:
        PostContentBlockedRunnable(
            /* [in] */ SessionCallbackRecord* host,
            /* [in] */ ITvContentRating* rating)
            : mHost(host)
            , mRating(rating)
        {}

        CARAPI Run();

    private:
        SessionCallbackRecord* mHost;
        AutoPtr<ITvContentRating> mRating;
    };

    class PostLayoutSurfaceRunnable
        : public Runnable
    {
    public:
        PostLayoutSurfaceRunnable(
            /* [in] */ SessionCallbackRecord* host,
            /* [in] */ Int32 left,
            /* [in] */ Int32 top,
            /* [in] */ Int32 right,
            /* [in] */ Int32 bottom)
            : mHost(host)
            , mLeft(left)
            , mTop(top)
            , mRight(right)
            , mBottom(bottom)
        {}

        CARAPI Run();

    private:
        SessionCallbackRecord* mHost;
        Int32 mLeft;
        Int32 mTop;
        Int32 mRight;
        Int32 mBottom;
    };

    class PostSessionEventRunnable
        : public Runnable
    {
    public:
        PostSessionEventRunnable(
            /* [in] */ SessionCallbackRecord* host,
            /* [in] */ const String& eventType,
            /* [in] */ IBundle* eventArgs)
            : mHost(host)
            , mEventType(eventType)
            , mEventArgs(eventArgs)
        {}

        CARAPI Run();

    private:
        SessionCallbackRecord* mHost;
        String mEventType;
        AutoPtr<IBundle> mEventArgs;
    };

    class PostInputStateChangedRunnable
        : public Runnable
    {
    public:
        PostInputStateChangedRunnable(
            /* [in] */ TvInputCallbackRecord* host,
            /* [in] */ const String& inputId,
            /* [in] */ Int32 state)
            : mHost(host)
            , mInputId(inputId)
            , mState(state)
        {}

        CARAPI Run();

    private:
        TvInputCallbackRecord* mHost;
        String mInputId;
        Int32 mState;
    };

    class PostInputAddedRunnable
        : public Runnable
    {
    public:
        PostInputAddedRunnable(
            /* [in] */ TvInputCallbackRecord* host,
            /* [in] */ const String& inputId)
            : mHost(host)
            , mInputId(inputId)
        {}

        CARAPI Run();

    private:
        TvInputCallbackRecord* mHost;
        String mInputId;
    };

    class PostInputRemovedRunnable
        : public Runnable
    {
    public:
        PostInputRemovedRunnable(
            /* [in] */ TvInputCallbackRecord* host,
            /* [in] */ const String& inputId)
            : mHost(host)
            , mInputId(inputId)
        {}

        CARAPI Run();

    private:
        TvInputCallbackRecord* mHost;
        String mInputId;
    };

    class PostInputUpdatedRunnable
        : public Runnable
    {
    public:
        PostInputUpdatedRunnable(
            /* [in] */ TvInputCallbackRecord* host,
            /* [in] */ const String& inputId)
            : mHost(host)
            , mInputId(inputId)
        {}

        CARAPI Run();

    private:
        TvInputCallbackRecord* mHost;
        String mInputId;
    };

    class TvInputClient
        : public Object
        , public IITvInputClient
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        TvInputClient(
            /* [in] */ CTvInputManager* host)
            : mHost(host)
        {}

        CARAPI OnSessionCreated(
            /* [in] */ const String& inputId,
            /* [in] */ IBinder * token,
            /* [in] */ IInputChannel * channel,
            /* [in] */ Int32 seq);

        CARAPI OnSessionReleased(
            /* [in] */ Int32 seq);

        CARAPI OnSessionEvent(
            /* [in] */ const String& name,
            /* [in] */ IBundle * args,
            /* [in] */ Int32 seq);

        CARAPI OnChannelRetuned(
            /* [in] */ IUri * channelUri,
            /* [in] */ Int32 seq);

        CARAPI OnTracksChanged(
            /* [in] */ IList * tracks,
            /* [in] */ Int32 seq);

        CARAPI OnTrackSelected(
            /* [in] */ Int32 type,
            /* [in] */ const String& trackId,
            /* [in] */ Int32 seq);

        CARAPI OnVideoAvailable(
            /* [in] */ Int32 seq);

        CARAPI OnVideoUnavailable(
            /* [in] */ Int32 reason,
            /* [in] */ Int32 seq);

        CARAPI OnContentAllowed(
            /* [in] */ Int32 seq);

        CARAPI OnContentBlocked(
            /* [in] */ const String& rating,
            /* [in] */ Int32 seq);

        CARAPI OnLayoutSurface(
            /* [in] */ Int32 left,
            /* [in] */ Int32 top,
            /* [in] */ Int32 right,
            /* [in] */ Int32 bottom,
            /* [in] */ Int32 seq);

        CARAPI ToString(
            /* [out] */ String* result);

    private:
        CTvInputManager* mHost;
    };

    class TvInputManagerCallback
        : public Object
        , public IITvInputManagerCallback
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        TvInputManagerCallback(
            /* [in] */ CTvInputManager* host)
            : mHost(host)
        {}

        CARAPI OnInputStateChanged(
            /* [in] */ const String& inputId,
            /* [in] */ Int32 state);

        CARAPI OnInputAdded(
            /* [in] */ const String& inputId);

        CARAPI OnInputRemoved(
            /* [in] */ const String& inputId);

        CARAPI OnInputUpdated(
            /* [in] */ const String& inputId);

        CARAPI ToString(
            /* [out] */ String* result);

    private:
        CTvInputManager* mHost;
    };

    class TvInputHardwareCallback
        : public Object
        , public IITvInputHardwareCallback
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        TvInputHardwareCallback(
            /* [in] */ CTvInputManager* host,
            /* [in] */ ITvInputManagerHardwareCallback * callback)
            : mHost(host)
            , mCallback(callback)
        {}

        CARAPI OnReleased();

        CARAPI OnStreamConfigChanged(
            /* [in] */ ArrayOf<ITvStreamConfig*> * configs);

        CARAPI ToString(
            /* [out] */ String* result);

    private:
        CTvInputManager* mHost;
        AutoPtr<ITvInputManagerHardwareCallback> mCallback;
    };

public:
    friend class CTvInputManagerSession;

    CTvInputManager();

    virtual ~CTvInputManager();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IITvInputManager* service,
        /* [in] */ Int32 userId);

    /**
     * Returns the complete list of TV inputs on the system.
     *
     * @return List of {@link TvInputInfo} for each TV input that describes its meta information.
     */
    CARAPI GetTvInputList(
        /* [out] */ IList ** result);

    /**
     * Returns the {@link TvInputInfo} for a given TV input.
     *
     * @param inputId The ID of the TV input.
     * @return the {@link TvInputInfo} for a given TV input. {@code null} if not found.
     */
    CARAPI GetTvInputInfo(
        /* [in] */ const String& inputId,
        /* [out] */ ITvInputInfo ** result);

    /**
     * Returns the state of a given TV input. It retuns one of the following:
     * <ul>
     * <li>{@link #INPUT_STATE_CONNECTED}
     * <li>{@link #INPUT_STATE_CONNECTED_STANDBY}
     * <li>{@link #INPUT_STATE_DISCONNECTED}
     * </ul>
     *
     * @param inputId The id of the TV input.
     * @throws IllegalArgumentException if the argument is {@code null} or if there is no
     *        {@link TvInputInfo} corresponding to {@code inputId}.
     */
    CARAPI GetInputState(
        /* [in] */ const String& inputId,
        /* [out] */ Int32 * result);

    /**
     * Registers a {@link TvInputCallback}.
     *
     * @param callback A callback used to monitor status of the TV inputs.
     * @param handler A {@link Handler} that the status change will be delivered to.
     * @throws IllegalArgumentException if any of the arguments is {@code null}.
     */
    CARAPI RegisterCallback(
        /* [in] */ ITvInputManagerTvInputCallback * cb,
        /* [in] */ IHandler * handler);

    /**
     * Unregisters the existing {@link TvInputCallback}.
     *
     * @param callback The existing callback to remove.
     * @throws IllegalArgumentException if any of the arguments is {@code null}.
     */
    CARAPI UnregisterCallback(
        /* [in] */ ITvInputManagerTvInputCallback * cb);

    /**
     * Returns the user's parental controls enabled state.
     *
     * @return {@code true} if the user enabled the parental controls, {@code false} otherwise.
     */
    CARAPI IsParentalControlsEnabled(
        /* [out] */ Boolean * result);

    /**
     * Sets the user's parental controls enabled state.
     *
     * @param enabled The user's parental controls enabled state. {@code true} if the user enabled
     *            the parental controls, {@code false} otherwise.
     * @see #isParentalControlsEnabled
     * @hide
     */
    CARAPI SetParentalControlsEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Checks whether a given TV content rating is blocked by the user.
     *
     * @param rating The TV content rating to check.
     * @return {@code true} if the given TV content rating is blocked, {@code false} otherwise.
     */
    CARAPI IsRatingBlocked(
        /* [in] */ ITvContentRating * rating,
        /* [out] */ Boolean * result);

    /**
     * Returns the list of blocked content ratings.
     *
     * @return the list of content ratings blocked by the user.
     * @hide
     */
    CARAPI GetBlockedRatings(
        /* [out] */ IList ** result);

    /**
     * Adds a user blocked content rating.
     *
     * @param rating The content rating to block.
     * @see #isRatingBlocked
     * @see #removeBlockedRating
     * @hide
     */
    CARAPI AddBlockedRating(
        /* [in] */ ITvContentRating * rating);

    /**
     * Removes a user blocked content rating.
     *
     * @param rating The content rating to unblock.
     * @see #isRatingBlocked
     * @see #addBlockedRating
     * @hide
     */
    CARAPI RemoveBlockedRating(
        /* [in] */ ITvContentRating * rating);

    /**
     * Returns the list of all TV content rating systems defined.
     * @hide
     */
    CARAPI GetTvContentRatingSystemList(
        /* [out] */ IList ** result);

    /**
     * Creates a {@link Session} for a given TV input.
     * <p>
     * The number of sessions that can be created at the same time is limited by the capability of
     * the given TV input.
     * </p>
     *
     * @param inputId The id of the TV input.
     * @param callback A callback used to receive the created session.
     * @param handler A {@link Handler} that the session creation will be delivered to.
     * @throws IllegalArgumentException if any of the arguments is {@code null}.
     * @hide
     */
    CARAPI CreateSession(
        /* [in] */ const String& inputId,
        /* [in] */ ITvInputManagerSessionCallback * cb,
        /* [in] */ IHandler * handler);

    /**
     * Returns the TvStreamConfig list of the given TV input.
     *
     * @param inputId the id of the TV input.
     * @return List of {@link TvStreamConfig} which is available for capturing
     *   of the given TV input.
     * @hide
     */
    CARAPI GetAvailableTvStreamConfigList(
        /* [in] */ const String& inputId,
        /* [out] */ IList ** result);

    /**
     * Take a snapshot of the given TV input into the provided Surface.
     *
     * @param inputId the id of the TV input.
     * @param surface the {@link Surface} to which the snapshot is captured.
     * @param config the {@link TvStreamConfig} which is used for capturing.
     * @return true when the {@link Surface} is ready to be captured.
     * @hide
     */
    CARAPI CaptureFrame(
        /* [in] */ const String& inputId,
        /* [in] */ ISurface * surface,
        /* [in] */ ITvStreamConfig * config,
        /* [out] */ Boolean * result);

    /**
     * Returns true if there is only a single TV input session.
     *
     * @hide
     */
    CARAPI IsSingleSessionActive(
        /* [out] */ Boolean * result);

    /**
     * Returns a list of TvInputHardwareInfo objects representing available hardware.
     *
     * @hide
     */
    CARAPI GetHardwareList(
        /* [out] */ IList ** result);

    /**
     * Returns acquired TvInputManager.Hardware object for given deviceId.
     *
     * If there are other Hardware object acquired for the same deviceId, calling this method will
     * preempt the previously acquired object and report {@link HardwareCallback#onReleased} to the
     * old object.
     *
     * @hide
     */
    CARAPI AcquireTvInputHardware(
        /* [in] */ Int32 deviceId,
        /* [in] */ ITvInputManagerHardwareCallback * cb,
        /* [in] */ ITvInputInfo * info,
        /* [out] */ ITvInputManagerHardware ** result);

    /**
     * Releases previously acquired hardware object.
     *
     * @hide
     */
    CARAPI ReleaseTvInputHardware(
        /* [in] */ Int32 deviceId,
        /* [in] */ ITvInputManagerHardware * hardware);

private:
    static String TAG;

    AutoPtr<IITvInputManager> mService;

    Object mLock;

    // @GuardedBy(mLock)
    AutoPtr<ILinkedList> mCallbackRecords;

    // A mapping from TV input ID to the state of corresponding input.
    // @GuardedBy(mLock)
    AutoPtr<IArrayMap> mStateMap;

    // A mapping from the sequence number of a session to its SessionCallbackRecord.
    AutoPtr<ISparseArray> mSessionCallbackRecordMap;

    // A sequence number for the next session to be created. Should be protected by a lock
    // {@code mSessionCallbackRecordMap}.
    Int32 mNextSeq;

    AutoPtr<IITvInputClient> mClient;

    AutoPtr<IITvInputManagerCallback> mManagerCallback;

    Int32 mUserId;
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_CTVINPUTMANAGER_H__
