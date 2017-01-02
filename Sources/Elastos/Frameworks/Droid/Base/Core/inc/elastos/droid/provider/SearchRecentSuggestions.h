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

#ifndef __ELASTOS_DROID_PROVIDER_SEARCHRECENTSUGGESTIONS_H__
#define __ELASTOS_DROID_PROVIDER_SEARCHRECENTSUGGESTIONS_H__

#include "Elastos.Droid.Provider.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;
using Elastos::Utility::Concurrent::ISemaphore;

namespace Elastos {
namespace Droid {
namespace Provider {

class SearchRecentSuggestions
    : public Object
    , public ISearchRecentSuggestions
{
public: // private
    class SuggestionColumns
    {
    public:
        static const String DISPLAY1;
        static const String DISPLAY2;
        static const String QUERY;
        static const String DATE;
    };

public:
    CAR_INTERFACE_DECL()

    SearchRecentSuggestions();

    virtual ~SearchRecentSuggestions();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ const String& authority,
        /* [in] */ Int32 mode);

    CARAPI GetQUERIES_PROJECTION_1LINE(
        /* [out, callee] */ ArrayOf<String>** info);

    CARAPI GetQUERIES_PROJECTION_2LINE(
        /* [out, callee] */ ArrayOf<String>** info);

    CARAPI SaveRecentQuery(
        /* [in] */ const String& queryString,
        /* [in] */ const String& line2);

    CARAPI ClearHistory();

    // Visible for testing.
    CARAPI_(void) WaitForSave();

protected:
    CARAPI TruncateHistory(
        /* [in] */ IContentResolver* cr,
        /* [in] */ Int32 maxEntries);

private:
    CARAPI_(void) SaveRecentQueryBlocking(
        /* [in] */ const String& queryString,
        /* [in] */ const String& line2);

public:
    static const AutoPtr<ArrayOf<String> > QUERIES_PROJECTION_1LINE;
    static const AutoPtr<ArrayOf<String> > QUERIES_PROJECTION_2LINE;

private:
    // debugging support
    static const String LOGTAG;
    static const Int32 MAX_HISTORY_COUNT;
    static const AutoPtr<ISemaphore> sWritesInProgress;
    AutoPtr<IContext> mContext;
    String mAuthority;
    Boolean mTwoLineDisplay;
    AutoPtr<IUri> mSuggestionsUri;
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_SEARCHRECENTSUGGESTIONS_H__
