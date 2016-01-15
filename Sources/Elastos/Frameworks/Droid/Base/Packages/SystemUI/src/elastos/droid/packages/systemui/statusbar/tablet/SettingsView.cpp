#include "elastos/droid/systemui/statusbar/tablet/SettingsView.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/os/CUserHandle.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/systemui/SystemUIR.h"

using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::SystemUI::SystemUIR;
using Elastos::Droid::SystemUI::StatusBar::Policy::IToggleSlider;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {


SettingsView::SettingsView()
{}

SettingsView::SettingsView(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : LinearLayout(context, attrs)
{}

SettingsView::SettingsView(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
    : LinearLayout(context, attrs, defStyle)
{}

ECode SettingsView::OnFinishInflate()
{
    ASSERT_SUCCEEDED(LinearLayout::OnFinishInflate());

    AutoPtr<IContext> context = GetContext();

    AutoPtr<IView> tmpView;
    // tmpView = FindViewById(SystemUIR::id::airplane_checkbox);
    // AutoPtr<ICompoundButton> compBtn = ICompoundButton::Probe(tmpView.Get());
    // // TODO CAirplaneModeController::New(context, compBtn, (IAirplaneModeController**)&mAirplane);

    tmpView = FindViewById(SystemUIR::id::network);
    tmpView->SetOnClickListener(THIS_PROBE(IViewOnClickListener));

    mRotationLockContainer = FindViewById(SystemUIR::id::rotate);
    mRotationLockSeparator = FindViewById(SystemUIR::id::rotate_separator);
    //TODO mRotate = new AutoRotateController(context,
    //         (CompoundButton)findViewById(SystemUIR::id::rotate_checkbox),
    //         new AutoRotateController.RotationLockCallbacks() {
    //             @Override
    //             public void setRotationLockControlVisibility(boolean show) {
    //                 mRotationLockContainer.setVisibility(show ? IView::VISIBLE : IView::GONE);
    //                 mRotationLockSeparator.setVisibility(show ? IView::VISIBLE : IView::GONE);
    //             }
    //         });

    tmpView = FindViewById(SystemUIR::id::brightness_icon);
    AutoPtr<IImageView> brightnessIcon = IImageView::Probe(tmpView.Get());
    tmpView = FindViewById(SystemUIR::id::brightness);
    AutoPtr<IToggleSlider> brightness = IToggleSlider::Probe(tmpView.Get());
    // TODO CBrightnessController::New(context, brightnessIcon, brightness, (IBrightnessController**)&mBrightness);

    tmpView = FindViewById(SystemUIR::id::brightness);
    AutoPtr<ICompoundButton> doNotDesturbCheckbox = ICompoundButton::Probe(tmpView.Get());
    //TODO CDoNotDisturbController::New(context, doNotDesturbCheckbox, (IDoNotDisturbController**)&mDoNotDisturb);

    tmpView = FindViewById(SystemUIR::id::settings);
    tmpView->SetOnClickListener(THIS_PROBE(IViewOnClickListener));

    return NOERROR;
}

ECode SettingsView::OnDetachedFromWindow()
{
    LinearLayout::OnDetachedFromWindow();
    // //mAirplane = NULL;
    // mDoNotDisturb = NULL;
    // mRotate = NULL;
    return NOERROR;
}

ECode SettingsView::OnClick(
    /* [in] */ IView* v)
{
    Int32 id;
    v->GetId(&id);
    switch (id) {
        case SystemUIR::id::network:
            OnClickNetwork();
            break;
        case SystemUIR::id::settings:
            OnClickSettings();
            break;
    }
    return NOERROR;
}

AutoPtr<IStatusBarManager> SettingsView::GetStatusBarManager()
{
    AutoPtr<IContext> context = GetContext();
    AutoPtr<IInterface> tmpObj;
    context->GetSystemService(IContext::STATUS_BAR_SERVICE, (IInterface**)&tmpObj);
    AutoPtr<IStatusBarManager> sbm = IStatusBarManager::Probe(tmpObj.Get());
    return sbm;
}

// Network
// ----------------------------
void SettingsView::OnClickNetwork()
{
    AutoPtr<IIntent> intent;
    CIntent::New(ISettings::ACTION_WIFI_SETTINGS, (IIntent**)&intent);
    intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    AutoPtr<IContext> context = GetContext();
    context->StartActivity(intent);
    AutoPtr<IStatusBarManager> sbm = GetStatusBarManager();
    if (sbm)
        sbm->CollapsePanels();
}

// Settings
// ----------------------------
void SettingsView::OnClickSettings()
{
    AutoPtr<IIntent> intent;
    CIntent::New(ISettings::ACTION_SETTINGS, (IIntent**)&intent);
    intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    AutoPtr<IContext> context = GetContext();
    AutoPtr<IUserHandle> user;
    CUserHandle::New(IUserHandle::USER_CURRENT, (IUserHandle**)&user);
    context->StartActivityAsUser(intent, user);
    AutoPtr<IStatusBarManager> sbm = GetStatusBarManager();
    if (sbm)
        sbm->CollapsePanels();
}

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
