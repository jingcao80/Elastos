
#include "elastos/droid/teleservice/phone/Ringer.h"
#include "elastos/droid/teleservice/phone/PhoneUtils.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Looper.h"
#include "Elastos.Droid.Provider.h"
#include <Elastos.CoreLibrary.Core.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IAudioAttributesBuilder;
using Elastos::Droid::Media::CAudioAttributesBuilder;
using Elastos::Droid::Media::IRingtoneManagerHelper;
using Elastos::Droid::Media::CRingtoneManagerHelper;
using Elastos::Droid::Os::CSystemVibrator;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Core::IThread;
using Elastos::Core::CThread;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

Ringer::VibratorThread::VibratorThread(
    /* [in] */ Ringer* host)
    : mHost(host)
{
    Thread::constructor();
}

ECode Ringer::VibratorThread::Run()
{
    while(mHost->mContinueVibrating) {
        mHost->mVibrator->Vibrate(VIBRATE_LENGTH, VIBRATION_ATTRIBUTES);
        SystemClock::Sleep(VIBRATE_LENGTH + PAUSE_LENGTH);
    }
    return NOERROR;
}

Ringer::Worker::Worker(
    /* [in] */ Ringer* host,
    /* [in] */ const String& name)
    : mHost(host)
{
    AutoPtr<IThread> t;
    CThread::New(NULL, this, name, (IThread**)&t);
    t->Start();

    {
        AutoLock syncLock(mLock);
        while (mLooper == NULL) {
            //try {
            assert(0);
            //mLock->Wait();
            //} catch (InterruptedException ex) {
            //}
        }
    }
}

ECode Ringer::Worker::GetLooper(
    /* [out] */ ILooper** looper)
{
    VALIDATE_NOT_NULL(looper);

    *looper = mLooper;
    REFCOUNT_ADD(*looper);
    return NOERROR;
}

ECode Ringer::Worker::Run()
{
    {
        AutoLock syncLock(mLock);
        Looper::Prepare();
        mLooper = Looper::GetMyLooper();
        mLock.NotifyAll();
    }
    return Looper::Loop();
}

ECode Ringer::Worker::Quit()
{
    return mLooper->Quit();
}

Ringer::MyHandler::MyHandler(
    /* [in] */ Ringer* host,
    /* [in] */ ILooper* looper)
    : mHost(host)
{
    Handler::constructor(looper);
}

ECode Ringer::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IRingtone> r;
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case PLAY_RING_ONCE:
        {
            if (DBG) Log(String("mRingHandler: PLAY_RING_ONCE..."));
            Boolean res;
            if (mHost->mRingtone == NULL && (HasMessages(STOP_RING, &res), !res)) {
                // create the ringtone with the uri
                if (DBG) {
                    StringBuilder sb;
                    sb += "creating ringtone: ";
                    sb += TO_CSTR(mHost->mCustomRingtoneUri);
                    Log(sb.ToString());
                }

                AutoPtr<IRingtoneManagerHelper> helper;
                CRingtoneManagerHelper::AcquireSingleton((IRingtoneManagerHelper**)&helper);
                helper->GetRingtone(mHost->mContext, mHost->mCustomRingtoneUri, (IRingtone**)&r);
                {
                    AutoLock syncLock(mHost);
                    Boolean res;
                    if (HasMessages(STOP_RING, &res), !res) {
                        mHost->mRingtone = r;
                    }
                }
            }
            r = mHost->mRingtone;
            Boolean tmp;
            if (r != NULL && (HasMessages(STOP_RING, &tmp), !tmp) && (r->IsPlaying(&res), !res)) {
                PhoneUtils::SetAudioMode();
                r->Play();
                {
                    AutoLock syncLock(mHost);
                    if (mHost->mFirstRingStartTime < 0) {
                        mHost->mFirstRingStartTime = SystemClock::GetElapsedRealtime();
                    }
                }
            }
            break;
        }
        case STOP_RING:
        {
            if (DBG) Log(String("mRingHandler: STOP_RING..."));
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            r = IRingtone::Probe(obj);
            if (r != NULL) {
                r->Stop();
            }
            else {
                if (DBG) {
                    StringBuilder sb;
                    sb += "- STOP_RING with null ringtone!  msg = ";
                    sb += TO_CSTR(msg);
                    Log(sb.ToString());
                }
            }
            AutoPtr<ILooper> looper;
            GetLooper((ILooper**)&looper);
            looper->Quit();
            break;
        }
    }
    return NOERROR;
}

