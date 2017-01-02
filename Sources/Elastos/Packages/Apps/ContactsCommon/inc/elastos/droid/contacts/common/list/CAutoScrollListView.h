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

#ifndef __ELASTOS_APPS_CONTACTS_COMMON_LIST_CAUTOSCROLLLISTVIEW_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_LIST_CAUTOSCROLLLISTVIEW_H__

#include "_Elastos_Apps_Contacts_Common_List_CAutoScrollListView.h"

using Elastos::Droid::Widget::ListView;

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace List {

/**
 * A ListView that can be asked to scroll (smoothly or otherwise) to a specific
 * position.  This class takes advantage of similar functionality that exists
 * in {@link ListView} and enhances it.
 */
CarClass(CAutoScrollListView)
    , public ListView
    , public IAutoScrollListView
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAutoScrollListView();

    virtual ~CAutoScrollListView();

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
    static const Float PREFERRED_SELECTION_OFFSET_FROM_TOP; // = 0.33f;

    Int32 mRequestedScrollPosition; // = -1;
    Boolean mSmoothScrollRequested;
};

} // List
} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_LIST_CAUTOSCROLLLISTVIEW_H__
