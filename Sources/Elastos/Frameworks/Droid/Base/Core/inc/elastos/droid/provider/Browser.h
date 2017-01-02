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

#ifndef __ELASTOS_DROID_PROVIDER_BROWSER_H__
#define __ELASTOS_DROID_PROVIDER_BROWSER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Webkit::IWebIconDatabaseIconListener;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Provider {

class Browser
{
public:
    static CARAPI SaveBookmark(
        /* [in] */ IContext* c,
        /* [in] */ const String& title,
        /* [in] */ const String& url);

    static CARAPI SendString(
        /* [in] */ IContext* context,
        /* [in] */ const String& string);

    static CARAPI SendString(
        /* [in] */ IContext* c,
        /* [in] */ const String& stringToSend,
        /* [in] */ const String& chooserDialogTitle);

    static CARAPI GetAllBookmarks(
        /* [in] */ IContentResolver* cr,
        /* [out] */ ICursor** bookmarks);

    static CARAPI GetAllVisitedUrls(
        /* [in] */ IContentResolver* cr,
        /* [out] */ ICursor** urls);

    static CARAPI UpdateVisitedHistory(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& url,
        /* [in] */ Boolean real);

    static CARAPI GetVisitedHistory(
        /* [in] */ IContentResolver* cr,
        /* [out] */ ArrayOf<String>** urls);

    static CARAPI TruncateHistory(
        /* [in] */ IContentResolver* cr);

    static CARAPI CanClearHistory(
        /* [in] */ IContentResolver* cr,
        /* [out] */ Boolean* res);

    static CARAPI ClearHistory(
        /* [in] */ IContentResolver* cr);

    static CARAPI DeleteHistoryTimeFrame(
        /* [in] */ IContentResolver* cr,
        /* [in] */ Int64 begin,
        /* [in] */ Int64 end);

    static CARAPI DeleteFromHistory(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& url);

    static CARAPI AddSearchUrl(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& search);

    static CARAPI ClearSearches(
        /* [in] */ IContentResolver* cr);

    static CARAPI RequestAllIcons(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& where,
        /* [in] */ IWebIconDatabaseIconListener* listener);

private:
    static CARAPI_(void) AddOrUrlEquals(
        /* [in] */ StringBuilder& sb);

    static CARAPI_(AutoPtr<ICursor>) GetVisitedLike(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& url);

    static CARAPI_(void) DeleteHistoryWhere(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& whereClause);

public:
    static const AutoPtr<IUri> BOOKMARKS_URI;
    static const AutoPtr<IUri> SEARCHES_URI;
    static const AutoPtr<ArrayOf<String> > HISTORY_PROJECTION;
    static const AutoPtr<ArrayOf<String> > TRUNCATE_HISTORY_PROJECTION;
    static const AutoPtr<ArrayOf<String> > SEARCHES_PROJECTION;

private:
    static const String LOGTAG;

    static const Int32 MAX_HISTORY_COUNT;
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_BROWSER_H__