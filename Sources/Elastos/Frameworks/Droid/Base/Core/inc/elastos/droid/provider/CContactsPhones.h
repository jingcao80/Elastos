#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSPHONES_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSPHONES_H__

#include "_Elastos_Droid_Provider_CContactsPhones.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsPhones)
    , public Singleton
    , public IContactsPhones
    , public IBaseColumns
    , public IContactsPhonesColumns
    , public IContactsPeopleColumns
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * The content:// style URL for this table
     * @deprecated see {@link android.provider.ContactsContract}
     */
    //@Deprecated
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * The content:// style URL for filtering phone numbers
     * @deprecated see {@link android.provider.ContactsContract}
     */
    //@Deprecated
    CARAPI GetCONTENT_FILTER_URL(
        /* [out] */ IUri** uri);

    /**
     * @deprecated see {@link android.provider.ContactsContract}
     */
    // @Deprecated
    CARAPI GetDisplayLabel(
        /* [in] */ IContext* context,
        /* [in] */ Int32 type,
        /* [in] */ ICharSequence* sequence,
        /* [in] */ ArrayOf<ICharSequence*>* labelArray,
        /* [out] */ ICharSequence** label);

    /**
     * @deprecated see {@link android.provider.ContactsContract}
     */
    // @Deprecated
    CARAPI GetDisplayLabel(
        /* [in] */ IContext* context,
        /* [in] */ Int32 type,
        /* [in] */ ICharSequence* sequence,
        /* [out] */ ICharSequence** label);
};

} // Provider
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSPHONES_H__
