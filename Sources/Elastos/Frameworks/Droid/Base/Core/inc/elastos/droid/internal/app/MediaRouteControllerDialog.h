
#ifndef __ELASTOS_DROID_INTERNAL_APP_MEDIAROUTECONTROLLERDIALOG_H__
#define __ELASTOS_DROID_INTERNAL_APP_MEDIAROUTECONTROLLERDIALOG_H__

#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/app/Dialog.h"

using Elastos::Droid::App::Dialog;
using Elastos::Droid::Media::IMediaRouter;
using Elastos::Droid::Media::IMediaRouterCallback;
using Elastos::Droid::Media::IMediaRouterRouteInfo;
using Elastos::Droid::Media::IMediaRouterRouteGroup;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::ISeekBar;
using Elastos::Droid::Widget::ISeekBarOnSeekBarChangeListener;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

class MediaRouteControllerDialog
    : public Dialog
    , public IMediaRouteControllerDialog
{
private:
    class MediaRouterCallback
        : public Object
        // : public MediaRouter::SimpleCallback
    {
    public:
        MediaRouterCallback(
            /* [in] */ MediaRouteControllerDialog* host);

        // @Override
        CARAPI OnRouteUnselected(
            /* [in] */ IMediaRouter* router,
            /* [in] */ Int32 type,
            /* [in] */ IMediaRouterRouteInfo* info);

        // @Override
        CARAPI OnRouteChanged(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IMediaRouterRouteInfo* route);

        // @Override
        CARAPI OnRouteVolumeChanged(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IMediaRouterRouteInfo* route);;

        // @Override
        CARAPI OnRouteGrouped(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IMediaRouterRouteInfo* info,
            /* [in] */ IMediaRouterRouteGroup* group,
            /* [in] */ Int32 index);

        // @Override
        CARAPI OnRouteUngrouped(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IMediaRouterRouteInfo* info,
            /* [in] */ IMediaRouterRouteGroup* group);

    private:
        MediaRouteControllerDialog* mHost;
    };


    class OnSeekBarChangeListener
        : public Object
        , public ISeekBarOnSeekBarChangeListener
    {
    private:
        class StopTrackingTouch : public Runnable
        {
        public:
            StopTrackingTouch(
                /* [in] */ MediaRouteControllerDialog* host);

            CARAPI Run();

        private:
            MediaRouteControllerDialog* mHost;
        };

    public:
        OnSeekBarChangeListener(
            /* [in] */ MediaRouteControllerDialog* host);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnStartTrackingTouch(
            /* [in] */ ISeekBar* seekBar);

        // @Override
        CARAPI OnStopTrackingTouch(
            /* [in] */ ISeekBar* seekBar);

        // @Override
        CARAPI OnProgressChanged(
            /* [in] */ ISeekBar* seekBar,
            /* [in] */ Int32 progress,
            /* [in] */ Boolean fromUser);

    private:
        AutoPtr<Runnable> mStopTrackingTouch;
        MediaRouteControllerDialog* mHost;
    };

    class ViewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        ViewOnClickListener(
            /* [in] */ MediaRouteControllerDialog* host);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        MediaRouteControllerDialog* mHost;
    };

public:
    MediaRouteControllerDialog();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 theme);

    /**
     * Gets the route that this dialog is controlling.
     */
    CARAPI GetRoute(
        /* [out] */ IMediaRouterRouteInfo** route);

    /**
     * Provides the subclass an opportunity to create a view that will
     * be included within the body of the dialog to offer additional media controls
     * for the currently playing content.
     *
     * @param savedInstanceState The dialog's saved instance state.
     * @return The media control view, or NULL if none.
     */
    CARAPI OnCreateMediaControlView(
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** view);

    /**
     * Gets the media control view that was created by {@link #onCreateMediaControlView(Bundle)}.
     *
     * @return The media control view, or NULL if none.
     */
    CARAPI GetMediaControlView(
        /* [out] */ IView** view);

    /**
     * Sets whether to enable the volume slider and volume control using the volume keys
     * when the route supports it.
     * <p>
     * The default value is TRUE.
     * </p>
     */
    CARAPI SetVolumeControlEnabled(
        /* [in] */ Boolean enable);

    /**
     * Returns whether to enable the volume slider and volume control using the volume keys
     * when the route supports it.
     */
    CARAPI IsVolumeControlEnabled(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnAttachedToWindow();

    // @Override
    CARAPI OnDetachedFromWindow();

    // @Override
    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

protected:
    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    CARAPI_(Boolean) Update();

    CARAPI_(AutoPtr<IDrawable>) GetIconDrawable();

    CARAPI_(void) UpdateVolume();

    CARAPI_(Boolean) IsVolumeControlAvailable();

private:
    // Time to wait before updating the volume when the user lets go of the seek bar
    // to allow the route provider time to propagate the change and publish a new
    // route descriptor.
    static const Int32 VOLUME_UPDATE_DELAY_MILLIS;

    AutoPtr<IMediaRouter> mRouter;
    AutoPtr<IMediaRouterCallback> mCallback;
    AutoPtr<IMediaRouterRouteInfo> mRoute;

    Boolean mCreated;
    AutoPtr<IDrawable> mMediaRouteConnectingDrawable;
    AutoPtr<IDrawable> mMediaRouteOnDrawable;
    AutoPtr<IDrawable> mCurrentIconDrawable;

    Boolean mVolumeControlEnabled;
    AutoPtr<ILinearLayout> mVolumeLayout;
    AutoPtr<ISeekBar> mVolumeSlider;
    Boolean mVolumeSliderTouched;

    AutoPtr<IView> mControlView;
    AutoPtr<IButton> mDisconnectButton;
};

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_MEDIAROUTECONTROLLERDIALOG_H__
