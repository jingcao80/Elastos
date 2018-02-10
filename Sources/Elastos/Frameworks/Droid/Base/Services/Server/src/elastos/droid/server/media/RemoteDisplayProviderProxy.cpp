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

#include "elastos/droid/server/media/RemoteDisplayProviderProxy.h"
#include <elastos/utility/Objects.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Utility::Objects;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Media {

// {7D60ED5E-65E5-4229-806C-1AFD69AF0AF1}
extern "C" const InterfaceID EIID_IRemoteDisplayProviderProxyCallback =
        { 0x7d60ed5e, 0x65e5, 0x4229, { 0x80, 0x6c, 0x1a, 0xfd, 0x69, 0xaf, 0xa, 0xf1 } };

//==============================================================================
//                  RemoteDisplayProviderProxy::DisplayStateChangedRunnable
//==============================================================================

ECode RemoteDisplayProviderProxy::DisplayStateChangedRunnable::Run()
{
    mHost->mScheduledDisplayStateChangedCallback = FALSE;
    if (mHost->mDisplayStateCallback != NULL) {
        mHost->mDisplayStateCallback->OnDisplayStateChanged(mHost, mHost->mDisplayState);
    }
    return NOERROR;
}


//==============================================================================
//                  RemoteDisplayProviderProxy::Connection::RegisterRunnable
//==============================================================================

ECode RemoteDisplayProviderProxy::Connection::RegisterRunnable::Run()
{
    mHost->mHost->OnConnectionReady(mHost);
    return NOERROR;
}


//==============================================================================
//                  RemoteDisplayProviderProxy::Connection::ProxyDiedRunnable
//==============================================================================

ECode RemoteDisplayProviderProxy::Connection::ProxyDiedRunnable::Run()
{
    mHost->mHost->OnConnectionDied(mHost);
    return NOERROR;
}


//==============================================================================
//                  RemoteDisplayProviderProxy::Connection::PostStateChangedRunnable
//==============================================================================

ECode RemoteDisplayProviderProxy::Connection::PostStateChangedRunnable::Run()
{
    mHost->mHost->OnDisplayStateChanged(mHost, mState);
    return NOERROR;
}


//==============================================================================
//                  RemoteDisplayProviderProxy::Connection
//==============================================================================

RemoteDisplayProviderProxy::Connection::Connection(
    /* [in] */ IIRemoteDisplayProvider* provider,
    /* [in] */ RemoteDisplayProviderProxy* host)
    : mProvider(provider)
    , mHost(host)
{
    CProviderCallback::NewByFriend((HANDLE)this, (CProviderCallback**)&mCallback);
}

CAR_INTERFACE_IMPL(RemoteDisplayProviderProxy::Connection, Object, IProxyDeathRecipient)

Boolean RemoteDisplayProviderProxy::Connection::Register()
{
    // try {
    AutoPtr<IBinder> b = IBinder::Probe(mProvider);
    AutoPtr<IProxy> proxy = (IProxy*)b->Probe(EIID_IProxy);
    if (proxy != NULL) {
        if (FAILED(proxy->LinkToDeath(this, 0))) {
            ProxyDied();
            return FALSE;
        }
    }
    mProvider->SetCallback((IIRemoteDisplayCallback*)mCallback.Get());
    AutoPtr<IRunnable> runnable = (IRunnable*)new RegisterRunnable(this);
    Boolean result;
    mHost->mHandler->Post(runnable, &result);
    return TRUE;
    // } catch (RemoteException ex) {
    //     binderDied();
    // }
    // return false;
}

void RemoteDisplayProviderProxy::Connection::Dispose()
{
    AutoPtr<IBinder> b = IBinder::Probe(mProvider);
    AutoPtr<IProxy> proxy = (IProxy*)b->Probe(EIID_IProxy);
    if (proxy != NULL) {
        Boolean result;
        proxy->UnlinkToDeath(this, 0, &result);
    }
    mCallback->Dispose();
}

