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

package com.android.server.fingerprint;

using Elastos::Droid::App::IService;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IIBinder;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IRemoteException;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Service::Ifingerprint.FingerprintManager;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::ISlog;

using Elastos::Droid::Server::ISystemService;

using Elastos::Droid::Service::Ifingerprint.FingerprintUtils;
using Elastos::Droid::Service::Ifingerprint.IFingerprintService;
using Elastos::Droid::Service::Ifingerprint.IFingerprintServiceReceiver;

using Elastos::IO::IPrintWriter;
using Elastos::lang.ref.WeakReference;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IMap.Entry;
using Elastos::Utility::ISet;

/**
 * A service to manage multiple clients that want to access the fingerprint HAL API.
 * The service is responsible for maintaining a list of clients and dispatching all
 * fingerprint -related events.
 *
 * @hide
 */
public class FingerprintService extends SystemService {
    private final String TAG = "FingerprintService";
    private static const Boolean DEBUG = TRUE;
    private ArrayMap<IBinder, ClientData> mClients = new ArrayMap<IBinder, ClientData>();

    private static const Int32 MSG_NOTIFY = 10;

    Handler mHandler = new Handler() {
        CARAPI HandleMessage(android.os.Message msg) {
            switch (msg.what) {
                case MSG_NOTIFY:
                    HandleNotify(msg.arg1, msg.arg2, (Integer) msg.obj);
                    break;

                default:
                    Slogger::W(TAG, "Unknown message:" + msg.what);
            }
        }
    };
    private Context mContext;

    private static const Int32 STATE_IDLE = 0;
    private static const Int32 STATE_LISTENING = 1;
    private static const Int32 STATE_ENROLLING = 2;
    private static const Int32 STATE_REMOVING = 3;
    private static const Int64 MS_PER_SEC = 1000;
    public static const String USE_FINGERPRINT = "android.permission.USE_FINGERPRINT";
    public static const String ENROLL_FINGERPRINT = "android.permission.ENROLL_FINGERPRINT";

    private static const class ClientData {
        public IFingerprintServiceReceiver receiver;
        Int32 state;
        Int32 userId;
        public TokenWatcher tokenWatcher;
        IBinder GetToken() { return tokenWatcher->GetToken(); }
    }

    private class TokenWatcher implements IBinder.DeathRecipient {
        WeakReference<IBinder> token;

        TokenWatcher(IBinder token) {
            this.token = new WeakReference<IBinder>(token);
        }

        IBinder GetToken() { return token->Get(); }
        CARAPI BinderDied() {
            mClients->Remove(token);
            this.token = NULL;
        }

        protected void Finalize() throws Throwable {
            try {
                if (token != NULL) {
                    if (DEBUG) Slogger::W(TAG, "removing leaked reference: " + token);
                    mClients->Remove(token);
                }
            } finally {
                super->Finalize();
            }
        }
    }

    public FingerprintService(Context context) {
        Super(context);
        mContext = context;
        NativeInit(this);
    }

    // TODO: Move these into separate process
    // JNI methods to communicate from FingerprintManagerService to HAL
    native Int32 NativeEnroll(Int32 timeout);
    native Int32 NativeEnrollCancel();
    native Int32 NativeRemove(Int32 fingerprintId);
    native Int32 NativeOpenHal();
    native Int32 NativeCloseHal();
    native void NativeInit(FingerprintService service);

    // JNI methods for communicating from HAL to clients
    void Notify(Int32 msg, Int32 arg1, Int32 arg2) {
        mHandler->ObtainMessage(MSG_NOTIFY, msg, arg1, arg2).SendToTarget();
    }

    void HandleNotify(Int32 msg, Int32 arg1, Int32 arg2) {
        Slogger::V(TAG, "HandleNotify(msg=" + msg + ", arg1=" + arg1 + ", arg2=" + arg2 + ")");
        for (Int32 i = 0; i < mClients->Size(); i++) {
            ClientData clientData = mClients->ValueAt(i);
            if (clientData == NULL || clientData.receiver == NULL) {
                if (DEBUG) Slogger::V(TAG, "clientData at " + i + " is invalid!!");
                continue;
            }
            switch (msg) {
                case FingerprintManager.FINGERPRINT_ERROR: {
                    final Int32 error = arg1;
                    try {
                        clientData.receiver->OnError(error);
                    } catch (RemoteException e) {
                        Slogger::E(TAG, "can't send message to client. Did it die?", e);
                        mClients->Remove(mClients->KeyAt(i));
                    }
                }
                break;
                case FingerprintManager.FINGERPRINT_ACQUIRED: {
                    final Int32 acquireInfo = arg1;
                    try {
                        clientData.receiver->OnAcquired(acquireInfo);
                    } catch (RemoteException e) {
                        Slogger::E(TAG, "can't send message to client. Did it die?", e);
                        mClients->Remove(mClients->KeyAt(i));
                    }
                    break;
                }
                case FingerprintManager.FINGERPRINT_PROCESSED: {
                    final Int32 fingerId = arg1;
                    try {
                        clientData.receiver->OnProcessed(fingerId);
                    } catch (RemoteException e) {
                        Slogger::E(TAG, "can't send message to client. Did it die?", e);
                        mClients->Remove(mClients->KeyAt(i));
                    }
                    break;
                }
                case FingerprintManager.FINGERPRINT_TEMPLATE_ENROLLING: {
                    final Int32 fingerId = arg1;
                    final Int32 remaining = arg2;
                    if (clientData.state == STATE_ENROLLING) {
                        // Only send enroll updates to clients that are actually enrolling
                        try {
                            clientData.receiver->OnEnrollResult(fingerId, remaining);
                        } catch (RemoteException e) {
                            Slogger::E(TAG, "can't send message to client. Did it die?", e);
                            mClients->Remove(mClients->KeyAt(i));
                        }
                        // Update the database with new finger id.
                        // TODO: move to client code (Settings)
                        if (remaining == 0) {
                            FingerprintUtils->AddFingerprintIdForUser(fingerId,
                                    mContext->GetContentResolver(), clientData.userId);
                            clientData.state = STATE_IDLE; // Nothing left to do
                        }
                    } else {
                        if (DEBUG) Slogger::W(TAG, "Client not enrolling");
                        break;
                    }
                    break;
                }
                case FingerprintManager.FINGERPRINT_TEMPLATE_REMOVED: {
                    Int32 fingerId = arg1;
                    if (fingerId == 0) throw new IllegalStateException("Got illegal id from HAL");
                    FingerprintUtils->RemoveFingerprintIdForUser(fingerId,
                            mContext->GetContentResolver(), clientData.userId);
                    if (clientData.receiver != NULL) {
                        try {
                            clientData.receiver->OnRemoved(fingerId);
                        } catch (RemoteException e) {
                            Slogger::E(TAG, "can't send message to client. Did it die?", e);
                            mClients->Remove(mClients->KeyAt(i));
                        }
                    }
                    clientData.state = STATE_LISTENING;
                }
                break;
            }
        }
    }