const String Ringer::TAG("Ringer");

static Boolean initDBG()
{
    AutoPtr<ISystemProperties> helper;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&helper);
    Int32 tmp;
    helper->GetInt32(String("ro.debuggable"), 0, &tmp);

    return (IPhoneGlobals::DBG_LEVEL >= 1) && (tmp == 1);
}

const Boolean Ringer::DBG = initDBG();

const Int32 Ringer::VIBRATE_LENGTH = 1000; // ms
const Int32 Ringer::PAUSE_LENGTH = 1000; // ms

static AutoPtr<IAudioAttributes> initVIBRATION_ATTRIBUTES()
{
    AutoPtr<IAudioAttributesBuilder> builder;
    CAudioAttributesBuilder::New((IAudioAttributesBuilder**)&builder);
    builder->SetContentType(IAudioAttributes::CONTENT_TYPE_SONIFICATION);
    builder->SetUsage(IAudioAttributes::USAGE_NOTIFICATION_RINGTONE);
    AutoPtr<IAudioAttributes> tmp;
    builder->Build((IAudioAttributes**)&tmp);
    return tmp;
}

AutoPtr<IAudioAttributes> Ringer::VIBRATION_ATTRIBUTES = initVIBRATION_ATTRIBUTES();

AutoPtr<Ringer> Ringer::sInstance;

Object Ringer::mClassLock;

AutoPtr<Ringer> Ringer::Init(
    /* [in] */ IContext* context,
    /* [in] */ IBluetoothManager* bluetoothManager)
{
    {
        AutoLock syncLock(mClassLock);
        if (sInstance == NULL) {
            sInstance = new Ringer(context, bluetoothManager);
        }
        else {
            Logger::W(TAG, "init() called multiple times!  sInstance = %s", TO_CSTR(sInstance));
        }
        return sInstance;
    }
}

Ringer::Ringer(
    /* [in] */ IContext* context,
    /* [in] */ IBluetoothManager* bluetoothManager)
    : mBluetoothManager(bluetoothManager)
    , mContinueVibrating(FALSE)
    , mContext(context)
    , mFirstRingEventTime(-1)
    , mFirstRingStartTime(-1)
{
    AutoPtr<ISettingsSystem> helper;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&helper);
    helper->GetDEFAULT_RINGTONE_URI((IUri**)&mCustomRingtoneUri);

    AutoPtr<IServiceManager> manager;
    CServiceManager::AcquireSingleton((IServiceManager**)&manager);
    AutoPtr<IInterface> obj;
    manager->GetService(IContext::POWER_SERVICE, (IInterface**)&obj);
    mPowerManager = IIPowerManager::Probe(obj);
    // We don't rely on getSystemService(Context.VIBRATOR_SERVICE) to make sure this
    // vibrator object will be isolated from others.
    CSystemVibrator::New(context, (IVibrator**)&mVibrator);
}

ECode Ringer::UpdateRingerContextAfterRadioTechnologyChange(
    /* [in] */ IPhone* phone)
{
    if(DBG) Logger::D(TAG, "updateRingerContextAfterRadioTechnologyChange...");
    return phone->GetContext((IContext**)&mContext);
}

ECode Ringer::IsRinging(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    {
        AutoLock syncLock(this);
        *result = (IsRingtonePlaying() || IsVibrating());
        return NOERROR;
    }
}

Boolean Ringer::IsRingtonePlaying()
{
    {
        AutoLock syncLock(this);
        Boolean res,res2;
        return (mRingtone != NULL && (mRingtone->IsPlaying(&res), res)) ||
                (mRingHandler != NULL && (mRingHandler->HasMessages(PLAY_RING_ONCE, &res2), res2));
    }
}

Boolean Ringer::IsVibrating()
{
    {
        AutoLock syncLock(this);
        return (mVibratorThread != NULL);
    }
}

