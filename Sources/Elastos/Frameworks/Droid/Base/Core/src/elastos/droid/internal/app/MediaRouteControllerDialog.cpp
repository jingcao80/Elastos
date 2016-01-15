
#include "Elastos.Droid.Media.h"
#include "elastos/droid/internal/app/MediaRouteControllerDialog.h"
#include "elastos/droid/R.h"

using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::EIID_ISeekBarOnSeekBarChangeListener;
using Elastos::Droid::Widget::IFrameLayout;
using Elastos::Droid::Widget::IProgressBar;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

MediaRouteControllerDialog::MediaRouterCallback::MediaRouterCallback(
    /* [in] */ MediaRouteControllerDialog* host)
    : mHost(host)
{}

ECode MediaRouteControllerDialog::MediaRouterCallback::OnRouteUnselected(
    /* [in] */ IMediaRouter* router,
    /* [in] */ Int32 type,
    /* [in] */ IMediaRouterRouteInfo* info)
{
    mHost->Update();
    return NOERROR;
}

ECode MediaRouteControllerDialog::MediaRouterCallback::OnRouteChanged(
    /* [in] */ IMediaRouter* router,
    /* [in] */ IMediaRouterRouteInfo* route)
{
    mHost->Update();
    return NOERROR;
}

ECode MediaRouteControllerDialog::MediaRouterCallback::OnRouteVolumeChanged(
    /* [in] */ IMediaRouter* router,
    /* [in] */ IMediaRouterRouteInfo* route)
{
    if (route == mHost->mRoute) {
        mHost->UpdateVolume();
    }
    return NOERROR;
}

ECode MediaRouteControllerDialog::MediaRouterCallback::OnRouteGrouped(
    /* [in] */ IMediaRouter* router,
    /* [in] */ IMediaRouterRouteInfo* info,
    /* [in] */ IMediaRouterRouteGroup* group,
    /* [in] */ Int32 index)
{
    mHost->Update();
    return NOERROR;
}

ECode MediaRouteControllerDialog::MediaRouterCallback::OnRouteUngrouped(
    /* [in] */ IMediaRouter* router,
    /* [in] */ IMediaRouterRouteInfo* info,
    /* [in] */ IMediaRouterRouteGroup* group)
{
    mHost->Update();
    return NOERROR;
}

MediaRouteControllerDialog::OnSeekBarChangeListener::StopTrackingTouch::StopTrackingTouch(
    /* [in] */ MediaRouteControllerDialog* host)
    : mHost(host)
{}

ECode MediaRouteControllerDialog::OnSeekBarChangeListener::StopTrackingTouch::Run()
{
    if (mHost->mVolumeSliderTouched) {
        mHost->mVolumeSliderTouched = FALSE;
        mHost->UpdateVolume();
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(MediaRouteControllerDialog::OnSeekBarChangeListener, Object,
    ISeekBarOnSeekBarChangeListener)

MediaRouteControllerDialog::OnSeekBarChangeListener::OnSeekBarChangeListener(
    /* [in] */ MediaRouteControllerDialog* host)
    : mHost(host)
{
    mStopTrackingTouch = new StopTrackingTouch(host);
}

ECode MediaRouteControllerDialog::OnSeekBarChangeListener::OnStartTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    if (mHost->mVolumeSliderTouched) {
        Boolean res;
        IView::Probe(mHost->mVolumeSlider)->RemoveCallbacks(mStopTrackingTouch, &res);
    }
    else {
        mHost->mVolumeSliderTouched = TRUE;
    }
    return NOERROR;
}

ECode MediaRouteControllerDialog::OnSeekBarChangeListener::OnStopTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    // Defer resetting mVolumeSliderTouched to allow the media route provider
    // a little time to settle into its new state and publish the final
    // volume update.
    Boolean res;
    IView::Probe(mHost->mVolumeSlider)->PostDelayed(
        mStopTrackingTouch, VOLUME_UPDATE_DELAY_MILLIS, &res);
    return NOERROR;
}

