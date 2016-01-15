
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/content/CContentUris.h"
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/Intent.h"
#include "elastos/droid/database/DatabaseUtils.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/Browser.h"
#include "elastos/droid/provider/BrowserContract.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <Elastos.CoreLibrary.IO.h>

using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Intent;
using Elastos::Droid::Database::DatabaseUtils;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::Uri;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Slogger;
using Elastos::Core::CoreUtils;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Provider {

const String Browser::LOGTAG("browser");
const Int32 Browser::MAX_HISTORY_COUNT = 250;

static AutoPtr<IUri> initBOOKMARKSURI()
{
    AutoPtr<IUri> uri;
    Uri::Parse(String("content://browser/bookmarks"), (IUri**)&uri);
    return uri;
}

static AutoPtr<IUri> initSEARCHESURI()
{
    AutoPtr<IUri> uri;
    Uri::Parse(String("content://browser/searches"), (IUri**)&uri);
    return uri;
}

const AutoPtr<IUri> Browser::BOOKMARKS_URI = initBOOKMARKSURI();

const AutoPtr<IUri> Browser::SEARCHES_URI = initSEARCHESURI();

static AutoPtr<ArrayOf<String> > initHISTORY_PROJECTION()
{
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(10);

    (*args)[0] = IBaseColumns::ID;
    (*args)[1] = IBrowserBookmarkColumns::URL;
    (*args)[2] = IBrowserBookmarkColumns::VISITS;
    (*args)[3] = IBrowserBookmarkColumns::DATE;
    (*args)[4] = IBrowserBookmarkColumns::BOOKMARK;
    (*args)[5] = IBrowserBookmarkColumns::TITLE;
    (*args)[6] = IBrowserBookmarkColumns::FAVICON;
    (*args)[7] = IBrowserBookmarkColumns::THUMBNAIL;
    (*args)[8] = IBrowserBookmarkColumns::TOUCH_ICON;
    (*args)[9] = IBrowserBookmarkColumns::USER_ENTERED;

    return args;
}

static AutoPtr<ArrayOf<String> > initTRUNCATE_HISTORY_PROJECTION()
{
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(2);
    (*args)[0] = IBaseColumns::ID;
    (*args)[1] = IBrowserBookmarkColumns::DATE;
    return args;
}

static AutoPtr<ArrayOf<String> > initSEARCHES_PROJECTION()
{
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(3);
    (*args)[0] = IBaseColumns::ID;
    (*args)[1] = IBrowserSearchColumns::SEARCH;
    (*args)[2] = IBrowserSearchColumns::DATE;
    return args;
}

const AutoPtr<ArrayOf<String> > Browser::HISTORY_PROJECTION =
    initHISTORY_PROJECTION();

const AutoPtr<ArrayOf<String> > Browser::TRUNCATE_HISTORY_PROJECTION =
    initTRUNCATE_HISTORY_PROJECTION();

const AutoPtr<ArrayOf<String> > Browser::SEARCHES_PROJECTION =
    initSEARCHES_PROJECTION();

ECode Browser::SaveBookmark(
    /* [in] */ IContext* c,
    /* [in] */ const String& title,
    /* [in] */ const String& url)
{
    AutoPtr<IIntent> i;
    CIntent::New(IIntent::ACTION_INSERT, BOOKMARKS_URI.Get(), (IIntent**)&i);
    i->PutExtra(String("title"), title);
    i->PutExtra(String("url"), url);
    return c->StartActivity(i.Get());
}

ECode Browser::SendString(
    /* [in] */ IContext* context,
    /* [in] */ const String& string)
{
    VALIDATE_NOT_NULL(context);
    String str;
    // context->GetString(com.android.internal.R.string.sendText, &str);
    return SendString(context, string, str);
}

ECode Browser::SendString(
    /* [in] */ IContext* c,
    /* [in] */ const String& stringToSend,
    /* [in] */ const String& chooserDialogTitle)
{
    AutoPtr<IIntent> send;
    CIntent::New(IIntent::ACTION_SEND, (IIntent**)&send);
    send->SetType(String("text/plain"));
    send->PutExtra(IIntent::EXTRA_TEXT, stringToSend);

    // try {
    AutoPtr<ICharSequence> csq = CoreUtils::Convert(chooserDialogTitle);
    AutoPtr<IIntent> i = Intent::CreateChooser(send.Get(), csq.Get());
    // In case this is called from outside an Activity
    i->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    return c->StartActivity(i.Get());
    // } catch(android.content.ActivityNotFoundException ex) {
         // if no app handles it, do nothing
//     }
}

ECode Browser::GetAllBookmarks(
    /* [in] */ IContentResolver* cr,
    /* [out] */ ICursor** bookmarks)
{
    VALIDATE_NOT_NULL(cr);
    AutoPtr<ArrayOf<String> > strs = ArrayOf<String>::Alloc(1);
    strs->Set(0, IBrowserContractCommonColumns::URL);
    return cr->Query(BrowserContract::Bookmarks::CONTENT_URI.Get(),
             strs.Get(), IBrowserContractBookmarks::IS_FOLDER + " = 0",
             NULL, String(NULL), bookmarks);
}

ECode Browser::GetAllVisitedUrls(
    /* [in] */ IContentResolver* cr,
    /* [out] */ ICursor** urls)
{
    VALIDATE_NOT_NULL(urls);
    VALIDATE_NOT_NULL(cr);
    AutoPtr<ArrayOf<String> > str = ArrayOf<String>::Alloc(1);
    str->Set(0, IBrowserContractCommonColumns::URL);

    return cr->Query(BrowserContract::Combined::CONTENT_URI.Get(),
            str.Get(), String(NULL), NULL,
            IBrowserContractCommonColumns::DATE_CREATED + " ASC", urls);
}

void Browser::AddOrUrlEquals(
    /* [in] */ StringBuilder& sb)
{
    sb.Append(String(" OR ") + IBrowserBookmarkColumns::URL + " = ");
}

AutoPtr<ICursor> Browser::GetVisitedLike(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& url)
{
    Boolean secure = FALSE;
    String compareString = url;
    if (compareString.StartWith("http://")) {
        compareString = compareString.Substring(7);
    } else if (compareString.StartWith("https://")) {
        compareString = compareString.Substring(8);
        secure = TRUE;
    }
    if (compareString.StartWith("www.")) {
        compareString = compareString.Substring(4);
    }
    AutoPtr<StringBuilder> whereClause;
    if (secure) {
        whereClause = new StringBuilder(IBrowserContractCommonColumns::URL + " = ");
        DatabaseUtils::AppendEscapedSQLString(IStringBuilder::Probe(whereClause),
                String("https://") + compareString);
        AddOrUrlEquals(*whereClause);
        DatabaseUtils::AppendEscapedSQLString(whereClause,
                String("https://www.") + compareString);
    } else {
        whereClause = new StringBuilder(IBrowserContractCommonColumns::URL + " = ");
        DatabaseUtils::AppendEscapedSQLString(whereClause,
                compareString);
        AddOrUrlEquals(*whereClause);
        String wwwString = String("www.") + compareString;
        DatabaseUtils::AppendEscapedSQLString(whereClause,
                wwwString);
        AddOrUrlEquals(*whereClause);
        DatabaseUtils::AppendEscapedSQLString(whereClause,
                String("http://") + compareString);
        AddOrUrlEquals(*whereClause);
        DatabaseUtils::AppendEscapedSQLString(whereClause,
                String("http://") + wwwString);
    }
    AutoPtr<ArrayOf<String> > str = ArrayOf<String>::Alloc(2);
    str->Set(0, IBrowserContractCommonColumns::ID);
    str->Set(1, IBrowserContractHistoryColumns::VISITS);

    AutoPtr<ICursor> outCursor;
    cr->Query(BrowserContract::History::CONTENT_URI.Get(), str.Get(),
        whereClause->ToString(), NULL, String(NULL), (ICursor**)&outCursor);
    return outCursor;
}

ECode Browser::UpdateVisitedHistory(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& url,
    /* [in] */ Boolean real)
{
    Int64 now;
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&now);
    AutoPtr<ICursor> c;
    // try {
    c = GetVisitedLike(cr, url);
    /* We should only get one answer that is exactly the same. */
    Boolean flag = FALSE;
    AutoPtr<IContentValues> values;
    if ((c->MoveToFirst(&flag), flag)) {
        CContentValues::New((IContentValues**)&values);
        Int32 columnValue;
        if (real) {
            c->GetInt32(1, &columnValue);
            values->Put(IBrowserContractHistoryColumns::VISITS, columnValue + 1);
        } else {
            values->Put(IBrowserContractHistoryColumns::USER_ENTERED, 1);
        }

        values->Put(IBrowserContractHistoryColumns::DATE_LAST_VISITED, now);
        AutoPtr<IContentUris> contentUri;
        CContentUris::AcquireSingleton((IContentUris**)&contentUri);
        Int64 value;
        c->GetInt64(0, &value);
        AutoPtr<IUri> inUri;
        contentUri->WithAppendedId(BrowserContract::History::CONTENT_URI, value, (IUri**)&inUri);
        Int32 rowsAffected;
        cr->Update(inUri, values.Get(), String(NULL), NULL, &rowsAffected);
    } else {
        TruncateHistory(cr);
        values = NULL;
        CContentValues::New((IContentValues**)&values);
        Int32 visits;
        Int32 user_entered;
        if (real) {
            visits = 1;
            user_entered = 0;
        } else {
            visits = 0;
            user_entered = 1;
        }
        values->Put(IBrowserContractCommonColumns::URL, url);
        values->Put(IBrowserContractHistoryColumns::VISITS, visits);
        values->Put(IBrowserContractHistoryColumns::DATE_LAST_VISITED, now);
        values->Put(IBrowserContractCommonColumns::TITLE, url);
        values->Put(IBrowserContractCommonColumns::DATE_CREATED, 0);
        values->Put(IBrowserContractHistoryColumns::USER_ENTERED, user_entered);
        AutoPtr<IUri> outUri;
        cr->Insert(BrowserContract::History::CONTENT_URI, values, (IUri**)&outUri);
    }
    // } catch (IllegalStateException e) {
    // Slogger::E(LOGTAG, "updateVisitedHistory");
    // } finally {
    if (c != NULL)  return ICloseable::Probe(c)->Close();
    // }
}

