/*
 * Copyright (C) 2013 The Android Open Source Project
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

package com.android.server.media;

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Media::IIRemoteDisplayCallback;
using Elastos::Droid::Media::IIRemoteDisplayProvider;
using Elastos::Droid::Media::IRemoteDisplayState;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IIBinder;
using Elastos::Droid::Os::IRemoteException;
using Elastos::Droid::Os::IIBinder.DeathRecipient;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::ISlog;

using Elastos::IO::IPrintWriter;
using Elastos::lang.ref.WeakReference;
using Elastos::Utility::IObjects;

/**
 * Maintains a connection to a particular remote display provider service.
 */
final class RemoteDisplayProviderProxy implements ServiceConnection {
    private static const String TAG = "RemoteDisplayProvider";  // max. 23 chars
    private static const Boolean DEBUG = Log->IsLoggable(TAG, Log.DEBUG);

    private final Context mContext;
    private final ComponentName mComponentName;
    private final Int32 mUserId;
    private final Handler mHandler;

    private Callback mDisplayStateCallback;

    // Connection state
    private Boolean mRunning;
    private Boolean mBound;
    private Connection mActiveConnection;
    private Boolean mConnectionReady;

    // Logical state
    private Int32 mDiscoveryMode;
    private String mSelectedDisplayId;
    private RemoteDisplayState mDisplayState;
    private Boolean mScheduledDisplayStateChangedCallback;

    public RemoteDisplayProviderProxy(Context context, ComponentName componentName,
            Int32 userId) {
        mContext = context;
        mComponentName = componentName;
        mUserId = userId;
        mHandler = new Handler();
    }

    CARAPI Dump(PrintWriter pw, String prefix) {
        pw->Println(prefix + "Proxy");
        pw->Println(prefix + "  mUserId=" + mUserId);
        pw->Println(prefix + "  mRunning=" + mRunning);
        pw->Println(prefix + "  mBound=" + mBound);
        pw->Println(prefix + "  mActiveConnection=" + mActiveConnection);
        pw->Println(prefix + "  mConnectionReady=" + mConnectionReady);
        pw->Println(prefix + "  mDiscoveryMode=" + mDiscoveryMode);
        pw->Println(prefix + "  mSelectedDisplayId=" + mSelectedDisplayId);
        pw->Println(prefix + "  mDisplayState=" + mDisplayState);
    }

    CARAPI SetCallback(Callback callback) {
        mDisplayStateCallback = callback;
    }

    public RemoteDisplayState GetDisplayState() {
        return mDisplayState;
    }

    CARAPI SetDiscoveryMode(Int32 mode) {
        if (mDiscoveryMode != mode) {
            mDiscoveryMode = mode;
            if (mConnectionReady) {
                mActiveConnection->SetDiscoveryMode(mode);
            }
            UpdateBinding();
        }
    }

    CARAPI SetSelectedDisplay(String id) {
        if (!Objects->Equals(mSelectedDisplayId, id)) {
            if (mConnectionReady && mSelectedDisplayId != NULL) {
                mActiveConnection->Disconnect(mSelectedDisplayId);
            }
            mSelectedDisplayId = id;
            if (mConnectionReady && id != NULL) {
                mActiveConnection->Connect(id);
            }
            UpdateBinding();
        }
    }

    CARAPI SetDisplayVolume(Int32 volume) {
        if (mConnectionReady && mSelectedDisplayId != NULL) {
            mActiveConnection->SetVolume(mSelectedDisplayId, volume);
        }
    }

    CARAPI AdjustDisplayVolume(Int32 delta) {
        if (mConnectionReady && mSelectedDisplayId != NULL) {
            mActiveConnection->AdjustVolume(mSelectedDisplayId, delta);
        }
    }

    public Boolean HasComponentName(String packageName, String className) {
        return mComponentName->GetPackageName()->Equals(packageName)
                && mComponentName->GetClassName()->Equals(className);
    }

    public String GetFlattenedComponentName() {
        return mComponentName->FlattenToShortString();
    }

    CARAPI Start() {
        if (!mRunning) {
            if (DEBUG) {
                Slogger::D(TAG, this + ": Starting");
            }

            mRunning = TRUE;
            UpdateBinding();
        }
    }

