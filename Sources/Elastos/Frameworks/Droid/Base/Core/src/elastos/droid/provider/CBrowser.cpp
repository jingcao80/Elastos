
#include "Elastos.Droid.Net.h"
#include "elastos/droid/provider/Browser.h"
#include "elastos/droid/provider/CBrowser.h"

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CBrowser)

CAR_INTERFACE_IMPL(CBrowser, Singleton, IBrowser)

ECode CBrowser::GetBOOKMARKSURI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = Browser::BOOKMARKS_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CBrowser::GetSEARCHESURI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = Browser::SEARCHES_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CBrowser::SaveBookmark(
    /* [in] */ IContext* c,
    /* [in] */ const String& title,
    /* [in] */ const String& url)
{
    return Browser::SaveBookmark(c, title, url);
}

ECode CBrowser::SendString(
    /* [in] */ IContext* context,
    /* [in] */ const String& string)
{
    return Browser::SendString(context, string);
}

ECode CBrowser::SendString(
    /* [in] */ IContext* c,
    /* [in] */ const String& stringToSend,
    /* [in] */ const String& chooserDialogTitle)
{
    return Browser::SendString(c, stringToSend, chooserDialogTitle);
}

ECode CBrowser::GetAllBookmarks(
    /* [in] */ IContentResolver* cr,
    /* [out] */ ICursor** bookmarks)
{
    return Browser::GetAllBookmarks(cr, bookmarks);
}

ECode CBrowser::GetAllVisitedUrls(
    /* [in] */ IContentResolver* cr,
    /* [out] */ ICursor** urls)
{
    return Browser::GetAllVisitedUrls(cr, urls);
}

ECode CBrowser::UpdateVisitedHistory(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& url,
    /* [in] */ Boolean real)
{
    return Browser::UpdateVisitedHistory(cr, url, real);
}

ECode CBrowser::GetVisitedHistory(
     /* [in] */ IContentResolver* cr,
     /* [out] */ ArrayOf<String>** urls)
{
    return Browser::GetVisitedHistory(cr, urls);
}

ECode CBrowser::TruncateHistory(
    /* [in] */ IContentResolver* cr)
{
    return Browser::TruncateHistory(cr);
}

ECode CBrowser::CanClearHistory(
    /* [in] */ IContentResolver* cr,
    /* [out] */ Boolean* res)
{
    return Browser::CanClearHistory(cr, res);
}

ECode CBrowser::ClearHistory(
    /* [in] */ IContentResolver* cr)
{
    return Browser::ClearHistory(cr);
}

ECode CBrowser::DeleteHistoryTimeFrame(
    /* [in] */ IContentResolver* cr,
    /* [in] */ Int64 begin,
    /* [in] */ Int64 end)
{
    return Browser::DeleteHistoryTimeFrame(cr, begin, end);
}

ECode CBrowser::DeleteFromHistory(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& url)
{
    return Browser::DeleteFromHistory(cr, url);
}

ECode CBrowser::AddSearchUrl(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& search)
{
    return Browser::AddSearchUrl(cr, search);
}

ECode CBrowser::ClearSearches(
    /* [in] */ IContentResolver* cr)
{
    return Browser::ClearSearches(cr);
}

ECode CBrowser::RequestAllIcons(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& where,
    /* [in] */ IWebIconDatabaseIconListener* listener)
{
    return Browser::RequestAllIcons(cr, where, listener);
}

ECode CBrowser::GetHISTORY_PROJECTION(
    /* [out, callee] */ ArrayOf<String>** projections)
{
    VALIDATE_NOT_NULL(projections);
    *projections = Browser::HISTORY_PROJECTION.Get();
    REFCOUNT_ADD(*projections);
    return NOERROR;
}

ECode CBrowser::GetTRUNCATE_HISTORY_PROJECTION(
    /* [out, callee] */ ArrayOf<String>** projections)
{
    VALIDATE_NOT_NULL(projections);
    *projections = Browser::TRUNCATE_HISTORY_PROJECTION.Get();
    REFCOUNT_ADD(*projections);
    return NOERROR;
}

ECode CBrowser::GetSEARCHES_PROJECTION(
    /* [out, callee] */ ArrayOf<String>** projections)
{
    VALIDATE_NOT_NULL(projections);
    *projections = Browser::SEARCHES_PROJECTION;
    REFCOUNT_ADD(*projections);
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
