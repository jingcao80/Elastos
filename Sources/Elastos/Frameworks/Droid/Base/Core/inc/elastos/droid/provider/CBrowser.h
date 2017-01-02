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

#ifndef __ELASTOS_DROID_PROVIDER_CBROWSER_H__
#define __ELASTOS_DROID_PROVIDER_CBROWSER_H__

#include "_Elastos_Droid_Provider_CBrowser.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Webkit::IWebIconDatabaseIconListener;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CBrowser)
    , public Singleton
    , public IBrowser
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetBOOKMARKSURI(
        /* [out] */ IUri** uri);

    CARAPI GetSEARCHESURI(
        /* [out] */ IUri** uri);

    /**
     *  Open an activity to save a bookmark. Launch with a title
     *  and/or a url, both of which can be edited by the user before saving.
     *
     *  @param c        Context used to launch the activity to add a bookmark.
     *  @param title    Title for the bookmark. Can be null or empty string.
     *  @param url      Url for the bookmark. Can be null or empty string.
     */
    CARAPI SaveBookmark(
        /* [in] */ IContext* c,
        /* [in] */ const String& title,
        /* [in] */ const String& url);

    /**
     * Sends the given string using an Intent with {@link Intent#ACTION_SEND} and a mime type
     * of text/plain. The string is put into {@link Intent#EXTRA_TEXT}.
     *
     * @param context the context used to start the activity
     * @param string the string to send
     */
    CARAPI SendString(
        /* [in] */ IContext* context,
        /* [in] */ const String& string);

    /**
     *  Find an application to handle the given string and, if found, invoke
     *  it with the given string as a parameter.
     *  @param c Context used to launch the new activity.
     *  @param stringToSend The string to be handled.
     *  @param chooserDialogTitle The title of the dialog that allows the user
     *  to select between multiple applications that are all capable of handling
     *  the string.
     *  @hide pending API council approval
     */
    CARAPI SendString(
        /* [in] */ IContext* c,
        /* [in] */ const String& stringToSend,
        /* [in] */ const String& chooserDialogTitle);

    /**
     *  Return a cursor pointing to a list of all the bookmarks. The cursor will have a single
     *  column, {@link BookmarkColumns#URL}.
     *  <p>
     *  Requires {@link android.Manifest.permission#READ_HISTORY_BOOKMARKS}
     *
     *  @param cr   The ContentResolver used to access the database.
     */
    CARAPI GetAllBookmarks(
        /* [in] */ IContentResolver* cr,
        /* [out] */ ICursor** bookmarks);

    /**
     *  Return a cursor pointing to a list of all visited site urls. The cursor will
     *  have a single column, {@link BookmarkColumns#URL}.
     *  <p>
     *  Requires {@link android.Manifest.permission#READ_HISTORY_BOOKMARKS}
     *
     *  @param cr   The ContentResolver used to access the database.
     */
    CARAPI GetAllVisitedUrls(
        /* [in] */ IContentResolver* cr,
        /* [out] */ ICursor** urls);

    /**
     *  Update the visited history to acknowledge that a site has been
     *  visited.
     *  Requires {@link android.Manifest.permission#READ_HISTORY_BOOKMARKS}
     *  Requires {@link android.Manifest.permission#WRITE_HISTORY_BOOKMARKS}
     *  @param cr   The ContentResolver used to access the database.
     *  @param url  The site being visited.
     *  @param real If true, this is an actual visit, and should add to the
     *              number of visits.  If false, the user entered it manually.
     */
    CARAPI UpdateVisitedHistory(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& url,
        /* [in] */ Boolean real);

    /**
     *  Returns all the URLs in the history.
     *  Requires {@link android.Manifest.permission#READ_HISTORY_BOOKMARKS}
     *  @param cr   The ContentResolver used to access the database.
     *  @hide pending API council approval
     */
    CARAPI GetVisitedHistory(
        /* [in] */ IContentResolver* cr,
        /* [out] */ ArrayOf<String>** urls);

    /**
     * If there are more than MAX_HISTORY_COUNT non-bookmark history
     * items in the bookmark/history table, delete TRUNCATE_N_OLDEST
     * of them.  This is used to keep our history table to a
     * reasonable size.  Note: it does not prune bookmarks.  If the
     * user wants 1000 bookmarks, the user gets 1000 bookmarks.
     *  Requires {@link android.Manifest.permission#READ_HISTORY_BOOKMARKS}
     *  Requires {@link android.Manifest.permission#WRITE_HISTORY_BOOKMARKS}
     *
     * @param cr The ContentResolver used to access the database.
     */
    CARAPI TruncateHistory(
        /* [in] */ IContentResolver* cr);

    /**
     * Returns whether there is any history to clear.
     *  Requires {@link android.Manifest.permission#READ_HISTORY_BOOKMARKS}
     * @param cr   The ContentResolver used to access the database.
     * @return boolean  True if the history can be cleared.
     */
    CARAPI CanClearHistory(
        /* [in] */ IContentResolver* cr,
        /* [out] */ Boolean* res);

    /**
     *  Delete all entries from the bookmarks/history table which are
     *  not bookmarks.  Also set all visited bookmarks to unvisited.
     *  Requires {@link android.Manifest.permission#WRITE_HISTORY_BOOKMARKS}
     *  @param cr   The ContentResolver used to access the database.
     */
    CARAPI ClearHistory(
        /* [in] */ IContentResolver* cr);

    /**
     * Delete all history items from begin to end.
     *  Requires {@link android.Manifest.permission#WRITE_HISTORY_BOOKMARKS}
     * @param cr    The ContentResolver used to access the database.
     * @param begin First date to remove.  If -1, all dates before end.
     *              Inclusive.
     * @param end   Last date to remove. If -1, all dates after begin.
     *              Non-inclusive.
     */
    CARAPI DeleteHistoryTimeFrame(
        /* [in] */ IContentResolver* cr,
        /* [in] */ Int64 begin,
        /* [in] */ Int64 end);

    /**
     * Remove a specific url from the history database.
     *  Requires {@link android.Manifest.permission#WRITE_HISTORY_BOOKMARKS}
     * @param cr    The ContentResolver used to access the database.
     * @param url   url to remove.
     */
    CARAPI DeleteFromHistory(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& url);

    /**
     * Add a search string to the searches database.
     *  Requires {@link android.Manifest.permission#READ_HISTORY_BOOKMARKS}
     *  Requires {@link android.Manifest.permission#WRITE_HISTORY_BOOKMARKS}
     * @param cr   The ContentResolver used to access the database.
     * @param search    The string to add to the searches database.
     */
    CARAPI AddSearchUrl(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& search);

    /**
     * Remove all searches from the search database.
     *  Requires {@link android.Manifest.permission#WRITE_HISTORY_BOOKMARKS}
     * @param cr   The ContentResolver used to access the database.
     */
    CARAPI ClearSearches(
        /* [in] */ IContentResolver* cr);

    /**
     *  Request all icons from the database.  This call must either be called
     *  in the main thread or have had Looper.prepare() invoked in the calling
     *  thread.
     *  Requires {@link android.Manifest.permission#READ_HISTORY_BOOKMARKS}
     *  @param  cr The ContentResolver used to access the database.
     *  @param  where Clause to be used to limit the query from the database.
     *          Must be an allowable string to be passed into a database query.
     *  @param  listener IconListener that gets the icons once they are
     *          retrieved.
     */
    CARAPI RequestAllIcons(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& where,
        /* [in] */ IWebIconDatabaseIconListener* listener);

    CARAPI GetHISTORY_PROJECTION(
        /* [out, callee] */ ArrayOf<String>** projections);

    CARAPI GetTRUNCATE_HISTORY_PROJECTION(
        /* [out, callee] */ ArrayOf<String>** projections);

    CARAPI GetSEARCHES_PROJECTION(
        /* [out, callee] */ ArrayOf<String>** projections);
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PROVIDER_CBROWSER_H__
