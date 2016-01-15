#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACT_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACT_H__

#include "_Elastos_Droid_Provider_CContactsContract.h"
#include "elastos/droid/provider/ContactsContract.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsContract)
    , public Singleton
    , public IContactsContract
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /** A content:// style uri to the authority for the contacts provider */
    CARAPI GetAUTHORITY_URI(
        /* [out] */ IUri** uri);

    /**
     * This method can be used to identify whether the given ID is associated with profile
     * data.  It does not necessarily indicate that the ID is tied to valid data, merely
     * that accessing data using this ID will result in profile access checks and will only
     * return data from the profile.
     *
     * @param id The ID to check.
     * @return Whether the ID is associated with profile data.
     */
    CARAPI IsProfileId(
        /* [in] */ Int64 id,
        /* [out] */ Boolean* res);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACT_H__
