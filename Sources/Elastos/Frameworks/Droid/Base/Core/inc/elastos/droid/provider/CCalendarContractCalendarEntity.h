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