ECode Browser::GetVisitedHistory(
     /* [in] */ IContentResolver* cr,
     /* [out] */ ArrayOf<String>** urls)
{
    VALIDATE_NOT_NULL(urls);
    AutoPtr<ICursor> c;
    AutoPtr<ArrayOf<String> > str;
    // try {
    AutoPtr<ArrayOf<String> > projection = ArrayOf<String>::Alloc(1);
    (*projection)[0] = IBrowserContractCommonColumns::URL;
    ECode ec = cr->Query(BrowserContract::History::CONTENT_URI, projection.Get(),
        IBrowserContractHistoryColumns::VISITS + " > 0", NULL, String(NULL), (ICursor**)&c);
    if (!SUCCEEDED(ec)) goto EXIT;
    if (c == NULL) {
        urls = NULL;
        return NOERROR;
    }

    Int32 count;
    c->GetCount(&count);
    str = ArrayOf<String>::Alloc(count);
    Int32 i;
    Boolean bSucceeded;
    while ((c->MoveToNext(&bSucceeded), bSucceeded)) {
        String columnValue;
        c->GetString(0, &columnValue);
        (*str)[i] = columnValue;
        i++;
    }
    // } catch (IllegalStateException e) {
    *urls = str;
    REFCOUNT_ADD(*urls);
    // } finally {
    if (c != NULL) ICloseable::Probe(c)->Close();
    return NOERROR;
    // }
EXIT:
    {
        if (ec == (ECode)E_ILLEGAL_STATE_EXCEPTION)
        {
            Slogger::E(LOGTAG, "getVisitedHistory%d", ec);
            urls = NULL;
            return NOERROR;
        }
    }
}

