#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTRAWCONTACTS_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTRAWCONTACTS_H__

#include "_Elastos_Droid_Provider_CContactsContractRawContacts.h"
#include "elastos/droid/content/CursorEntityIterator.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/net/Uri.h"
#include <elastos/core/Singleton.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::CursorEntityIterator;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::ICursorEntityIterator;
using Elastos::Droid::Content::IEntity;
using Elastos::Droid::Content::IEntityIterator;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsContractRawContacts)
    , public Singleton
    , public IContactsContractRawContacts
    , public IBaseColumns
    , public IContactsContractRawContactsColumns
    , public IContactsContractContactOptionsColumns
    , public IContactsContractContactNameColumns
    , public IContactsContractSyncColumns
{
private:
    class EntityIteratorImpl
        : public CursorEntityIterator
    {
    public:
        CAR_INTERFACE_DECL()

        EntityIteratorImpl();

        ~EntityIteratorImpl();

        CARAPI constructor(
            /* [in] */ ICursor* cursor);

        // @Override
        CARAPI GetEntityAndIncrementCursor(
            /* [in] */ ICursor* cursor,
            /* [out] */ IEntity** entity);

        //override
        CARAPI HasNext(
            /* [out] */ Boolean* result);

        CARAPI GetNext(
            /* [out] */ IInterface** object);

        CARAPI Remove();

        CARAPI Reset();

        CARAPI Close();

    private:
        static const AutoPtr<ArrayOf<String> > DATA_KEYS;
    };

public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * The content:// style URI for this table, which requests a directory of
     * raw contact rows matching the selection criteria.
     */
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * Build a {@link android.provider.ContactsContract.Contacts#CONTENT_LOOKUP_URI}
     * style {@link Uri} for the parent {@link android.provider.ContactsContract.Contacts}
     * entry of the given {@link RawContacts} entry.
     */
    CARAPI GetContactLookupUri(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* rawContactUri,
        /* [out] */ IUri** uri);

    /**
     * TODO: javadoc
     * @param cursor
     * @return
     */
    CARAPI NewEntityIterator(
        /* [in] */ ICursor* cursor,
        /* [out] */ IEntityIterator** iterator);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTRAWCONTACTS_H__