void RemoteDisplayProviderProxy::Connection::SetDiscoveryMode(
    /* [in] */ Int32 mode)
{
    // try {
    if (FAILED(mProvider->SetDiscoveryMode(mode))) {
        Slogger::E(RemoteDisplayProviderProxy::TAG, "Failed to deliver request to set discovery mode.");
    }
    // } catch (RemoteException ex) {
    //     Slog.e(TAG, "Failed to deliver request to set discovery mode.", ex);
    // }
}

void RemoteDisplayProviderProxy::Connection::Connect(
    /* [in] */ const String& id)
{
    // try {
    if (FAILED(mProvider->Connect(id))) {
        Slogger::E(RemoteDisplayProviderProxy::TAG, "Failed to deliver request to connect to display.");
    }
    // } catch (RemoteException ex) {
    //     Slog.e(TAG, "Failed to deliver request to connect to display.", ex);
    // }
}

void RemoteDisplayProviderProxy::Connection::Disconnect(
    /* [in] */ const String& id)
{
    // try {
    if (FAILED(mProvider->Disconnect(id))) {
        Slogger::E(RemoteDisplayProviderProxy::TAG, "Failed to deliver request to Disconnect from display.");
    }
    // } catch (RemoteException ex) {
    //     Slog.e(TAG, "Failed to deliver request to disconnect from display.", ex);
    // }
}

void RemoteDisplayProviderProxy::Connection::SetVolume(
    /* [in] */ const String& id,
    /* [in] */ Int32 volume)
{
    // try {
    if (FAILED(mProvider->SetVolume(id, volume))) {
        Slogger::E(RemoteDisplayProviderProxy::TAG, "Failed to deliver request to set display volume.");
    }
    // } catch (RemoteException ex) {
    //     Slog.e(TAG, "Failed to deliver request to set display volume.", ex);
    // }
}

void RemoteDisplayProviderProxy::Connection::AdjustVolume(
    /* [in] */ const String& id,
    /* [in] */ Int32 volume)
{
    // try {
    if (FAILED(mProvider->AdjustVolume(id, volume))) {
        Slogger::E(RemoteDisplayProviderProxy::TAG, "Failed to deliver request to adjust display volume.");
    }
    // } catch (RemoteException ex) {
    //     Slog.e(TAG, "Failed to deliver request to adjust display volume.", ex);
    // }
}

ECode RemoteDisplayProviderProxy::Connection::ProxyDied()
{
    AutoPtr<IRunnable> runnable = (IRunnable*)new ProxyDiedRunnable(this);
    Boolean result;
    mHost->mHandler->Post(runnable, &result);
    return NOERROR;
}

void RemoteDisplayProviderProxy::Connection::PostStateChanged(
    /* [in] */ IRemoteDisplayState* state)
{
    AutoPtr<IRunnable> runnable = (IRunnable*)new PostStateChangedRunnable(this, state);
    Boolean result;
    mHost->mHandler->Post(runnable, &result);
}


//==============================================================================
//                  RemoteDisplayProviderProxy
//==============================================================================

const String RemoteDisplayProviderProxy::TAG("RemoteDisplayProvider");
const Boolean RemoteDisplayProviderProxy::DEBUG;

RemoteDisplayProviderProxy::RemoteDisplayProviderProxy(
    /* [in] */ IContext* context,
    /* [in] */ IComponentName* componentName,
    /* [in] */ Int32 userId)
    : mContext(context)
    , mComponentName(componentName)
    , mUserId(userId)
    , mRunning(FALSE)
    , mBound(FALSE)
    , mConnectionReady(FALSE)
    , mDiscoveryMode(0)
    , mScheduledDisplayStateChangedCallback(FALSE)
{
    CHandler::New((IHandler**)&mHandler);
    mDisplayStateChanged = (IRunnable*)new DisplayStateChangedRunnable(this);
}

CAR_INTERFACE_IMPL(RemoteDisplayProviderProxy, Object, IServiceConnection)

void RemoteDisplayProviderProxy::SetCallback(
    /* [in] */ ICallback* callback)
{
    mDisplayStateCallback = callback;
}

AutoPtr<IRemoteDisplayState> RemoteDisplayProviderProxy::GetDisplayState()
{
    return mDisplayState;
}