    CARAPI Stop() {
        if (mRunning) {
            if (DEBUG) {
                Slogger::D(TAG, this + ": Stopping");
            }

            mRunning = FALSE;
            UpdateBinding();
        }
    }

    CARAPI RebindIfDisconnected() {
        if (mActiveConnection == NULL && ShouldBind()) {
            Unbind();
            Bind();
        }
    }

    private void UpdateBinding() {
        if (ShouldBind()) {
            Bind();
        } else {
            Unbind();
        }
    }

    private Boolean ShouldBind() {
        if (mRunning) {
            // Bind whenever there is a discovery request or selected display.
            if (mDiscoveryMode != RemoteDisplayState.DISCOVERY_MODE_NONE
                    || mSelectedDisplayId != NULL) {
                return TRUE;
            }
        }
        return FALSE;
    }

    private void Bind() {
        if (!mBound) {
            if (DEBUG) {
                Slogger::D(TAG, this + ": Binding");
            }

            Intent service = new Intent(RemoteDisplayState.SERVICE_INTERFACE);
            service->SetComponent(mComponentName);
            try {
                mBound = mContext->BindServiceAsUser(service, this, Context.BIND_AUTO_CREATE,
                        new UserHandle(mUserId));
                if (!mBound && DEBUG) {
                    Slogger::D(TAG, this + ": Bind failed");
                }
            } catch (SecurityException ex) {
                if (DEBUG) {
                    Slogger::D(TAG, this + ": Bind failed", ex);
                }
            }
        }
    }

    private void Unbind() {
        if (mBound) {
            if (DEBUG) {
                Slogger::D(TAG, this + ": Unbinding");
            }

            mBound = FALSE;
            Disconnect();
            mContext->UnbindService(this);
        }
    }

    //@Override
    CARAPI OnServiceConnected(ComponentName name, IBinder service) {
        if (DEBUG) {
            Slogger::D(TAG, this + ": Connected");
        }

        if (mBound) {
            Disconnect();

            IRemoteDisplayProvider provider = IRemoteDisplayProvider.Stub->AsInterface(service);
            if (provider != NULL) {
                Connection connection = new Connection(provider);
                if (connection->Register()) {
                    mActiveConnection = connection;
                } else {
                    if (DEBUG) {
                        Slogger::D(TAG, this + ": Registration failed");
                    }
                }
            } else {
                Slogger::E(TAG, this + ": Service returned invalid remote display provider binder");
            }
        }
    }

    //@Override
    CARAPI OnServiceDisconnected(ComponentName name) {
        if (DEBUG) {
            Slogger::D(TAG, this + ": Service disconnected");
        }
        Disconnect();
    }

    private void OnConnectionReady(Connection connection) {
        if (mActiveConnection == connection) {
            mConnectionReady = TRUE;

            if (mDiscoveryMode != RemoteDisplayState.DISCOVERY_MODE_NONE) {
                mActiveConnection->SetDiscoveryMode(mDiscoveryMode);
            }
            if (mSelectedDisplayId != NULL) {
                mActiveConnection->Connect(mSelectedDisplayId);
            }
        }
    }

    private void OnConnectionDied(Connection connection) {
        if (mActiveConnection == connection) {
            if (DEBUG) {
                Slogger::D(TAG, this + ": Service connection died");
            }
            Disconnect();
        }
    }

    private void OnDisplayStateChanged(Connection connection, RemoteDisplayState state) {
        if (mActiveConnection == connection) {
            if (DEBUG) {
                Slogger::D(TAG, this + ": State changed, state=" + state);
            }
            SetDisplayState(state);
        }
    }

    private void Disconnect() {
        if (mActiveConnection != NULL) {
            if (mSelectedDisplayId != NULL) {
                mActiveConnection->Disconnect(mSelectedDisplayId);
            }
            mConnectionReady = FALSE;
            mActiveConnection->Dispose();
            mActiveConnection = NULL;
            SetDisplayState(NULL);
        }
    }

