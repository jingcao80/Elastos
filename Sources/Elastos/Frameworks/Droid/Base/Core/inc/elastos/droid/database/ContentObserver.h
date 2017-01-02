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

#ifndef __ELASTOS_DROID_DATABASE_CONTENTOBSERVER_H__
#define __ELASTOS_DROID_DATABASE_CONTENTOBSERVER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"
#include <elastos/core/Runnable.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IHandler;
using Elastos::Core::Runnable;
using Elastos::Droid::Database::IContentObserver;

namespace Elastos {
namespace Droid {
namespace Database {
/**
 * Receives call backs for changes to content. Must be implemented by objects which are added
 * to a {@link ContentObservable}.
 */
class ECO_PUBLIC ContentObserver
    : public Object
    , public IContentObserver
{
private:
    class ECO_LOCAL NotificationRunnable
        : public Runnable
    {
    public:
        NotificationRunnable(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri,
            /* [in] */ Int32 userId,
            /* [in] */ ContentObserver* owner);

        CARAPI Run();

    private:
        Boolean mSelfChange;
        AutoPtr<IUri> mUri;
        Int32 mUserId;
        ContentObserver* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    TO_STRING_IMPL("ContentObserver")

    ContentObserver();

    virtual ~ContentObserver();

    CARAPI constructor();

    /**
     * Creates a content observer.
     *
     * @param handler The handler to run {@link #onChange} on, or null if none.
     */
    CARAPI constructor(
        /* [in]  */ IHandler* handler);

    /**
     * Gets access to the binder transport object. Not for public consumption.
     *
     * {@hide}
     */
    virtual CARAPI GetContentObserver(
        /* [out] */ IIContentObserver** contentObserver);

    /**
     * Gets access to the binder transport object, and unlinks the transport object
     * from the ContentObserver. Not for public consumption.
     *
     * {@hide}
     */
    virtual CARAPI ReleaseContentObserver(
        /* [out] */ IIContentObserver** contentObserver);

    /**
     * Returns true if this observer is interested receiving self-change notifications.
     *
     * Subclasses should override this method to indicate whether the observer
     * is interested in receiving notifications for changes that it made to the
     * content itself.
     *
     * @return True if self-change notifications should be delivered to the observer.
     */
    virtual CARAPI DeliverSelfNotifications(
        /* [out] */ Boolean* res);

    /**
     * This method is called when a content change occurs.
     * <p>
     * Subclasses should override this method to handle content changes.
     * </p>
     *
     * @param selfChange True if this is a self-change notification.
     */
    virtual CARAPI OnChange(
        /* [in] */ Boolean selfChange);

    /**
     * This method is called when a content change occurs.
     * Includes the changed content Uri when available.
     * <p>
     * Subclasses should override this method to handle content changes.
     * To ensure correct operation on older versions of the framework that
     * did not provide a Uri argument, applications should also implement
     * the {@link #onChange(boolean)} overload of this method whenever they
     * implement the {@link #onChange(boolean, Uri)} overload.
     * </p><p>
     * Example implementation:
     * <pre><code>
     * // Implement the onChange(boolean) method to delegate the change notification to
     * // the onChange(boolean, Uri) method to ensure correct operation on older versions
     * // of the framework that did not have the onChange(boolean, Uri) method.
     * {@literal @Override}
     * public void onChange(boolean selfChange) {
     *     onChange(selfChange, null);
     * }
     *
     * // Implement the onChange(boolean, Uri) method to take advantage of the new Uri argument.
     * {@literal @Override}
     * public void onChange(boolean selfChange, Uri uri) {
     *     // Handle change.
     * }
     * </code></pre>
     * </p>
     *
     * @param selfChange True if this is a self-change notification.
     * @param uri The Uri of the changed content, or null if unknown.
     */
    virtual CARAPI OnChange(
        /* [in] */ Boolean selfChange,
        /* [in] */ IUri* uri);

    /**
     * Dispatches a change notification to the observer. Includes the changed
     * content Uri when available and also the user whose content changed.
     *
     * @param selfChange True if this is a self-change notification.
     * @param uri The Uri of the changed content, or null if unknown.
     * @param userId The user whose content changed. Can be either a specific
     *         user or {@link UserHandle#USER_ALL}.
     *
     * @hide
     */
    virtual CARAPI OnChange(
        /* [in] */ Boolean selfChange,
        /* [in] */ IUri* uri,
        /* [in] */ Int32 userId);

    /**
     * Dispatches a change notification to the observer.
     * <p>
     * If a {@link Handler} was supplied to the {@link ContentObserver} constructor,
     * then a call to the {@link #onChange} method is posted to the handler's message queue.
     * Otherwise, the {@link #onChange} method is invoked immediately on this thread.
     * </p>
     *
     * @param selfChange True if this is a self-change notification.
     *
     * @deprecated Use {@link #dispatchChange(boolean, Uri)} instead.
     */
    //@Deprecated
    virtual CARAPI DispatchChange(
        /* [in] */ Boolean selfChange);

    /**
     * Dispatches a change notification to the observer.
     * Includes the changed content Uri when available.
     * <p>
     * If a {@link Handler} was supplied to the {@link ContentObserver} constructor,
     * then a call to the {@link #onChange} method is posted to the handler's message queue.
     * Otherwise, the {@link #onChange} method is invoked immediately on this thread.
     * </p>
     *
     * @param selfChange True if this is a self-change notification.
     * @param uri The Uri of the changed content, or null if unknown.
     */
    virtual CARAPI DispatchChange(
        /* [in] */ Boolean selfChange,
        /* [in] */ IUri* uri);

    /**
     * Dispatches a change notification to the observer. Includes the changed
     * content Uri when available and also the user whose content changed.
     * <p>
     * If a {@link Handler} was supplied to the {@link ContentObserver} constructor,
     * then a call to the {@link #onChange} method is posted to the handler's message queue.
     * Otherwise, the {@link #onChange} method is invoked immediately on this thread.
     * </p>
     *
     * @param selfChange True if this is a self-change notification.
     * @param uri The Uri of the changed content, or null if unknown.
     * @param userId The user whose content changed.
     */
    virtual CARAPI DispatchChange(
        /* [in] */ Boolean selfChange,
        /* [in] */ IUri* uri,
        /* [in] */ Int32 userId);

protected:
    Object mLock;
    AutoPtr<IContentObserverTransport> mTransport;
    AutoPtr<IHandler> mHandler;
};

} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_CONTENTOBSERVER_H__
