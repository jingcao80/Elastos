#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTSYNCSTATE_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTSYNCSTATE_H__

#include "_Elastos_Droid_Provider_CContactsContractSyncState.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Content::IContentProviderClient;
using Elastos::Droid::Content::IContentProviderOperation;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Utility::IPair;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsContractSyncState)
    , public Singleton
    , IContactsContractSyncState
    , ISyncStateContractColumns
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * The content:// style URI for this table
     */
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * @see android.provider.SyncStateContract.Helpers#get
     */
    CARAPI Get(
        /* [in] */ IContentProviderClient* provider,
        /* [in] */ IAccount* account,
        /* [out] */ ArrayOf<Byte>** value);

    CARAPI GetWithUri(
        /* [in] */ IContentProviderClient* provider,
        /* [in] */ IAccount* account,
        /* [out] */ IPair** value);

    /**
     * @see android.provider.SyncStateContract.Helpers#set
     */
    CARAPI Set(
        /* [in] */ IContentProviderClient* provider,
        /* [in] */ IAccount* account,
        /* [in] */ ArrayOf<Byte>* data);

    /**
     * @see android.provider.SyncStateContract.Helpers#newSetOperation
     */
    CARAPI NewSetOperation(
        /* [in] */ IAccount* account,
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ IContentProviderOperation** operation);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTSYNCSTATE_H__
