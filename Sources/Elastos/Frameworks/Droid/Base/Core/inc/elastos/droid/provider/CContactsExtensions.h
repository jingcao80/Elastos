#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSEXTENSIONS_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSEXTENSIONS_H__

#include "_Elastos_Droid_Provider_CContactsExtensions.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsExtensions)
    , public Singleton
    , public IContactsExtensions
    , public IBaseColumns
    , public IContactsExtensionsColumns
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
};

} // Provider
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSEXTENSIONS_H__
