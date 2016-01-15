#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSPRESENCE_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSPRESENCE_H__

#include "_Elastos_Droid_Provider_CContactsPresence.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Widget::IImageView;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsPresence)
    , public Singleton
    , public IContactsPresence
    , public IBaseColumns
    , public IContactsPresenceColumns
    , public IContactsPeopleColumns
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /**
     * The content:// style URL for this table
     * @deprecated see {@link android.provider.ContactsContract}
     */
    //@Deprecated
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * Gets the resource ID for the proper presence icon.
     *
     * @param status the status to get the icon for
     * @return the resource ID for the proper presence icon
     * @deprecated see {@link android.provider.ContactsContract}
     */
    // @Deprecated
    CARAPI GetPresenceIconResourceId(
        /* [in] */ Int32 status,
        /* [out] */ Int32* id);

    /**
     * Sets a presence icon to the proper graphic
     *
     * @param icon the icon to to set
     * @param serverStatus that status
     * @deprecated see {@link android.provider.ContactsContract}
     */
    // @Deprecated
    CARAPI SetPresenceIcon(
        /* [in] */ IImageView* icon,
        /* [in] */ Int32 serverStatus);
};

} // Provider
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSPRESENCE_H__
