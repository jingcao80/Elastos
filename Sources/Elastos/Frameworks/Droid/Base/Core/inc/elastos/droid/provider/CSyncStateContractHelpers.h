#ifndef __ELASTOS_DROID_PROVIDER_CSYNCSTATECONTRACTHELPERS_H__
#define __ELASTOS_DROID_PROVIDER_CSYNCSTATECONTRACTHELPERS_H__

#include "_Elastos_Droid_Provider_CSyncStateContractHelpers.h"
#include "elastos/droid/provider/SyncStateContractHelpers.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CSyncStateContractHelpers)
    , public Singleton
    , public ISyncStateContractHelpers
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Get the sync state that is associated with the account or null.
     * @param provider the {@link ContentProviderClient} that is to be used to communicate
     * with the {@link android.content.ContentProvider} that contains the sync state.
     * @param uri the uri of the sync state
     * @param account the {@link Account} whose sync state should be returned
     * @return the sync state or null if there is no sync state associated with the account
     * @throws RemoteException if there is a failure communicating with the remote
     * {@link android.content.ContentProvider}
     */
    CARAPI Get(
        /* [in] */ IContentProviderClient* provider,
        /* [in] */ IUri* uri,
        /* [in] */ IAccount* account,
        /* [out, callee] */ ArrayOf<Byte>** value);

    /**
     * Assigns the data array as the sync state for the given account.
     * @param provider the {@link ContentProviderClient} that is to be used to communicate
     * with the {@link android.content.ContentProvider} that contains the sync state.
     * @param uri the uri of the sync state
     * @param account the {@link Account} whose sync state should be set
     * @param data the byte[] that contains the sync state
     * @throws RemoteException if there is a failure communicating with the remote
     * {@link android.content.ContentProvider}
     */
    CARAPI Set(
        /* [in] */ IContentProviderClient* provider,
        /* [in] */ IUri* uri,
        /* [in] */ IAccount* account,
        /* [in] */ ArrayOf<Byte>* data);

    CARAPI Insert(
        /* [in] */ IContentProviderClient* provider,
        /* [in] */ IUri* uri,
        /* [in] */ IAccount* account,
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ IUri** retUri);

    CARAPI Update(
        /* [in] */ IContentProviderClient* provider,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<Byte>* data);

    CARAPI GetWithUri(
       /* [in] */ IContentProviderClient* provider,
       /* [in] */ IUri* uri,
       /* [in] */ IAccount* account,
       /* [out] */ IPair** value);

    /**
     * Creates and returns a ContentProviderOperation that assigns the data array as the
     * sync state for the given account.
     * @param uri the uri of the sync state
     * @param account the {@link Account} whose sync state should be set
     * @param data the byte[] that contains the sync state
     * @return the new ContentProviderOperation that assigns the data array as the
     * account's sync state
     */
    CARAPI NewSetOperation(
        /* [in] */ IUri* uri,
        /* [in] */ IAccount* account,
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ IContentProviderOperation** operation);

    /**
     * Creates and returns a ContentProviderOperation that assigns the data array as the
     * sync state for the given account.
     * @param uri the uri of the specific sync state to set
     * @param data the byte[] that contains the sync state
     * @return the new ContentProviderOperation that assigns the data array as the
     * account's sync state
     */
    CARAPI NewUpdateOperation(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ IContentProviderOperation** operation);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CSYNCSTATECONTRACTHELPERS_H__
