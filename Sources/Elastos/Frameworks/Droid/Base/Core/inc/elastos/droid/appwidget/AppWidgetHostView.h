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

#ifndef __ELASTOS_DROID_APPWIDGET_APPWIDGETHOSTVIEW_H__
#define __ELASTOS_DROID_APPWIDGET_APPWIDGETHOSTVIEW_H__

#include <Elastos.Droid.AppWidget.h>
#include <Elastos.Droid.Internal.h>
#include "elastos/droid/widget/FrameLayout.h"
#include "elastos/droid/utility/SparseArray.h"

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Internal::AppWidget::IIAppWidgetService;
using Elastos::Droid::Utility::SparseArray;
using Elastos::Droid::View::ILayoutInflaterFilter;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Widget::IRemoteViewsOnClickHandler;
using Elastos::Droid::Widget::IRemoteViews;

namespace Elastos {
namespace Droid {
namespace AppWidget {

class ECO_PUBLIC AppWidgetHostView
    : public FrameLayout
    , public IAppWidgetHostView
{
public:
    class ECO_LOCAL ParcelableSparseArray
        : public SparseArray
        , public IParcelable
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI WriteToParcel(
            /* [in] */ IParcel* dest);

        CARAPI ReadFromParcel(
            /* [in] */ IParcel* source);
    };

private:
    // When we're inflating the initialLayout for a AppWidget, we only allow
    // views that are allowed in RemoteViews.
    class MyLayoutInflaterFilter
        : public Object
        , public ILayoutInflaterFilter
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI OnLoadClass(
            /* [in] */ IClassInfo* clazz,
            /* [out] */ Boolean* allowed);
    };

public:
    CAR_INTERFACE_DECL()

    AppWidgetHostView();

    ~AppWidgetHostView();

    /**
     * Create a host view.  Uses default fade animations.
     */
    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IRemoteViewsOnClickHandler* handler);

    /**
     * Create a host view. Uses specified animations when pushing
     * {@link #updateAppWidget(RemoteViews)}.
     *
     * @param animationIn Resource ID of in animation to use
     * @param animationOut Resource ID of out animation to use
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 animationIn,
        /* [in] */ Int32 animationOut);

    /**
     * Pass the given handler to RemoteViews when updating this widget. Unless this
     * is done immediatly after construction, a call to {@link #updateAppWidget(RemoteViews)}
     * should be made.
     * @param handler
     * @hide
     */
    CARAPI SetOnClickHandler(
        /* [in] */ IRemoteViewsOnClickHandler* handler);

    /**
     * Set the AppWidget that will be displayed by this view. This method also adds default padding
     * to widgets, as described in {@link #getDefaultPaddingForWidget(Context, ComponentName, Rect)}
     * and can be overridden in order to add custom padding.
     */

    CARAPI SetAppWidget(
        /* [in] */ Int32 appWidgetId,
        /* [in] */ IAppWidgetProviderInfo* info);

    /**
     * As of ICE_CREAM_SANDWICH we are automatically adding padding to widgets targeting
     * ICE_CREAM_SANDWICH and higher. The new widget design guidelines strongly recommend
     * that widget developers do not add extra padding to their widgets. This will help
     * achieve consistency among widgets.
     *
     * Note: this method is only needed by developers of AppWidgetHosts. The method is provided in
     * order for the AppWidgetHost to account for the automatic padding when computing the number
     * of cells to allocate to a particular widget.
     *
     * @param context the current context
     * @param component the component name of the widget
     * @param padding Rect in which to place the output, if null, a new Rect will be allocated and
     *                returned
     * @return default padding for this widget, in pixels
     */
    static CARAPI_(AutoPtr<IRect>) GetDefaultPaddingForWidget(
        /* [in] */ IContext* context,
        /* [in] */ IComponentName* component,
        /* [in] */ IRect* padding);

    CARAPI GetAppWidgetId(
        /* [out] */ Int32* appWidgetId);

    CARAPI GetAppWidgetInfo(
        /* [out] */ IAppWidgetProviderInfo** info);

    /**
     * Provide guidance about the size of this widget to the AppWidgetManager. The widths and
     * heights should correspond to the full area the AppWidgetHostView is given. Padding added by
     * the framework will be accounted for automatically. This information gets embedded into the
     * AppWidget options and causes a callback to the AppWidgetProvider.
     * @see AppWidgetProvider#onAppWidgetOptionsChanged(Context, AppWidgetManager, int, Bundle)
     *
     * @param newOptions The bundle of options, in addition to the size information,
     *          can be null.
     * @param minWidth The minimum width in dips that the widget will be displayed at.
     * @param minHeight The maximum height in dips that the widget will be displayed at.
     * @param maxWidth The maximum width in dips that the widget will be displayed at.
     * @param maxHeight The maximum height in dips that the widget will be displayed at.
     *
     */
    CARAPI UpdateAppWidgetSize(
        /* [in] */ IBundle* newOptions,
        /* [in] */ Int32 minWidth,
        /* [in] */ Int32 minHeight,
        /* [in] */ Int32 maxWidth,
        /* [in] */ Int32 maxHeight);

    /**
     * @hide
     */
    CARAPI UpdateAppWidgetSize(
        /* [in] */ IBundle* newOptions,
        /* [in] */ Int32 minWidth,
        /* [in] */ Int32 minHeight,
        /* [in] */ Int32 maxWidth,
        /* [in] */ Int32 maxHeight,
        /* [in] */ Boolean ignorePadding);

    /**
     * Specify some extra information for the widget provider. Causes a callback to the
     * AppWidgetProvider.
     * @see AppWidgetProvider#onAppWidgetOptionsChanged(Context, AppWidgetManager, int, Bundle)
     *
     * @param options The bundle of options information.
     */
    CARAPI UpdateAppWidgetOptions(
        /* [in] */ IBundle* options);

    /** {@inheritDoc} */
    // @Override
    CARAPI GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** params);

    /**
     * Update the AppWidgetProviderInfo for this view, and reset it to the
     * initial layout.
     */
    CARAPI ResetAppWidget(
        /* [in] */ IAppWidgetProviderInfo* info);

    /**
     * Process a set of {@link RemoteViews} coming in as an update from the
     * AppWidget provider. Will animate into these new views as needed
     */
    CARAPI UpdateAppWidget(
        /* [in] */ IRemoteViews* remoteViews);

    /**
     * Process data-changed notifications for the specified view in the specified
     * set of {@link RemoteViews} views.
     */
    CARAPI ViewDataChanged(
        /* [in] */ Int32 viewId);

    // @Override
    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

