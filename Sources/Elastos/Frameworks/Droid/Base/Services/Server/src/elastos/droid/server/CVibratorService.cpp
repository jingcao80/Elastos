
#include "CVibratorService.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/Manifest.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringBuilder.h>
#include <hardware_legacy/vibrator.h>

using Elastos::Core::EIID_IRunnable;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::View::IInputDevice;

namespace Elastos {
namespace Droid {
namespace Server {

const String CVibratorService::TAG("VibratorService");


ECode CVibratorService::VibrationRunnable::Run()
{
    AutoLock Lock(mHost->mVibrationsLock);
    mHost->DoCancelVibrateLocked();
    mHost->StartNextVibrationLocked();
    return NOERROR;
}

//====================================================================
// CVibratorService::Vibration
//====================================================================
CVibratorService::Vibration::Vibration(
    /* [in] */ IBinder* token,
    /* [in] */ Int64 millis,
    /* [in] */ Int32 uid,
    /* [in] */ CVibratorService* owner)
    : mToken(token)
    , mTimeout(millis)
    , mStartTime(SystemClock::GetUptimeMillis())
    , mRepeat(0)
    , mUid(uid)
    , mOwner(owner)
{}

CVibratorService::Vibration::Vibration(
    /* [in] */ IBinder* token,
    /* [in] */ ArrayOf<Int64>* pattern,
    /* [in] */ Int32 repeat,
    /* [in] */ Int32 uid,
    /* [in] */ CVibratorService* owner)
    : mToken(token)
    , mTimeout(0)
    , mStartTime(SystemClock::GetUptimeMillis())
    , mPattern(pattern)
    , mRepeat(repeat)
    , mUid(uid)
    , mOwner(owner)
{}

// CVibratorService::Vibration::Vibration(
//     /* [in] */ IBinder* token,
//     /* [in] */ Int64 millis,
//     /* [in] */ ArrayOf<Int64>* pattern,
//     /* [in] */ Int32 repeat,
//      [in]  Int32 uid)
// {
//     mToken = token;
//     mTimeout = millis;
//     mStartTime = SystemClock::GetUptimeMillis();
//     mPattern = pattern;
//     mRepeat = repeat;
//     mUid = uid;
// }

CAR_INTERFACE_IMPL(CVibratorService::Vibration, IProxyDeathRecipient);

ECode CVibratorService::Vibration::ProxyDied()
{
    AutoLock Lock(mOwner->mVibrationsLock);

    mOwner->mVibrations.Remove(this);
    if (this == mOwner->mCurrentVibration) {
        mOwner->DoCancelVibrateLocked();
        mOwner->StartNextVibrationLocked();
    }
    return NOERROR;
}

Boolean CVibratorService::Vibration::HasLongerTimeout(
    /* [in] */ Int64 millis)
{
    if (mTimeout == 0) {
        // This is a pattern, return false to play the simple
        // vibration.
        return FALSE;
    }
    if ((mStartTime + mTimeout)
            < (SystemClock::GetUptimeMillis() + millis)) {
        // If this vibration will end before the time passed in, let
        // the new vibration play.
        return FALSE;
    }
    return TRUE;
}


//====================================================================
// CVibratorService::MyContentObserver
//====================================================================
CVibratorService::MyContentObserver::MyContentObserver(
    /* [in] */ IHandler* handler,
    /* [in] */ CVibratorService* owner)
    : ContentObserver(handler)
    , mOwner(owner)
{}

ECode CVibratorService::MyContentObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    mOwner->UpdateInputDeviceVibrators();
    return NOERROR;
}


//====================================================================
// CVibratorService::MyBroadcastReceiver
//====================================================================
CVibratorService::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ CVibratorService* owner)
    : mOwner(owner)
{}

ECode CVibratorService::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mOwner->UpdateInputDeviceVibrators();
    return NOERROR;
}


//====================================================================
// CVibratorService::VibrateThread
//====================================================================
CVibratorService::VibrateThread::VibrateThread(
    /* [in] */ Vibration* vib,
    /* [in] */ CVibratorService* owner)
    : mVibration(vib)
    , mDone(FALSE)
    , mOwner(owner)
{
    mOwner->mTmpWorkSource->Set(vib->mUid);
    mOwner->mWakeLock->SetWorkSource(mOwner->mTmpWorkSource);
    mOwner->mWakeLock->AcquireLock();
    CThread::New((IThread**)&mThread);
}

