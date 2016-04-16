#include "Elastos.Droid.Utility.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/BindingManagerImpl.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ThreadUtils.h"
#include "elastos/droid/webkit/webview/chromium/native/base/SysUtils.h"
#include "elastos/core/AutoLock.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Webkit::Webview::Chromium::Base::ThreadUtils;
using Elastos::Droid::Webkit::Webview::Chromium::Base::SysUtils;
using Elastos::Core::AutoLock;
using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

//===============================================================
//    BindingManagerImpl::ManagedConnection::DoUnbindRunnable
//===============================================================

BindingManagerImpl::ManagedConnection::DoUnbindRunnable::DoUnbindRunnable(
    /* [in] */ ManagedConnection* owner,
    /* [in] */ ChildProcessConnection* connection)
    : mOwner(owner)
    , mConnection(connection)
{
}

CAR_INTERFACE_IMPL(BindingManagerImpl::ManagedConnection::DoUnbindRunnable, Runnable, IRunnable);

ECode BindingManagerImpl::ManagedConnection::DoUnbindRunnable::Run()
{
    if (mConnection->IsStrongBindingBound()) {
        mConnection->RemoveStrongBinding();
    }

    return NOERROR;
}

//===============================================================
//    BindingManagerImpl::ManagedConnection::DelayedRunnable
//===============================================================

BindingManagerImpl::ManagedConnection::DelayedRunnable::DelayedRunnable(
    /* [in] */ ManagedConnection* owner,
    /* [in] */ ChildProcessConnection* connection)
    : mOwner(owner)
    , mConnection(connection)
{
}

CAR_INTERFACE_IMPL(BindingManagerImpl::ManagedConnection::DelayedRunnable, Runnable, IRunnable);

ECode BindingManagerImpl::ManagedConnection::DelayedRunnable::Run()
{
    if (mConnection->IsInitialBindingBound()) {
        mConnection->RemoveInitialBinding();
    }

    return NOERROR;
}

//===============================================================
//             BindingManagerImpl::ManagedConnection
//===============================================================

BindingManagerImpl::ManagedConnection::ManagedConnection(
    /* [in] */ BindingManagerImpl* owner,
    /* [in] */ ChildProcessConnection* connection)
    : mOwner(owner)
    , mConnection(connection)
    , mInForeground(FALSE)
    , mBoundForBackgroundPeriod(FALSE)
    , mWasOomProtected(FALSE)
{
}

/** Removes the initial service binding. */
void BindingManagerImpl::ManagedConnection::RemoveInitialBinding()
{
    const AutoPtr<ChildProcessConnection> connection = mConnection;
    if (connection == NULL || !connection->IsInitialBindingBound()) return;

    AutoPtr<IRunnable> runnable = new DelayedRunnable(this, connection);
    ThreadUtils::PostOnUiThreadDelayed(runnable, mOwner->mRemoveInitialBindingDelay);
}

/** Adds a strong service binding. */
void BindingManagerImpl::ManagedConnection::AddStrongBinding()
{
    AutoPtr<ChildProcessConnection> connection = mConnection;
    if (connection == NULL) return;

    connection->AddStrongBinding();
}

/** Removes a strong service binding. */
void BindingManagerImpl::ManagedConnection::RemoveStrongBinding()
{
    const AutoPtr<ChildProcessConnection> connection = mConnection;
    // We have to fail gracefully if the strong binding is not present, as on low-end the
    // binding could have been removed by dropOomBindings() when a new service was started.
    if (connection == NULL || !connection->IsStrongBindingBound()) return;

    // This runnable performs the actual unbinding. It will be executed synchronously when
    // on low-end devices and posted with a delay otherwise.
    AutoPtr<IRunnable> doUnbind =  new DoUnbindRunnable(this, connection);

    if (mOwner->mIsLowMemoryDevice) {
        doUnbind->Run();
    }
    else {
        ThreadUtils::PostOnUiThreadDelayed(doUnbind, mOwner->mRemoveStrongBindingDelay);
    }
}

