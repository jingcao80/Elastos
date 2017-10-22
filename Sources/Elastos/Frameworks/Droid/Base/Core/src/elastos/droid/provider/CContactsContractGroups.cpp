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

#include "Elastos.Droid.Database.h"
#include "elastos/droid/provider/CContactsContractGroups.h"
#include "elastos/droid/provider/ContactsContract.h"
#include "elastos/droid/database/DatabaseUtils.h"
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/content/CEntity.h"
#include "elastos/droid/net/Uri.h"

using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::CEntity;
using Elastos::Droid::Content::EIID_ICursorEntityIterator;
using Elastos::Droid::Content::EIID_IEntityIterator;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::ICursorEntityIterator;
using Elastos::Droid::Content::IEntity;
using Elastos::Droid::Content::IEntityIterator;
using Elastos::Droid::Database::DatabaseUtils;
using Elastos::Droid::Net::Uri;
using Elastos::Utility::EIID_IIterator;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContractGroups)

CAR_INTERFACE_IMPL_4(CContactsContractGroups, Singleton
    , IContactsContractGroups
    , IBaseColumns
    , IContactsContractGroupsColumns
    , IContactsContractSyncColumns)

ECode CContactsContractGroups::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::WithAppendedPath(ContactsContract::AUTHORITY_URI.Get(), String("groups"), uri);
}

ECode CContactsContractGroups::GetCONTENTSUMMARYURI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::WithAppendedPath(ContactsContract::AUTHORITY_URI.Get(), String("groups_summary"), uri);
}

ECode CContactsContractGroups::NewEntityIterator(
    /* [in] */ ICursor* cursor,
    /* [out] */ IEntityIterator** iterator)
{
    VALIDATE_NOT_NULL(iterator);

    AutoPtr<EntityIteratorImpl> impl = new EntityIteratorImpl();
    impl->constructor(cursor);
    *iterator = (IEntityIterator*)impl.Get();
    REFCOUNT_ADD(*iterator);
    return NOERROR;
}

ECode CContactsContractGroups::EntityIteratorImpl::constructor(
    /* [in] */ ICursor* cursor)
{
    return CursorEntityIterator::constructor(cursor);
}

CAR_INTERFACE_IMPL_3(CContactsContractGroups::EntityIteratorImpl, Object, ICursorEntityIterator, IEntityIterator, IIterator)


CContactsContractGroups::EntityIteratorImpl::EntityIteratorImpl()
{
}

CContactsContractGroups::EntityIteratorImpl::~EntityIteratorImpl()
{
}

ECode CContactsContractGroups::EntityIteratorImpl::GetEntityAndIncrementCursor(
    /* [in] */ ICursor* cursor,
    /* [out] */ IEntity** entity)
{
    VALIDATE_NOT_NULL(entity);

    // we expect the cursor is already at the row we need to read from
    AutoPtr<IContentValues> values;
    FAIL_RETURN(CContentValues::New((IContentValues**)&values))

    DatabaseUtils::CursorInt64ToContentValuesIfPresent(cursor, values, IBaseColumns::ID);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, values, ACCOUNT_NAME);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, values, ACCOUNT_TYPE);
    DatabaseUtils::CursorInt64ToContentValuesIfPresent(cursor, values, DIRTY);
    DatabaseUtils::CursorInt64ToContentValuesIfPresent(cursor, values, VERSION);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, values, SOURCE_ID);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, values, RES_PACKAGE);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, values, TITLE);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, values, TITLE_RES);
    DatabaseUtils::CursorInt64ToContentValuesIfPresent(cursor, values, GROUP_VISIBLE);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, values, IContactsContractBaseSyncColumns::SYNC1);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, values, IContactsContractBaseSyncColumns::SYNC2);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, values, IContactsContractBaseSyncColumns::SYNC3);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, values, IContactsContractBaseSyncColumns::SYNC4);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, values, SYSTEM_ID);
    DatabaseUtils::CursorInt64ToContentValuesIfPresent(cursor, values, DELETED);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, values, NOTES);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, values, SHOULD_SYNC);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, values, FAVORITES);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, values, AUTO_ADD);
    Boolean result;
    FAIL_RETURN(cursor->MoveToNext(&result))
    return CEntity::New(values, entity);
}

} //Provider
} //Droid
} //Elastos