CAR_INTERFACE_IMPL(CVibratorService::VibrateThread, IRunnable);

void CVibratorService::VibrateThread::Delay(
    /* [in] */ Int64 duration)
{
    if (duration > 0) {
        Int64 bedtime = duration + SystemClock::GetUptimeMillis();
        do {
            //try {
            mThread->Wait(duration);
            //}
            //catch (InterruptedException e) {
            //}
            if (mDone) {
                break;
            }
            duration = bedtime - SystemClock::GetUptimeMillis();
        } while (duration > 0);
    }
}

ECode CVibratorService::VibrateThread::Run()
{
    Process::SetThreadPriority(IProcess::THREAD_PRIORITY_URGENT_DISPLAY);
    {
        ISynchronize::Probe(mThread)->Lock();
        Int32 index = 0;
        AutoPtr<ArrayOf<Int64> > pattern = mVibration->mPattern;
        Int32 len = pattern->GetLength();
        Int32 repeat = mVibration->mRepeat;
        Int64 duration = 0;

        while (!mDone) {
            // add off-time duration to any accumulated on-time duration
            if (index < len) {
                duration += (*pattern)[index++];
            }

            // sleep until it is time to start the vibrator
            Delay(duration);
            if (mDone) {
                break;
            }

            if (index < len) {
                // read on-time duration and start the vibrator
                // duration is saved for delay() at top of loop
                duration = (*pattern)[index++];
                if (duration > 0) {
                    mOwner->DoVibratorOn(duration);
                }
            }
            else {
                if (repeat < 0) {
                    break;
                }
                else {
                    index = repeat;
                    duration = 0;
                }
            }
        }
        mOwner->mWakeLock->ReleaseLock();
        ISynchronize::Probe(mThread)->Unlock();
    }
    AutoLock Lock(mOwner->mVibrationsLock);
    // TODO: this object will be free?
    if (mOwner->mThread.Get() == this) {
        mOwner->mThread = NULL;
    }
    if (!mDone) {
        // If this vibration finished naturally, start the next
        // vibration.
        mOwner->mVibrations.Remove(mVibration);
        mOwner->UnlinkVibration(mVibration);
        mOwner->StartNextVibrationLocked();
    }
    return NOERROR;
}

ECode CVibratorService::VibrateThread::Start()
{
    return mThread->Start();
}

//====================================================================
// CVibratorService::IntentReceiver
//====================================================================
ECode CVibratorService::IntentReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IIntent::ACTION_SCREEN_OFF)) {
        AutoLock Lock(mOwner->mVibrationsLock);
        mOwner->DoCancelVibrateLocked();

        List<AutoPtr<Vibration> >::Iterator it;
        for (it = mOwner->mVibrations.Begin(); it != mOwner->mVibrations.End(); ++it) {
            mOwner->UnlinkVibration(*it);
        }

        mOwner->mVibrations.Clear();
    }
    return NOERROR;
}


//====================================================================
// CVibratorService
//====================================================================
CVibratorService::CVibratorService()
    : mVibrateInputDevicesSetting(FALSE)
    , mInputDeviceListenerRegistered(FALSE)
{
    CWorkSource::New((IWorkSource**)&mTmpWorkSource);
    CHandler::New((IHandler**)&mH);
    mIntentReceiver = new IntentReceiver(this);
}

ECode CVibratorService::constructor(
    /* [in] */ IContext* context)
{
    mVibrationRunnable = new VibrationRunnable(this);

    // Reset the hardware to a default state, in case this is a runtime
    // restart instead of a fresh boot.
    VibratorOff();

    mContext = context;
    AutoPtr<IPowerManager> pm;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&pm);
    pm->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, String("*vibrator*"), (IPowerManagerWakeLock**)&mWakeLock);
    mWakeLock->SetReferenceCounted(TRUE);

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_SCREEN_OFF);
    AutoPtr<IIntent> intent;
    context->RegisterReceiver((IBroadcastReceiver*)mIntentReceiver, filter, (IIntent**)&intent);
    return NOERROR;
}

Boolean CVibratorService::VibratorExists()
{
    return vibrator_exists() > 0 ? TRUE : FALSE;
}

