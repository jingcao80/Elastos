

package com.android.settings;

using Elastos::Droid::App::IListFragment;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;

/**
 * A ListFragment with a pinned header
 */
public class PinnedHeaderListFragment extends ListFragment {

    public PinnedHeaderListFragment() {
        Super();
    }

    /**
     * Set the pinned header view. This can only be done when the ListView is already created.
     *
     * @param pinnedHeaderView the view to be used for the pinned header view.
     */
    CARAPI SetPinnedHeaderView(View pinnedHeaderView) {
        ((ViewGroup) GetListView()->GetParent()).AddView(pinnedHeaderView, 0);
    }

    /**
     * Clear the pinned header view. This can only be done when the ListView is already created.
     */
    CARAPI ClearPinnedHeaderView() {
        ((ViewGroup) GetListView()->GetParent()).RemoveViewAt(0);
    }
}