ECode Browser::TruncateHistory(
    /* [in] */ IContentResolver* cr)
{
    // TODO make a single request to the provider to do this in a single transaction
    AutoPtr<ICursor> cursor;
    // try {
    // Select non-bookmark history, ordered by date
    AutoPtr<ArrayOf<String> > str =  ArrayOf<String>::Alloc(3);
    (*str)[0] = IBrowserContractSearches::ID;
    (*str)[1] = IBrowserContractCommonColumns::URL;
    (*str)[2] = IBrowserContractHistoryColumns::DATE_LAST_VISITED;

    cr->Query(BrowserContract::History::CONTENT_URI, str.Get(), String(NULL),
                    NULL, IBrowserContractHistoryColumns::DATE_LAST_VISITED + " ASC", (ICursor**)&cursor);

    Boolean succeeded = FALSE;
    cursor->MoveToFirst(&succeeded);
    Int32 count;
    cursor->GetCount(&count);
    ECode ec = NOERROR;
    if (succeeded && count >= MAX_HISTORY_COUNT) {
        Int64 columnValue;
        cursor->GetInt64(0, &columnValue);
        /* eliminate oldest history items */
        for (Int32 i = 0; i < IBrowser::TRUNCATE_N_OLDEST; i++) {
            AutoPtr<IContentUris> contentUri;
            CContentUris::AcquireSingleton((IContentUris**)&contentUri);
            AutoPtr<IUri> inUri;
            contentUri->WithAppendedId(BrowserContract::History::CONTENT_URI, columnValue, (IUri**)&inUri);
            Int32 rowsAffected;
            ec = cr->Delete(inUri, String(NULL), NULL, &rowsAffected);
            if (!SUCCEEDED(ec)) goto EXIT;
            if (!(cursor->MoveToNext(&succeeded), succeeded)) break;
        }
    }
    // } finally {
    if (cursor != NULL) ICloseable::Probe(cursor)->Close();
    // }
    return NOERROR;
EXIT:
    // } catch (IllegalStateException e) {
    if (ec == (ECode)E_ILLEGAL_STATE_EXCEPTION) {
        Slogger::E(LOGTAG, "truncateHistory%d", ec);
        return NOERROR;
    }
}