void CVibratorService::VibratorOn(
    /* [in] */ Int64 milliseconds)
{
    Slogger::I(TAG, "vibratorOn");
    vibrator_on(milliseconds);
}

void CVibratorService::VibratorOff()
{
    Slogger::I(TAG, "vibratorOff");
    vibrator_off();
}

ECode CVibratorService::SystemReady()
{
    mContext->GetSystemService(IContext::INPUT_SERVICE, (IInterface**)&mIm);

    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<IUri> uri;
    AutoPtr<ISettingsSystem> settingsSystem;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSystem);
    settingsSystem->GetUriFor(ISettingsSystem::VIBRATE_INPUT_DEVICES, (IUri**)&uri);
    AutoPtr<MyContentObserver> myCO = new MyContentObserver(mH, this);
    resolver->RegisterContentObserver(uri, TRUE, myCO.Get(), IUserHandle::USER_ALL);

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IIntent::ACTION_USER_SWITCHED, (IIntentFilter**)&filter);
    AutoPtr<MyBroadcastReceiver> myBR = new MyBroadcastReceiver(this);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(myBR.Get(), filter, String(NULL), mH, (IIntent**)&intent);

    UpdateInputDeviceVibrators();
    return NOERROR;
}

ECode CVibratorService::HasVibrator(
    /* [out] */ Boolean* hasVibrator)
{
    VALIDATE_NOT_NULL(hasVibrator);
    *hasVibrator = DoVibratorExists();
    return NOERROR;
}

ECode CVibratorService::Vibrate(
    /* [in] */ Int64 milliseconds,
    /* [in] */ IBinder* token)
{
    Int32 result = 0;
    FAIL_RETURN(mContext->CheckCallingOrSelfPermission(Elastos::Droid::Manifest::permission::VIBRATE, &result));
    if (result != IPackageManager::PERMISSION_GRANTED) {
        //throw new SecurityException("Requires VIBRATE permission");
        Slogger::E(TAG, "Requires VIBRATE permission");
        return E_SECURITY_EXCEPTION;
    }
    Int32 uid = Binder::GetCallingUid();
    // We're running in the system server so we cannot crash. Check for a
    // timeout of 0 or negative. This will ensure that a vibration has
    // either a timeout of > 0 or a non-null pattern.
    if (milliseconds <= 0 || (mCurrentVibration != NULL && mCurrentVibration->HasLongerTimeout(milliseconds))) {
        // Ignore this vibration since the current vibration will play for
        // longer than milliseconds.
        return NOERROR;
    }

    AutoPtr<Vibration> vib = new Vibration(token, milliseconds, uid, this);
    {
        AutoLock Lock(mVibrationsLock);
        RemoveVibrationLocked(token);
        DoCancelVibrateLocked();
        mCurrentVibration = vib;
        StartVibrationLocked(vib);
    }

    return NOERROR;
}

Boolean CVibratorService::IsAll0(
    /* [in] */ const ArrayOf<Int64>& pattern)
{
    Int32 N = pattern.GetLength();
    for (Int32 i = 0; i < N; i++) {
        if (pattern[i] != 0) {
            return FALSE;
        }
    }
    return TRUE;
}

