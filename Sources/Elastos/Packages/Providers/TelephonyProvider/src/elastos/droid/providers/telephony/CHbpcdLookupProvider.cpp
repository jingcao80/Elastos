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

#include "elastos/droid/providers/telephony/CHbpcdLookupProvider.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CUriMatcher;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::Sqlite::CSQLiteQueryBuilder;
using Elastos::Droid::Database::Sqlite::ISQLiteQueryBuilder;
using Elastos::Droid::Internal::Telephony::IHbpcdLookup;
using Elastos::Droid::Internal::Telephony::IHbpcdLookupArbitraryMccSidMatch;
using Elastos::Droid::Internal::Telephony::IHbpcdLookupMccIdd;
using Elastos::Droid::Internal::Telephony::IHbpcdLookupMccLookup;
using Elastos::Droid::Internal::Telephony::IHbpcdLookupMccSidConflicts;
using Elastos::Droid::Internal::Telephony::IHbpcdLookupMccSidRange;
using Elastos::Droid::Internal::Telephony::IHbpcdLookupNanpAreaCode;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CoreUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Telephony {

static AutoPtr<IUriMatcher> InitUriMatcher()
{
    AutoPtr<IUriMatcher> matcher;
    CUriMatcher::New(IUriMatcher::NO_MATCH, (IUriMatcher**)&matcher);
    return matcher;
}

const String CHbpcdLookupProvider::TAG("HbpcdLookupProvider");
const Boolean CHbpcdLookupProvider::DBG = TRUE;

const Int32 CHbpcdLookupProvider::MCC_IDD;
const Int32 CHbpcdLookupProvider::MCC_LOOKUP_TABLE;
const Int32 CHbpcdLookupProvider::MCC_SID_CONFLICT;
const Int32 CHbpcdLookupProvider::MCC_SID_RANGE;
const Int32 CHbpcdLookupProvider::NANP_AREA_CODE;
const Int32 CHbpcdLookupProvider::ARBITRARY_MCC_SID_MATCH;
const Int32 CHbpcdLookupProvider::MCC_IDD_ID;
const Int32 CHbpcdLookupProvider::MCC_LOOKUP_TABLE_ID;
const Int32 CHbpcdLookupProvider::MCC_SID_CONFLICT_ID;
const Int32 CHbpcdLookupProvider::MCC_SID_RANGE_ID;
const Int32 CHbpcdLookupProvider::NANP_AREA_CODE_ID;
const Int32 CHbpcdLookupProvider::ARBITRARY_MCC_SID_MATCH_ID;

AutoPtr<IHashMap> CHbpcdLookupProvider::sIddProjectionMap;
AutoPtr<IHashMap> CHbpcdLookupProvider::sLookupProjectionMap;
AutoPtr<IHashMap> CHbpcdLookupProvider::sConflictProjectionMap;
AutoPtr<IHashMap> CHbpcdLookupProvider::sRangeProjectionMap;
AutoPtr<IHashMap> CHbpcdLookupProvider::sNanpProjectionMap;
AutoPtr<IHashMap> CHbpcdLookupProvider::sArbitraryProjectionMap;

AutoPtr<IUriMatcher> CHbpcdLookupProvider::sURIMatcher = InitUriMatcher();

Boolean CHbpcdLookupProvider::InitMember()
{
    assert(sURIMatcher != NULL);
    sURIMatcher->AddURI(IHbpcdLookup::AUTHORITY, IHbpcdLookup::PATH_MCC_IDD, MCC_IDD);
    sURIMatcher->AddURI(IHbpcdLookup::AUTHORITY,
            IHbpcdLookup::PATH_MCC_LOOKUP_TABLE, MCC_LOOKUP_TABLE);
    // following URI is a joint table of MCC_LOOKUP_TABLE and MCC_SID_CONFLIct.
    sURIMatcher->AddURI(IHbpcdLookup::AUTHORITY,
            IHbpcdLookup::PATH_MCC_SID_CONFLICT, MCC_SID_CONFLICT);
    sURIMatcher->AddURI(IHbpcdLookup::AUTHORITY, IHbpcdLookup::PATH_MCC_SID_RANGE, MCC_SID_RANGE);
    sURIMatcher->AddURI(IHbpcdLookup::AUTHORITY, IHbpcdLookup::PATH_NANP_AREA_CODE, NANP_AREA_CODE);
    sURIMatcher->AddURI(IHbpcdLookup::AUTHORITY,
            IHbpcdLookup::PATH_ARBITRARY_MCC_SID_MATCH, ARBITRARY_MCC_SID_MATCH);
    sURIMatcher->AddURI(IHbpcdLookup::AUTHORITY, IHbpcdLookup::PATH_MCC_IDD + "/#", MCC_IDD_ID);
    sURIMatcher->AddURI(IHbpcdLookup::AUTHORITY,
            IHbpcdLookup::PATH_MCC_LOOKUP_TABLE + "/#", MCC_LOOKUP_TABLE_ID);
    sURIMatcher->AddURI(IHbpcdLookup::AUTHORITY,
            IHbpcdLookup::PATH_MCC_SID_CONFLICT + "/#", MCC_SID_CONFLICT_ID);
    sURIMatcher->AddURI(IHbpcdLookup::AUTHORITY,
            IHbpcdLookup::PATH_MCC_SID_RANGE + "/#", MCC_SID_RANGE_ID);
    sURIMatcher->AddURI(IHbpcdLookup::AUTHORITY,
            IHbpcdLookup::PATH_NANP_AREA_CODE + "/#", NANP_AREA_CODE_ID);
    sURIMatcher->AddURI(IHbpcdLookup::AUTHORITY,
            IHbpcdLookup::PATH_ARBITRARY_MCC_SID_MATCH + "/#", ARBITRARY_MCC_SID_MATCH_ID);

    CHashMap::New((IHashMap**)&sIddProjectionMap);
    sIddProjectionMap->Put(CoreUtils::Convert(IHbpcdLookup::ID),
            CoreUtils::Convert(IHbpcdLookup::ID));
    sIddProjectionMap->Put(CoreUtils::Convert(IHbpcdLookupMccIdd::MCC),
            CoreUtils::Convert(IHbpcdLookupMccIdd::MCC));
    sIddProjectionMap->Put(CoreUtils::Convert(IHbpcdLookupMccIdd::IDD),
            CoreUtils::Convert(IHbpcdLookupMccIdd::IDD));

    CHashMap::New((IHashMap**)&sLookupProjectionMap);
    sLookupProjectionMap->Put(CoreUtils::Convert(IHbpcdLookup::ID),
            CoreUtils::Convert(IHbpcdLookup::ID));
    sLookupProjectionMap->Put(CoreUtils::Convert(IHbpcdLookupMccLookup::MCC),
            CoreUtils::Convert(IHbpcdLookupMccLookup::MCC));
    sLookupProjectionMap->Put(CoreUtils::Convert(IHbpcdLookupMccLookup::COUNTRY_CODE),
            CoreUtils::Convert(IHbpcdLookupMccLookup::COUNTRY_CODE));
    sLookupProjectionMap->Put(CoreUtils::Convert(IHbpcdLookupMccLookup::COUNTRY_NAME),
            CoreUtils::Convert(IHbpcdLookupMccLookup::COUNTRY_NAME));
    sLookupProjectionMap->Put(CoreUtils::Convert(IHbpcdLookupMccLookup::NDD),
            CoreUtils::Convert(IHbpcdLookupMccLookup::NDD));
    sLookupProjectionMap->Put(CoreUtils::Convert(IHbpcdLookupMccLookup::NANPS),
            CoreUtils::Convert(IHbpcdLookupMccLookup::NANPS));
    sLookupProjectionMap->Put(CoreUtils::Convert(IHbpcdLookupMccLookup::GMT_OFFSET_LOW),
            CoreUtils::Convert(IHbpcdLookupMccLookup::GMT_OFFSET_LOW));
    sLookupProjectionMap->Put(CoreUtils::Convert(IHbpcdLookupMccLookup::GMT_OFFSET_HIGH),
            CoreUtils::Convert(IHbpcdLookupMccLookup::GMT_OFFSET_HIGH));
    sLookupProjectionMap->Put(CoreUtils::Convert(IHbpcdLookupMccLookup::GMT_DST_LOW),
            CoreUtils::Convert(IHbpcdLookupMccLookup::GMT_DST_LOW));
    sLookupProjectionMap->Put(CoreUtils::Convert(IHbpcdLookupMccLookup::GMT_DST_HIGH),
            CoreUtils::Convert(IHbpcdLookupMccLookup::GMT_DST_HIGH));

    // when we do query, we will join it with MccLookup table
    CHashMap::New((IHashMap**)&sConflictProjectionMap);
    // MccLookup.MCC is duped to IHbpcdLookupMccSidConflicts::MCC
    sConflictProjectionMap->Put(CoreUtils::Convert(IHbpcdLookupMccLookup::GMT_OFFSET_LOW),
            CoreUtils::Convert(IHbpcdLookupProvider::TABLE_MCC_LOOKUP_TABLE + "." + IHbpcdLookupMccLookup::GMT_OFFSET_LOW));
    sConflictProjectionMap->Put(CoreUtils::Convert(IHbpcdLookupMccLookup::GMT_OFFSET_HIGH),
            CoreUtils::Convert(IHbpcdLookupProvider::TABLE_MCC_LOOKUP_TABLE + "." + IHbpcdLookupMccLookup::GMT_OFFSET_HIGH));
    sConflictProjectionMap->Put(CoreUtils::Convert(IHbpcdLookupMccLookup::GMT_DST_LOW),
            CoreUtils::Convert(IHbpcdLookupProvider::TABLE_MCC_LOOKUP_TABLE + "." + IHbpcdLookupMccLookup::GMT_DST_LOW));
    sConflictProjectionMap->Put(CoreUtils::Convert(IHbpcdLookupMccLookup::GMT_DST_HIGH),
            CoreUtils::Convert(IHbpcdLookupProvider::TABLE_MCC_LOOKUP_TABLE + "." + IHbpcdLookupMccLookup::GMT_DST_HIGH));
    sConflictProjectionMap->Put(CoreUtils::Convert(IHbpcdLookupMccSidConflicts::MCC),
            CoreUtils::Convert(IHbpcdLookupProvider::TABLE_MCC_SID_CONFLICT + "." + IHbpcdLookupMccSidConflicts::MCC));
    sConflictProjectionMap->Put(CoreUtils::Convert(IHbpcdLookupMccSidConflicts::SID_CONFLICT),
            CoreUtils::Convert(IHbpcdLookupProvider::TABLE_MCC_SID_CONFLICT + "." + IHbpcdLookupMccSidConflicts::SID_CONFLICT));

    CHashMap::New((IHashMap**)&sRangeProjectionMap);
    sRangeProjectionMap->Put(CoreUtils::Convert(IHbpcdLookup::ID),
            CoreUtils::Convert(IHbpcdLookup::ID));
    sRangeProjectionMap->Put(CoreUtils::Convert(IHbpcdLookupMccSidRange::MCC),
            CoreUtils::Convert(IHbpcdLookupMccSidRange::MCC));
    sRangeProjectionMap->Put(CoreUtils::Convert(IHbpcdLookupMccSidRange::RANGE_LOW),
            CoreUtils::Convert(IHbpcdLookupMccSidRange::RANGE_LOW));
    sRangeProjectionMap->Put(CoreUtils::Convert(IHbpcdLookupMccSidRange::RANGE_HIGH),
            CoreUtils::Convert(IHbpcdLookupMccSidRange::RANGE_HIGH));

    CHashMap::New((IHashMap**)&sNanpProjectionMap);
    sNanpProjectionMap->Put(CoreUtils::Convert(IHbpcdLookup::ID),
            CoreUtils::Convert(IHbpcdLookup::ID));
    sNanpProjectionMap->Put(CoreUtils::Convert(IHbpcdLookupNanpAreaCode::AREA_CODE),
            CoreUtils::Convert(IHbpcdLookupNanpAreaCode::AREA_CODE));

    CHashMap::New((IHashMap**)&sArbitraryProjectionMap);
    sArbitraryProjectionMap->Put(CoreUtils::Convert(IHbpcdLookup::ID),
            CoreUtils::Convert(IHbpcdLookup::ID));
    sArbitraryProjectionMap->Put(CoreUtils::Convert(IHbpcdLookupArbitraryMccSidMatch::MCC),
            CoreUtils::Convert(IHbpcdLookupArbitraryMccSidMatch::MCC));
    sArbitraryProjectionMap->Put(CoreUtils::Convert(IHbpcdLookupArbitraryMccSidMatch::SID),
            CoreUtils::Convert(IHbpcdLookupArbitraryMccSidMatch::SID));

    return TRUE;
}

Boolean CHbpcdLookupProvider::s_Init = CHbpcdLookupProvider::InitMember();

CAR_OBJECT_IMPL(CHbpcdLookupProvider)

ECode CHbpcdLookupProvider::constructor()
{
    return ContentProvider::constructor();
}

ECode CHbpcdLookupProvider::OnCreate(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)

    if (DBG) {
        Logger::D(TAG, "onCreate");
    }
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    CHbpcdLookupDatabaseHelper::NewByFriend(ctx, (CHbpcdLookupDatabaseHelper**)&mDbHelper);

    *succeeded = TRUE;
    return NOERROR;
}

