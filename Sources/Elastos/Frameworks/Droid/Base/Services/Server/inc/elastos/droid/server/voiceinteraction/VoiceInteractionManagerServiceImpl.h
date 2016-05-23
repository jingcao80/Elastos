/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.server.voiceinteraction;

using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IActivityManagerNative;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IIBinder;
using Elastos::Droid::Os::IRemoteException;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Service::Ivoice.IVoiceInteractionService;
using Elastos::Droid::Service::Ivoice.IVoiceInteractionSession;
using Elastos::Droid::Service::Ivoice.IVoiceInteractionSessionService;
using Elastos::Droid::Service::Ivoice.VoiceInteractionService;
using Elastos::Droid::Service::Ivoice.VoiceInteractionServiceInfo;
using Elastos::Droid::Utility::ISlog;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::IWindowManager;

using Elastos::Droid::internal.app.IVoiceInteractor;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

class VoiceInteractionManagerServiceImpl {
    static const String TAG = "VoiceInteractionServiceManager";

    final Boolean mValid;

    final Context mContext;
    final Handler mHandler;
    final Object mLock;
    final Int32 mUser;
    final ComponentName mComponent;
    final IActivityManager mAm;
    final VoiceInteractionServiceInfo mInfo;
    final ComponentName mSessionComponentName;
    final IWindowManager mIWindowManager;
    Boolean mBound = FALSE;
    IVoiceInteractionService mService;

    SessionConnection mActiveSession;

