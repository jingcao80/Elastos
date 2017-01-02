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

#include "elastos/droid/contacts/common/list/AutoScrollListView.h"

using Elastos::Droid::Contacts::Common::List::EIID_IAutoScrollListView;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

const Float AutoScrollListView::PREFERRED_SELECTION_OFFSET_FROM_TOP;

CAR_INTERFACE_IMPL(AutoScrollListView, ListView, IAutoScrollListView)

ECode AutoScrollListView::constructor(
    /* [in] */ IContext* context)
{
    return ListView::constructor(context);
}

ECode AutoScrollListView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return ListView::constructor(context, attrs);
}

ECode AutoScrollListView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return ListView::constructor(context, attrs, defStyle);
}

ECode AutoScrollListView::RequestPositionToScreen(
    /* [in] */ Int32 position,
    /* [in] */ Boolean smoothScroll)
{
    mRequestedScrollPosition = position;
    mSmoothScrollRequested = smoothScroll;
    return RequestLayout();
}

ECode AutoScrollListView::LayoutChildren()
{
    FAIL_RETURN(ListView::LayoutChildren())
    if (mRequestedScrollPosition == -1) {
        return NOERROR;
    }

    Int32 position = mRequestedScrollPosition;
    mRequestedScrollPosition = -1;

    Int32 firstPos;
    GetFirstVisiblePosition(&firstPos);
    Int32 firstPosition = firstPos + 1;
    Int32 lastPosition;
    GetLastVisiblePosition(&lastPosition);
    if (position >= firstPosition && position <= lastPosition) {
        return NOERROR; // Already on screen
    }

    Int32 height;
    GetHeight(&height);
    Int32 offset = (Int32)(height * PREFERRED_SELECTION_OFFSET_FROM_TOP);
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
            if (GetCount(&count), preliminaryPosition >= count) {
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
} // Droid
} // Elastos