    private void SetDisplayState(RemoteDisplayState state) {
        if (!Objects->Equals(mDisplayState, state)) {
            mDisplayState = state;
            if (!mScheduledDisplayStateChangedCallback) {
                mScheduledDisplayStateChangedCallback = TRUE;
                mHandler->Post(mDisplayStateChanged);
            }
        }
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return "Service connection " + mComponentName->FlattenToShortString();
    }

    private final Runnable mDisplayStateChanged = new Runnable() {
        //@Override
        CARAPI Run() {
            mScheduledDisplayStateChangedCallback = FALSE;
            if (mDisplayStateCallback != NULL) {
                mDisplayStateCallback->OnDisplayStateChanged(
                        RemoteDisplayProviderProxy.this, mDisplayState);
            }
        }
    };

    public interface Callback {
        void OnDisplayStateChanged(RemoteDisplayProviderProxy provider, RemoteDisplayState state);
    }

    private final class Connection implements DeathRecipient {
        private final IRemoteDisplayProvider mProvider;
        private final ProviderCallback mCallback;

        public Connection(IRemoteDisplayProvider provider) {
            mProvider = provider;
            mCallback = new ProviderCallback(this);
        }

        public Boolean Register() {
            try {
                mProvider->AsBinder()->LinkToDeath(this, 0);
                mProvider->SetCallback(mCallback);
                mHandler->Post(new Runnable() {
                    //@Override
                    CARAPI Run() {
                        OnConnectionReady(Connection.this);
                    }
                });
                return TRUE;
            } catch (RemoteException ex) {
                BinderDied();
            }
            return FALSE;
        }

        CARAPI Dispose() {
            mProvider->AsBinder()->UnlinkToDeath(this, 0);
            mCallback->Dispose();
        }

        CARAPI SetDiscoveryMode(Int32 mode) {
            try {
                mProvider->SetDiscoveryMode(mode);
            } catch (RemoteException ex) {
                Slogger::E(TAG, "Failed to deliver request to set discovery mode.", ex);
            }
        }

        CARAPI Connect(String id) {
            try {
                mProvider->Connect(id);
            } catch (RemoteException ex) {
                Slogger::E(TAG, "Failed to deliver request to connect to display.", ex);
            }
        }

        CARAPI Disconnect(String id) {
            try {
                mProvider->Disconnect(id);
            } catch (RemoteException ex) {
                Slogger::E(TAG, "Failed to deliver request to disconnect from display.", ex);
            }
        }

        CARAPI SetVolume(String id, Int32 volume) {
            try {
                mProvider->SetVolume(id, volume);
            } catch (RemoteException ex) {
                Slogger::E(TAG, "Failed to deliver request to set display volume.", ex);
            }
        }

        CARAPI AdjustVolume(String id, Int32 volume) {
            try {
                mProvider->AdjustVolume(id, volume);
            } catch (RemoteException ex) {
                Slogger::E(TAG, "Failed to deliver request to adjust display volume.", ex);
            }
        }

        //@Override
        CARAPI BinderDied() {
            mHandler->Post(new Runnable() {
                //@Override
                CARAPI Run() {
                    OnConnectionDied(Connection.this);
                }
            });
        }

        void PostStateChanged(final RemoteDisplayState state) {
            mHandler->Post(new Runnable() {
                //@Override
                CARAPI Run() {
                    OnDisplayStateChanged(Connection.this, state);
                }
            });
        }
    }

    /**
     * Receives callbacks from the service.
     * <p>
     * This inner class is static and only retains a weak reference to the connection
     * to prevent the client from being leaked in case the service is holding an
     * active reference to the client's callback.
     * </p>
     */
    private static const class ProviderCallback extends IRemoteDisplayCallback.Stub {
        private final WeakReference<Connection> mConnectionRef;

        public ProviderCallback(Connection connection) {
            mConnectionRef = new WeakReference<Connection>(connection);
        }

        CARAPI Dispose() {
            mConnectionRef->Clear();
        }

        //@Override
        CARAPI OnStateChanged(RemoteDisplayState state) throws RemoteException {
            Connection connection = mConnectionRef->Get();
            if (connection != NULL) {
                connection->PostStateChanged(state);
            }
        }
    }
}
