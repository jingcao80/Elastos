#ifndef  __ELASTOS_DROID_PHONE_RINGER_H__
#define  __ELASTOS_DROID_PHONE_RINGER_H__

#include "_Elastos.Droid.Server.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
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
    static const String LOG_TAG;
    static const Boolean DBG;

    static const Int32 PLAY_RING_ONCE;
    static const Int32 STOP_RING;

    static const Int32 VIBRATE_LENGTH; // ms
    static const Int32 PAUSE_LENGTH; // ms

    static AutoPtr<IAudioAttributes> VIBRATION_ATTRIBUTES;

    /** The singleton instance. */
    static AutoPtr<Ringer> sInstance;

    // Uri for the ringtone.
    AutoPtr<IUri> mCustomRingtoneUri = Settings.System.DEFAULT_RINGTONE_URI;

    AutoPtr<IBluetoothManager> mBluetoothManager;
    AutoPtr<IRingtone> mRingtone;
    AutoPtr<IVibrator> mVibrator;
    AutoPtr<IIPowerManager> mPowerManager;
    /*volatile*/ Boolean mContinueVibrating;
    AutoPtr<IVibratorThread> mVibratorThread;
    AutoPtr<IContext> mContext;
    AutoPtr<IWorker> mRingThread;
    AutoPtr<IHandler> mRingHandler;
    Int64 mFirstRingEventTime = -1;
    Int64 mFirstRingStartTime = -1;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_RINGER_H__