ECode Ringer::Ring()
{
    if (DBG) Log(String("ring()..."));

    {
        AutoLock syncLock(this);
        //try {
        Boolean res;
        if (mBluetoothManager->ShowBluetoothIndication(&res), res) {
            mPowerManager->SetAttentionLight(TRUE, 0x000000ff);
        }
        else {
            mPowerManager->SetAttentionLight(TRUE, 0x00ffffff);
        }
        //} catch (RemoteException ex) {
            // the other end of this binder call is in the system process.
        //}

        if ((ShouldVibrate(&res), res) && mVibratorThread == NULL) {
            mContinueVibrating = TRUE;
            mVibratorThread = new VibratorThread(this);
            if (DBG) Log(String("- starting vibrator..."));
            mVibratorThread->Start();
        }
        AutoPtr<IInterface> obj;
        mContext->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
        AutoPtr<IAudioManager> audioManager = IAudioManager::Probe(obj);

        Int32 volume;
        if ((audioManager->GetStreamVolume(IAudioManager::STREAM_RING, &volume), volume) == 0) {
            if (DBG) Log(String("skipping ring because volume is zero"));
            return NOERROR;
        }

        MakeLooper();
        if (mFirstRingEventTime < 0) {
            mFirstRingEventTime = SystemClock::GetElapsedRealtime();
            Boolean res;
            mRingHandler->SendEmptyMessage(PLAY_RING_ONCE, &res);
        }
        else {
            // For repeat rings, figure out by how much to delay
            // the ring so that it happens the correct amount of
            // time after the previous ring
            if (mFirstRingStartTime > 0) {
                // Delay subsequent rings by the delta between event
                // and play time of the first ring
                if (DBG) {
                    StringBuilder sb;
                    sb += "delaying ring by ";
                    sb += mFirstRingStartTime - mFirstRingEventTime;
                    Log(sb.ToString());
                }
                Boolean res;
                mRingHandler->SendEmptyMessageDelayed(PLAY_RING_ONCE,
                        mFirstRingStartTime - mFirstRingEventTime, &res);
            }
            else {
                // We've gotten two ring events so far, but the ring
                // still hasn't started. Reset the event time to the
                // time of this event to maintain correct spacing.
                mFirstRingEventTime = SystemClock::GetElapsedRealtime();
            }
        }
    }
    return NOERROR;
}

ECode Ringer::ShouldVibrate(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
    AutoPtr<IAudioManager> audioManager = IAudioManager::Probe(obj);

    Int32 ringerMode;
    audioManager->GetRingerMode(&ringerMode);
    assert(0);
    // if (SettingsUtil::GetVibrateWhenRingingSetting(mContext)) {
    //     *result = ringerMode != IAudioManager::RINGER_MODE_SILENT;
    //     return NOERROR;
    // }
    // else {
    //     *result = ringerMode == IAudioManager::RINGER_MODE_VIBRATE;
    //     return NOERROR;
    // }
    return NOERROR;
}

ECode Ringer::StopRing()
{
    {
        AutoLock syncLock(this);
        if (DBG) Log(String("stopRing()..."));

        //try {
        mPowerManager->SetAttentionLight(FALSE, 0x00000000);
        //} catch (RemoteException ex) {
            // the other end of this binder call is in the system process.
        //}

        if (mRingHandler != NULL) {
            mRingHandler->RemoveCallbacksAndMessages(NULL);
            AutoPtr<IMessage> msg;
            mRingHandler->ObtainMessage(STOP_RING, (IMessage**)&msg);
            msg->SetObj(TO_IINTERFACE(mRingtone));
            Boolean res;
            mRingHandler->SendMessage(msg, &res);
            PhoneUtils::SetAudioMode();
            mRingThread = NULL;
            mRingHandler = NULL;
            mRingtone = NULL;
            mFirstRingEventTime = -1;
            mFirstRingStartTime = -1;
        }
        else {
            if (DBG) Log(String("- stopRing: null mRingHandler!"));
        }

        if (mVibratorThread != NULL) {
            if (DBG) Log(String("- stopRing: cleaning up vibrator thread..."));
            mContinueVibrating = FALSE;
            mVibratorThread = NULL;
        }
        // Also immediately cancel any vibration in progress.
        mVibrator->Cancel();
    }
    return NOERROR;
}

ECode Ringer::SetCustomRingtoneUri(
    /* [in] */ IUri* uri)
{
    if (uri != NULL) {
        mCustomRingtoneUri = uri;
    }
    return NOERROR;
}

void Ringer::MakeLooper()
{
    if (mRingThread == NULL) {
        mRingThread = new Worker(this, String("ringer"));
        AutoPtr<ILooper> looper;
        mRingThread->GetLooper((ILooper**)&looper);
        mRingHandler = new MyHandler(this, looper);
    }
}

void Ringer::Log(
    /* [in] */ const String& msg)
{
    Logger::D(TAG, msg);
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos