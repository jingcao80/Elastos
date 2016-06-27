#ifndef  __ELASTOS_DROID_PHONE_RINGER_H__
#define  __ELASTOS_DROID_PHONE_RINGER_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/os/Handler.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>
#include <elastos/core/Thread.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Media::IRingtone;
using Elastos::Droid::Media::IAudioAttributes;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IIPowerManager;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::Handler;
using Elastos::Core::Object;
using Elastos::Core::Thread;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * Ringer manager for the Phone app.
 */
class Ringer
    : public Object
{
private:
    class VibratorThread
        : public Thread
    {
    public:
        TO_STRING_IMPL("Ringer::VibratorThread")

        VibratorThread(
            /* [in] */ Ringer* host);

        CARAPI Run();

    private:
        Ringer* mHost;
    };

    class Worker
        : public Runnable
    {
    public:
        TO_STRING_IMPL("Ringer::Worker")

        Worker(
            /* [in] */ Ringer* host,
            /* [in] */ const String& name);

        CARAPI GetLooper(
            /* [out] */ ILooper** looper);

        CARAPI Run();

        CARAPI Quit();

    private:
        Ringer* mHost;

        Object mLock;
        AutoPtr<ILooper> mLooper;
    };

    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("Ringer::MyHandler")

        MyHandler(
            /* [in] */ Ringer* host,
            /* [in] */ ILooper* looper);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        Ringer* mHost;
    };

public:
    TO_STRING_IMPL("Ringer")

    /**
     * Initialize the singleton Ringer instance.
     * This is only done once, at startup, from PhoneApp.onCreate().
     */
    static CARAPI_(AutoPtr<Ringer>) Init(
        /* [in] */ IContext* context,
        /* [in] */ IBluetoothManager* bluetoothManager);

    /**
     * After a radio technology change, e.g. from CDMA to GSM or vice versa,
     * the Context of the Ringer has to be updated. This is done by that function.
     *
     * @parameter Phone, the new active phone for the appropriate radio
     * technology
     */
    CARAPI UpdateRingerContextAfterRadioTechnologyChange(
        /* [in] */ IPhone* phone);

    /**
     * @return true if we're playing a ringtone and/or vibrating
     *     to indicate that there's an incoming call.
     *     ("Ringing" here is used in the general sense.  If you literally
     *     need to know if we're playing a ringtone or vibrating, use
     *     isRingtonePlaying() or isVibrating() instead.)
     *
     * @see isVibrating
     * @see isRingtonePlaying
     */
    CARAPI IsRinging(
        /* [out] */ Boolean* result);

    /**
     * Starts the ringtone and/or vibrator
     */
    CARAPI Ring();

    CARAPI ShouldVibrate(
        /* [out] */ Boolean* result);

    /**
     * Stops the ringtone and/or vibrator if any of these are actually
     * ringing/vibrating.
     */
    CARAPI StopRing();

    /**
     * Sets the ringtone uri in preparation for ringtone creation
     * in makeLooper().  This uri is defaulted to the phone-wide
     * default ringtone.
     */
    CARAPI SetCustomRingtoneUri(
        /* [in] */ IUri* uri);

private:
    /** Private constructor; @see init() */
    Ringer(
        /* [in] */ IContext* context,
        /* [in] */ IBluetoothManager* bluetoothManager);

    /**
     * @return true if the ringtone is playing
     * @see isVibrating
     * @see isRinging
     */
    CARAPI_(Boolean) IsRingtonePlaying();

    /**
     * @return true if we're vibrating in response to an incoming call
     * @see isVibrating
     * @see isRinging
     */
    CARAPI_(Boolean) IsVibrating();

    CARAPI_(void) MakeLooper();

    static CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    static const String TAG;
    static const Boolean DBG;

    static const Int32 PLAY_RING_ONCE = 1;
    static const Int32 STOP_RING = 3;

    static const Int32 VIBRATE_LENGTH; // ms
    static const Int32 PAUSE_LENGTH; // ms

    static AutoPtr<IAudioAttributes> VIBRATION_ATTRIBUTES;

    /** The singleton instance. */
    static AutoPtr<Ringer> sInstance;

    // Uri for the ringtone.
    AutoPtr<IUri> mCustomRingtoneUri;

    AutoPtr<IBluetoothManager> mBluetoothManager;
    AutoPtr<IRingtone> mRingtone;
    AutoPtr<IVibrator> mVibrator;
    AutoPtr<IIPowerManager> mPowerManager;
    /*volatile*/ Boolean mContinueVibrating;
    AutoPtr<VibratorThread> mVibratorThread;
    AutoPtr<IContext> mContext;
    AutoPtr<Worker> mRingThread;
    AutoPtr<IHandler> mRingHandler;
    Int64 mFirstRingEventTime;
    Int64 mFirstRingStartTime;

    static Object mClassLock;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_RINGER_H__