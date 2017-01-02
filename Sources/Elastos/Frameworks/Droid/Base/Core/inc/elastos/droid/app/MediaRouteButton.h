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

#ifndef __ELASTOS_DROID_APP_MEDIAROUTE_BUTTON_H__
#define __ELASTOS_DROID_APP_MEDIAROUTE_BUTTON_H__

#include "Elastos.Droid.App.h"
#include "elastos/droid/view/View.h"
//#include "elastos/droid/media/CMediaRouter.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContextWrapper;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Media::IMediaRouter;
//using Elastos::Droid::Media::CMediaRouter;
using Elastos::Droid::Media::IMediaRouterRouteGroup;
using Elastos::Droid::Media::IMediaRouterRouteInfo;
using Elastos::Droid::Media::IMediaRouterCallback;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * The media route action provider displays a {@link MediaRouteButton media route button}
 * in the application's {@link ActionBar} to allow the user to select routes and
 * to control the currently selected route.
 * <p>
 * The application must specify the kinds of routes that the user should be allowed
 * to select by specifying the route types with the {@link #setRouteTypes} method.
 * </p><p>
 * Refer to {@link MediaRouteButton} for a description of the button that will
 * appear in the action bar menu.  Note that instead of disabling the button
 * when no routes are available, the action provider will instead make the
 * menu item invisible.  In this way, the button will only be visible when it
 * is possible for the user to discover and select a matching route.
 * </p>
 */
class MediaRouteButton
    : public Elastos::Droid::View::View
    , public IMediaRouteButton
{
private:
     class MyMediaRouterCallback
        : public Object // CMediaRouter::SimpleCallback
    {
    public:
        MyMediaRouterCallback();

        CARAPI constructor(
            /* [in] */ IMediaRouteButton* provider);

        //@Override
        CARAPI OnRouteAdded(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IMediaRouterRouteInfo* info);

        //@Override
        CARAPI OnRouteRemoved(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IMediaRouterRouteInfo* info);

        //@Override
        CARAPI OnRouteChanged(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IMediaRouterRouteInfo* info);

        //@Override
        CARAPI OnRouteSelected(
            /* [in] */ IMediaRouter* router,
            /* [in] */ Int32 type,
            /* [in] */ IMediaRouterRouteInfo* info);

        //@Override
        CARAPI OnRouteUnselected(
            /* [in] */ IMediaRouter* router,
            /* [in] */ Int32 type,
            /* [in] */ IMediaRouterRouteInfo* info);

        //@Override
        CARAPI OnRouteGrouped(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IMediaRouterRouteInfo* info,
            /* [in] */ IMediaRouterRouteGroup* group,
            /* [in] */ Int32 index);

        //@Override
        CARAPI OnRouteUngrouped(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IMediaRouterRouteInfo* info,
            /* [in] */ IMediaRouterRouteGroup* group);

    private:
        CARAPI CallRefreshRoute();

    private:
        AutoPtr<IWeakReference> mProviderWeak;
    };

public:
    CAR_INTERFACE_DECL()

    MediaRouteButton();

    virtual ~MediaRouteButton();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    /**
     * Gets the media route types for filtering the routes that the user can
     * select using the media route chooser dialog.
     *
     * @return The route types.
     */
    CARAPI GetRouteTypes(
        /* [out] */ Int32* result);

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
     * Show the route chooser or controller dialog.
     * <p>
     * If the default route is selected or if the currently selected route does
     * not match the {@link #getRouteTypes route types}, then shows the route chooser dialog.
     * Otherwise, shows the route controller dialog to offer the user
     * a choice to disconnect from the route or perform other control actions
     * such as setting the route's volume.
     * </p><p>
     * This will attach a {@link DialogFragment} to the containing Activity.
     * </p>
     */
    CARAPI ShowDialog();

    Boolean ShowDialogInternal();

    CARAPI GetActivity(
        /* [out] */ IActivity** activity);

    /**
     * Sets whether to enable showing a toast with the content descriptor of the
     * button when the button is long pressed.
     */
    void SetCheatSheetEnabled(
        /* [in] */ Boolean enable);

    //@Override
    CARAPI PerformClick(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI PerformLongClick(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnCreateDrawableState(
        /* [in] */ Int32 extraSpace,
        /* [out, callee] */ ArrayOf<Int32>** drawableState);

    //@Override
    CARAPI JumpDrawablesToCurrentState();

    //@Override
    CARAPI SetVisibility(
        /* in] */ Int32 visibility);

    //@Override
    CARAPI OnAttachedToWindow();

    //@Override
    CARAPI OnDetachedFromWindow();

    //@Override
    CARAPI DrawableStateChanged();

protected:

    //@Override
    Boolean VerifyDrawable(
        /* [in] */ IDrawable* who);

    //@Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    //@Override
    void OnDraw(
        /* [in] */ ICanvas* canvas);


private:
     void SetRemoteIndicatorDrawable(
        /* [in] */ IDrawable* d);

    void RefreshRoute();

private:
    AutoPtr<IMediaRouter> mRouter;
    AutoPtr<IMediaRouterCallback> mCallback;

    Int32 mRouteTypes;

    Boolean mAttachedToWindow;

    AutoPtr<IDrawable> mRemoteIndicator;
    Boolean mRemoteActive;
    Boolean mCheatSheetEnabled;
    Boolean mIsConnecting;

    Int32 mMinWidth;
    Int32 mMinHeight;

    AutoPtr<IViewOnClickListener> mExtendedSettingsClickListener;

    // The checked state is used when connected to a remote route.
    static const AutoPtr<ArrayOf<Int32> > CHECKED_STATE_SET;

    // The activated state is used while connecting to a remote route.
    static const AutoPtr<ArrayOf<Int32> > ACTIVATED_STATE_SET;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_MEDIAROUTE_BUTTON_H__
