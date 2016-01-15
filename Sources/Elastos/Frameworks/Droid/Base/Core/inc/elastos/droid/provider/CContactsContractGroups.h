#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTGROUPS_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTGROUPS_H__

#include "_Elastos_Droid_Provider_CContactsContractGroups.h"
#include "elastos/droid/content/CursorEntityIterator.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::CursorEntityIterator;
using Elastos::Droid::Content::IEntity;
using Elastos::Droid::Content::IEntityIterator;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsContractGroups)
    , public Singleton
    , public IContactsContractGroups
    , public IBaseColumns
    , public IContactsContractGroupsColumns
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
    };

public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * The content:// style URI for this table
     */
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * The content:// style URI for this table joined with details data from
     * {@link ContactsContract.Data}.
     */
    CARAPI GetCONTENTSUMMARYURI(
        /* [out] */ IUri** uri);

    CARAPI NewEntityIterator(
        /* [in] */ ICursor* cursor,
        /* [out] */ IEntityIterator** iterator);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTGROUPS_H__