/**
 * Drops the service bindings. This is used on low-end to drop bindings of the current
 * service when a new one is created.
 */
void BindingManagerImpl::ManagedConnection::DropBindings()
{
    assert(mOwner->mIsLowMemoryDevice);
    AutoPtr<ChildProcessConnection> connection = mConnection;
    if (connection == NULL) return;

    connection->DropOomBindings();
}

/**
 * Sets the visibility of the service, adding or removing the strong binding as needed. This
 * also removes the initial binding, as the service visibility is now known.
 */
void BindingManagerImpl::ManagedConnection::SetInForeground(
    /* [in] */ Boolean nextInForeground)
{
    if (!mInForeground && nextInForeground) {
        AddStrongBinding();
    }
    else if (mInForeground && !nextInForeground) {
        RemoveStrongBinding();
    }

    RemoveInitialBinding();
    mInForeground = nextInForeground;
}

/**
 * Sets or removes additional binding when the service is main service during the embedder
 * background period.
 */
void BindingManagerImpl::ManagedConnection::SetBoundForBackgroundPeriod(
    /* [in] */ Boolean nextBound)
{
    if (!mBoundForBackgroundPeriod && nextBound) {
        AddStrongBinding();
    }
    else if (mBoundForBackgroundPeriod && !nextBound) {
        RemoveStrongBinding();
    }

    mBoundForBackgroundPeriod = nextBound;
}

Boolean BindingManagerImpl::ManagedConnection::IsOomProtected()
{
    // When a process crashes, we can be queried about its oom status before or after the
    // connection is cleared. For the latter case, the oom status is stashed in
    // mWasOomProtected.
    return mConnection != NULL ?
            mConnection->IsOomProtectedOrWasWhenDied() : mWasOomProtected;
}

void BindingManagerImpl::ManagedConnection::ClearConnection()
{
    mWasOomProtected = mConnection->IsOomProtectedOrWasWhenDied();
    mConnection = NULL;
}

/** @return true iff the reference to the connection is no longer held */
//@VisibleForTesting
Boolean BindingManagerImpl::ManagedConnection::IsConnectionCleared()
{
    return mConnection == NULL;
}

//===============================================================
//                   BindingManagerImpl
//===============================================================

const String BindingManagerImpl::TAG("BindingManager");

// Delay of 1 second used when removing the initial oom binding of a process.
const Int64 BindingManagerImpl::REMOVE_INITIAL_BINDING_DELAY_MILLIS;

// Delay of 1 second used when removing temporary strong binding of a process (only on
// non-low-memory devices).
const Int64 BindingManagerImpl::DETACH_AS_ACTIVE_HIGH_END_DELAY_MILLIS;

/**
 * The constructor is private to hide parameters exposed for testing from the regular consumer.
 * Use factory methods to create an instance.
 */
BindingManagerImpl::BindingManagerImpl(
    /* [in] */ Boolean isLowMemoryDevice,
    /* [in] */ Int64 removeInitialBindingDelay,
    /* [in] */ Int64 removeStrongBindingDelay)
    : mRemoveInitialBindingDelay(removeInitialBindingDelay)
    , mRemoveStrongBindingDelay(removeStrongBindingDelay)
    , mIsLowMemoryDevice(isLowMemoryDevice)
{
}

AutoPtr<BindingManagerImpl> BindingManagerImpl::CreateBindingManager()
{
    return new BindingManagerImpl(SysUtils::IsLowEndDevice(),
            REMOVE_INITIAL_BINDING_DELAY_MILLIS, DETACH_AS_ACTIVE_HIGH_END_DELAY_MILLIS);
}

/**
 * Creates a testing instance of BindingManager. Testing instance will have the unbinding delays
 * set to 0, so that the tests don't need to deal with actual waiting.
 * @param isLowEndDevice true iff the created instance should apply low-end binding policies
 */
AutoPtr<BindingManagerImpl> BindingManagerImpl::CreateBindingManagerForTesting(
    /* [in] */ Boolean isLowEndDevice)
{
    return new BindingManagerImpl(isLowEndDevice, 0, 0);
}