ECode CVibratorService::VibratePattern(
    /* [in] */ ArrayOf<Int64>* pattern,
    /* [in] */ Int32 repeat,
    /* [in] */ IBinder* token)
{
    Int32 result = 0;
    FAIL_RETURN(mContext->CheckCallingOrSelfPermission(Elastos::Droid::Manifest::permission::VIBRATE, &result));
    if (result != IPackageManager::PERMISSION_GRANTED) {
        //throw new SecurityException("Requires VIBRATE permission");
        Slogger::E(TAG, "Requires VIBRATE permission");
        return E_SECURITY_EXCEPTION;
    }
    Int32 uid = Binder::GetCallingUid();
    // so wakelock calls will succeed
    Int64 identity = Binder::ClearCallingIdentity();
    //try {
    if (FALSE) {
        StringBuilder sb("");
        Int32 N = pattern->GetLength();
        for (Int32 i = 0; i < N; i++) {
            sb += " ";
            sb += (*pattern)[i];
        }
        Slogger::I(TAG, "vibrating with pattern: %s", sb.ToString().string());
    }

    // we're running in the server so we can't fail
    if (pattern == NULL || pattern->GetLength() == 0
            || IsAll0(*pattern)
            || repeat >= pattern->GetLength() || token == NULL) {
        return NOERROR;
    }

    AutoPtr<Vibration> vib = new Vibration(token, pattern, repeat, uid, this);
    AutoPtr<IProxy> proxy = (IProxy*)token->Probe(EIID_IProxy);
    //try {
    if (proxy != NULL) proxy->LinkToDeath(vib, 0);
    //} catch (RemoteException e) {
    //    return;
    //}

    {
        AutoLock Lock(mVibrationsLock);
        RemoveVibrationLocked(token);
        DoCancelVibrateLocked();
        if (repeat >= 0) {
            mVibrations.PushFront(vib);
            StartNextVibrationLocked();
        }
        else {
            // A negative repeat means that this pattern is not meant
            // to repeat. Treat it like a simple vibration.
            mCurrentVibration = vib;
            StartVibrationLocked(vib);
        }
    }
    //}
    //finally {
    Binder::RestoreCallingIdentity(identity);
    //}
    return NOERROR;
}

ECode CVibratorService::CancelVibrate(
    /* [in] */ IBinder* token)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::VIBRATE, String("cancelVibrate")));

    // so wakelock calls will succeed
    Int64 identity = Binder::ClearCallingIdentity();
    //try {
    AutoLock Lock(mVibrationsLock);
    AutoPtr<Vibration> vib = RemoveVibrationLocked(token);
    if (vib == mCurrentVibration) {
        DoCancelVibrateLocked();
        StartNextVibrationLocked();
    }
    //}
    //finally {
    Binder::RestoreCallingIdentity(identity);
    //}
    return NOERROR;
}

void CVibratorService::DoCancelVibrateLocked()
{
    if (mThread != NULL) {
        ISynchronize::Probe(mThread->mThread)->Lock();
        mThread->mDone = TRUE;
        mThread->mThread->Notify();
        ISynchronize::Probe(mThread->mThread)->Unlock();
        mThread = NULL;
    }
    DoVibratorOff();
    mH->RemoveCallbacks(mVibrationRunnable);
}

// Lock held on mVibrations
void CVibratorService::StartNextVibrationLocked()
{
    if (mVibrations.Begin() == mVibrations.End()) {
        mCurrentVibration = NULL;
        return;
    }
    mCurrentVibration = mVibrations.GetFront();
    StartVibrationLocked(mCurrentVibration);
}

// Lock held on mVibrations
void CVibratorService::StartVibrationLocked(
    /* [in] */ Vibration* vib)
{
    if (vib->mTimeout != 0) {
        DoVibratorOn(vib->mTimeout);

        Boolean result;
        mH->PostDelayed(mVibrationRunnable, vib->mTimeout, &result);
    }
    else {
        // mThread better be null here. doCancelVibrate should always be
        // called before startNextVibrationLocked or startVibrationLocked.
        mThread = new VibrateThread(vib, this);
        mThread->Start();
    }
}

// Lock held on mVibrations
AutoPtr<CVibratorService::Vibration> CVibratorService::RemoveVibrationLocked(
    /* [in] */ IBinder* token)
{
    List<AutoPtr<Vibration> >::Iterator it;
    for (it = mVibrations.Begin(); it != mVibrations.End(); ++it) {
        AutoPtr<Vibration> vib = *it;
        if (vib->mToken.Get() == token) {
            mVibrations.Erase(it);
            UnlinkVibration(vib);
            return vib;
        }
    }
    // We might be looking for a simple vibration which is only stored in
    // mCurrentVibration.
    if (mCurrentVibration != NULL && mCurrentVibration->mToken.Get() == token) {
        UnlinkVibration(mCurrentVibration);
        return mCurrentVibration;
    }
    return NULL;
}

void CVibratorService::UnlinkVibration(
    /* [in] */ Vibration* vib)
{
    if (vib->mPattern != NULL) {
        // If Vibration object has a pattern,
        // the Vibration object has also been linkedToDeath.
        AutoPtr<IProxy> proxy = (IProxy*)vib->mToken->Probe(EIID_IProxy);
        Boolean result;
        proxy->UnlinkToDeath(vib, 0, &result);
    }
}

