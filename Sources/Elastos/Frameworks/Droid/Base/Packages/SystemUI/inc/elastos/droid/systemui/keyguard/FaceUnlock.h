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

#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_FACEUNLOCK_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_FACEUNLOCK_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Internal::Policy::IIFaceLockInterface;
using Elastos::Droid::Internal::Policy::IIFaceLockCallback;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IHandlerCallback;
using Elastos::Droid::View::IView;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

class FaceUnlock
    : public Object
    , public IBiometricSensorUnlock
    , public IHandlerCallback
{
private:
    class MyServiceConnection
        : public Object
        , public IServiceConnection
    {
    public:
        TO_STRING_IMPL("FaceUnlock::MyServiceConnection")

        CAR_INTERFACE_DECL()

        MyServiceConnection(
            /* [in] */ FaceUnlock* host)
            : mHost(host)
        {}

        /**
         * Called when the Face Unlock service connects after calling bind().
         */
        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* className,
            /* [in] */ IBinder* iservice);

        /**
         * Called if the Face Unlock service unexpectedly disconnects.  This indicates an error.
         */
        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* className);

    private:
        FaceUnlock* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * Stores some of the structures that Face Unlock will need to access and creates the handler
     * will be used to execute messages on the UI thread.
     */
    FaceUnlock(
        /* [in] */ IContext* context);

    CARAPI SetKeyguardCallback(
        /* [in] */ IKeyguardSecurityCallback* keyguardScreenCallback);

    /**
     * Stores and displays the view that Face Unlock is allowed to draw within.
     * TODO: since the layout object will eventually be shared by multiple biometric unlock
     * methods, we will have to add our other views (background, cancel button) here.
     */
    CARAPI InitializeView(
        /* [in] */ IView* biometricUnlockView);

    /**
     * Indicates whether Face Unlock is currently running.
     */
    CARAPI IsRunning(
        /* [out] */ Boolean* result);

    /**
     * Dismisses face unlock and goes to the backup lock
     */
    CARAPI StopAndShowBackup();

    /**
     * Binds to the Face Unlock service.  Face Unlock will be started when the bind completes.  The
     * Face Unlock view is displayed to hide the backup lock while the service is starting up.
     * Called on the UI thread.
     */
    CARAPI Start(
        /* [out] */ Boolean* result);

    /**
     * Stops Face Unlock and unbinds from the service.  Called on the UI thread.
     */
    CARAPI Stop(
        /* [out] */ Boolean* result);

    /**
     * Frees up resources used by Face Unlock and stops it if it is still running.
     */
    CARAPI CleanUp();
    /**
     * Returns the Device Policy Manager quality for Face Unlock, which is BIOMETRIC_WEAK.
     */
    CARAPI GetQuality(
        /* [out] */ Int32* quality);

    /**
     * Handles messages such that everything happens on the UI thread in a deterministic order.
     * Calls from the Face Unlock service come from binder threads.  Calls from lockscreen typically
     * come from the UI thread.  This makes sure there are no race conditions between those calls.
     */
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg,
        /* [out] */ Boolean* result);

    /**
     * Tells the service to start its UI via an AIDL interface.  Called when the
     * onServiceConnected() callback is received.
     */
    CARAPI HandleServiceConnected();

    /**
     * Called when the onServiceDisconnected() callback is received.  This should not happen during
     * normal operation.  It indicates an error has occurred.
     */
    CARAPI HandleServiceDisconnected();

    /**
     * Stops the Face Unlock service and tells the device to grant access to the user.
     */
    CARAPI HandleUnlock(
        /* [in] */ Int32 authenticatedUserId);

    /**
     * Stops the Face Unlock service and goes to the backup lock.
     */
    CARAPI HandleCancel();

    /**
     * Increments the number of failed Face Unlock attempts.
     */
    CARAPI HandleReportFailedAttempt();
    /**
     * If the screen is on, pokes the wakelock to keep the screen alive and active for a specific
     * amount of time.
     */
    CARAPI HandlePokeWakelock(
        /* [in] */ Int32 millis);

private:
    /**
     * Tells the Face Unlock service to start displaying its UI and start processing.
     */
    CARAPI_(void) StartUi(
        /* [in] */ IBinder* windowToken,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h);

    /**
     * Tells the Face Unlock service to stop displaying its UI and stop processing.
     */
    CARAPI_(void) StopUi();

public:
    AutoPtr<IKeyguardSecurityCallback> mKeyguardScreenCallback;

private:
    friend class CFaceUnlockFaceLockCallback;

    static const Boolean DEBUG;
    static const String TAG;
    static const String FACE_LOCK_PACKAGE;

    AutoPtr<IContext> mContext;
    AutoPtr<ILockPatternUtils> mLockPatternUtils;

    // TODO: is mServiceRunning needed or can we just use mIsRunning or check if mService is null?
    Boolean mServiceRunning;
    // TODO: now that the code has been restructure to do almost all operations from a handler, this
    // lock may no longer be necessary.
    Object mServiceRunningLock;
    AutoPtr<IIFaceLockInterface> mService;
    Boolean mBoundToService;
    AutoPtr<IView> mFaceUnlockView;

    AutoPtr<IHandler> mHandler;
    const Int32 MSG_SERVICE_CONNECTED;
    const Int32 MSG_SERVICE_DISCONNECTED;
    const Int32 MSG_UNLOCK;
    const Int32 MSG_CANCEL;
    const Int32 MSG_REPORT_FAILED_ATTEMPT;
    const Int32 MSG_POKE_WAKELOCK;

    // TODO: This was added for the purpose of adhering to what the biometric interface expects
    // the isRunning() function to return.  However, it is probably not necessary to have both
    // mRunning and mServiceRunning.  I'd just rather wait to change that logic.
    /*volatile*/ Boolean mIsRunning;

    /**
     * Implements service connection methods.
     */
    AutoPtr<IServiceConnection> mConnection;

    /**
     * Implements the AIDL biometric unlock service callback interface.
     */
    AutoPtr<IIFaceLockCallback> mFaceUnlockCallback;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_KEYGUARD_FACEUNLOCK_H__