// @Override
ECode CHbpcdLookupProvider::Query(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projectionIn,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder,
    /* [out] */ ICursor** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<ISQLiteQueryBuilder> qb;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&qb);
    String orderBy;
    String groupBy;
    Boolean useDefaultOrder = TextUtils::IsEmpty(sortOrder);

    Int32 match;
    sURIMatcher->Match(uri, &match);

    switch (match) {
        case MCC_IDD: {
            qb->SetTables(IHbpcdLookupProvider::TABLE_MCC_IDD);
            qb->SetProjectionMap(IMap::Probe(sIddProjectionMap));
            if (useDefaultOrder) {
                orderBy = IHbpcdLookupMccIdd::DEFAULT_SORT_ORDER;
            }
            break;
        }
        case MCC_LOOKUP_TABLE: {
            qb->SetTables(IHbpcdLookupProvider::TABLE_MCC_LOOKUP_TABLE);
            qb->SetProjectionMap(IMap::Probe(sLookupProjectionMap));
            if (useDefaultOrder) {
                orderBy = IHbpcdLookupMccLookup::DEFAULT_SORT_ORDER;
            }
            groupBy = IHbpcdLookupMccLookup::COUNTRY_NAME;
            break;
        }
        case MCC_SID_CONFLICT: {
            StringBuilder joinT;
            joinT.Append(IHbpcdLookupProvider::TABLE_MCC_LOOKUP_TABLE);
            joinT.Append(" INNER JOIN ");
            joinT.Append(IHbpcdLookupProvider::TABLE_MCC_SID_CONFLICT);
            joinT.Append(" ON (");
            joinT.Append(IHbpcdLookupProvider::TABLE_MCC_LOOKUP_TABLE); // table name
            joinT.Append(".");
            joinT.Append(IHbpcdLookupMccLookup::MCC); // column name
            joinT.Append(" = ");
            joinT.Append(IHbpcdLookupProvider::TABLE_MCC_SID_CONFLICT); // table name
            joinT.Append(".");
            joinT.Append(IHbpcdLookupMccSidConflicts::MCC); //column name
            joinT.Append(")");
            String str;
            joinT.ToString(&str);
            qb->SetTables(str);
            qb->SetProjectionMap(IMap::Probe(sConflictProjectionMap));
            break;
        }
        case MCC_SID_RANGE: {
            qb->SetTables(IHbpcdLookupProvider::TABLE_MCC_SID_RANGE);
            qb->SetProjectionMap(IMap::Probe(sRangeProjectionMap));
            if (useDefaultOrder) {
                orderBy = IHbpcdLookupMccIdd::DEFAULT_SORT_ORDER;
            }
            break;
        }
        case NANP_AREA_CODE: {
            qb->SetTables(IHbpcdLookupProvider::TABLE_NANP_AREA_CODE);
            qb->SetProjectionMap(IMap::Probe(sNanpProjectionMap));
            if (useDefaultOrder) {
                orderBy = IHbpcdLookupNanpAreaCode::DEFAULT_SORT_ORDER;
            }
            break;
        }
        case ARBITRARY_MCC_SID_MATCH: {
            qb->SetTables(IHbpcdLookupProvider::TABLE_ARBITRARY_MCC_SID_MATCH);
            qb->SetProjectionMap(IMap::Probe(sArbitraryProjectionMap));
            if (useDefaultOrder) {
                orderBy = IHbpcdLookupArbitraryMccSidMatch::DEFAULT_SORT_ORDER;
            }
            break;
        }
        case MCC_IDD_ID: {
            qb->SetTables(IHbpcdLookupProvider::TABLE_MCC_IDD);
            qb->SetProjectionMap(IMap::Probe(sIddProjectionMap));
            qb->AppendWhere(CoreUtils::Convert(IHbpcdLookupProvider::TABLE_MCC_IDD + "._id="));
            AutoPtr<IList> list;
            uri->GetPathSegments((IList**)&list);
            AutoPtr<IInterface> obj;
            list->Get(1, (IInterface**)&obj);
            qb->AppendWhere(ICharSequence::Probe(obj));
            if (useDefaultOrder) {
                orderBy = IHbpcdLookupMccIdd::DEFAULT_SORT_ORDER;
            }
            break;
        }
        case MCC_LOOKUP_TABLE_ID: {
            qb->SetTables(IHbpcdLookupProvider::TABLE_MCC_LOOKUP_TABLE);
            qb->SetProjectionMap(IMap::Probe(sLookupProjectionMap));
            qb->AppendWhere(CoreUtils::Convert(IHbpcdLookupProvider::TABLE_MCC_LOOKUP_TABLE + "._id="));
            AutoPtr<IList> list;
            uri->GetPathSegments((IList**)&list);
            AutoPtr<IInterface> obj;
            list->Get(1, (IInterface**)&obj);
            qb->AppendWhere(ICharSequence::Probe(obj));
            if (useDefaultOrder) {
                orderBy = IHbpcdLookupMccLookup::DEFAULT_SORT_ORDER;
            }
            break;
        }
        case MCC_SID_CONFLICT_ID: {
            qb->SetTables(IHbpcdLookupProvider::TABLE_MCC_SID_CONFLICT);
            qb->AppendWhere(CoreUtils::Convert(IHbpcdLookupProvider::TABLE_MCC_SID_CONFLICT + "._id="));
            AutoPtr<IList> list;
            uri->GetPathSegments((IList**)&list);
            AutoPtr<IInterface> obj;
            list->Get(1, (IInterface**)&obj);
            qb->AppendWhere(ICharSequence::Probe(obj));
            if (useDefaultOrder) {
                orderBy = IHbpcdLookupMccSidConflicts::DEFAULT_SORT_ORDER;
            }
            break;
        }
        case MCC_SID_RANGE_ID: {
            qb->SetTables(IHbpcdLookupProvider::TABLE_MCC_SID_RANGE);
            qb->SetProjectionMap(IMap::Probe(sRangeProjectionMap));
            qb->AppendWhere(CoreUtils::Convert(IHbpcdLookupProvider::TABLE_MCC_SID_RANGE + "._id="));
            AutoPtr<IList> list;
            uri->GetPathSegments((IList**)&list);
            AutoPtr<IInterface> obj;
            list->Get(1, (IInterface**)&obj);
            qb->AppendWhere(ICharSequence::Probe(obj));
            if (useDefaultOrder) {
                orderBy = IHbpcdLookupMccIdd::DEFAULT_SORT_ORDER;
            }
            break;
        }
        case NANP_AREA_CODE_ID: {
            qb->SetTables(IHbpcdLookupProvider::TABLE_NANP_AREA_CODE);
            qb->SetProjectionMap(IMap::Probe(sNanpProjectionMap));
            qb->AppendWhere(CoreUtils::Convert(IHbpcdLookupProvider::TABLE_NANP_AREA_CODE + "._id="));
            AutoPtr<IList> list;
            uri->GetPathSegments((IList**)&list);
            AutoPtr<IInterface> obj;
            list->Get(1, (IInterface**)&obj);
            qb->AppendWhere(ICharSequence::Probe(obj));
            if (useDefaultOrder) {
                orderBy = IHbpcdLookupNanpAreaCode::DEFAULT_SORT_ORDER;
            }
            break;
        }
        case ARBITRARY_MCC_SID_MATCH_ID: {
            qb->SetTables(IHbpcdLookupProvider::TABLE_ARBITRARY_MCC_SID_MATCH);
            qb->SetProjectionMap(IMap::Probe(sArbitraryProjectionMap));
            qb->AppendWhere(CoreUtils::Convert(IHbpcdLookupProvider::TABLE_ARBITRARY_MCC_SID_MATCH + "._id="));
            AutoPtr<IList> list;
            uri->GetPathSegments((IList**)&list);
            AutoPtr<IInterface> obj;
            list->Get(1, (IInterface**)&obj);
            qb->AppendWhere(ICharSequence::Probe(obj));
            if (useDefaultOrder) {
                orderBy = IHbpcdLookupArbitraryMccSidMatch::DEFAULT_SORT_ORDER;
            }
            break;
        }
        default:
            // throw new IllegalArgumentException("Unknown URI " + uri);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (!useDefaultOrder) {
        orderBy = sortOrder;
    }

    AutoPtr<ISQLiteDatabase> db;
    mDbHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<ICursor> c;
    qb->Query(db, projectionIn, selection, selectionArgs, groupBy, String(NULL), orderBy, (ICursor**)&c);
    if (c != NULL) {
        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        AutoPtr<IContentResolver> cr;
        ctx->GetContentResolver((IContentResolver**)&cr);
        c->SetNotificationUri(cr, uri);
    }

    *result = c;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