void CVibratorService::UpdateInputDeviceVibrators()
{
    AutoLock Lock(mVibrationsLock);
    DoCancelVibrateLocked();

    {
        AutoLock Lock(mInputDeviceVibratorsLock);

        mVibrateInputDevicesSetting = FALSE;
        //try {
        AutoPtr<IContentResolver> resolver;
        mContext->GetContentResolver((IContentResolver**)&resolver);
        Int32 value = 0;
        AutoPtr<ISettingsSystem> settingsSystem;
        CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSystem);
        settingsSystem->GetInt32ForUser(
                resolver,
                String("vibrate_input_devices"),
                IUserHandle::USER_CURRENT,
                &value);
        mVibrateInputDevicesSetting = (value > 0);
        //} catch (SettingNotFoundException snfe) {
        //}

        if (mVibrateInputDevicesSetting) {
            if (!mInputDeviceListenerRegistered) {
                mInputDeviceListenerRegistered = TRUE;
                mIm->RegisterInputDeviceListener((IInputDeviceListener*)this, mH);
            }
        }
        else {
            if (mInputDeviceListenerRegistered) {
                mInputDeviceListenerRegistered = FALSE;
                mIm->UnregisterInputDeviceListener((IInputDeviceListener*)this);
            }
        }

        mInputDeviceVibrators.Clear();
        if (mVibrateInputDevicesSetting) {
            AutoPtr< ArrayOf<Int32> > ids;
            mIm->GetInputDeviceIds((ArrayOf<Int32>**)&ids);
            Int32 len = ids->GetLength();
            for (Int32 i = 0; i < len; i++) {
                AutoPtr<IInputDevice> device;
                mIm->GetInputDevice((*ids)[i], (IInputDevice**)&device);
                AutoPtr<IVibrator> vibrator;
                device->GetVibrator((IVibrator**)&vibrator);
                Boolean hasVibrator = FALSE;
                vibrator->HasVibrator(&hasVibrator);
                if (hasVibrator) {
                    mInputDeviceVibrators.PushBack(vibrator);
                }
            }
        }
    }

    StartNextVibrationLocked();
}

ECode CVibratorService::OnInputDeviceAdded(
    /* [in] */ Int32 deviceId)
{
    UpdateInputDeviceVibrators();
    return NOERROR;
}

//@Override
ECode CVibratorService::OnInputDeviceChanged(
    /* [in] */ Int32 deviceId)
{
    UpdateInputDeviceVibrators();
    return NOERROR;
}

//@Override
ECode CVibratorService::OnInputDeviceRemoved(
    /* [in] */ Int32 deviceId)
{
    UpdateInputDeviceVibrators();
    return NOERROR;
}

Boolean CVibratorService::DoVibratorExists() {
    // For now, we choose to ignore the presence of input devices that have vibrators
    // when reporting whether the device has a vibrator.  Applications often use this
    // information to decide whether to enable certain features so they expect the
    // result of hasVibrator() to be constant.  For now, just report whether
    // the device has a built-in vibrator.
    //synchronized(mInputDeviceVibrators) {
    //    return !mInputDeviceVibrators.isEmpty() || vibratorExists();
    //}
    return VibratorExists();
}

void CVibratorService::DoVibratorOn(
    /* [in] */ Int64 millis)
{
    AutoLock Lock(mInputDeviceVibratorsLock);
    if (mInputDeviceVibrators.Begin() != mInputDeviceVibrators.End()) {
        List<AutoPtr<IVibrator> >::Iterator it;
        for (it = mInputDeviceVibrators.Begin(); it != mInputDeviceVibrators.End(); ++it) {
            (*it)->Vibrate(millis);
        }
    }
    else {
        VibratorOn(millis);
    }
}

void CVibratorService::DoVibratorOff()
{
    AutoLock Lock(mInputDeviceVibratorsLock);
    if (mInputDeviceVibrators.Begin() != mInputDeviceVibrators.End()) {
        List<AutoPtr<IVibrator> >::Iterator it;
        for (it = mInputDeviceVibrators.Begin(); it != mInputDeviceVibrators.End(); ++it) {
            (*it)->Cancel();
        }
    }
    else {
        VibratorOff();
    }
}

ECode CVibratorService::ToString(
        /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
}

}//namespace Server
}//namespace Droid
}//namespace Elastos
