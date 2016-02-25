

package com.android.settings.search;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Provider::ISearchIndexableData;

/**
 * Indexable raw data for Search.
 *
 * This is the raw data used by the Indexer and should match its data model.
 *
 * See {@link Indexable} and {@link android.provider.SearchIndexableResource}.
 */
public class SearchIndexableRaw extends SearchIndexableData {

    /**
     * Title's raw data.
     */
    public String title;

    /**
     * Summary's raw data when the data is "ON".
     */
    public String summaryOn;

    /**
     * Summary's raw data when the data is "OFF".
     */
    public String summaryOff;

    /**
     * Entries associated with the raw data (when the data can have several values).
     */
    public String entries;

    /**
     * Keywords' raw data.
     */
    public String keywords;

    /**
     * Fragment's or Activity's title associated with the raw data.
     */
    public String screenTitle;

    public SearchIndexableRaw(Context context) {
        Super(context);
    }
}
