
#ifndef __ELASTOS_DROID_DATABASE_CONTENTOBSERVABLE_H__
#define __ELASTOS_DROID_DATABASE_CONTENTOBSERVABLE_H__

#include "elastos/droid/database/Observable.h"

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Database {
/**
 * A specialization of {@link Observable} for {@link ContentObserver}
 * that provides methods for sending notifications to a list of
 * {@link ContentObserver} objects.
 */
class ContentObservable : public Observable
{
public:
    // Even though the generic method defined in Observable would be perfectly
    // fine on its own, we can't delete this overridden method because it would
    // potentially break binary compatibility with existing applications.
    //@Override
    virtual CARAPI RegisterObserver(
        /* [in] */ IInterface* observer);

   /**
     * Invokes {@link ContentObserver#dispatchChange(boolean)} on each observer.
     * <p>
     * If <code>selfChange</code> is true, only delivers the notification
     * to the observer if it has indicated that it wants to receive self-change
     * notifications by implementing {@link ContentObserver#deliverSelfNotifications}
     * to return true.
     * </p>
     *
     * @param selfChange True if this is a self-change notification.
     *
     * @deprecated Use {@link #dispatchChange(boolean, Uri)} instead.
     */
    virtual CARAPI DispatchChange(
        /* [in] */ Boolean selfChange);

    /**
     * Invokes {@link ContentObserver#dispatchChange(boolean, Uri)} on each observer.
     * Includes the changed content Uri when available.
     * <p>
     * If <code>selfChange</code> is true, only delivers the notification
     * to the observer if it has indicated that it wants to receive self-change
     * notifications by implementing {@link ContentObserver#deliverSelfNotifications}
     * to return true.
     * </p>
     *
     * @param selfChange True if this is a self-change notification.
     * @param uri The Uri of the changed content, or null if unknown.
     */
    virtual CARAPI DispatchChange(
        /* [in] */ Boolean selfChange,
        /* [in] */ IUri* uri);

    /**
     * Invokes {@link ContentObserver#onChange} on each observer.
     *
     * @param selfChange True if this is a self-change notification.
     *
     * @deprecated Use {@link #dispatchChange} instead.
     */
    //@Deprecated
    virtual CARAPI NotifyChange(
        /* [in] */ Boolean selfChange);
};

} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_CONTENTOBSERVABLE_H__
