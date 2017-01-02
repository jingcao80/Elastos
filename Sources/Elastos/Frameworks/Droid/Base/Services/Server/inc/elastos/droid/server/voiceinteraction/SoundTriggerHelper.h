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

/**
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

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Hardware::soundtrigger::IIRecognitionStatusCallback;
using Elastos::Droid::Hardware::soundtrigger::ISoundTrigger;
using Elastos::Droid::Hardware::soundtrigger::ISoundTrigger.Keyphrase;
using Elastos::Droid::Hardware::soundtrigger::ISoundTrigger.KeyphraseRecognitionEvent;
using Elastos::Droid::Hardware::soundtrigger::ISoundTrigger.KeyphraseRecognitionExtra;
using Elastos::Droid::Hardware::soundtrigger::ISoundTrigger.KeyphraseSoundModel;
using Elastos::Droid::Hardware::soundtrigger::ISoundTrigger.ModuleProperties;
using Elastos::Droid::Hardware::soundtrigger::ISoundTrigger.RecognitionConfig;
using Elastos::Droid::Hardware::soundtrigger::ISoundTrigger.RecognitionEvent;
using Elastos::Droid::Hardware::soundtrigger::ISoundTrigger.SoundModelEvent;
using Elastos::Droid::Hardware::soundtrigger::ISoundTriggerModule;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IRemoteException;
using Elastos::Droid::Telephony::IPhoneStateListener;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Utility::ISlog;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IUUID;

/**
 * Helper for {@link SoundTrigger} APIs.
 * Currently this just acts as an abstraction over all SoundTrigger API calls.
 *
 * @hide
 */
public class SoundTriggerHelper implements SoundTrigger.StatusListener {
    static const String TAG = "SoundTriggerHelper";
    static const Boolean DBG = FALSE;

    /**
     * Return codes for {@link #StartRecognition(Int32, KeyphraseSoundModel,
     *      IRecognitionStatusCallback, RecognitionConfig)},
     * {@link #StopRecognition(Int32, IRecognitionStatusCallback)}
     */
    public static const Int32 STATUS_ERROR = SoundTrigger.STATUS_ERROR;
    public static const Int32 STATUS_OK = SoundTrigger.STATUS_OK;

    private static const Int32 INVALID_VALUE = Integer.MIN_VALUE;

    /** The {@link ModuleProperties} for the system, or NULL if none exists. */
    final ModuleProperties moduleProperties;

    /** The properties for the DSP module */
    private SoundTriggerModule mModule;
    private final Object mLock = new Object();
    private final Context mContext;
    private final TelephonyManager mTelephonyManager;
    private final PhoneStateListener mPhoneStateListener;
    private final PowerManager mPowerManager;

    // TODO: Since many layers currently only deal with one recognition
    // we simplify things by assuming one listener here too.
    private IRecognitionStatusCallback mActiveListener;
    private Int32 mKeyphraseId = INVALID_VALUE;
    private Int32 mCurrentSoundModelHandle = INVALID_VALUE;
    private UUID mCurrentSoundModelUuid = NULL;
    // FIXME: Ideally this should not be stored if allowMultipleTriggers happens at a lower layer.
    private RecognitionConfig mRecognitionConfig = NULL;
    private Boolean mRequested = FALSE;
    private Boolean mCallActive = FALSE;
    private Boolean mIsPowerSaveMode = FALSE;
    // Indicates if the native sound trigger service is disabled or not.
    // This is an indirect indication of the microphone being open in some other application.
    private Boolean mServiceDisabled = FALSE;
    private Boolean mStarted = FALSE;
    private PowerSaveModeListener mPowerSaveModeListener;

    SoundTriggerHelper(Context context) {
        ArrayList <ModuleProperties> modules = new ArrayList<>();
        Int32 status = SoundTrigger->ListModules(modules);
        mContext = context;
        mTelephonyManager = (TelephonyManager) context->GetSystemService(Context.TELEPHONY_SERVICE);
        mPowerManager = (PowerManager) context->GetSystemService(Context.POWER_SERVICE);
        mPhoneStateListener = new MyCallStateListener();
        if (status != SoundTrigger.STATUS_OK || modules->Size() == 0) {
            Slogger::W(TAG, "listModules status=" + status + ", # of modules=" + modules->Size());
            moduleProperties = NULL;
            mModule = NULL;
        } else {
            // TODO: Figure out how to determine which module corresponds to the DSP hardware.
            moduleProperties = modules->Get(0);
        }
    }