ECode Browser::CanClearHistory(
    /* [in] */ IContentResolver* cr,
    /* [out] */ Boolean* res)
{
    AutoPtr<ICursor> cursor;
    Boolean ret = FALSE;
    AutoPtr<ArrayOf<String> > str = ArrayOf<String>::Alloc(2);
    (*str)[0] = IBrowserContractCommonColumns::ID;
    (*str)[1] = IBrowserContractHistoryColumns::VISITS;
    // try {
    ECode ec = cr->Query(BrowserContract::History::CONTENT_URI.Get(),
        str.Get(), String(NULL), NULL, String(NULL), (ICursor**)&cursor);
    if (!SUCCEEDED(ec)) goto EXIT;
    Int32 count;
    ret = (cursor->GetCount(&count), count) > 0;
    *res = ret;
    if (cursor != NULL) ICloseable::Probe(cursor)->Close();
    return NOERROR;
    // } finally {
        // if (cursor != null) cursor.close();
    // }
EXIT:
// } catch (IllegalStateException e) {
        // Log.e(LOGTAG, "canClearHistory", e);
    if (ec == (ECode)E_ILLEGAL_STATE_EXCEPTION) {
        Slogger::E(LOGTAG, "canClearHistory%d", ec);
    }
    if (cursor != NULL) ICloseable::Probe(cursor)->Close();

    return NOERROR;
}

ECode Browser::ClearHistory(
    /* [in] */ IContentResolver* cr)
{
    DeleteHistoryWhere(cr, String(NULL));
    return NOERROR;
}

