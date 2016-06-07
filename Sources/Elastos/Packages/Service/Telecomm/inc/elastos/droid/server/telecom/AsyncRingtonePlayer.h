
#ifndef __ELASTOS_DROID_SERVER_TELECOM_ASYNCRINGTONEPLAYER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_ASYNCRINGTONEPLAYER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/os/Handler.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IRingtone;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Plays the default ringtone. Uses {@link Ringtone} in a separate thread so that this class can be
 * used from the main thread.
 */
class AsyncRingtonePlayer
    : public Object
{
private:
    class SubHandler
        : public Handler
    {
    public:
        SubHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ AsyncRingtonePlayer* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        AsyncRingtonePlayer* mHost;
    };

public:
    AsyncRingtonePlayer();

    CARAPI SetPhoneId(
        /* [in] */ Int32 phoneId);

    CARAPI constructor(
        /* [in] */ IContext* context);

    /** Plays the ringtone. */
    CARAPI Play(
        /* [in] */ IUri* ringtone,
        /* [in] */ Float incStartVolume,
        /* [in] */ Int32 incRampUpTime);

    /** Stops playing the ringtone. */
    CARAPI Stop();

private:
    /**
     * Posts a message to the ringtone-thread handler. Creates the handler if specified by the
     * parameter shouldCreateHandler.
     *
     * @param messageCode The message to post.
     * @param shouldCreateHandler True when a handler should be created to handle this message.
     */
    CARAPI PostMessage(
        /* [in] */ Int32 messageCode,
        /* [in] */ Boolean shouldCreateHandler,
        /* [in] */ IUri* ringtone,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2);

    /**
     * Creates a new ringtone Handler running in its own thread.
     */
    CARAPI GetNewHandler(
        /* [out] */ IHandler** result);

    /**
     * Starts the actual playback of the ringtone. Executes on ringtone-thread.
     */
    CARAPI HandlePlay(
        /* [in] */ IUri* ringtoneUri,
        /* [in] */ Float incStartVolume,
        /* [in] */ Int32 incRampUpTime);

    CARAPI HandleRepeat();

    /**
     * Stops the playback of the ringtone. Executes on the ringtone-thread.
     */
    CARAPI HandleStop();

    CARAPI GetRingtone(
        /* [in] */ IUri* ringtoneUri,
        /* [out] */ IRingtone** result);

private:
    // Message codes used with the ringtone thread.
    static const Int32 EVENT_PLAY;

    static const Int32 EVENT_STOP;

    static const Int32 EVENT_REPEAT;

    static const Int32 EVENT_INCREASE_VOLUME;

    // The interval in which to restart the ringer.
    static const Int32 RESTART_RINGER_MILLIS;

    /** Handler running on the ringtone thread. */
    AutoPtr<IHandler> mHandler;

    /** The current ringtone. Only used by the ringtone thread. */
    AutoPtr<IRingtone> mRingtone;

    Float mIncrementAmount;

    Float mCurrentIncrementVolume;

    Int32 mPhoneId;

    /**
     * The context.
     */
    AutoPtr<IContext> mContext;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_ASYNCRINGTONEPLAYER_H__