    /**
     * Starts recognition for the given keyphraseId.
     *
     * @param keyphraseId The identifier of the keyphrase for which
     *        the recognition is to be started.
     * @param soundModel The sound model to use for recognition.
     * @param listener The listener for the recognition events related to the given keyphrase.
     * @return One of {@link #STATUS_ERROR} or {@link #STATUS_OK}.
     */
    Int32 StartRecognition(Int32 keyphraseId,
            KeyphraseSoundModel soundModel,
            IRecognitionStatusCallback listener,
            RecognitionConfig recognitionConfig) {
        if (soundModel == NULL || listener == NULL || recognitionConfig == NULL) {
            return STATUS_ERROR;
        }

        {    AutoLock syncLock(mLock);
            if (DBG) {
                Slogger::D(TAG, "startRecognition for keyphraseId=" + keyphraseId
                        + " soundModel=" + soundModel + ", listener=" + listener->AsBinder()
                        + ", recognitionConfig=" + recognitionConfig);
                Slogger::D(TAG, "moduleProperties=" + moduleProperties);
                Slogger::D(TAG, "current listener="
                        + (mActiveListener == NULL ? "NULL" : mActiveListener->AsBinder()));
                Slogger::D(TAG, "current SoundModel handle=" + mCurrentSoundModelHandle);
                Slogger::D(TAG, "current SoundModel UUID="
                        + (mCurrentSoundModelUuid == NULL ? NULL : mCurrentSoundModelUuid));
            }

            if (!mStarted) {
                // Get the current call state synchronously for the first recognition.
                mCallActive = mTelephonyManager->GetCallState() != TelephonyManager.CALL_STATE_IDLE;
                // Register for call state changes when the first call to start recognition occurs.
                mTelephonyManager->Listen(mPhoneStateListener, PhoneStateListener.LISTEN_CALL_STATE);

                // Register for power saver mode changes when the first call to start recognition
                // occurs.
                if (mPowerSaveModeListener == NULL) {
                    mPowerSaveModeListener = new PowerSaveModeListener();
                    mContext->RegisterReceiver(mPowerSaveModeListener,
                            new IntentFilter(PowerManager.ACTION_POWER_SAVE_MODE_CHANGED));
                }
                mIsPowerSaveMode = mPowerManager->IsPowerSaveMode();
            }

            if (moduleProperties == NULL) {
                Slogger::W(TAG, "Attempting startRecognition without the capability");
                return STATUS_ERROR;
            }
            if (mModule == NULL) {
                mModule = SoundTrigger->AttachModule(moduleProperties.id, this, NULL);
                if (mModule == NULL) {
                    Slogger::W(TAG, "startRecognition cannot attach to sound trigger module");
                    return STATUS_ERROR;
                }
            }

            // Unload the previous model if the current one isn't invalid
            // and, it's not the same as the new one, or we are already started
            // if we are already started, we can get multiple calls to start
            // if the underlying sound model changes, in which case we should unload and reload.
            // The model reuse helps only in cases when we trigger and stop internally
            // without a start recognition call.
            if (mCurrentSoundModelHandle != INVALID_VALUE
                    && (!soundModel.uuid->Equals(mCurrentSoundModelUuid) || mStarted)) {
                Slogger::W(TAG, "Unloading previous sound model");
                Int32 status = mModule->UnloadSoundModel(mCurrentSoundModelHandle);
                if (status != SoundTrigger.STATUS_OK) {
                    Slogger::W(TAG, "unloadSoundModel call failed with " + status);
                }
                mCurrentSoundModelHandle = INVALID_VALUE;
                mCurrentSoundModelUuid = NULL;
                mStarted = FALSE;
            }

            // If the previous recognition was by a different listener,
            // Notify them that it was stopped.
            if (mActiveListener != NULL && mActiveListener->AsBinder() != listener->AsBinder()) {
                Slogger::W(TAG, "Canceling previous recognition");
                try {
                    mActiveListener->OnError(STATUS_ERROR);
                } catch (RemoteException e) {
                    Slogger::W(TAG, "RemoteException in onDetectionStopped", e);
                }
                mActiveListener = NULL;
            }

            // Load the sound model if the current one is NULL.
            Int32 soundModelHandle = mCurrentSoundModelHandle;
            if (mCurrentSoundModelHandle == INVALID_VALUE
                    || mCurrentSoundModelUuid == NULL) {
                Int32[] handle = new Int32[] { INVALID_VALUE };
                Int32 status = mModule->LoadSoundModel(soundModel, handle);
                if (status != SoundTrigger.STATUS_OK) {
                    Slogger::W(TAG, "loadSoundModel call failed with " + status);
                    return status;
                }
                if (handle[0] == INVALID_VALUE) {
                    Slogger::W(TAG, "loadSoundModel call returned invalid sound model handle");
                    return STATUS_ERROR;
                }
                soundModelHandle = handle[0];
            } else {
                if (DBG) Slogger::D(TAG, "Reusing previously loaded sound model");
            }

            // Start the recognition.
            mRequested = TRUE;
            mKeyphraseId = keyphraseId;
            mCurrentSoundModelHandle = soundModelHandle;
            mCurrentSoundModelUuid = soundModel.uuid;
            mRecognitionConfig = recognitionConfig;
            // Register the new listener. This replaces the old one.
            // There can only be a maximum of one active listener at any given time.
            mActiveListener = listener;

            return UpdateRecognitionLocked(FALSE /* don't notify for synchronous calls */);
        }
    }