void RemoteDisplayProviderProxy::SetDiscoveryMode(
    /* [in] */ Int32 mode)
{
    if (mDiscoveryMode != mode) {
        mDiscoveryMode = mode;
        if (mConnectionReady) {
            mActiveConnection->SetDiscoveryMode(mode);
        }
        UpdateBinding();
    }
}

void RemoteDisplayProviderProxy::SetSelectedDisplay(
    /* [in] */ const String& id)
{
    if (!mSelectedDisplayId.Equals(id)) {
        if (mConnectionReady && !mSelectedDisplayId.IsNull()) {
            mActiveConnection->Disconnect(mSelectedDisplayId);
        }
        mSelectedDisplayId = id;
        if (mConnectionReady && !id.IsNull()) {
            mActiveConnection->Connect(id);
        }
        UpdateBinding();
    }
}

void RemoteDisplayProviderProxy::SetDisplayVolume(
    /* [in] */ Int32 volume)
{
    if (mConnectionReady && !mSelectedDisplayId.IsNull()) {
        mActiveConnection->SetVolume(mSelectedDisplayId, volume);
    }
}

void RemoteDisplayProviderProxy::AdjustDisplayVolume(
    /* [in] */ Int32 delta)
{
    if (mConnectionReady && !mSelectedDisplayId.IsNull()) {
        mActiveConnection->AdjustVolume(mSelectedDisplayId, delta);
    }
}

Boolean RemoteDisplayProviderProxy::HasComponentName(
    /* [in] */ const String& packageName,
    /* [in] */ const String& className)
{
    String pkgN, clsN;
    return (mComponentName->GetPackageName(&pkgN), pkgN.Equals(packageName))
        && (mComponentName->GetClassName(&clsN), clsN.Equals(className));
}

String RemoteDisplayProviderProxy::GetFlattenedComponentName()
{
    String str;
    mComponentName->FlattenToShortString(&str);
    return str;
}

void RemoteDisplayProviderProxy::Start()
{
    if (!mRunning) {
        if (DEBUG) {
            String str;
            ToString(&str);
            Slogger::D(TAG, "%s: Starting", str.string());
        }

        mRunning = TRUE;
        UpdateBinding();
    }
}

void RemoteDisplayProviderProxy::Stop()
{
    if (mRunning) {
        if (DEBUG) {
            String str;
            ToString(&str);
            Slogger::D(TAG, "%s: Stopping", str.string());
        }

        mRunning = FALSE;
        UpdateBinding();
    }
}

void RemoteDisplayProviderProxy::RebindIfDisconnected()
{
    if (mActiveConnection == NULL && ShouldBind()) {
        Unbind();
        Bind();
    }
}

void RemoteDisplayProviderProxy::UpdateBinding()
{
    if (ShouldBind()) {
        Bind();
    }
    else {
        Unbind();
    }
}

Boolean RemoteDisplayProviderProxy::ShouldBind()
{
    if (mRunning) {
        // Bind whenever there is a discovery request or selected display.
        if (mDiscoveryMode != IRemoteDisplayState::DISCOVERY_MODE_NONE
                || !mSelectedDisplayId.IsNull()) {
            return TRUE;
        }
    }
    return FALSE;
}

void RemoteDisplayProviderProxy::Bind()
{
    if (!mBound) {
        if (DEBUG) {
            String str;
            ToString(&str);
            Slogger::D(TAG, "%s: Binding", str.string());
        }

        AutoPtr<IIntent> service;
        CIntent::New(IRemoteDisplayState::SERVICE_INTERFACE, (IIntent**)&service);
        service->SetComponent(mComponentName);
        // try {
        AutoPtr<IUserHandle> userHandle;
        CUserHandle::New(mUserId, (IUserHandle**)&userHandle);
        ECode ec = mContext->BindServiceAsUser(service, this, IContext::BIND_AUTO_CREATE, userHandle, &mBound);
        if (ec == (ECode)E_SECURITY_EXCEPTION) {
            if (DEBUG) {
                String str;
                ToString(&str);
                Slogger::D(TAG, "%s: Bind failed", str.string());
            }
        }
        if (!mBound && DEBUG) {
            String str;
            ToString(&str);
            Slogger::D(TAG, "%s: Bind failed", str.string());
        }
        // } catch (SecurityException ex) {
        //     if (DEBUG) {
        //         Slog.d(TAG, this + ": Bind failed", ex);
        //     }
        // }
    }
}

