
#include "CContentProviderOne.h"
#include "CDatabaseOpenHelper.h"
#include "Utils.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::IUriMatcher;
using Elastos::Droid::Content::CUriMatcher;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;
using Elastos::Core::StringBuilder;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace ContentProviderDemo {

static const String TAG("ContentProviderDemo::CContentProviderOne");

CAR_OBJECT_IMPL(CContentProviderOne)

AutoPtr<IUriMatcher> CContentProviderOne::sUriMatcher;

AutoPtr<IUriMatcher> CContentProviderOne::GetUriMatcher()
{
    if (sUriMatcher == NULL) {
        CUriMatcher::New(IUriMatcher::NO_MATCH, (IUriMatcher**)&sUriMatcher);
        sUriMatcher->AddURI(Utils::AUTOHORITY, Utils::TABLE_NAME, Utils::ITEM);
        sUriMatcher->AddURI(Utils::AUTOHORITY, Utils::TABLE_NAME + "/#", Utils::ITEM_ID);
    }
    return sUriMatcher;
}

ECode CContentProviderOne::constructor()
{
    Logger::I(TAG, "constructor");
    return ContentProvider::constructor();
}

ECode CContentProviderOne::OnCreate(
    /* [out] */ Boolean* succeeded)
{
    Logger::I(TAG, "OnCreate");
    VALIDATE_NOT_NULL(succeeded)
    *succeeded = FALSE;

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    ECode ec = CDatabaseOpenHelper::New(context, (ISQLiteOpenHelper**)&mDBOpenHelper);
    Logger::I(TAG, "create CDatabaseOpenHelper, mDBOpenHelper:%p, ec=%08x", mDBOpenHelper.Get(), ec);
    *succeeded = mDBOpenHelper != NULL;
    return NOERROR;
}

ECode CContentProviderOne::GetType(
    /* [in] */ IUri* uri,
    /* [out] */ String* type)
{
    Logger::I(TAG, "GetType: %s", TO_CSTR(uri));
    VALIDATE_NOT_NULL(type)
    *type = NULL;

    Int32 matchCode;
    GetUriMatcher()->Match(uri, &matchCode);
    switch (matchCode) {
        case Utils::ITEM:
            *type = Utils::CONTENT_TYPE;
            return NOERROR;

        case Utils::ITEM_ID:
            *type = Utils::CONTENT_ITEM_TYPE;
            return NOERROR;
    }

    Logger::E(TAG, "Unknown URI %s", TO_CSTR(uri));
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CContentProviderOne::Insert(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [out] */ IUri** insertedItemUri)
{
    Logger::I(TAG, "Insert uri:%s, values:%s", TO_CSTR(uri), TO_CSTR(values));
    VALIDATE_NOT_NULL(insertedItemUri)
    *insertedItemUri = NULL;

    AutoPtr<ISQLiteDatabase> db;
    mDBOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
    assert(db != NULL);

    Int32 matchCode;
    GetUriMatcher()->Match(uri, &matchCode);
    if (matchCode != Utils::ITEM) {
        Logger::E(TAG, "invalid URI %s, matchCode:%d", TO_CSTR(uri), matchCode);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int64 rowId = -1;
    ECode ec = db->Insert(Utils::TABLE_NAME, Utils::TAG_ID, values, &rowId);
    if (rowId > 0) {
        AutoPtr<IContentUris> contentUris;
        CContentUris::AcquireSingleton((IContentUris**)&contentUris);
        AutoPtr<IUri> noteUri;
        contentUris->WithAppendedId(Utils::CONTENT_URI, rowId, (IUri**)&noteUri);
        Logger::I(TAG, "WithAppendedId: %s", TO_CSTR(noteUri));

        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IContentResolver> contentResolver;
        context->GetContentResolver((IContentResolver**)&contentResolver);
        if (contentResolver) {
            contentResolver->NotifyChange(noteUri, NULL);
        }

        *insertedItemUri = noteUri;
        REFCOUNT_ADD(*insertedItemUri)
        return NOERROR;
    }

    Logger::E(TAG, "Unknown URI %s, rowId=%d, ec=%08x", TO_CSTR(uri), rowId, ec);
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CContentProviderOne::Query(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder,
    /* [out] */ ICursor** cursor)
{
    Logger::I(TAG, "Query uri: [%s]", TO_CSTR(uri));
    Logger::I(TAG, "Query projection: %d items:", projection ? projection->GetLength() : 0);
    if (projection) {
        for (Int32 i = 0; i < projection->GetLength(); ++i) {
            Logger::I(TAG, "                     %d : [%s]", i + 1, (*projection)[i].string());
        }
    }
    Logger::I(TAG, "Query selection: [%s], selectionArgs %d items:", selection.string(), selectionArgs ? selectionArgs->GetLength() : 0);
    if (projection) {
        for (Int32 i = 0; i < selectionArgs->GetLength(); ++i) {
            Logger::I(TAG, "                     %d : [%s]", i + 1, (*selectionArgs)[i].string());
        }
    }
    Logger::I(TAG, "Query sortOrder: [%s]", sortOrder.string());
    VALIDATE_NOT_NULL(cursor)
    *cursor = NULL;

    AutoPtr<ISQLiteDatabase> db;
    mDBOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
    assert(db != NULL);

    Int32 matchCode;
    GetUriMatcher()->Match(uri, &matchCode);

    Logger::I(TAG, "Query URI [%s], matchCode:%d", TO_CSTR(uri), matchCode);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);

    ECode ec = NOERROR;
    String nullStr;
    AutoPtr<ICursor> c;
    switch (matchCode) {
        case Utils::ITEM:
            ec = db->Query(Utils::TABLE_NAME, projection, selection, selectionArgs, nullStr, nullStr, sortOrder, (ICursor**)&c);
            if (FAILED(ec)) {
                Logger::E(TAG, "failed to Query, ec = %08x", ec);
            }
            assert(c != NULL);
            c->SetNotificationUri(contentResolver, uri);
            *cursor = c;
            REFCOUNT_ADD(*cursor)
            return NOERROR;

        case Utils::ITEM_ID: {
            AutoPtr<IList> segments;
            uri->GetPathSegments((IList**)&segments);
            AutoPtr<IInterface> obj;
            segments->Get(1, (IInterface**)&obj);
            String id = Object::ToString(obj);
            StringBuilder sb(Utils::TAG_ID);
            sb += id;
            if (!selection.IsNullOrEmpty()) {
                sb += "AND(";
                sb += selection;
                sb += ")";
            }
            String sel = sb.ToString();

            ec = db->Query(Utils::TABLE_NAME, projection, sel, selectionArgs, nullStr, nullStr, sortOrder, (ICursor**)&c);
            if (FAILED(ec)) {
                Logger::E(TAG, "failed to Query [%s], ec = %08x", sel.string(), ec);
            }
            assert(c != NULL);
            c->SetNotificationUri(contentResolver, uri);
            *cursor = c;
            REFCOUNT_ADD(*cursor)
            return NOERROR;
        }
    }

    Logger::E(TAG, "Unknown URI %s", TO_CSTR(uri));
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CContentProviderOne::Update(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* rowsAffected)
{
    Logger::I(TAG, "Update");
    VALIDATE_NOT_NULL(rowsAffected)
    *rowsAffected = 0;
    return NOERROR;
}

ECode CContentProviderOne::Delete(
    /* [in] */ IUri* uri,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* rowsAffected)
{
    Logger::I(TAG, "Delete");
    VALIDATE_NOT_NULL(rowsAffected)
    *rowsAffected = 0;

    AutoPtr<ISQLiteDatabase> db;
    mDBOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
    assert(db != NULL);

    Int32 matchCode;
    GetUriMatcher()->Match(uri, &matchCode);
    Logger::I(TAG, "Delete URI %s, matchCode:%d", TO_CSTR(uri), matchCode);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);

    ECode ec = NOERROR;
    switch(matchCode) {
        case Utils::ITEM:
            ec = db->Delete(Utils::TABLE_NAME, selection, selectionArgs, rowsAffected);
            Logger::I(TAG, "Delete %d rows, ec=%08x", *rowsAffected, ec);
            if (contentResolver) {
                contentResolver->NotifyChange(uri, NULL);
            }
            return NOERROR;

        case Utils::ITEM_ID: {
            AutoPtr<IList> segments;
            uri->GetPathSegments((IList**)&segments);
            AutoPtr<IInterface> obj;
            segments->Get(1, (IInterface**)&obj);
            String id = Object::ToString(obj);
            StringBuilder sb(Utils::TAG_ID);
            sb += id;
            if (!selection.IsNullOrEmpty()) {
                sb += "AND(";
                sb += selection;
                sb += ")";
            }
            String sel = sb.ToString();

            ec = db->Delete(Utils::TABLE_NAME, sel, selectionArgs, rowsAffected);
            Logger::I(TAG, "Delete [%s] %d rows, ec=%08x", sel.string(), *rowsAffected, ec);
            if (contentResolver) {
                contentResolver->NotifyChange(uri, NULL);
            }
            return NOERROR;
        }
    }

    Logger::E(TAG, "Unknown URI %s", TO_CSTR(uri));
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

} // namespace ContentProviderDemo
} // namespace DevSamples
} // namespace Elastos
