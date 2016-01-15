#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTS_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTS_H__

#include "_Elastos_Droid_Provider_CContacts.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContacts)
    , public Singleton
    , public IContacts
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * no public constructor since this is a utility class
     */
    CARAPI constructor();

    /**
     * The content:// style URL for this provider
     * @deprecated see {@link android.provider.ContactsContract}
     */
    //@Deprecated
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

private:
    static const String TAG;
};

} // Provider
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTS_H__