void RemoteDisplayProviderProxy::Unbind()
{
    if (mBound) {
        if (DEBUG) {
            String str;
            ToString(&str);
            Slogger::D(TAG, "%s: Unbinding", str.string());
        }

        mBound = FALSE;
        Disconnect();
        mContext->UnbindService(this);
    }
}

ECode RemoteDisplayProviderProxy::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    if (DEBUG) {
        String str;
        ToString(&str);
        Slogger::D(TAG, "%s: Connected", str.string());
    }

    if (mBound) {
        Disconnect();

        AutoPtr<IIRemoteDisplayProvider> provider = IIRemoteDisplayProvider::Probe(service);
        if (provider != NULL) {
            AutoPtr<Connection> connection = new Connection(provider, this);
            if (connection->Register()) {
                mActiveConnection = connection;
            }
            else {
                if (DEBUG) {
                    String str;
                    ToString(&str);
                    Slogger::D(TAG, "%s: Registration failed", str.string());
                }
            }
        }
        else {
            String str;
            ToString(&str);
            Slogger::D(TAG, "%s: Service returned invalid remote display provider binder", str.string());
        }
    }
    return NOERROR;
}

ECode RemoteDisplayProviderProxy::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    if (DEBUG) {
        String str;
        ToString(&str);
        Slogger::D(TAG, "%s: Service disconnected", str.string());
    }
    Disconnect();
    return NOERROR;
}

void RemoteDisplayProviderProxy::OnConnectionReady(
    /* [in] */ Connection* connection)
{
    if (mActiveConnection.Get() == connection) {
        mConnectionReady = TRUE;

        if (mDiscoveryMode != IRemoteDisplayState::DISCOVERY_MODE_NONE) {
            mActiveConnection->SetDiscoveryMode(mDiscoveryMode);
        }
        if (!mSelectedDisplayId.IsNull()) {
            mActiveConnection->Connect(mSelectedDisplayId);
        }
    }
}

void RemoteDisplayProviderProxy::OnConnectionDied(
    /* [in] */ Connection* connection)
{
    if (mActiveConnection.Get() == connection) {
        if (DEBUG) {
            String str;
            ToString(&str);
            Slogger::D(TAG, "%s: Service connection died", str.string());
        }
        Disconnect();
    }
}

void RemoteDisplayProviderProxy::OnDisplayStateChanged(
    /* [in] */ Connection* connection,
    /* [in] */ IRemoteDisplayState* state)
{
    if (mActiveConnection.Get() == connection) {
        if (DEBUG) {
            String str;
            ToString(&str);
            Slogger::D(TAG, "%s: State changed, state=%p", str.string(), state);
        }
        SetDisplayState(state);
    }
}

void RemoteDisplayProviderProxy::Disconnect()
{
    if (mActiveConnection != NULL) {
        if (!mSelectedDisplayId.IsNull()) {
            mActiveConnection->Disconnect(mSelectedDisplayId);
        }
        mConnectionReady = FALSE;
        mActiveConnection->Dispose();
        mActiveConnection = NULL;
        SetDisplayState(NULL);
    }
}

void RemoteDisplayProviderProxy::SetDisplayState(
    /* [in] */ IRemoteDisplayState* state)
{
    if (!Objects::Equals(mDisplayState.Get(), state)) {
        mDisplayState = state;
        if (!mScheduledDisplayStateChangedCallback) {
            mScheduledDisplayStateChangedCallback = TRUE;
            Boolean result;
            mHandler->Post(mDisplayStateChanged, &result);
        }
    }
}

ECode RemoteDisplayProviderProxy::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    String s;
    mComponentName->FlattenToShortString(&s);
    *str = String("Service connection ") + s;
    return NOERROR;
}

} // namespace Media
} // namespace Server
} // namespace Droid
} // namespace Elastos
