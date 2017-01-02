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

#ifndef __ELASTOS_APPS_CONTACTS_COMMON_UTIL_CVIEWUTIL_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_UTIL_CVIEWUTIL_H__

#include "_Elastos_Apps_Contacts_Common_Util_CViewUtil.h"
#include <elastos/core/Singleton.h>

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Util {

CarClass(CViewUtil)
    , public Singleton
    , public IViewUtil
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns the width as specified in the LayoutParams
     * @throws IllegalStateException Thrown if the view's width is unknown before a layout pass
     * s
     */
    CARAPI GetConstantPreLayoutWidth(
        /* [in] */ IView* view,
        /* [out] */ Int32* width);

    /**
     * Returns a boolean indicating whether or not the view's layout direction is RTL
     *
     * @param view - A valid view
     * @return True if the view's layout direction is RTL
     */
    CARAPI IsViewLayoutRtl(
        /* [in] */ IView* view,
        /* [out] */ Boolean* result);

    /**
     * Adds a rectangular outline to a view. This can be useful when you want to add a shadow
     * to a transparent view. See b/16856049.
     * @param view view that the outline is added to
     * @param res The resources file.
     */
    CARAPI AddRectangularOutlineProvider(
        /* [in] */ IView* view,
        /* [in] */ IResources* res);

    /**
     * Configures the floating action button, clipping it to a circle and setting its translation z.
     * @param view The float action button's view.
     * @param res The resources file.
     */
    CARAPI SetupFloatingActionButton(
        /* [in] */ IView* view,
        /* [in] */ IResources* res);

    /**
     * Adds padding to the bottom of the given {@link ListView} so that the floating action button
     * does not obscure any content.
     *
     * @param listView to add the padding to
     * @param res valid resources object
     */
    CARAPI AddBottomPaddingToListViewForFab(
        /* [in] */ IListView* listView,
        /* [in] */ IResources* res);
};

} // Util
} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_UTIL_CVIEWUTIL_H__
