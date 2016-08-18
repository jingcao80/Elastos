
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_AUTOSCROLLLISTVIEW_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_AUTOSCROLLLISTVIEW_H__

#include "Elastos.Droid.Widget.h"
#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/widget/ListView.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Widget::ListView;

namespace Elastos{
namespace Droid{
namespace Contacts {
namespace Common {
namespace List {

/**
 * A ListView that can be asked to scroll (smoothly or otherwise) to a specific
 * position.  This class takes advantage of similar functionality that exists
 * in {@link ListView} and enhances it.
 */
class AutoScrollListView
    : public ListView
    , public IAutoScrollListView
{
public:
    AutoScrollListView()
        : mRequestedScrollPosition(-1)
        , mSmoothScrollRequested(FALSE)
    {}

    virtual ~AutoScrollListView();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    /**
     * Brings the specified position to view by optionally performing a jump-scroll maneuver:
     * first it jumps to some position near the one requested and then does a smooth
     * scroll to the requested position.  This creates an impression of full smooth
     * scrolling without actually traversing the entire list.  If smooth scrolling is
     * not requested, instantly positions the requested item at a preferred offset.
     */
    CARAPI RequestPositionToScreen(
        /* [in] */ Int32 position,
        /* [in] */ Boolean smoothScroll);

protected:
    // @Override
    CARAPI LayoutChildren();

private:
    /**
     * Position the element at about 1/3 of the list height
     */
    static const Float PREFERRED_SELECTION_OFFSET_FROM_TOP = 0.33f;

    Int32 mRequestedScrollPosition;
    Boolean mSmoothScrollRequested;
};

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_AUTOSCROLLLISTVIEW_H__