ECode MediaRouteControllerDialog::OnSeekBarChangeListener::OnProgressChanged(
    /* [in] */ ISeekBar* seekBar,
    /* [in] */ Int32 progress,
    /* [in] */ Boolean fromUser)
{
    if (fromUser) {
        mHost->mRoute->RequestSetVolume(progress);
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(MediaRouteControllerDialog::ViewOnClickListener, Object,
    IViewOnClickListener)

MediaRouteControllerDialog::ViewOnClickListener::ViewOnClickListener(
    /* [in] */ MediaRouteControllerDialog* host)
    : mHost(host)
{
}

ECode MediaRouteControllerDialog::ViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    Boolean isSelected;
    mHost->mRoute->IsSelected(&isSelected);
    if (isSelected) {
        AutoPtr<IMediaRouterRouteInfo> route;
        mHost->mRouter->GetDefaultRoute((IMediaRouterRouteInfo**)&route);
        route->Select();
    }
    return mHost->Dismiss();
}

const Int32 MediaRouteControllerDialog::VOLUME_UPDATE_DELAY_MILLIS = 250;

CAR_INTERFACE_IMPL(MediaRouteControllerDialog, Dialog, IMediaRouteControllerDialog)

MediaRouteControllerDialog::MediaRouteControllerDialog()
    : mCreated(FALSE)
    , mVolumeControlEnabled(TRUE)
    , mVolumeSliderTouched(FALSE)
{}

ECode MediaRouteControllerDialog::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 theme)
{
    Dialog::constructor(context, theme);

    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::MEDIA_ROUTER_SERVICE, (IInterface**)&service);
    mRouter = IMediaRouter::Probe(service);
    assert(0);
    // mCallback = new MediaRouterCallback();
    mRouter->GetSelectedRoute((IMediaRouterRouteInfo**)&mRoute);
    return NOERROR;
}

ECode MediaRouteControllerDialog::GetRoute(
    /* [out] */ IMediaRouterRouteInfo** route)
{
    VALIDATE_NOT_NULL(route)
    *route = mRoute;
    REFCOUNT_ADD(*route)
    return NOERROR;
}

ECode MediaRouteControllerDialog::OnCreateMediaControlView(
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)
    *view = NULL;
    return NOERROR;
}

ECode MediaRouteControllerDialog::GetMediaControlView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)
    *view = mControlView;
    REFCOUNT_ADD(*view)
    return NOERROR;
}

ECode MediaRouteControllerDialog::SetVolumeControlEnabled(
    /* [in] */ Boolean enable)
{
    if (mVolumeControlEnabled != enable) {
        mVolumeControlEnabled = enable;
        if (mCreated) {
            UpdateVolume();
        }
    }
    return NOERROR;
}

ECode MediaRouteControllerDialog::IsVolumeControlEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mVolumeControlEnabled;
    return NOERROR;
}

ECode MediaRouteControllerDialog::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Dialog::OnCreate(savedInstanceState);

    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    Boolean res;
    window->RequestFeature(IWindow::FEATURE_LEFT_ICON, &res);

    SetContentView(R::layout::media_route_controller_dialog);

    AutoPtr<IView> tempView;
    FindViewById(R::id::media_route_volume_layout, (IView**)&tempView);
    mVolumeLayout = ILinearLayout::Probe(tempView);
    tempView = NULL;
    FindViewById(R::id::media_route_volume_slider, (IView**)&tempView);
    mVolumeSlider = ISeekBar::Probe(tempView);

    AutoPtr<OnSeekBarChangeListener> listener = new OnSeekBarChangeListener(this);
    mVolumeSlider->SetOnSeekBarChangeListener(listener);

    tempView = NULL;
    FindViewById(R::id::media_route_disconnect_button, (IView**)&tempView);
    mDisconnectButton = IButton::Probe(tempView);
    AutoPtr<ViewOnClickListener> listener2 = new ViewOnClickListener(this);
    tempView->SetOnClickListener(listener2);

    mCreated = TRUE;
    if (Update()) {
        OnCreateMediaControlView(savedInstanceState, (IView**)&mControlView);
        tempView = NULL;
        FindViewById(R::id::media_route_control_frame, (IView**)&tempView);
        AutoPtr<IFrameLayout> controlFrame = IFrameLayout::Probe(tempView);
        if (mControlView != NULL) {
            IViewGroup::Probe(controlFrame)->AddView(mControlView);
            tempView->SetVisibility(IView::VISIBLE);
        }
        else {
            tempView->SetVisibility(IView::GONE);
        }
    }
    return NOERROR;
}