    void StartEnroll(IBinder token, Int64 timeout, Int32 userId) {
        ClientData clientData = mClients->Get(token);
        if (clientData != NULL) {
            if (clientData.userId != userId) throw new IllegalStateException("Bad user");
            clientData.state = STATE_ENROLLING;
            NativeEnroll((Int32) (timeout / MS_PER_SEC));
        } else {
            Slogger::W(TAG, "Enroll(): No listener registered");
        }
    }

    void StartEnrollCancel(IBinder token, Int32 userId) {
        ClientData clientData = mClients->Get(token);
        if (clientData != NULL) {
            if (clientData.userId != userId) throw new IllegalStateException("Bad user");
            clientData.state = STATE_LISTENING;
            NativeEnrollCancel();
        } else {
            Slogger::W(TAG, "EnrollCancel(): No listener registered");
        }
    }

    // Remove all fingerprints for the given user.
    void StartRemove(IBinder token, Int32 fingerId, Int32 userId) {
        ClientData clientData = mClients->Get(token);
        if (clientData != NULL) {
            if (clientData.userId != userId) throw new IllegalStateException("Bad user");
            clientData.state = STATE_REMOVING;
            // The fingerprint id will be removed when we get confirmation from the HAL
            Int32 result = NativeRemove(fingerId);
            if (result != 0) {
                Slogger::W(TAG, "Error removing fingerprint with id = " + fingerId);
            }
        } else {
            Slogger::W(TAG, "Remove(" + token + "): No listener registered");
        }
    }

    void AddListener(IBinder token, IFingerprintServiceReceiver receiver, Int32 userId) {
        if (DEBUG) Slogger::V(TAG, "StartListening(" + receiver + ")");
        if (mClients->Get(token) == NULL) {
            ClientData clientData = new ClientData();
            clientData.state = STATE_LISTENING;
            clientData.receiver = receiver;
            clientData.userId = userId;
            clientData.tokenWatcher = new TokenWatcher(token);
            try {
                token->LinkToDeath(clientData.tokenWatcher, 0);
                mClients->Put(token, clientData);
            } catch (RemoteException e) {
                Slogger::W(TAG, "caught remote exception in linkToDeath: ", e);
            }
        } else {
            if (DEBUG) Slogger::V(TAG, "listener already registered for " + token);
        }
    }

    void RemoveListener(IBinder token, Int32 userId) {
        if (DEBUG) Slogger::V(TAG, "StopListening(" + token + ")");
        ClientData clientData = mClients->Get(token);
        if (clientData != NULL) {
            token->UnlinkToDeath(clientData.tokenWatcher, 0);
            mClients->Remove(token);
        } else {
            if (DEBUG) Slogger::V(TAG, "listener not registered: " + token);
        }
        mClients->Remove(token);
    }

    void CheckPermission(String permisison) {
        // TODO
    }

    private final class FingerprintServiceWrapper extends IFingerprintService.Stub {
        //@Override // Binder call
        CARAPI Enroll(IBinder token, Int64 timeout, Int32 userId) {
            CheckPermission(ENROLL_FINGERPRINT);
            StartEnroll(token, timeout, userId);
        }

        //@Override // Binder call
        CARAPI EnrollCancel(IBinder token,Int32 userId) {
            CheckPermission(ENROLL_FINGERPRINT);
            StartEnrollCancel(token, userId);
        }

        //@Override // Binder call
        CARAPI Remove(IBinder token, Int32 fingerprintId, Int32 userId) {
            CheckPermission(ENROLL_FINGERPRINT); // TODO: Maybe have another permission
            StartRemove(token, fingerprintId, userId);
        }

        //@Override // Binder call
        CARAPI StartListening(IBinder token, IFingerprintServiceReceiver receiver, Int32 userId)
        {
            CheckPermission(USE_FINGERPRINT);
            AddListener(token, receiver, userId);
        }

        //@Override // Binder call
        CARAPI StopListening(IBinder token, Int32 userId) {
            CheckPermission(USE_FINGERPRINT);
            RemoveListener(token, userId);
        }
    }

    //@Override
    CARAPI OnStart() {
       PublishBinderService(Context.FINGERPRINT_SERVICE, new FingerprintServiceWrapper());
       NativeOpenHal();
    }

}
