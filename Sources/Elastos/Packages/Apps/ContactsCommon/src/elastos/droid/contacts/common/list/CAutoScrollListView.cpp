
#include "CAutoScrollListView.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace List {

const Float CAutoScrollListView::PREFERRED_SELECTION_OFFSET_FROM_TOP = 0.33f;

CAR_INTERFACE_IMPL(CAutoScrollListView, Object, IAutoScrollListView)

CAR_OBJECT_IMPL(CAutoScrollListView)

CAutoScrollListView::CAutoScrollListView()
    : mRequestedScrollPosition(-1)
{}

CAutoScrollListView::~CAutoScrollListView()
{}

ECode CAutoScrollListView::constructor(
    /* [in] */ IContext* context)
{
    return ListView::constructor(context);
}

ECode CAutoScrollListView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return ListView::constructor(context, attrs);
}

ECode CAutoScrollListView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return ListView::constructor(context, attrs, defStyle);
}

ECode CAutoScrollListView::RequestPositionToScreen(
    /* [in] */ Int32 position,
    /* [in] */ Boolean smoothScroll)
{
    mRequestedScrollPosition = position;
    mSmoothScrollRequested = smoothScroll;
    RequestLayout();

    return NOERROR;
}

ECode CAutoScrollListView::LayoutChildren()
{
    ListView::LayoutChildren();
    if (mRequestedScrollPosition == -1) {
        return NOERROR;
    }

    Int32 position = mRequestedScrollPosition;
    mRequestedScrollPosition = -1;

    Int32 firstPosition = GetFirstVisiblePosition() + 1;
    Int32 lastPosition = GetLastVisiblePosition();
    if (position >= firstPosition && position <= lastPosition) {
        return NOERROR; // Already on screen
    }

    Int32 height;
    GetHeight(&height);
    Int32 offset = (Int32) (height * PREFERRED_SELECTION_OFFSET_FROM_TOP);
    if (!mSmoothScrollRequested) {
        SetSelectionFromTop(position, offset);

        // Since we have changed the scrolling position, we need to redo child layout
        // Calling "requestLayout" in the middle of a layout pass has no effect,
        // so we call layoutChildren explicitly
        ListView::LayoutChildren();

    }
    else {
        // We will first position the list a couple of screens before or after
        // the new selection and then scroll smoothly to it.
        Int32 twoScreens = (lastPosition - firstPosition) * 2;
        Int32 preliminaryPosition;
        if (position < firstPosition) {
            preliminaryPosition = position + twoScreens;
            Int32 count;
            GetCount(&count);
            if (preliminaryPosition >= count) {
                preliminaryPosition = count - 1;
            }
            if (preliminaryPosition < firstPosition) {
                SetSelection(preliminaryPosition);
                ListView::LayoutChildren();
            }
        }
        else {
            preliminaryPosition = position - twoScreens;
            if (preliminaryPosition < 0) {
                preliminaryPosition = 0;
            }
            if (preliminaryPosition > lastPosition) {
                SetSelection(preliminaryPosition);
                ListView::LayoutChildren();
            }
        }


        SmoothScrollToPositionFromTop(position, offset);
    }

    return NOERROR;
}

} // List
} // Common
} // Contacts
} // Apps
} // Elastos
