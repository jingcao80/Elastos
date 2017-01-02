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

#ifndef __ELASTOS_DROID_SETTINGS_SEARCH_SEARCHINDEXABLERAW_H__
#define __ELASTOS_DROID_SETTINGS_SEARCH_SEARCHINDEXABLERAW_H__

#include "elastos/droid/provider/SearchIndexableData.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Provider::SearchIndexableData;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Search {

/**
 * Indexable raw data for Search.
 *
 * This is the raw data used by the Indexer and should match its data model.
 *
 * See {@link Indexable} and {@link android.provider.SearchIndexableResource}.
 */
class SearchIndexableRaw
    : public SearchIndexableData
    , public ISearchIndexableRaw
{
public:
    CAR_INTERFACE_DECL();

    SearchIndexableRaw();

    ~SearchIndexableRaw();

    CARAPI constructor(
        /* [in] */ IContext* context);

public:
    /**
     * Title's raw data.
     */
    String mTitle;

    /**
     * Summary's raw data when the data is "ON".
     */
    String mSummaryOn;

    /**
     * Summary's raw data when the data is "OFF".
     */
    String mSummaryOff;

    /**
     * Entries associated with the raw data (when the data can have several values).
     */
    String mEntries;

    /**
     * Keywords' raw data.
     */
    String mKeywords;

    /**
     * Fragment's or Activity's title associated with the raw data.
     */
    String mScreenTitle;
};

} // namespace Search
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_SEARCH_SEARCHINDEXABLERAW_H__
