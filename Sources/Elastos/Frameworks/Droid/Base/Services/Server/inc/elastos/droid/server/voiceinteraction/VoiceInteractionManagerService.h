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

using Elastos::Droid::Manifest;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IAppGlobals;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Hardware::soundtrigger::IIRecognitionStatusCallback;
using Elastos::Droid::Hardware::soundtrigger::ISoundTrigger.KeyphraseSoundModel;
using Elastos::Droid::Hardware::soundtrigger::ISoundTrigger.ModuleProperties;
using Elastos::Droid::Hardware::soundtrigger::ISoundTrigger.RecognitionConfig;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IIBinder;
using Elastos::Droid::Os::IParcel;
using Elastos::Droid::Os::IRemoteException;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Service::Ivoice.IVoiceInteractionService;
using Elastos::Droid::Service::Ivoice.IVoiceInteractionSession;
using Elastos::Droid::Service::Ivoice.VoiceInteractionService;
using Elastos::Droid::Service::Ivoice.VoiceInteractionServiceInfo;
using Elastos::Droid::Speech::IRecognitionService;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ISlog;

using Elastos::Droid::internal.app.IVoiceInteractionManagerService;
using Elastos::Droid::internal.app.IVoiceInteractor;
using Elastos::Droid::internal.content.PackageMonitor;
using Elastos::Droid::internal.os.BackgroundThread;
using Elastos::Droid::Server::ISystemService;
using Elastos::Droid::Server::IUiThread;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IList;

/**
 * SystemService that publishes an IVoiceInteractionManagerService.
 */
public class VoiceInteractionManagerService extends SystemService {
    static const String TAG = "VoiceInteractionManagerService";
    static const Boolean DEBUG = FALSE;

    final Context mContext;
    final ContentResolver mResolver;
    final DatabaseHelper mDbHelper;
    final SoundTriggerHelper mSoundTriggerHelper;

    public VoiceInteractionManagerService(Context context) {
        Super(context);
        mContext = context;
        mResolver = context->GetContentResolver();
        mDbHelper = new DatabaseHelper(context);
        mSoundTriggerHelper = new SoundTriggerHelper(context);
    }

    //@Override
    CARAPI OnStart() {
        PublishBinderService(Context.VOICE_INTERACTION_MANAGER_SERVICE, mServiceStub);
    }

    //@Override
    CARAPI OnBootPhase(Int32 phase) {
        if (phase == PHASE_THIRD_PARTY_APPS_CAN_START) {
            mServiceStub->SystemRunning(IsSafeMode());
        }
    }

    //@Override
    CARAPI OnStartUser(Int32 userHandle) {
        mServiceStub->InitForUser(userHandle);
    }

    //@Override
    CARAPI OnSwitchUser(Int32 userHandle) {
        mServiceStub->SwitchUser(userHandle);
    }

    // implementation entry point and binder service
    private final VoiceInteractionManagerServiceStub mServiceStub
            = new VoiceInteractionManagerServiceStub();

    class VoiceInteractionManagerServiceStub extends IVoiceInteractionManagerService.Stub {

        VoiceInteractionManagerServiceImpl mImpl;

        private Boolean mSafeMode;
        private Int32 mCurUser;

        //@Override
        public Boolean OnTransact(Int32 code, Parcel data, Parcel reply, Int32 flags)
                throws RemoteException {
            try {
                return super->OnTransact(code, data, reply, flags);
            } catch (RuntimeException e) {
                // The activity manager only throws security exceptions, so let's
                // log all others.
                if (!(e instanceof SecurityException)) {
                    Slog->Wtf(TAG, "VoiceInteractionManagerService Crash", e);
                }
                throw e;
            }
        }