protected:
    // @Override
    CARAPI DispatchSaveInstanceState(
        /* [in] */ ISparseArray* container);

    // @Override
    CARAPI DispatchRestoreInstanceState(
        /* [in] */ ISparseArray* container);

    // @Override
    CARAPI_(Boolean) DrawChild(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IView* child,
        /* [in] */ Int64 drawingTime);

    /**
     * Prepare the given view to be shown. This might include adjusting
     * {@link FrameLayout.LayoutParams} before inserting.
     */
    CARAPI_(void) PrepareView(
        /* [in] */ IView* view);

    /**
     * Inflate and return the default layout requested by AppWidget provider.
     */
    CARAPI_(AutoPtr<IView>) GetDefaultView();

    /**
     * Inflate and return a view that represents an error state.
     */
    CARAPI_(AutoPtr<IView>) GetErrorView();

private:
    CARAPI_(Int32) GenerateId();

    /**
     * Build a {@link Context} cloned into another package name, usually for the
     * purposes of reading remote resources.
     */
    CARAPI_(AutoPtr<IContext>) GetRemoteContext();

protected:
    static const String TAG;
    static const Boolean LOGD;
    static const Boolean CROSSFADE;

    static const Int32 VIEW_MODE_NOINIT = 0;
    static const Int32 VIEW_MODE_CONTENT = 1;
    static const Int32 VIEW_MODE_ERROR = 2;
    static const Int32 VIEW_MODE_DEFAULT = 3;

    static const Int32 FADE_DURATION = 1000;

    // When we're inflating the initialLayout for a AppWidget, we only allow
    // views that are allowed in RemoteViews.
    static AutoPtr<ILayoutInflaterFilter> sInflaterFilter;

    AutoPtr<IContext> mRemoteContext;

    Int32 mAppWidgetId;
    AutoPtr<IAppWidgetProviderInfo> mInfo;
    AutoPtr<IView> mView;
    Int32 mViewMode;
    Int32 mLayoutId;
    Int64 mFadeStartTime;
    AutoPtr<IBitmap> mOld;
    AutoPtr<IPaint> mOldPaint;

private:
    AutoPtr<IRemoteViewsOnClickHandler> mOnClickHandler;
};

} // namespace AppWidget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APPWIDGET_APPWIDGETHOSTVIEW_H__
