//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_PROVIDER_CCALENDARCONTRACTCANLENDARENTITY_H__
#define __ELASTOS_DROID_PROVIDER_CCALENDARCONTRACTCANLENDARENTITY_H__

#include "_Elastos_Droid_Provider_CCalendarContractEventsEntity.h"
#include "elastos/droid/content/CursorEntityIterator.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IEntity;
using Elastos::Droid::Content::CursorEntityIterator;
using Elastos::Droid::Content::IEntityIterator;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentProviderClient;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CCalendarContractEventsEntity)
    , public Singleton
    , public ICalendarContractEventsEntity
    , public IBaseColumns
    , public ICalendarContractSyncColumns
    , public ICalendarContractEventsColumns
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
            /* [in] */ ICursor* cursor,
            /* [in] */ IContentResolver* resolver);

        CARAPI constructor(
            /* [in] */ ICursor* cursor,
            /* [in] */ IContentProviderClient* provider);

        // @Override
        CARAPI GetEntityAndIncrementCursor(
            /* [in] */ ICursor* cursor,
            /* [out] */ IEntity** entity);

    private:
        AutoPtr<IContentResolver> mResolver;
        AutoPtr<IContentProviderClient> mProvider;

        static AutoPtr<ArrayOf<String> > REMINDERS_PROJECTION;
        static Int32 COLUMN_MINUTES;
        static Int32 COLUMN_METHOD;

        static AutoPtr<ArrayOf<String> > ATTENDEES_PROJECTION;
        static Int32 COLUMN_ATTENDEE_NAME;
        static Int32 COLUMN_ATTENDEE_EMAIL;
        static Int32 COLUMN_ATTENDEE_RELATIONSHIP;
        static Int32 COLUMN_ATTENDEE_TYPE;
        static Int32 COLUMN_ATTENDEE_STATUS;
        static Int32 COLUMN_ATTENDEE_IDENTITY;
        static Int32 COLUMN_ATTENDEE_ID_NAMESPACE;

        static AutoPtr<ArrayOf<String> > EXTENDED_PROJECTION;
        static Int32 COLUMN_ID;
        static Int32 COLUMN_NAME;
        static Int32 COLUMN_VALUE;
        static String WHERE_EVENT_ID;
    };

public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * The content:// style URL for this table
     */
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * Creates a new iterator for events
     *
     * @param cursor An event query
     * @param resolver For performing additional queries
     * @return an EntityIterator containing one entity per event in the
     *         cursor
     */
    CARAPI NewEntityIterator(
        /* [in] */ ICursor* cursor,
        /* [in] */ IContentResolver* resolver,
        /* [out] */ IEntityIterator** iterator);

    /**
     * Creates a new iterator for events
     *
     * @param cursor An event query
     * @param provider For performing additional queries
     * @return an EntityIterator containing one entity per event in the
     *         cursor
     */
    CARAPI NewEntityIterator(
        /* [in] */ ICursor* cursor,
        /* [in] */ IContentProviderClient* provider,
        /* [out] */ IEntityIterator** iterator);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCALENDARCONTRACTCANLENDARENTITY_H__