        CARAPI InitForUser(Int32 userHandle) {
            if (DEBUG) Slogger::I(TAG, "initForUser user=" + userHandle);
            String curInteractorStr = Settings.Secure->GetStringForUser(
                    mContext->GetContentResolver(),
                    Settings.Secure.VOICE_INTERACTION_SERVICE, userHandle);
            ComponentName curRecognizer = GetCurRecognizer(userHandle);
            VoiceInteractionServiceInfo curInteractorInfo = NULL;
            if (curInteractorStr == NULL && curRecognizer != NULL) {
                // If there is no interactor setting, that means we are upgrading
                // from an older platform version.  If the current recognizer is not
                // set or matches the preferred recognizer, then we want to upgrade
                // the user to have the default voice interaction service enabled.
                curInteractorInfo = FindAvailInteractor(userHandle, curRecognizer);
                if (curInteractorInfo != NULL) {
                    // Looks good!  We'll apply this one.  To make it happen, we clear the
                    // recognizer so that we don't think we have anything set and will
                    // re-apply the settings.
                    curRecognizer = NULL;
                }
            }

            if (curRecognizer != NULL) {
                // If we already have at least a recognizer, then we probably want to
                // leave things as they are...  unless something has disappeared.
                IPackageManager pm = AppGlobals->GetPackageManager();
                ServiceInfo interactorInfo = NULL;
                ServiceInfo recognizerInfo = NULL;
                ComponentName curInteractor = !TextUtils->IsEmpty(curInteractorStr)
                        ? ComponentName->UnflattenFromString(curInteractorStr) : NULL;
                try {
                    recognizerInfo = pm->GetServiceInfo(curRecognizer, 0, userHandle);
                    if (curInteractor != NULL) {
                        interactorInfo = pm->GetServiceInfo(curInteractor, 0, userHandle);
                    }
                } catch (RemoteException e) {
                }
                // If the apps for the currently set components still exist, then all is okay.
                if (recognizerInfo != NULL && (curInteractor == NULL || interactorInfo != NULL)) {
                    return;
                }
            }

            // Initializing settings, look for an interactor first.
            if (curInteractorInfo == NULL) {
                curInteractorInfo = FindAvailInteractor(userHandle, NULL);
            }
            if (curInteractorInfo != NULL) {
                // Eventually it will be an error to not specify this.
                SetCurInteractor(new ComponentName(curInteractorInfo->GetServiceInfo().packageName,
                        curInteractorInfo->GetServiceInfo().name), userHandle);
                if (curInteractorInfo->GetRecognitionService() != NULL) {
                    SetCurRecognizer(
                            new ComponentName(curInteractorInfo->GetServiceInfo().packageName,
                                    curInteractorInfo->GetRecognitionService()), userHandle);
                    return;
                }
            }

            // No voice interactor, we'll just set up a simple recognizer.
            curRecognizer = FindAvailRecognizer(NULL, userHandle);
            if (curRecognizer != NULL) {
                if (curInteractorInfo == NULL) {
                    SetCurInteractor(NULL, userHandle);
                }
                SetCurRecognizer(curRecognizer, userHandle);
            }
        }

        CARAPI SystemRunning(Boolean safeMode) {
            mSafeMode = safeMode;

            mPackageMonitor->Register(mContext, BackgroundThread->GetHandler()->GetLooper(),
                    UserHandle.ALL, TRUE);
            new SettingsObserver(UiThread->GetHandler());

            {    AutoLock syncLock(this);
                mCurUser = ActivityManager->GetCurrentUser();
                SwitchImplementationIfNeededLocked(FALSE);
            }
        }

        CARAPI SwitchUser(Int32 userHandle) {
            {    AutoLock syncLock(this);
                mCurUser = userHandle;
                SwitchImplementationIfNeededLocked(FALSE);
            }
        }

        void SwitchImplementationIfNeededLocked(Boolean force) {
            if (!mSafeMode) {
                String curService = Settings.Secure->GetStringForUser(
                        mResolver, Settings.Secure.VOICE_INTERACTION_SERVICE, mCurUser);
                ComponentName serviceComponent = NULL;
                if (curService != NULL && !curService->IsEmpty()) {
                    try {
                        serviceComponent = ComponentName->UnflattenFromString(curService);
                    } catch (RuntimeException e) {
                        Slog->Wtf(TAG, "Bad voice interaction service name " + curService, e);
                        serviceComponent = NULL;
                    }
                }
                if (force || mImpl == NULL || mImpl.mUser != mCurUser
                        || !mImpl.mComponent->Equals(serviceComponent)) {
                    mSoundTriggerHelper->StopAllRecognitions();
                    if (mImpl != NULL) {
                        mImpl->ShutdownLocked();
                    }
                    if (serviceComponent != NULL) {
                        mImpl = new VoiceInteractionManagerServiceImpl(mContext,
                                UiThread->GetHandler(), this, mCurUser, serviceComponent);
                        mImpl->StartLocked();
                    } else {
                        mImpl = NULL;
                    }
                }
            }
        }