    final BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            if (IIntent::ACTION_CLOSE_SYSTEM_DIALOGS->Equals(intent->GetAction())) {
                {    AutoLock syncLock(mLock);
                    if (mActiveSession != NULL && mActiveSession.mSession != NULL) {
                        try {
                            mActiveSession.mSession->CloseSystemDialogs();
                        } catch (RemoteException e) {
                        }
                    }
                }
            }
        }
    };

    final ServiceConnection mConnection = new ServiceConnection() {
        //@Override
        CARAPI OnServiceConnected(ComponentName name, IBinder service) {
            {    AutoLock syncLock(mLock);
                mService = IVoiceInteractionService.Stub->AsInterface(service);
                try {
                    mService->Ready();
                } catch (RemoteException e) {
                }
            }
        }

        //@Override
        CARAPI OnServiceDisconnected(ComponentName name) {
            mService = NULL;
        }
    };

    final class SessionConnection implements ServiceConnection {
        final IBinder mToken = new Binder();
        final Bundle mArgs;
        Boolean mBound;
        IVoiceInteractionSessionService mService;
        IVoiceInteractionSession mSession;
        IVoiceInteractor mInteractor;

        SessionConnection(Bundle args) {
            mArgs = args;
            Intent serviceIntent = new Intent(VoiceInteractionService.SERVICE_INTERFACE);
            serviceIntent->SetComponent(mSessionComponentName);
            mBound = mContext->BindServiceAsUser(serviceIntent, this,
                    Context.BIND_AUTO_CREATE, new UserHandle(mUser));
            if (mBound) {
                try {
                    mIWindowManager->AddWindowToken(mToken,
                            WindowManager.LayoutParams.TYPE_VOICE_INTERACTION);
                } catch (RemoteException e) {
                    Slogger::W(TAG, "Failed adding window token", e);
                }
            } else {
                Slogger::W(TAG, "Failed binding to voice interaction session service " + mComponent);
            }
        }

        //@Override
        CARAPI OnServiceConnected(ComponentName name, IBinder service) {
            {    AutoLock syncLock(mLock);
                mService = IVoiceInteractionSessionService.Stub->AsInterface(service);
                if (mActiveSession == this) {
                    try {
                        mService->NewSession(mToken, mArgs);
                    } catch (RemoteException e) {
                        Slogger::W(TAG, "Failed adding window token", e);
                    }
                }
            }
        }

        //@Override
        CARAPI OnServiceDisconnected(ComponentName name) {
            mService = NULL;
        }

        CARAPI Cancel() {
            if (mBound) {
                if (mSession != NULL) {
                    try {
                        mSession->Destroy();
                    } catch (RemoteException e) {
                        Slogger::W(TAG, "Voice interation session already dead");
                    }
                }
                if (mSession != NULL) {
                    try {
                        mAm->FinishVoiceTask(mSession);
                    } catch (RemoteException e) {
                    }
                }
                mContext->UnbindService(this);
                try {
                    mIWindowManager->RemoveWindowToken(mToken);
                } catch (RemoteException e) {
                    Slogger::W(TAG, "Failed removing window token", e);
                }
                mBound = FALSE;
                mService = NULL;
                mSession = NULL;
                mInteractor = NULL;
            }
        }

        CARAPI Dump(String prefix, PrintWriter pw) {
            pw->Print(prefix); pw->Print("mToken="); pw->Println(mToken);
            pw->Print(prefix); pw->Print("mArgs="); pw->Println(mArgs);
            pw->Print(prefix); pw->Print("mBound="); pw->Println(mBound);
            if (mBound) {
                pw->Print(prefix); pw->Print("mService="); pw->Println(mService);
                pw->Print(prefix); pw->Print("mSession="); pw->Println(mSession);
                pw->Print(prefix); pw->Print("mInteractor="); pw->Println(mInteractor);
            }
        }
    };

    VoiceInteractionManagerServiceImpl(Context context, Handler handler, Object lock,
            Int32 userHandle, ComponentName service) {
        mContext = context;
        mHandler = handler;
        mLock = lock;
        mUser = userHandle;
        mComponent = service;
        mAm = ActivityManagerNative->GetDefault();
        VoiceInteractionServiceInfo info;
        try {
            info = new VoiceInteractionServiceInfo(context->GetPackageManager(), service);
        } catch (PackageManager.NameNotFoundException e) {
            Slogger::W(TAG, "Voice interaction service not found: " + service);
            mInfo = NULL;
            mSessionComponentName = NULL;
            mIWindowManager = NULL;
            mValid = FALSE;
            return;
        }
        mInfo = info;
        if (mInfo->GetParseError() != NULL) {
            Slogger::W(TAG, "Bad voice interaction service: " + mInfo->GetParseError());
            mSessionComponentName = NULL;
            mIWindowManager = NULL;
            mValid = FALSE;
            return;
        }
        mValid = TRUE;
        mSessionComponentName = new ComponentName(service->GetPackageName(),
                mInfo->GetSessionService());
        mIWindowManager = IWindowManager.Stub->AsInterface(
                ServiceManager->GetService(Context.WINDOW_SERVICE));
        IntentFilter filter = new IntentFilter();
        filter->AddAction(IIntent::ACTION_CLOSE_SYSTEM_DIALOGS);
        mContext->RegisterReceiver(mBroadcastReceiver, filter, NULL, handler);
    }

    CARAPI StartSessionLocked(Int32 callingPid, Int32 callingUid, Bundle args) {
        if (mActiveSession != NULL) {
            mActiveSession->Cancel();
            mActiveSession = NULL;
        }
        mActiveSession = new SessionConnection(args);
    }

    public Boolean DeliverNewSessionLocked(Int32 callingPid, Int32 callingUid, IBinder token,
            IVoiceInteractionSession session, IVoiceInteractor interactor) {
        if (mActiveSession == NULL || token != mActiveSession.mToken) {
            Slogger::W(TAG, "deliverNewSession does not match active session");
            return FALSE;
        }
        mActiveSession.mSession = session;
        mActiveSession.mInteractor = interactor;
        return TRUE;
    }

    public Int32 StartVoiceActivityLocked(Int32 callingPid, Int32 callingUid, IBinder token,
            Intent intent, String resolvedType) {
        try {
            if (mActiveSession == NULL || token != mActiveSession.mToken) {
                Slogger::W(TAG, "startVoiceActivity does not match active session");
                return ActivityManager.START_CANCELED;
            }
            intent = new Intent(intent);
            intent->AddCategory(Intent.CATEGORY_VOICE);
            intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK | IIntent::FLAG_ACTIVITY_MULTIPLE_TASK);
            return mAm->StartVoiceActivity(mComponent->GetPackageName(), callingPid, callingUid,
                    intent, resolvedType, mActiveSession.mSession, mActiveSession.mInteractor,
                    0, NULL, NULL, mUser);
        } catch (RemoteException e) {
            throw new IllegalStateException("Unexpected remote error", e);
        }
    }


    CARAPI FinishLocked(Int32 callingPid, Int32 callingUid, IBinder token) {
        if (mActiveSession == NULL || token != mActiveSession.mToken) {
            Slogger::W(TAG, "finish does not match active session");
            return;
        }
        mActiveSession->Cancel();
        mActiveSession = NULL;
    }

    CARAPI DumpLocked(FileDescriptor fd, PrintWriter pw, String[] args) {
        if (!mValid) {
            pw->Print("  NOT VALID: ");
            if (mInfo == NULL) {
                pw->Println("no info");
            } else {
                pw->Println(mInfo->GetParseError());
            }
            return;
        }
        pw->Print("  mComponent="); pw->Println(mComponent->FlattenToShortString());
        pw->Print("  Session service="); pw->Println(mInfo->GetSessionService());
        pw->Print("  Settings activity="); pw->Println(mInfo->GetSettingsActivity());
        pw->Print("  mBound="); pw->Print(mBound);  pw->Print(" mService="); pw->Println(mService);
        if (mActiveSession != NULL) {
            pw->Println("  Active session:");
            mActiveSession->Dump("    ", pw);
        }
    }

    void StartLocked() {
        Intent intent = new Intent(VoiceInteractionService.SERVICE_INTERFACE);
        intent->SetComponent(mComponent);
        mBound = mContext->BindServiceAsUser(intent, mConnection,
                Context.BIND_AUTO_CREATE, new UserHandle(mUser));
        if (!mBound) {
            Slogger::W(TAG, "Failed binding to voice interaction service " + mComponent);
        }
    }

    void ShutdownLocked() {
        try {
            if (mService != NULL) {
                mService->Shutdown();
            }
        } catch (RemoteException e) {
            Slogger::W(TAG, "RemoteException in shutdown", e);
        }

        if (mBound) {
            mContext->UnbindService(mConnection);
            mBound = FALSE;
        }
        if (mValid) {
            mContext->UnregisterReceiver(mBroadcastReceiver);
        }
    }

    void NotifySoundModelsChangedLocked() {
        if (mService == NULL) {
            Slogger::W(TAG, "Not bound to voice interaction service " + mComponent);
        }
        try {
            mService->SoundModelsChanged();
        } catch (RemoteException e) {
            Slogger::W(TAG, "RemoteException while calling soundModelsChanged", e);
        }
    }
}