    /**
     * Stops recognition for the given {@link Keyphrase} if a recognition is
     * currently active.
     *
     * @param keyphraseId The identifier of the keyphrase for which
     *        the recognition is to be stopped.
     * @param listener The listener for the recognition events related to the given keyphrase.
     *
     * @return One of {@link #STATUS_ERROR} or {@link #STATUS_OK}.
     */
    Int32 StopRecognition(Int32 keyphraseId, IRecognitionStatusCallback listener) {
        if (listener == NULL) {
            return STATUS_ERROR;
        }

        {    AutoLock syncLock(mLock);
            if (DBG) {
                Slogger::D(TAG, "stopRecognition for keyphraseId=" + keyphraseId
                        + ", listener=" + listener->AsBinder());
                Slogger::D(TAG, "current listener="
                        + (mActiveListener == NULL ? "NULL" : mActiveListener->AsBinder()));
            }

            if (moduleProperties == NULL || mModule == NULL) {
                Slogger::W(TAG, "Attempting stopRecognition without the capability");
                return STATUS_ERROR;
            }

            if (mActiveListener == NULL) {
                // startRecognition hasn't been called or it failed.
                Slogger::W(TAG, "Attempting stopRecognition without a successful startRecognition");
                return STATUS_ERROR;
            }
            if (mActiveListener->AsBinder() != listener->AsBinder()) {
                // We don't allow a different listener to stop the recognition than the one
                // that started it.
                Slogger::W(TAG, "Attempting stopRecognition for another recognition");
                return STATUS_ERROR;
            }

            // Stop recognition if it's the current one, ignore otherwise.
            mRequested = FALSE;
            Int32 status = UpdateRecognitionLocked(FALSE /* don't notify for synchronous calls */);
            if (status != SoundTrigger.STATUS_OK) {
                return status;
            }

            status = mModule->UnloadSoundModel(mCurrentSoundModelHandle);
            if (status != SoundTrigger.STATUS_OK) {
                Slogger::W(TAG, "unloadSoundModel call failed with " + status);
            }

            // Clear the internal state once the recognition has been stopped.
            // Unload sound model call may fail in scenarios, and we'd still want
            // to reload the sound model.
            InternalClearStateLocked();
            return status;
        }
    }

    /**
     * Stops all recognitions active currently and clears the internal state.
     */
    void StopAllRecognitions() {
        {    AutoLock syncLock(mLock);
            if (moduleProperties == NULL || mModule == NULL) {
                return;
            }

            if (mCurrentSoundModelHandle == INVALID_VALUE) {
                return;
            }

            mRequested = FALSE;
            Int32 status = UpdateRecognitionLocked(FALSE /* don't notify for synchronous calls */);
            status = mModule->UnloadSoundModel(mCurrentSoundModelHandle);
            if (status != SoundTrigger.STATUS_OK) {
                Slogger::W(TAG, "unloadSoundModel call failed with " + status);
            }

            InternalClearStateLocked();
        }
    }

    //---- SoundTrigger.StatusListener methods
    //@Override
    CARAPI OnRecognition(RecognitionEvent event) {
        if (event == NULL || !(event instanceof KeyphraseRecognitionEvent)) {
            Slogger::W(TAG, "Invalid recognition event!");
            return;
        }

        if (DBG) Slogger::D(TAG, "onRecognition: " + event);
        {    AutoLock syncLock(mLock);
            if (mActiveListener == NULL) {
                Slogger::W(TAG, "received onRecognition event without any listener for it");
                return;
            }
            switch (event.status) {
                // Fire aborts/failures to all listeners since it's not tied to a keyphrase.
                case SoundTrigger.RECOGNITION_STATUS_ABORT:
                    OnRecognitionAbortLocked();
                    break;
                case SoundTrigger.RECOGNITION_STATUS_FAILURE:
                    OnRecognitionFailureLocked();
                    break;
                case SoundTrigger.RECOGNITION_STATUS_SUCCESS:
                    OnRecognitionSuccessLocked((KeyphraseRecognitionEvent) event);
                    break;
            }
        }
    }

