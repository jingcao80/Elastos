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

#ifndef __ELASTOS_DROID_DATABASE_CURSORTOBULKCURSORADAPTOR_H__
#define __ELASTOS_DROID_DATABASE_CURSORTOBULKCURSORADAPTOR_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "_Elastos_Droid_Database_CCursorToBulkCursorAdaptor.h"
#include "elastos/droid/database/BulkCursorNative.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/database/CBulkCursorDescriptor.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Database {

/**
 * Wraps a BulkCursor around an existing Cursor making it remotable.
 *
 * {@hide}
 */
CarClass(CCursorToBulkCursorAdaptor)
    , public BulkCursorNative
    , public ICursorToBulkCursorAdaptor
    , public IProxyDeathRecipient
{
private:
    class ContentObserverProxy : public ContentObserver
    {
    public:
        TO_STRING_IMPL("ContentObserverProxy")

        ContentObserverProxy();

        CARAPI constructor(
            /* [in] */ IIContentObserver* remoteObserver,
            /* [in] */ IProxyDeathRecipient* recipient);

        CARAPI UnlinkToDeath(
            /* [in] */ IProxyDeathRecipient* recipient,
            /* [out] */ Boolean* result);

        CARAPI DeliverSelfNotifications(
            /* [out] */ Boolean* result);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

    protected:
        AutoPtr<IIContentObserver> mRemote;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ ICursor* cursor,
        /* [in] */ IIContentObserver* observer,
        /* [in] */ const String& providerName);

    /**
     * Returns an object that contains sufficient metadata to reconstruct
     * the cursor remotely.  May throw if an error occurs when executing the query
     * and obtaining the row count.
     */
    CARAPI GetBulkCursorDescriptor(
        /* [out] */ IBulkCursorDescriptor** result);

    CARAPI GetWindow(
        /* [in] */ Int32 position,
        /* [out] */ ICursorWindow** result);

    CARAPI OnMove(
        /* [in] */ Int32 position);

    CARAPI Deactivate();

    CARAPI Close();

    CARAPI Requery(
        /* [in] */ IIContentObserver* observer,
        /* [out] */ Int32* result);

    CARAPI GetExtras(
        /* [out] */ IBundle** result);

    CARAPI Respond(
        /* [in] */ IBundle* extras,
        /* [out] */ IBundle** result);

    CARAPI ProxyDied();

    TO_STRING_IMPL("CCursorToBulkCursorAdaptor")

private:
    CARAPI_(void) CloseFilledWindowLocked();

    CARAPI_(void) DisposeLocked();

    CARAPI ThrowIfCursorIsClosed();

    /**
     * Create a ContentObserver from the observer and register it as an observer on the
     * underlying cursor.
     * @param observer the IContentObserver that wants to monitor the cursor
     * @throws IllegalStateException if an observer is already registered
     */
    CARAPI CreateAndRegisterObserverProxyLocked(
        /* [in] */ IIContentObserver* observer);

    /** Unregister the observer if it is already registered. */
    CARAPI UnregisterObserverProxyLocked();

private:
    static const String TAG;

    Object mLock;
    String mProviderName;
    AutoPtr<ContentObserverProxy> mObserver;

    /**
     * The cursor that is being adapted.
     * This field is set to null when the cursor is closed.
     */
    AutoPtr<ICrossProcessCursor> mCursor;

    /**
     * The cursor window that was filled by the cross process cursor in the
     * case where the cursor does not support getWindow.
     * This field is only ever non-null when the window has actually be filled.
     */
    AutoPtr<ICursorWindow> mFilledWindow;
};

} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_CURSORTOBULKCURSORADAPTOR_H__