        VoiceInteractionServiceInfo FindAvailInteractor(Int32 userHandle, ComponentName recognizer) {
            List<ResolveInfo> available =
                    mContext->GetPackageManager()->QueryIntentServicesAsUser(
                            new Intent(VoiceInteractionService.SERVICE_INTERFACE), 0, userHandle);
            Int32 numAvailable = available->Size();

            if (numAvailable == 0) {
                Slogger::W(TAG, "no available voice interaction services found for user " + userHandle);
                return NULL;
            } else {
                // Find first system package.  We never want to allow third party services to
                // be automatically selected, because those require approval of the user.
                VoiceInteractionServiceInfo foundInfo = NULL;
                for (Int32 i=0; i<numAvailable; i++) {
                    ServiceInfo cur = available->Get(i).serviceInfo;
                    if ((cur.applicationInfo.flags&ApplicationInfo.FLAG_SYSTEM) != 0) {
                        ComponentName comp = new ComponentName(cur.packageName, cur.name);
                        try {
                            VoiceInteractionServiceInfo info = new VoiceInteractionServiceInfo(
                                    mContext->GetPackageManager(), comp, userHandle);
                            if (info->GetParseError() == NULL) {
                                if (recognizer == NULL || info->GetServiceInfo().packageName->Equals(
                                        recognizer->GetPackageName())) {
                                    if (foundInfo == NULL) {
                                        foundInfo = info;
                                    } else {
                                        Slogger::W(TAG, "More than one voice interaction service, "
                                                + "picking first "
                                                + new ComponentName(
                                                        foundInfo->GetServiceInfo().packageName,
                                                        foundInfo->GetServiceInfo().name)
                                                + " over "
                                                + new ComponentName(cur.packageName, cur.name));
                                    }
                                }
                            } else {
                                Slogger::W(TAG, "Bad interaction service " + comp + ": "
                                        + info->GetParseError());
                            }
                        } catch (PackageManager.NameNotFoundException e) {
                            Slogger::W(TAG, "Failure looking up interaction service " + comp);
                        } catch (RemoteException e) {
                        }
                    }
                }

                return foundInfo;
            }
        }

        ComponentName GetCurInteractor(Int32 userHandle) {
            String curInteractor = Settings.Secure->GetStringForUser(
                    mContext->GetContentResolver(),
                    Settings.Secure.VOICE_INTERACTION_SERVICE, userHandle);
            if (TextUtils->IsEmpty(curInteractor)) {
                return NULL;
            }
            if (DEBUG) Slogger::I(TAG, "getCurInteractor curInteractor=" + curInteractor
                    + " user=" + userHandle);
            return ComponentName->UnflattenFromString(curInteractor);
        }

        void SetCurInteractor(ComponentName comp, Int32 userHandle) {
            Settings.Secure->PutStringForUser(mContext->GetContentResolver(),
                    Settings.Secure.VOICE_INTERACTION_SERVICE,
                    comp != NULL ? comp->FlattenToShortString() : "", userHandle);
            if (DEBUG) Slogger::I(TAG, "setCurInteractor comp=" + comp
                    + " user=" + userHandle);
        }

