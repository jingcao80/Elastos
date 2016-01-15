#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTSTATUSUPDATES_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTSTATUSUPDATES_H__

#include "_Elastos_Droid_Provider_CContactsContractStatusUpdates.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsContractStatusUpdates)
    , public Singleton
    , public IContactsContractStatusUpdates
    , public IContactsContractStatusColumns
    , public IContactsContractPresenceColumns
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
     * The content:// style URI for this table, specific to the user's profile.
     */
    CARAPI GetPROFILE_CONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * Gets the resource ID for the proper presence icon.
     *
     * @param status the status to get the icon for
     * @return the resource ID for the proper presence icon
     */
    CARAPI GetPresenceIconResourceId(
        /* [in] */ Int32 status,
        /* [out] */ Int32* id);

    /**
     * Returns the precedence of the status code the higher number being the higher precedence.
     *
     * @param status The status code.
     * @return An integer representing the precedence, 0 being the lowest.
     */
    CARAPI GetPresencePrecedence(
        /* [in] */ Int32 status,
        /* [out] */ Int32* precedence);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTSTATUSUPDATES_H__
