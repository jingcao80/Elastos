#ifndef __ELASTOS_DROID_PROVIDER_SYNCSTATECONTRACTHELPERS_H__
#define __ELASTOS_DROID_PROVIDER_SYNCSTATECONTRACTHELPERS_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Content::IContentProviderClient;
using Elastos::Droid::Content::IContentProviderOperation;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Utility::IPair;

namespace Elastos {
namespace Droid {
namespace Provider {

class SyncStateContractHelpers
{
public:
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
    static CARAPI Get(
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
    static CARAPI Set(
        /* [in] */ IContentProviderClient* provider,
        /* [in] */ IUri* uri,
        /* [in] */ IAccount* account,
        /* [in] */ ArrayOf<Byte>* data);

    static CARAPI Insert(
        /* [in] */ IContentProviderClient* provider,
        /* [in] */ IUri* uri,
        /* [in] */ IAccount* account,
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ IUri** retUri);

    static CARAPI Update(
        /* [in] */ IContentProviderClient* provider,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<Byte>* data);

    static CARAPI GetWithUri(
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
    static CARAPI NewSetOperation(
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
    static CARAPI NewUpdateOperation(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ IContentProviderOperation** operation);

public:
    /**
     * The content:// style URI for this table
     */
    static const AutoPtr<IUri> CONTENT_URI;

private:
    static AutoPtr<ArrayOf<String> > DATA_PROJECTION;
    static const String SELECT_BY_ACCOUNT;
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_SYNCSTATECONTRACTHELPERS_H__