        ComponentName FindAvailRecognizer(String prefPackage, Int32 userHandle) {
            List<ResolveInfo> available =
                    mContext->GetPackageManager()->QueryIntentServicesAsUser(
                            new Intent(RecognitionService.SERVICE_INTERFACE), 0, userHandle);
            Int32 numAvailable = available->Size();

            if (numAvailable == 0) {
                Slogger::W(TAG, "no available voice recognition services found for user " + userHandle);
                return NULL;
            } else {
                if (prefPackage != NULL) {
                    for (Int32 i=0; i<numAvailable; i++) {
                        ServiceInfo serviceInfo = available->Get(i).serviceInfo;
                        if (prefPackage->Equals(serviceInfo.packageName)) {
                            return new ComponentName(serviceInfo.packageName, serviceInfo.name);
                        }
                    }
                }
                if (numAvailable > 1) {
                    Slogger::W(TAG, "more than one voice recognition service found, picking first");
                }

                ServiceInfo serviceInfo = available->Get(0).serviceInfo;
                return new ComponentName(serviceInfo.packageName, serviceInfo.name);
            }
        }

        ComponentName GetCurRecognizer(Int32 userHandle) {
            String curRecognizer = Settings.Secure->GetStringForUser(
                    mContext->GetContentResolver(),
                    Settings.Secure.VOICE_RECOGNITION_SERVICE, userHandle);
            if (TextUtils->IsEmpty(curRecognizer)) {
                return NULL;
            }
            if (DEBUG) Slogger::I(TAG, "getCurRecognizer curRecognizer=" + curRecognizer
                    + " user=" + userHandle);
            return ComponentName->UnflattenFromString(curRecognizer);
        }

        void SetCurRecognizer(ComponentName comp, Int32 userHandle) {
            Settings.Secure->PutStringForUser(mContext->GetContentResolver(),
                    Settings.Secure.VOICE_RECOGNITION_SERVICE,
                    comp != NULL ? comp->FlattenToShortString() : "", userHandle);
            if (DEBUG) Slogger::I(TAG, "setCurRecognizer comp=" + comp
                    + " user=" + userHandle);
        }

        //@Override
        CARAPI StartSession(IVoiceInteractionService service, Bundle args) {
            {    AutoLock syncLock(this);
                if (mImpl == NULL || mImpl.mService == NULL
                        || service->AsBinder() != mImpl.mService->AsBinder()) {
                    throw new SecurityException(
                            "Caller is not the current voice interaction service");
                }
                final Int32 callingPid = Binder->GetCallingPid();
                final Int32 callingUid = Binder->GetCallingUid();
                final Int64 caller = Binder->ClearCallingIdentity();
                try {
                    mImpl->StartSessionLocked(callingPid, callingUid, args);
                } finally {
                    Binder->RestoreCallingIdentity(caller);
                }
            }
        }

        //@Override
        public Boolean DeliverNewSession(IBinder token, IVoiceInteractionSession session,
                IVoiceInteractor interactor) {
            {    AutoLock syncLock(this);
                if (mImpl == NULL) {
                    throw new SecurityException(
                            "deliverNewSession without running voice interaction service");
                }
                final Int32 callingPid = Binder->GetCallingPid();
                final Int32 callingUid = Binder->GetCallingUid();
                final Int64 caller = Binder->ClearCallingIdentity();
                try {
                    return mImpl->DeliverNewSessionLocked(callingPid, callingUid, token, session,
                            interactor);
                } finally {
                    Binder->RestoreCallingIdentity(caller);
                }
            }
        }

        //@Override
        public Int32 StartVoiceActivity(IBinder token, Intent intent, String resolvedType) {
            {    AutoLock syncLock(this);
                if (mImpl == NULL) {
                    Slogger::W(TAG, "startVoiceActivity without running voice interaction service");
                    return ActivityManager.START_CANCELED;
                }
                final Int32 callingPid = Binder->GetCallingPid();
                final Int32 callingUid = Binder->GetCallingUid();
                final Int64 caller = Binder->ClearCallingIdentity();
                if (!SystemProperties->GetBoolean("persist.test.voice_interaction", FALSE)) {
                    throw new SecurityException("Voice interaction not supported");
                }
                try {
                    return mImpl->StartVoiceActivityLocked(callingPid, callingUid, token,
                            intent, resolvedType);
                } finally {
                    Binder->RestoreCallingIdentity(caller);
                }
            }
        }