//@Override
void BindingManagerImpl::AddNewConnection(
    /* [in] */ Int32 pid,
    /* [in] */ ChildProcessConnection* connection)
{
    {
        AutoLock lock(mLastInForegroundLock);
        if (mIsLowMemoryDevice && mLastInForeground != NULL) mLastInForeground->DropBindings();
    }

    // This will reset the previous entry for the pid in the unlikely event of the OS
    // reusing renderer pids.
    {
        AutoLock lock(mManagedConnectionsLock);
        AutoPtr<ManagedConnection> managedConnection = new ManagedConnection(this, connection);
        mManagedConnections->Put(pid, TO_IINTERFACE(managedConnection));
    }
}

//@Override
void BindingManagerImpl::SetInForeground(
    /* [in] */ Int32 pid,
    /* [in] */ Boolean inForeground)
{
     AutoPtr<ManagedConnection> managedConnection;

     {
         AutoLock lock(mManagedConnectionsLock);
         AutoPtr<IInterface> obj;
         mManagedConnections->Get(pid, (IInterface**)&obj);
         managedConnection = (ManagedConnection*)(IObject::Probe(obj));
     }

     if (managedConnection == NULL) {
         Logger::W(TAG, "Cannot setInForeground() - never saw a connection for the pid: %d", pid);
         return;
     }

     {
         AutoLock lock(mLastInForegroundLock);
         managedConnection->SetInForeground(inForeground);
         if (inForeground) mLastInForeground = managedConnection;
     }
}

//@Override
void BindingManagerImpl::OnSentToBackground()
{
    assert(mBoundForBackgroundPeriod == NULL);
    {
        AutoLock lock(mLastInForegroundLock);
        // mLastInForeground can be null at this point as the embedding application could be
        // used in foreground without spawning any renderers.
        if (mLastInForeground != NULL) {
            mLastInForeground->SetBoundForBackgroundPeriod(TRUE);
            mBoundForBackgroundPeriod = mLastInForeground;
        }
    }
}

//@Override
void BindingManagerImpl::OnBroughtToForeground()
{
    if (mBoundForBackgroundPeriod != NULL) {
        mBoundForBackgroundPeriod->SetBoundForBackgroundPeriod(FALSE);
        mBoundForBackgroundPeriod = NULL;
    }
}

//@Override
Boolean BindingManagerImpl::IsOomProtected(
    /* [in] */ Int32 pid)
{
    // In the unlikely event of the OS reusing renderer pid, the call will refer to the most
    // recent renderer of the given pid. The binding state for a pid is being reset in
    // addNewConnection().
    AutoPtr<ManagedConnection> managedConnection;
    {
        AutoLock lock(mManagedConnectionsLock);
        AutoPtr<IInterface> obj;
        mManagedConnections->Get(pid, (IInterface**)&obj);
        managedConnection = (ManagedConnection*)(IObject::Probe(obj));
    }

    return managedConnection != NULL ? managedConnection->IsOomProtected() : FALSE;
}

//@Override
void BindingManagerImpl::ClearConnection(
    /* [in] */ Int32 pid)
{
    AutoPtr<ManagedConnection> managedConnection;
    {
        AutoLock lock(mManagedConnectionsLock);
        AutoPtr<IInterface> obj;
        mManagedConnections->Get(pid, (IInterface**)&obj);
        managedConnection = (ManagedConnection*)(IObject::Probe(obj));
    }

    if (managedConnection != NULL) managedConnection->ClearConnection();
}

/** @return true iff the connection reference is no longer held */
//@VisibleForTesting
Boolean BindingManagerImpl::IsConnectionCleared(
    /* [in] */ Int32 pid)
{
    AutoLock lock(mManagedConnectionsLock);
    AutoPtr<IInterface> obj;
    mManagedConnections->Get(pid, (IInterface**)&obj);
    AutoPtr<ManagedConnection> managedConnection;
    managedConnection = (ManagedConnection*)(IObject::Probe(obj));
    return managedConnection->IsConnectionCleared();
}

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