    //@Override
    CARAPI OnSoundModelUpdate(SoundModelEvent event) {
        if (event == NULL) {
            Slogger::W(TAG, "Invalid sound model event!");
            return;
        }
        if (DBG) Slogger::D(TAG, "onSoundModelUpdate: " + event);
        {    AutoLock syncLock(mLock);
            OnSoundModelUpdatedLocked(event);
        }
    }

    //@Override
    CARAPI OnServiceStateChange(Int32 state) {
        if (DBG) Slogger::D(TAG, "onServiceStateChange, state: " + state);
        {    AutoLock syncLock(mLock);
            OnServiceStateChangedLocked(SoundTrigger.SERVICE_STATE_DISABLED == state);
        }
    }

    //@Override
    CARAPI OnServiceDied() {
        Slogger::E(TAG, "onServiceDied!!");
        {    AutoLock syncLock(mLock);
            OnServiceDiedLocked();
        }
    }

    private void OnCallStateChangedLocked(Boolean callActive) {
        if (mCallActive == callActive) {
            // We consider multiple call states as being active
            // so we check if something really changed or not here.
            return;
        }
        mCallActive = callActive;
        UpdateRecognitionLocked(TRUE /* notify */);
    }

    private void OnPowerSaveModeChangedLocked(Boolean isPowerSaveMode) {
        if (mIsPowerSaveMode == isPowerSaveMode) {
            return;
        }
        mIsPowerSaveMode = isPowerSaveMode;
        UpdateRecognitionLocked(TRUE /* notify */);
    }

    private void OnSoundModelUpdatedLocked(SoundModelEvent event) {
        // TODO: Handle sound model update here.
    }

    private void OnServiceStateChangedLocked(Boolean disabled) {
        if (disabled == mServiceDisabled) {
            return;
        }
        mServiceDisabled = disabled;
        UpdateRecognitionLocked(TRUE /* notify */);
    }

    private void OnRecognitionAbortLocked() {
        Slogger::W(TAG, "Recognition aborted");
        // No-op
        // This is handled via service state changes instead.
    }

    private void OnRecognitionFailureLocked() {
        Slogger::W(TAG, "Recognition failure");
        try {
            if (mActiveListener != NULL) {
                mActiveListener->OnError(STATUS_ERROR);
            }
        } catch (RemoteException e) {
            Slogger::W(TAG, "RemoteException in onError", e);
        } finally {
            InternalClearStateLocked();
        }
    }

    private void OnRecognitionSuccessLocked(KeyphraseRecognitionEvent event) {
        Slogger::I(TAG, "Recognition success");
        KeyphraseRecognitionExtra[] keyphraseExtras =
                ((KeyphraseRecognitionEvent) event).keyphraseExtras;
        if (keyphraseExtras == NULL || keyphraseExtras.length == 0) {
            Slogger::W(TAG, "Invalid keyphrase recognition event!");
            return;
        }
        // TODO: Handle more than one keyphrase extras.
        if (mKeyphraseId != keyphraseExtras[0].id) {
            Slogger::W(TAG, "received onRecognition event for a different keyphrase");
            return;
        }

        try {
            if (mActiveListener != NULL) {
                mActiveListener->OnDetected((KeyphraseRecognitionEvent) event);
            }
        } catch (RemoteException e) {
            Slogger::W(TAG, "RemoteException in onDetected", e);
        }

        mStarted = FALSE;
        mRequested = mRecognitionConfig.allowMultipleTriggers;
        // TODO: Remove this block if the lower layer supports multiple triggers.
        if (mRequested) {
            UpdateRecognitionLocked(TRUE /* notify */);
        }
    }

    private void OnServiceDiedLocked() {
        try {
            if (mActiveListener != NULL) {
                mActiveListener->OnError(SoundTrigger.STATUS_DEAD_OBJECT);
            }
        } catch (RemoteException e) {
            Slogger::W(TAG, "RemoteException in onError", e);
        } finally {
            InternalClearStateLocked();
            if (mModule != NULL) {
                mModule->Detach();
                mModule = NULL;
            }
        }
    }