        //@Override
        CARAPI Finish(IBinder token) {
            {    AutoLock syncLock(this);
                if (mImpl == NULL) {
                    Slogger::W(TAG, "finish without running voice interaction service");
                    return;
                }
                final Int32 callingPid = Binder->GetCallingPid();
                final Int32 callingUid = Binder->GetCallingUid();
                final Int64 caller = Binder->ClearCallingIdentity();
                try {
                    mImpl->FinishLocked(callingPid, callingUid, token);
                } finally {
                    Binder->RestoreCallingIdentity(caller);
                }
            }
        }

        //----------------- Model management APIs --------------------------------//

        //@Override
        public KeyphraseSoundModel GetKeyphraseSoundModel(Int32 keyphraseId, String bcp47Locale) {
            {    AutoLock syncLock(this);
                if (mContext->CheckCallingPermission(Manifest::permission::MANAGE_VOICE_KEYPHRASES)
                        != PackageManager.PERMISSION_GRANTED) {
                    throw new SecurityException("Caller does not hold the permission "
                            + Manifest::permission::MANAGE_VOICE_KEYPHRASES);
                }
            }

            if (bcp47Locale == NULL) {
                throw new IllegalArgumentException("Illegal Argument(s) in getKeyphraseSoundModel");
            }

            final Int32 callingUid = UserHandle->GetCallingUserId();
            final Int64 caller = Binder->ClearCallingIdentity();
            try {
                return mDbHelper->GetKeyphraseSoundModel(keyphraseId, callingUid, bcp47Locale);
            } finally {
                Binder->RestoreCallingIdentity(caller);
            }
        }

        //@Override
        public Int32 UpdateKeyphraseSoundModel(KeyphraseSoundModel model) {
            {    AutoLock syncLock(this);
                if (mContext->CheckCallingPermission(Manifest::permission::MANAGE_VOICE_KEYPHRASES)
                        != PackageManager.PERMISSION_GRANTED) {
                    throw new SecurityException("Caller does not hold the permission "
                            + Manifest::permission::MANAGE_VOICE_KEYPHRASES);
                }
                if (model == NULL) {
                    throw new IllegalArgumentException("Model must not be NULL");
                }
            }

            final Int64 caller = Binder->ClearCallingIdentity();
            try {
                if (mDbHelper->UpdateKeyphraseSoundModel(model)) {
                    {    AutoLock syncLock(this);
                        // Notify the voice interaction service of a change in sound models.
                        if (mImpl != NULL && mImpl.mService != NULL) {
                            mImpl->NotifySoundModelsChangedLocked();
                        }
                    }
                    return SoundTriggerHelper.STATUS_OK;
                } else {
                    return SoundTriggerHelper.STATUS_ERROR;
                }
            } finally {
                Binder->RestoreCallingIdentity(caller);
            }
        }

        //@Override
        public Int32 DeleteKeyphraseSoundModel(Int32 keyphraseId, String bcp47Locale) {
            {    AutoLock syncLock(this);
                if (mContext->CheckCallingPermission(Manifest::permission::MANAGE_VOICE_KEYPHRASES)
                        != PackageManager.PERMISSION_GRANTED) {
                    throw new SecurityException("Caller does not hold the permission "
                            + Manifest::permission::MANAGE_VOICE_KEYPHRASES);
                }
            }

            if (bcp47Locale == NULL) {
                throw new IllegalArgumentException(
                        "Illegal Argument(s) in deleteKeyphraseSoundModel");
            }

            final Int32 callingUid = UserHandle->GetCallingUserId();
            final Int64 caller = Binder->ClearCallingIdentity();
            Boolean deleted = FALSE;
            try {
                deleted = mDbHelper->DeleteKeyphraseSoundModel(keyphraseId, callingUid, bcp47Locale);
                return deleted ? SoundTriggerHelper.STATUS_OK : SoundTriggerHelper.STATUS_ERROR;
            } finally {
                if (deleted) {
                    {    AutoLock syncLock(this);
                        // Notify the voice interaction service of a change in sound models.
                        if (mImpl != NULL && mImpl.mService != NULL) {
                            mImpl->NotifySoundModelsChangedLocked();
                        }
                    }
                }
                Binder->RestoreCallingIdentity(caller);
            }
        }

