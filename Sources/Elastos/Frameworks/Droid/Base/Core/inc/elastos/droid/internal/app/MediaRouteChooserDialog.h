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

#ifndef __ELASTOS_DROID_INTERNAL_APP_MEDIAROUTECHOOSERDIALOG_H__
#define __ELASTOS_DROID_INTERNAL_APP_MEDIAROUTECHOOSERDIALOG_H__

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Media.h"
#include "elastos/droid/app/Dialog.h"
#include "elastos/droid/widget/ArrayAdapter.h"

using Elastos::Droid::App::Dialog;
using Elastos::Droid::Media::IMediaRouter;
using Elastos::Droid::Media::IMediaRouterCallback;
using Elastos::Droid::Media::IMediaRouterRouteInfo;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::ArrayAdapter;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Core::IComparator;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

class MediaRouteChooserDialog
    : public Dialog
    , public IMediaRouteChooserDialog
{
private:
    class RouteAdapter
        : public ArrayAdapter
        , public IAdapterViewOnItemClickListener
    {
    public:
        RouteAdapter(
            /* [in] */ IContext* context,
            /* [in] */ MediaRouteChooserDialog* host);

        CAR_INTERFACE_DECL()

        CARAPI Update();

        // @Override
        CARAPI AreAllItemsEnabled(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI IsEnabled(
            /* [in] */ Int32 position,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** retView);

        // @Override
        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

    private:
        AutoPtr<ILayoutInflater> mInflater;
        MediaRouteChooserDialog* mHost;
    };

    class MediaRouterCallback
        : public Object
        // : public MediaRouter::SimpleCallback
    {
    public:
        MediaRouterCallback(
            /* [in] */ MediaRouteChooserDialog* host);

        // @Override
        CARAPI OnRouteAdded(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IMediaRouterRouteInfo* info);

        // @Override
        CARAPI OnRouteRemoved(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IMediaRouterRouteInfo* info);

        // @Override
        CARAPI OnRouteChanged(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IMediaRouterRouteInfo* info);

        // @Override
        CARAPI OnRouteSelected(
            /* [in] */ IMediaRouter* router,
            /* [in] */ Int32 type,
            /* [in] */ IMediaRouterRouteInfo* info);

    private:
        MediaRouteChooserDialog* mHost;
    };

    class RouteComparator
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL()

        // @Override
        CARAPI Compare(
            /* [in] */ IInterface* _lhs,
            /* [in] */ IInterface* _rhs,
            /* [out] */ Int32* result);

    public:
        static const AutoPtr<RouteComparator> sInstance;
    };

public:
    MediaRouteChooserDialog();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 theme);

    /**
     * Gets the media route types for filtering the routes that the user can
     * select using the media route chooser dialog.
     *
     * @return The route types.
     */
    CARAPI GetRouteTypes(
        /* [out] */ Int32* types);

    /**
     * Sets the types of routes that will be shown in the media route chooser dialog
     * launched by this button.
     *
     * @param types The route types to match.
     */
    CARAPI SetRouteTypes(
        /* [in] */ Int32 types);

    CARAPI SetExtendedSettingsClickListener(
        /* [in] */ IViewOnClickListener* listener);

    /**
     * Returns true if the route should be included in the list.
     * <p>
     * The default implementation returns true for enabled non-default routes that
     * match the route types.  Subclasses can override this method to filter routes
     * differently.
     * </p>
     *
     * @param route The route to consider, never null.
     * @return True if the route should be included in the chooser dialog.
     */
    CARAPI OnFilterRoute(
        /* [in] */ IMediaRouterRouteInfo* route,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnAttachedToWindow();

    // @Override
    CARAPI OnDetachedFromWindow();

    /**
     * Refreshes the list of routes that are shown in the chooser dialog.
     */
    CARAPI RefreshRoutes();

protected:
    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    CARAPI_(void) UpdateExtendedSettingsButton();

private:
    AutoPtr<IMediaRouter> mRouter;
    AutoPtr<IMediaRouterCallback> mCallback;

    Int32 mRouteTypes;
    AutoPtr<IViewOnClickListener> mExtendedSettingsClickListener;
    AutoPtr<RouteAdapter> mAdapter;
    AutoPtr<IListView> mListView;
    AutoPtr<IButton> mExtendedSettingsButton;
    Boolean mAttachedToWindow;
};

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_MEDIAROUTECHOOSERDIALOG_H__