    private Int32 UpdateRecognitionLocked(Boolean notify) {
        if (mModule == NULL || moduleProperties == NULL
                || mCurrentSoundModelHandle == INVALID_VALUE || mActiveListener == NULL) {
            // Nothing to do here.
            return STATUS_OK;
        }

        Boolean start = mRequested && !mCallActive && !mServiceDisabled && !mIsPowerSaveMode;
        if (start == mStarted) {
            // No-op.
            return STATUS_OK;
        }

        // See if the recognition needs to be started.
        if (start) {
            // Start recognition.
            Int32 status = mModule->StartRecognition(mCurrentSoundModelHandle, mRecognitionConfig);
            if (status != SoundTrigger.STATUS_OK) {
                Slogger::W(TAG, "startRecognition failed with " + status);
                // Notify of error if needed.
                if (notify) {
                    try {
                        mActiveListener->OnError(status);
                    } catch (RemoteException e) {
                        Slogger::W(TAG, "RemoteException in onError", e);
                    }
                }
            } else {
                mStarted = TRUE;
                // Notify of resume if needed.
                if (notify) {
                    try {
                        mActiveListener->OnRecognitionResumed();
                    } catch (RemoteException e) {
                        Slogger::W(TAG, "RemoteException in onRecognitionResumed", e);
                    }
                }
            }
            return status;
        } else {
            // Stop recognition.
            Int32 status = mModule->StopRecognition(mCurrentSoundModelHandle);
            if (status != SoundTrigger.STATUS_OK) {
                Slogger::W(TAG, "stopRecognition call failed with " + status);
                if (notify) {
                    try {
                        mActiveListener->OnError(status);
                    } catch (RemoteException e) {
                        Slogger::W(TAG, "RemoteException in onError", e);
                    }
                }
            } else {
                mStarted = FALSE;
                // Notify of pause if needed.
                if (notify) {
                    try {
                        mActiveListener->OnRecognitionPaused();
                    } catch (RemoteException e) {
                        Slogger::W(TAG, "RemoteException in onRecognitionPaused", e);
                    }
                }
            }
            return status;
        }
    }

    private void InternalClearStateLocked() {
        mStarted = FALSE;
        mRequested = FALSE;

        mKeyphraseId = INVALID_VALUE;
        mCurrentSoundModelHandle = INVALID_VALUE;
        mCurrentSoundModelUuid = NULL;
        mRecognitionConfig = NULL;
        mActiveListener = NULL;

        // Unregister from call state changes.
        mTelephonyManager->Listen(mPhoneStateListener, PhoneStateListener.LISTEN_NONE);

        // Unregister from power save mode changes.
        if (mPowerSaveModeListener != NULL) {
            mContext->UnregisterReceiver(mPowerSaveModeListener);
            mPowerSaveModeListener = NULL;
        }
    }

    class MyCallStateListener extends PhoneStateListener {
        //@Override
        CARAPI OnCallStateChanged(Int32 state, String arg1) {
            if (DBG) Slogger::D(TAG, "onCallStateChanged: " + state);
            {    AutoLock syncLock(mLock);
                OnCallStateChangedLocked(TelephonyManager.CALL_STATE_IDLE != state);
            }
        }
    }

    class PowerSaveModeListener extends BroadcastReceiver {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            if (!PowerManager.ACTION_POWER_SAVE_MODE_CHANGED->Equals(intent->GetAction())) {
                return;
            }
            Boolean active = mPowerManager->IsPowerSaveMode();
            if (DBG) Slogger::D(TAG, "onPowerSaveModeChanged: " + active);
            {    AutoLock syncLock(mLock);
                OnPowerSaveModeChangedLocked(active);
            }
        }
    }

    void Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        {    AutoLock syncLock(mLock);
            pw->Print("  module properties=");
            pw->Println(moduleProperties == NULL ? "NULL" : moduleProperties);
            pw->Print("  keyphrase ID="); pw->Println(mKeyphraseId);
            pw->Print("  sound model handle="); pw->Println(mCurrentSoundModelHandle);
            pw->Print("  sound model UUID=");
            pw->Println(mCurrentSoundModelUuid == NULL ? "NULL" : mCurrentSoundModelUuid);
            pw->Print("  current listener=");
            pw->Println(mActiveListener == NULL ? "NULL" : mActiveListener->AsBinder());

            pw->Print("  requested="); pw->Println(mRequested);
            pw->Print("  started="); pw->Println(mStarted);
            pw->Print("  call active="); pw->Println(mCallActive);
            pw->Print("  power save mode active="); pw->Println(mIsPowerSaveMode);
            pw->Print("  service disabled="); pw->Println(mServiceDisabled);
        }
    }
}