        //----------------- SoundTrigger APIs --------------------------------//
        //@Override
        public Boolean IsEnrolledForKeyphrase(IVoiceInteractionService service, Int32 keyphraseId,
                String bcp47Locale) {
            {    AutoLock syncLock(this);
                if (mImpl == NULL || mImpl.mService == NULL
                        || service->AsBinder() != mImpl.mService->AsBinder()) {
                    throw new SecurityException(
                            "Caller is not the current voice interaction service");
                }
            }

            if (bcp47Locale == NULL) {
                throw new IllegalArgumentException("Illegal Argument(s) in isEnrolledForKeyphrase");
            }

            final Int32 callingUid = UserHandle->GetCallingUserId();
            final Int64 caller = Binder->ClearCallingIdentity();
            try {
                KeyphraseSoundModel model =
                        mDbHelper->GetKeyphraseSoundModel(keyphraseId, callingUid, bcp47Locale);
                return model != NULL;
            } finally {
                Binder->RestoreCallingIdentity(caller);
            }
        }

        //@Override
        public ModuleProperties GetDspModuleProperties(IVoiceInteractionService service) {
            // Allow the call if this is the current voice interaction service.
            {    AutoLock syncLock(this);
                if (mImpl == NULL || mImpl.mService == NULL
                        || service == NULL || service->AsBinder() != mImpl.mService->AsBinder()) {
                    throw new SecurityException(
                            "Caller is not the current voice interaction service");
                }

                final Int64 caller = Binder->ClearCallingIdentity();
                try {
                    return mSoundTriggerHelper.moduleProperties;
                } finally {
                    Binder->RestoreCallingIdentity(caller);
                }
            }
        }

        //@Override
        public Int32 StartRecognition(IVoiceInteractionService service, Int32 keyphraseId,
                String bcp47Locale, IRecognitionStatusCallback callback,
                RecognitionConfig recognitionConfig) {
            // Allow the call if this is the current voice interaction service.
            {    AutoLock syncLock(this);
                if (mImpl == NULL || mImpl.mService == NULL
                        || service == NULL || service->AsBinder() != mImpl.mService->AsBinder()) {
                    throw new SecurityException(
                            "Caller is not the current voice interaction service");
                }

                if (callback == NULL || recognitionConfig == NULL || bcp47Locale == NULL) {
                    throw new IllegalArgumentException("Illegal Argument(s) in startRecognition");
                }
            }

            Int32 callingUid = UserHandle->GetCallingUserId();
            final Int64 caller = Binder->ClearCallingIdentity();
            try {
                KeyphraseSoundModel soundModel =
                        mDbHelper->GetKeyphraseSoundModel(keyphraseId, callingUid, bcp47Locale);
                if (soundModel == NULL
                        || soundModel.uuid == NULL
                        || soundModel.keyphrases == NULL) {
                    Slogger::W(TAG, "No matching sound model found in startRecognition");
                    return SoundTriggerHelper.STATUS_ERROR;
                } else {
                    return mSoundTriggerHelper->StartRecognition(
                            keyphraseId, soundModel, callback, recognitionConfig);
                }
            } finally {
                Binder->RestoreCallingIdentity(caller);
            }
        }

        //@Override
        public Int32 StopRecognition(IVoiceInteractionService service, Int32 keyphraseId,
                IRecognitionStatusCallback callback) {
            // Allow the call if this is the current voice interaction service.
            {    AutoLock syncLock(this);
                if (mImpl == NULL || mImpl.mService == NULL
                        || service == NULL || service->AsBinder() != mImpl.mService->AsBinder()) {
                    throw new SecurityException(
                            "Caller is not the current voice interaction service");
                }
            }

            final Int64 caller = Binder->ClearCallingIdentity();
            try {
                return mSoundTriggerHelper->StopRecognition(keyphraseId, callback);
            } finally {
                Binder->RestoreCallingIdentity(caller);
            }
        }

