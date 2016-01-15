#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTDELETEDCONTACTS_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTDELETEDCONTACTS_H__

#include "_Elastos_Droid_Provider_CContactsContractDeletedContacts.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsContractDeletedContacts)
    , public Singleton
    , public IContactsContractDeletedContacts
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    CARAPI GetDAYS_KEPT_MILLISECONDS(
        /* [out] */ Int64* result);

private:
    /**
     * Number of days that the delete log will be kept.  After this time, delete records may be
     * deleted.
     *
     * @hide
     */
    static const Int32 DAYS_KEPT;
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTDELETEDCONTACTS_H__