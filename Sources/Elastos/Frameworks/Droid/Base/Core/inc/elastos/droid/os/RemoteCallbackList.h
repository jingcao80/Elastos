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

#ifndef __ELASTOS_DROID_OS_REMOTECALLBACKLIST_H__
#define __ELASTOS_DROID_OS_REMOTECALLBACKLIST_H__

#include "Elastos.Droid.Os.h"
#define HASH_FOR_OS
#include "elastos/droid/ext/frameworkhash.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * Takes care of the grunt work of maintaining a list of remote interfaces,
 * typically for the use of performing callbacks from a
 * {@link android.app.Service} to its clients.  In particular, this:
 *
 * <ul>
 * <li> Keeps track of a set of registered {@link IInterface} callbacks,
 * taking care to identify them through their underlying unique {@link IBinder}
 * (by calling {@link IInterface#asBinder IInterface.asBinder()}.
 * <li> Attaches a {@link IBinder.DeathRecipient IBinder.DeathRecipient} to
 * each registered interface, so that it can be cleaned out of the list if its
 * process goes away.
 * <li> Performs locking of the underlying list of interfaces to deal with
 * multithreaded incoming calls, and a thread-safe way to iterate over a
 * snapshot of the list without holding its lock.
 * </ul>
 *
 * <p>To use this class, simply create a single instance along with your
 * service, and call its {@link #register} and {@link #unregister} methods
 * as client register and unregister with your service.  To call back on to
 * the registered clients, use {@link #beginBroadcast},
 * {@link #getBroadcastItem}, and {@link #finishBroadcast}.
 *
 * <p>If a registered callback's process goes away, this class will take
 * care of automatically removing it from the list.  If you want to do
 * additional work in this situation, you can create a subclass that
 * implements the {@link #onCallbackDied} method.
 */
class ECO_PUBLIC RemoteCallbackList
    : public Object
    , public IRemoteCallbackList
{
private:
    class Callback
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        Callback(
            /* [in] */ IInterface* callback,
            /* [in] */ IInterface* cookie,
            /* [in] */ RemoteCallbackList* owner);

        CARAPI ProxyDied();

    public:
        AutoPtr<IInterface> mCallback;
        AutoPtr<IInterface> mCookie;
        RemoteCallbackList* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    RemoteCallbackList();

    /**
     * Simple version of {@link RemoteCallbackList#register(E, Object)}
     * that does not take a cookie object.
     */
    CARAPI Register(
        /* [in] */ IInterface* callback,
        /* [out] */ Boolean* result);

    /**
     * Add a new callback to the list.  This callback will remain in the list
     * until a corresponding call to {@link #unregister} or its hosting process
     * goes away.  If the callback was already registered (determined by
     * checking to see if the {@link IInterface#asBinder callback.asBinder()}
     * object is already in the list), then it will be left as-is.
     * Registrations are not counted; a single call to {@link #unregister}
     * will remove a callback after any number calls to register it.
     *
     * @param callback The callback interface to be added to the list.  Must
     * not be null -- passing null here will cause a NullPointerException.
     * Most services will want to check for null before calling this with
     * an object given from a client, so that clients can't crash the
     * service with bad data.
     *
     * @param cookie Optional additional data to be associated with this
     * callback.
     *
     * @return Returns true if the callback was successfully added to the list.
     * Returns false if it was not added, either because {@link #kill} had
     * previously been called or the callback's process has gone away.
     *
     * @see #unregister
     * @see #kill
     * @see #onCallbackDied
     */
    CARAPI Register(
        /* [in] */ IInterface* callback,
        /* [in] */ IInterface* cookie,
        /* [out] */ Boolean* result);

    /**
     * Remove from the list a callback that was previously added with
     * {@link #register}.  This uses the
     * {@link IInterface#asBinder callback.asBinder()} object to correctly
     * find the previous registration.
     * Registrations are not counted; a single unregister call will remove
     * a callback after any number calls to {@link #register} for it.
     *
     * @param callback The callback to be removed from the list.  Passing
     * null here will cause a NullPointerException, so you will generally want
     * to check for null before calling.
     *
     * @return Returns true if the callback was found and unregistered.  Returns
     * false if the given callback was not found on the list.
     *
     * @see #register
     */
    CARAPI Unregister(
        /* [in] */ IInterface* callback,
        /* [out] */ Boolean* result);

    /**
     * Disable this callback list.  All registered callbacks are unregistered,
     * and the list is disabled so that future calls to {@link #register} will
     * fail.  This should be used when a Service is stopping, to prevent clients
     * from registering callbacks after it is stopped.
     *
     * @see #register
     */
    CARAPI Kill();

    /**
     * Old version of {@link #onCallbackDied(E, Object)} that
     * does not provide a cookie.
     */
    CARAPI OnCallbackDied(
        /* [in] */ IInterface* callback);

    /**
     * Called when the process hosting a callback in the list has gone away.
     * The default implementation calls {@link #onCallbackDied(E)}
     * for backwards compatibility.
     *
     * @param callback The callback whose process has died.  Note that, since
     * its process has died, you can not make any calls on to this interface.
     * You can, however, retrieve its IBinder and compare it with another
     * IBinder to see if it is the same object.
     * @param cookie The cookie object original provided to
     * {@link #register(E, Object)}.
     *
     * @see #register
     */
    CARAPI OnCallbackDied(
        /* [in] */ IInterface* callback,
        /* [in] */ IInterface* cookie);

    /**
     * Prepare to start making calls to the currently registered callbacks.
     * This creates a copy of the callback list, which you can retrieve items
     * from using {@link #getBroadcastItem}.  Note that only one broadcast can
     * be active at a time, so you must be sure to always call this from the
     * same thread (usually by scheduling with {@link Handler}) or
     * do your own synchronization.  You must call {@link #finishBroadcast}
     * when done.
     *
     * <p>A typical loop delivering a broadcast looks like this:
     *
     * <pre>
     * int i = callbacks.beginBroadcast();
     * while (i &gt; 0) {
     *     i--;
     *     try {
     *         callbacks.getBroadcastItem(i).somethingHappened();
     *     } catch (RemoteException e) {
     *         // The RemoteCallbackList will take care of removing
     *         // the dead object for us.
     *     }
     * }
     * callbacks.finishBroadcast();</pre>
     *
     * @return Returns the number of callbacks in the broadcast, to be used
     * with {@link #getBroadcastItem} to determine the range of indices you
     * can supply.
     *
     * @see #getBroadcastItem
     * @see #finishBroadcast
     */
    CARAPI BeginBroadcast(
        /* [out] */ Int32* number);

    /**
     * Retrieve an item in the active broadcast that was previously started
     * with {@link #beginBroadcast}.  This can <em>only</em> be called after
     * the broadcast is started, and its data is no longer valid after
     * calling {@link #finishBroadcast}.
     *
     * <p>Note that it is possible for the process of one of the returned
     * callbacks to go away before you call it, so you will need to catch
     * {@link RemoteException} when calling on to the returned object.
     * The callback list itself, however, will take care of unregistering
     * these objects once it detects that it is no longer valid, so you can
     * handle such an exception by simply ignoring it.
     *
     * @param index Which of the registered callbacks you would like to
     * retrieve.  Ranges from 0 to 1-{@link #beginBroadcast}.
     *
     * @return Returns the callback interface that you can call.  This will
     * always be non-null.
     *
     * @see #beginBroadcast
     */
    CARAPI GetBroadcastItem(
        /* [in] */ Int32 index,
        /* [out] */ IInterface** callback);

    /**
     * Retrieve the cookie associated with the item
     * returned by {@link #getBroadcastItem(int)}.
     *
     * @see #getBroadcastItem
     */
    CARAPI GetBroadcastCookie(
        /* [in] */ Int32 index,
        /* [out] */ IInterface** cookie);

    /**
     * Clean up the state of a broadcast previously initiated by calling
     * {@link #beginBroadcast}.  This must always be called when you are done
     * with a broadcast.
     *
     * @see #beginBroadcast
     */
    CARAPI FinishBroadcast();

     /**
     * Returns the number of registered callbacks. Note that the number of registered
     * callbacks may differ from the value returned by {@link #beginBroadcast()} since
     * the former returns the number of callbacks registered at the time of the call
     * and the second the number of callback to which the broadcast will be delivered.
     * <p>
     * This function is useful to decide whether to schedule a broadcast if this
     * requires doing some work which otherwise would not be performed.
     * </p>
     *
     * @return The size.
     */
    CARAPI GetRegisteredCallbackCount(
        /* [out] */ Int32* count);

public:
    /*package*/
    HashMap< AutoPtr<IBinder>, AutoPtr<Callback> > mCallbacks;
    Object mCallbacksLock;

private:
    AutoPtr< ArrayOf<IInterface*> > mActiveBroadcast;
    Int32 mBroadcastCount;
    Boolean mKilled;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_REMOTECALLBACKLIST_H__
