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

#ifndef __ELASTOS_APPS_CONTACTS_COMMON_UTIL_VIEWUTIL_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_UTIL_VIEWUTIL_H__


namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Util {

/**
 * Provides static functions to work with views
 */
class ECO_PUBLIC ViewUtil
    : public Object
{
private:
    class OVALViewOutlineProvide
        : public Object
        , public IViewOutlineProvider
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI GetOutline(
            /* [in] */ IView* view,
            /* [in] */ IOutline* outline);
    };

    class RECTViewOutlineProvide
        : public Object
        , public IViewOutlineProvider
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI GetOutline(
            /* [in] */ IView* view,
            /* [in] */ IOutline* outline);
    };

public:
    static CARAPI GetConstantPreLayoutWidth(
        /* [in] */ IView* view,
        /* [out] */ Int32* result);

    static CARAPI_(Boolean) IsViewLayoutRtl(
        /* [in] */ IView* view);

    static CARAPI_(void) AddRectangularOutlineProvider(
        /* [in] */ IView* view,
        /* [in] */ IResources* res);

    static CARAPI_(void) SetupFloatingActionButton(
        /* [in] */ IView* view,
        /* [in] */ IResources* res);

    static CARAPI_(void) AddBottomPaddingToListViewForFab(
        /* [in] */ IListView* listView,
        /* [in] */ IResources* res);

private:
    static const AutoPtr<IViewOutlineProvider> OVAL_OUTLINE_PROVIDER;
    static const AutoPtr<IViewOutlineProvider> RECT_OUTLINE_PROVIDER;
};

} // Util
} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_UTIL_VIEWUTIL_H__