ECode MediaRouteControllerDialog::OnAttachedToWindow()
{
    Dialog::OnAttachedToWindow();

    mRouter->AddCallback(0, mCallback, IMediaRouter::CALLBACK_FLAG_UNFILTERED_EVENTS);
    Update();
    return NOERROR;
}

ECode MediaRouteControllerDialog::OnDetachedFromWindow()
{
    mRouter->RemoveCallback(mCallback);

    return Dialog::OnDetachedFromWindow();
}

ECode MediaRouteControllerDialog::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (keyCode == IKeyEvent::KEYCODE_VOLUME_DOWN
        || keyCode == IKeyEvent::KEYCODE_VOLUME_UP) {
        mRoute->RequestUpdateVolume(keyCode == IKeyEvent::KEYCODE_VOLUME_DOWN ? -1 : 1);
        *result = TRUE;
        return NOERROR;
    }
    return Dialog::OnKeyDown(keyCode, event, result);
}

ECode MediaRouteControllerDialog::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (keyCode == IKeyEvent::KEYCODE_VOLUME_DOWN
        || keyCode == IKeyEvent::KEYCODE_VOLUME_UP) {
        *result = TRUE;
        return NOERROR;
    }
    return Dialog::OnKeyUp(keyCode, event, result);
}

Boolean MediaRouteControllerDialog::Update()
{
    Boolean isSelected, isDefault;
    if (!(mRoute->IsSelected(&isSelected), isSelected) ||
        (mRoute->IsDefault(&isDefault), isDefault)) {
        Dismiss();
        return FALSE;
    }

    AutoPtr<ICharSequence> name;
    mRoute->GetName((ICharSequence**)&name);
    SetTitle(name);
    UpdateVolume();

    AutoPtr<IDrawable> icon = GetIconDrawable();
    if (icon != mCurrentIconDrawable) {
        mCurrentIconDrawable = icon;
        AutoPtr<IWindow> window;
        GetWindow((IWindow**)&window);
        window->SetFeatureDrawable(IWindow::FEATURE_LEFT_ICON, icon);
    }
    return TRUE;
}

AutoPtr<IDrawable> MediaRouteControllerDialog::GetIconDrawable()
{
    Boolean isConnecting;
    mRoute->IsConnecting(&isConnecting);
    if (isConnecting) {
        if (mMediaRouteConnectingDrawable == NULL) {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            context->GetDrawable(R::drawable::ic_media_route_connecting_holo_dark,
                (IDrawable**)&mMediaRouteConnectingDrawable);
        }
        return mMediaRouteConnectingDrawable;
    }
    else {
        if (mMediaRouteOnDrawable == NULL) {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            context->GetDrawable(R::drawable::ic_media_route_on_holo_dark,
                (IDrawable**)&mMediaRouteOnDrawable);
        }
        return mMediaRouteOnDrawable;
    }
}

void MediaRouteControllerDialog::UpdateVolume()
{
    if (!mVolumeSliderTouched) {
        if (IsVolumeControlAvailable()) {
            IView::Probe(mVolumeLayout)->SetVisibility(IView::VISIBLE);
            Int32 max, volume;
            mRoute->GetVolumeMax(&max);
            mRoute->GetVolume(&volume);
            IProgressBar::Probe(mVolumeSlider)->SetMax(max);
            IProgressBar::Probe(mVolumeSlider)->SetProgress(volume);
        }
        else {
            IView::Probe(mVolumeLayout)->SetVisibility(IView::GONE);
        }
    }
}

Boolean MediaRouteControllerDialog::IsVolumeControlAvailable()
{
    Int32 result;
    mRoute->GetVolumeHandling(&result);
    return mVolumeControlEnabled && result ==
        IMediaRouterRouteInfo::PLAYBACK_VOLUME_VARIABLE;
}

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos
