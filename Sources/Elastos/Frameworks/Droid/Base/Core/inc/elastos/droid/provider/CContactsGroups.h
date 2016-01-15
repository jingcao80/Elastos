#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSGROUPS_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSGROUPS_H__

#include "_Elastos_Droid_Provider_CContactsGroups.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsGroups)
    , public Singleton
    , public IContactsGroups
    , public IBaseColumns
    , public ISyncConstValue
    , public IContactsGroupsColumns
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * no public constructor since this is a utility class
     */
    CARAPI constructor();

    /**
     * The content:// style URL for this table
     * @deprecated see {@link android.provider.ContactsContract}
     */
    //@Deprecated
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * The content:// style URL for the table that holds the deleted
     * groups.
     * @deprecated see {@link android.provider.ContactsContract}
     */
    //@Deprecated
    CARAPI GetDELETED_CONTENT_URI(
        /* [out] */ IUri** uri);
};

} // Provider
} // Droid
} // Elastos

#endif //__CCONTACTSPEOPLE_H__