void Browser::DeleteHistoryWhere(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& whereClause)
{
    AutoPtr<ICursor> cursor;
    AutoPtr<ArrayOf<String> > str = ArrayOf<String>::Alloc(1);
    (*str)[0] = IBrowserContractCommonColumns::URL;
    // try {
    ECode ec = cr->Query(BrowserContract::History::CONTENT_URI.Get(), str.Get(), whereClause,
                NULL, String(NULL), (ICursor**)&cursor);
    if (!SUCCEEDED(ec)) goto EXIT;
    Boolean flag;
    flag = FALSE;
    if ((cursor->MoveToFirst(&flag), flag)) {
        Int32 rowsAffected;
        ec = cr->Delete(BrowserContract::History::CONTENT_URI.Get(), whereClause, NULL, &rowsAffected);
        if (!SUCCEEDED(ec)) goto EXIT;
    }
    if (cursor != NULL) ICloseable::Probe(cursor)->Close();
    return;
    // } catch (IllegalStateException e) {
        // Log.e(LOGTAG, "deleteHistoryWhere", e);
        // return;
    // } finally {
        // if (cursor != null) cursor.close();
    // }
EXIT:
    {
        if(ec == (ECode)E_ILLEGAL_STATE_EXCEPTION) {
            Slogger::E(LOGTAG, "deleteHistoryWhere%d", ec);
        }
        if (cursor != NULL) ICloseable::Probe(cursor)->Close();
        return;
    }
}

ECode Browser::DeleteHistoryTimeFrame(
    /* [in] */ IContentResolver* cr,
    /* [in] */ Int64 begin,
    /* [in] */ Int64 end)
{
    String whereClause;
    String date = IBrowserBookmarkColumns::DATE;
    if (-1 == begin) {
        if (-1 == end) {
            return ClearHistory(cr);
        }
        whereClause = date + " < " + StringUtils::ToString(end);
        ;
    } else if (-1 == end) {
        whereClause = date + " >= " + StringUtils::ToString(begin);
    } else {
        whereClause = date + " >= " + StringUtils::ToString(begin) + " AND " + date
                + " < " + StringUtils::ToString(end);
    }
    DeleteHistoryWhere(cr, whereClause);
    return NOERROR;
}

ECode Browser::DeleteFromHistory(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& url)
{
    //IBrowserContractCommonColumns::URL
    AutoPtr<ArrayOf<String> > str = ArrayOf<String>::Alloc(1);
    (*str)[0] = url;
    Int32 rowsAffected;
    return cr->Delete(BrowserContract::History::CONTENT_URI.Get(), IBrowserContractCommonColumns::URL + "=?", str.Get(), &rowsAffected);
}

ECode Browser::AddSearchUrl(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& search)
{
    // The content provider will take care of updating existing searches instead of duplicating
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    values->Put(IBrowserContractSearches::SEARCH, search);
    Int64 currentTimeMillis;
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&currentTimeMillis);
    values->Put(IBrowserContractSearches::DATE, currentTimeMillis);
    AutoPtr<IUri> uri;
    return cr->Insert(BrowserContract::Searches::CONTENT_URI.Get(), values.Get(), (IUri**)&uri);
}

ECode Browser::ClearSearches(
    /* [in] */ IContentResolver* cr)
{
    // FIXME: Should this clear the urls to which these searches lead?
    // (i.e. remove google.com/query= blah blah blah)
    // try {
    VALIDATE_NOT_NULL(cr);
    Int32 rowsAffected;
    ECode ec = cr->Delete(BrowserContract::Searches::CONTENT_URI.Get(),
        String(NULL), NULL, &rowsAffected);
    // } catch (IllegalStateException e) {
    if (!SUCCEEDED(ec)) {
        if (ec == (ECode)E_ILLEGAL_STATE_EXCEPTION) {
            Slogger::E(LOGTAG, "clearSearches%d", ec);
        }
    }
    return ec;
    // }
}

ECode Browser::RequestAllIcons(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& where,
    /* [in] */ IWebIconDatabaseIconListener* listener)
{
    // Do nothing: this is no longer used.
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