// @Override
ECode CHbpcdLookupProvider::GetType(
    /* [in] */ IUri* uri,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) {
        Logger::D(TAG, "getType");
    }

    *result = String(NULL);
    return NOERROR;
}

// @Override
ECode CHbpcdLookupProvider::Insert(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [out] */ IUri** result)
{
    // throw new UnsupportedOperationException("Failed to insert row into " + uri);
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

// @Override
ECode CHbpcdLookupProvider::Delete(
    /* [in] */ IUri* uri,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* result)
{
    // throw new UnsupportedOperationException("Cannot delete URL: " + uri);
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

// @Override
ECode CHbpcdLookupProvider::Update(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    Int32 count = 0;
    AutoPtr<ISQLiteDatabase> db;
    mDbHelper->GetWritableDatabase((ISQLiteDatabase**)&db);

    Int32 match;
    sURIMatcher->Match(uri, &match);
    switch (match) {
        case MCC_LOOKUP_TABLE:
            db->Update(IHbpcdLookupProvider::TABLE_MCC_LOOKUP_TABLE, values, selection, selectionArgs, &count);
            break;
        default:
            // throw new UnsupportedOperationException("Cannot update URL: " + uri);
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    *result = count;
    return NOERROR;
}

} // namespace Telephony
} // namespace Providers
} // namespace Droid
} // namespace Elastos
