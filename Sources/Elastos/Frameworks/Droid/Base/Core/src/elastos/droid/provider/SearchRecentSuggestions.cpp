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

#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/SearchRecentSuggestions.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::ISearchRecentSuggestionsProvider;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Text::TextUtils;
using Elastos::Utility::Concurrent::CSemaphore;
using Elastos::Utility::Concurrent::ISemaphore;
using Elastos::Utility::Logging::Slogger;
using Elastos::Core::CSystem;
using Elastos::Core::CThread;
using Elastos::Core::ISystem;
using Elastos::Core::IThread;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Provider {

const String SearchRecentSuggestions::SuggestionColumns::DISPLAY1(String("display1"));
const String SearchRecentSuggestions::SuggestionColumns::DISPLAY2(String("display2"));
const String SearchRecentSuggestions::SuggestionColumns::QUERY(String("query"));
const String SearchRecentSuggestions::SuggestionColumns::DATE(String("date"));

static AutoPtr<ArrayOf<String> > initQUERIES_PROJECTION_1LINE()
{
    AutoPtr<ArrayOf<String> > str = ArrayOf<String>::Alloc(4);
    (*str)[0] = IBaseColumns::ID;
    (*str)[1] = SearchRecentSuggestions::SuggestionColumns::DATE;
    (*str)[2] = SearchRecentSuggestions::SuggestionColumns::QUERY;
    (*str)[3] = SearchRecentSuggestions::SuggestionColumns::DISPLAY1;

    return str;
}

const AutoPtr<ArrayOf<String> > SearchRecentSuggestions::QUERIES_PROJECTION_1LINE =
    initQUERIES_PROJECTION_1LINE();

static AutoPtr<ArrayOf<String> > initQUERIES_PROJECTION_2LINE()
{
    AutoPtr<ArrayOf<String> > str = ArrayOf<String>::Alloc(5);
    (*str)[0] = IBaseColumns::ID;
    (*str)[1] = SearchRecentSuggestions::SuggestionColumns::DATE;
    (*str)[2] = SearchRecentSuggestions::SuggestionColumns::QUERY;
    (*str)[3] = SearchRecentSuggestions::SuggestionColumns::DISPLAY1;
    (*str)[3] = SearchRecentSuggestions::SuggestionColumns::DISPLAY2;

    return str;
}

const AutoPtr<ArrayOf<String> > SearchRecentSuggestions::QUERIES_PROJECTION_2LINE =
    initQUERIES_PROJECTION_2LINE();

const String SearchRecentSuggestions::LOGTAG("SearchSuggestions");

const Int32 SearchRecentSuggestions::MAX_HISTORY_COUNT = 250;

static AutoPtr<ISemaphore> initSemaphore()
{
    AutoPtr<ISemaphore> semph;
    CSemaphore::New(0, (ISemaphore**)&semph);
    return semph;
}

const AutoPtr<ISemaphore> SearchRecentSuggestions::sWritesInProgress = initSemaphore();

CAR_INTERFACE_IMPL(SearchRecentSuggestions, Object, ISearchRecentSuggestions)

SearchRecentSuggestions::SearchRecentSuggestions()
{}

SearchRecentSuggestions::~SearchRecentSuggestions()
{}

ECode SearchRecentSuggestions::constructor(
    /* [in] */ IContext* context,
    /* [in] */ const String& authority,
    /* [in] */ Int32 mode)
{
    if (TextUtils::IsEmpty(authority) || ((mode & ISearchRecentSuggestionsProvider::DATABASE_MODE_QUERIES) == 0)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // unpack mode flags
    mTwoLineDisplay = (0 != (mode & ISearchRecentSuggestionsProvider::DATABASE_MODE_2LINES));

    // saved values
    mContext = context;
    mAuthority = authority;

    // derived values
    Uri::Parse(String("content://") + mAuthority + "/suggestions", (IUri**)&mSuggestionsUri);
    return NOERROR;
}

ECode SearchRecentSuggestions::GetQUERIES_PROJECTION_1LINE(
    /* [out, callee] */ ArrayOf<String>** info)
{
    VALIDATE_NOT_NULL(info);
    *info = QUERIES_PROJECTION_1LINE;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

ECode SearchRecentSuggestions::GetQUERIES_PROJECTION_2LINE(
    /* [out, callee] */ ArrayOf<String>** info)
{
    VALIDATE_NOT_NULL(info);
    *info = QUERIES_PROJECTION_2LINE;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

ECode SearchRecentSuggestions::SaveRecentQuery(
    /* [in] */ const String& queryString,
    /* [in] */ const String& line2)
{
    if (TextUtils::IsEmpty(queryString)) {
        return NOERROR;
    }
    if (!mTwoLineDisplay && !TextUtils::IsEmpty(line2)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IThread> thread;
    CThread::New(String("saveRecentQuery"), (IThread**)&thread);
    return thread->Start();
}

void SearchRecentSuggestions::WaitForSave()
{
    // Acquire writes semaphore until there is nothing available.
    // This is to clean up after any previous callers to saveRecentQuery
    // who did not also call waitForSave().
    Int32 value;
    sWritesInProgress->AvailablePermits(&value);
    do {
        sWritesInProgress->AcquireUninterruptibly();
    } while (value > 0);
}

void SearchRecentSuggestions::SaveRecentQueryBlocking(
    /* [in] */ const String& queryString,
    /* [in] */ const String& line2)
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetCurrentTimeMillis(&now);

    // Use content resolver (not cursor) to insert/update this query
    // try {
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    values->Put(SuggestionColumns::DISPLAY1, queryString);
    if (mTwoLineDisplay) {
        values->Put(SuggestionColumns::DISPLAY2, line2);
    }
    values->Put(SuggestionColumns::QUERY, queryString);
    values->Put(SuggestionColumns::DATE, now);
    AutoPtr<IUri> uri;
    ECode ec = cr->Insert(mSuggestionsUri.Get(), values.Get(), (IUri**)&uri);
    if (!SUCCEEDED(ec)) {
        Slogger::E(LOGTAG, "saveRecentQuery%d", ec);
    }
    // } catch (RuntimeException e) {
    //     Log.e(LOGTAG, "saveRecentQuery", e);
    // }

    // Shorten the list (if it has become too long)
    TruncateHistory(cr.Get(), MAX_HISTORY_COUNT);
}

ECode SearchRecentSuggestions::ClearHistory()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    return TruncateHistory(cr.Get(), 0);
}

ECode SearchRecentSuggestions::TruncateHistory(
    /* [in] */ IContentResolver* cr,
    /* [in] */ Int32 maxEntries)
{
    if (maxEntries < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // try {
    // null means "delete all".  otherwise "delete but leave n newest"
    String selection;
    if (maxEntries > 0) {
        selection = String("_id IN ") +
                "(SELECT _id FROM suggestions" +
                " ORDER BY " + SuggestionColumns::DATE + " DESC" +
                " LIMIT -1 OFFSET " + StringUtils::ToString(maxEntries) + ")";
    }
    Int32 rowsAffected;
    ECode ec = cr->Delete(mSuggestionsUri.Get(), selection, NULL, &rowsAffected);
    FAIL_GOTO(ec, EXIT);
    return NOERROR;
    // } catch (RuntimeException e) {
        // Log.e(LOGTAG, "truncateHistory", e);
    // }
EXIT:
    if (ec == (ECode)E_RUNTIME_EXCEPTION) {
        Slogger::E(LOGTAG, "truncateHistory%d", ec);
    }
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
