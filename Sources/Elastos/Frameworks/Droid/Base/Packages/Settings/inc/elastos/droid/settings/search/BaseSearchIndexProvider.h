

package com.android.settings.search;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Provider::ISearchIndexableResource;

using Elastos::Utility::ICollections;
using Elastos::Utility::IList;

/**
 * A basic SearchIndexProvider that returns no data to index.
 */
public class BaseSearchIndexProvider implements Indexable.SearchIndexProvider {

    private static const List<String> EMPTY_LIST = Collections.<String>EmptyList();

    public BaseSearchIndexProvider() {
    }

    //@Override
    public List<SearchIndexableResource> GetXmlResourcesToIndex(Context context, Boolean enabled) {
        return NULL;
    }

    //@Override
    public List<SearchIndexableRaw> GetRawDataToIndex(Context context, Boolean enabled) {
        return NULL;
    }

    //@Override
    public List<String> GetNonIndexableKeys(Context context) {
        return EMPTY_LIST;
    }
}
