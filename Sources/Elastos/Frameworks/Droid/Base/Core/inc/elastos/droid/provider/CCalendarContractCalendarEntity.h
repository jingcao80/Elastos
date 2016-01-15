#ifndef __ELASTOS_DROID_PROVIDER_CCALENDARCONTRACTCANLENDARENTITY_H__
#define __ELASTOS_DROID_PROVIDER_CCALENDARCONTRACTCANLENDARENTITY_H__

#include "_Elastos_Droid_Provider_CCalendarContractCalendarEntity.h"
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

/**
 * Class that represents a Calendar Entity. There is one entry per calendar.
 * This is a helper class to make batch operations easier.
 */
CarClass(CCalendarContractCalendarEntity)
    , public Singleton
    , public ICalendarContractCalendarEntity
    , public IBaseColumns
    , public ICalendarContractSyncColumns
    , public ICalendarContractCalendarColumns
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
     * The default Uri used when creating a new calendar EntityIterator.
     */
    //@SuppressWarnings("hiding")
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * Creates an entity iterator for the given cursor. It assumes the
     * cursor contains a calendars query.
     *
     * @param cursor query on {@link #CONTENT_URI}
     * @return an EntityIterator of calendars
     */
    CARAPI NewEntityIterator(
        /* [in] */ ICursor* cursor,
        /* [out] */ IEntityIterator** iter);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCALENDARCONTRACTCANLENDARENTITY_H__
