#ifndef __ELASTOS_DROID_SETTINGS_PINNEDHEADERLISTFRAGMENT_H__
#define __ELASTOS_DROID_SETTINGS_PINNEDHEADERLISTFRAGMENT_H__

#include "elastos/droid/app/ListFragment.h"

using Elastos::Droid::App::ListFragment;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Settings {

/**
 * A ListFragment with a pinned header
 */
class PinnedHeaderListFragment
    : public ListFragment
{
public:
    PinnedHeaderListFragment();

    virtual ~PinnedHeaderListFragment();

    CARAPI constructor();

    /**
     * Set the pinned header view. This can only be done when the ListView is already created.
     *
     * @param pinnedHeaderView the view to be used for the pinned header view.
     */
    virtual CARAPI SetPinnedHeaderView(
        /* [in] */ IView* pinnedHeaderView);

    /**
     * Clear the pinned header view. This can only be done when the ListView is already created.
     */
    virtual CARAPI ClearPinnedHeaderView();
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_PINNEDHEADERLISTFRAGMENT_H__