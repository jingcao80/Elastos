
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTLISTITEMVIEW_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTLISTITEMVIEW_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/view/ViewGroup.h"

using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::Widget::ISelectionBoundsAdjuster;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

/**
 * A custom view for an item in the contact list.
 * The view contains the contact's photo, a set of text views (for name, status, etc...) and
 * icons for presence and call.
 * The view uses no XML file for layout and all the measurements and layouts are done
 * in the onMeasure and onLayout methods.
 *
 * The layout puts the contact's photo on the right side of the view, the call icon (if present)
 * to the left of the photo, the text lines are aligned to the left and the presence icon (if
 * present) is set to the left of the status line.
 *
 * The layout also supports a header (used as a header of a group of contacts) that is above the
 * contact's data and a divider between contact view.
 */
class ContactListItemView
    : public ViewGroup
    , public IContactListItemView
    , public ISelectionBoundsAdjuster
{
public:
    ContactListItemView();

    CAR_INTERFACE_DECL()

    static CARAPI_(PhotoPosition) GetDefaultPhotoPosition(
        /* [in] */ Boolean opposite);

private:
    // Style values for layout and appearance
    // The initialized values are defaults if none is provided through xml.
    private int mPreferredHeight = 0;
    private int mGapBetweenImageAndText = 0;
    private int mGapBetweenLabelAndData = 0;
    private int mPresenceIconMargin = 4;
    private int mPresenceIconSize = 16;
    private int mTextIndent = 0;
    private int mTextOffsetTop;
    private int mNameTextViewTextSize;
    private int mHeaderWidth;
    private Drawable mActivatedBackgroundDrawable;

    // Set in onLayout. Represent left and right position of the View on the screen.
    private int mLeftOffset;
    private int mRightOffset;

    /**
     * Used with {@link #mLabelView}, specifying the width ratio between label and data.
     */
    private int mLabelViewWidthWeight = 3;
    /**
     * Used with {@link #mDataView}, specifying the width ratio between label and data.
     */
    private int mDataViewWidthWeight = 5;
};

} // List
} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTLISTITEMVIEW_H__