        //@Override
        CARAPI Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
            if (mContext->CheckCallingOrSelfPermission(Manifest::permission::DUMP)
                    != PackageManager.PERMISSION_GRANTED) {
                pw->Println("Permission Denial: can't dump PowerManager from from pid="
                        + Binder->GetCallingPid()
                        + ", uid=" + Binder->GetCallingUid());
                return;
            }
            {    AutoLock syncLock(this);
                pw->Println("VOICE INTERACTION MANAGER (dumpsys voiceinteraction)\n");
                if (mImpl == NULL) {
                    pw->Println("  (No active implementation)");
                    return;
                }
                mImpl->DumpLocked(fd, pw, args);
            }
            mSoundTriggerHelper->Dump(fd, pw, args);
        }

        class SettingsObserver extends ContentObserver {
            SettingsObserver(Handler handler) {
                Super(handler);
                ContentResolver resolver = mContext->GetContentResolver();
                resolver->RegisterContentObserver(Settings.Secure->GetUriFor(
                        Settings.Secure.VOICE_INTERACTION_SERVICE), FALSE, this);
            }

            //@Override CARAPI OnChange(Boolean selfChange) {
                {    AutoLock syncLock(VoiceInteractionManagerServiceStub.this);
                    SwitchImplementationIfNeededLocked(FALSE);
                }
            }
        }

        PackageMonitor mPackageMonitor = new PackageMonitor() {
            //@Override
            public Boolean OnHandleForceStop(Intent intent, String[] packages, Int32 uid, Boolean doit) {
                return super->OnHandleForceStop(intent, packages, uid, doit);
            }

            //@Override
            CARAPI OnHandleUserStop(Intent intent, Int32 userHandle) {
            }

            //@Override
            CARAPI OnSomePackagesChanged() {
                Int32 userHandle = GetChangingUserId();
                if (DEBUG) Slogger::I(TAG, "onSomePackagesChanged user=" + userHandle);

                ComponentName curInteractor = GetCurInteractor(userHandle);
                ComponentName curRecognizer = GetCurRecognizer(userHandle);
                if (curRecognizer == NULL) {
                    // Could a new recognizer appear when we don't have one pre-installed?
                    if (AnyPackagesAppearing()) {
                        curRecognizer = FindAvailRecognizer(NULL, userHandle);
                        if (curRecognizer != NULL) {
                            SetCurRecognizer(curRecognizer, userHandle);
                        }
                    }
                    return;
                }

                if (curInteractor != NULL) {
                    Int32 change = IsPackageDisappearing(curInteractor->GetPackageName());
                    if (change == PACKAGE_PERMANENT_CHANGE) {
                        // The currently set interactor is permanently gone; fall back to
                        // the default config.
                        SetCurInteractor(NULL, userHandle);
                        SetCurRecognizer(NULL, userHandle);
                        InitForUser(userHandle);
                        return;
                    }

                    change = IsPackageAppearing(curInteractor->GetPackageName());
                    if (change != PACKAGE_UNCHANGED) {
                        // If current interactor is now appearing, for any reason, then
                        // restart our connection with it.
                        if (mImpl != NULL && curInteractor->GetPackageName()->Equals(
                                mImpl.mComponent->GetPackageName())) {
                            SwitchImplementationIfNeededLocked(TRUE);
                        }
                    }
                    return;
                }

                // There is no interactor, so just deal with a simple recognizer.
                Int32 change = IsPackageDisappearing(curRecognizer->GetPackageName());
                if (change == PACKAGE_PERMANENT_CHANGE
                        || change == PACKAGE_TEMPORARY_CHANGE) {
                    SetCurRecognizer(FindAvailRecognizer(NULL, userHandle), userHandle);

                } else if (IsPackageModified(curRecognizer->GetPackageName())) {
                    SetCurRecognizer(FindAvailRecognizer(curRecognizer->GetPackageName(),
                            userHandle), userHandle);
                }
            }
        };
    }
}
