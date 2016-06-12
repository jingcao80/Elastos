#include "elastos/droid/settings/wifi/WifiConfigUiForSetupWizardXL.h"
#include "../R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::View::EIID_IViewOnFocusChangeListener;
using Elastos::Droid::Widget::IEditText;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

const String WifiConfigUiForSetupWizardXL::TAG("SetupWizard");

//===============================================================================
//                  WifiConfigUiForSetupWizardXL::FocusRunnable
//===============================================================================

WifiConfigUiForSetupWizardXL::FocusRunnable::FocusRunnable(
    /* [in] */ IView* viewToBeFocused,
    /* [in] */ WifiConfigUiForSetupWizardXL* host)
    : mViewToBeFocused(viewToBeFocused)
    , mHost(host)
{}

WifiConfigUiForSetupWizardXL::FocusRunnable::~FocusRunnable()
{}

ECode WifiConfigUiForSetupWizardXL::FocusRunnable::Run()
{
    // mInputMethodManager->FocusIn(mViewToBeFocused);
    Boolean showSoftInputResult;
    mHost->mInputMethodManager->ShowSoftInput(mViewToBeFocused, 0, &showSoftInputResult);
    if (showSoftInputResult) {
        mHost->mActivity->SetPaddingVisibility(IView::GONE);
    }
    else {
        Logger::W(TAG, "Failed to show software keyboard ");
    }
    return NOERROR;
}

//===============================================================================
//                  WifiConfigUiForSetupWizardXL
//===============================================================================

CAR_INTERFACE_IMPL_3(WifiConfigUiForSetupWizardXL, Object, IWifiConfigUiForSetupWizardXL,
        IWifiConfigUiBase, IViewOnFocusChangeListener)

WifiConfigUiForSetupWizardXL::WifiConfigUiForSetupWizardXL(
    /* [in] */ WifiSettingsForSetupWizardXL* activity,
    /* [in] */ IViewGroup* parent,
    /* [in] */ IAccessPoint* accessPoint,
    /* [in] */ Boolean edit)
    : mEdit(FALSE)
{
    CHandler::New((IHandler**)&mHandler);

    mActivity = activity;
    AutoPtr<IView> view;
    activity->FindViewById(R::id::wifi_setup_connect, (IView**)&view);
    mConnectButton = IButton::Probe(view);
    view = NULL;
    activity->FindViewById(R::id::wifi_setup_cancel, (IView**)&view);
    mCancelButton = IButton::Probe(view);
    mAccessPoint = accessPoint;
    mEdit = edit;
    AutoPtr<IInterface> obj;
    activity->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    mInflater = ILayoutInflater::Probe(obj);

    mInflater->Inflate(R::layout::wifi_config_ui_for_setup_wizard, parent, TRUE, (IView**)&mView);
    mController = new WifiConfigController(this, mView, mAccessPoint, edit);

    obj = NULL;
    activity->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&obj);
    mInputMethodManager = IInputMethodManager::Probe(obj);

    view = NULL;
    mView->FindViewById(R::id::security_fields, (IView**)&view);
    Int32 visibility1, visibility2;
    view->GetVisibility(&visibility1);
    view = NULL;
    mView->FindViewById(R::id::type_ssid, (IView**)&view);
    view->GetVisibility(&visibility2);
    if (visibility1 == IView::VISIBLE) {
        RequestFocusAndShowKeyboard(R::id::password);
    }
    else if (visibility2 == IView::VISIBLE) {
        // Add Network flow.
        RequestFocusAndShowKeyboard(R::id::ssid);
    }
}

WifiConfigUiForSetupWizardXL::~WifiConfigUiForSetupWizardXL()
{}

ECode WifiConfigUiForSetupWizardXL::RequestFocusAndShowKeyboard(
    /* [in] */ Int32 editViewId)
{
    // Set Focus to password View.
    AutoPtr<IView> viewToBeFocused;
    mView->FindViewById(editViewId, (IView**)&viewToBeFocused);
    if (viewToBeFocused == NULL) {
        Logger::W(TAG, "password field to be focused not found.");
    }
    else if (IEditText::Probe(viewToBeFocused) == NULL) {
        Logger::W(TAG, "password field is not EditText");
    }
    else {
        Boolean res;
        if (viewToBeFocused->IsFocused(&res), res) {
            Logger::I(TAG, "Already focused");

            if (mInputMethodManager->ShowSoftInput(viewToBeFocused, 0, &res), !res) {
                Logger::W(TAG, "Failed to show SoftInput");
            }
        }
        else {
            // After acquiring the focus, we show software keyboard.
            viewToBeFocused->SetOnFocusChangeListener((IViewOnFocusChangeListener*)this);
            Boolean requestFocusResult;
            viewToBeFocused->RequestFocus(&requestFocusResult);
            String str("");
            str.AppendFormat("Focus request: %s", (requestFocusResult ? "successful" : "failed"));
            Logger::I(TAG, str.string());
            if (!requestFocusResult) {
                viewToBeFocused->SetOnFocusChangeListener(NULL);
            }
        }
    }
    return NOERROR;
}

AutoPtr<IView> WifiConfigUiForSetupWizardXL::GetView()
{
    return mView;
}

AutoPtr<IAccessPoint> WifiConfigUiForSetupWizardXL::GetAccessPoint()
{
    return mAccessPoint;
}

ECode WifiConfigUiForSetupWizardXL::GetController(
    /* [out] */ IWifiConfigController** controller)
{
    VALIDATE_NOT_NULL(controller)
    *controller = mController;
    REFCOUNT_ADD(*controller)
    return NOERROR;
}

ECode WifiConfigUiForSetupWizardXL::IsEdit(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = mEdit;
    return NOERROR;
}

ECode WifiConfigUiForSetupWizardXL::GetLayoutInflater(
    /* [out] */ ILayoutInflater** inflater)
{
    VALIDATE_NOT_NULL(inflater);
    *inflater = mInflater;
    REFCOUNT_ADD(*inflater)
    return NOERROR;
}

ECode WifiConfigUiForSetupWizardXL::GetSubmitButton(
    /* [out] */ IButton** button)
{
    VALIDATE_NOT_NULL(button)
    *button = mConnectButton;
    REFCOUNT_ADD(*button)
    return NOERROR;
}

ECode WifiConfigUiForSetupWizardXL::GetForgetButton(
    /* [out] */ IButton** button)
{
    VALIDATE_NOT_NULL(button)
    *button = NULL;
    return NOERROR;
}

ECode WifiConfigUiForSetupWizardXL::GetCancelButton(
    /* [out] */ IButton** button)
{
    VALIDATE_NOT_NULL(button)
    *button = mCancelButton;
    REFCOUNT_ADD(*button)
    return NOERROR;
}

ECode WifiConfigUiForSetupWizardXL::SetSubmitButton(
    /* [in] */ ICharSequence* text)
{
    IView::Probe(mConnectButton)->SetVisibility(IView::VISIBLE);
    ITextView::Probe(mConnectButton)->SetText(text);
    return NOERROR;
}

ECode WifiConfigUiForSetupWizardXL::SetForgetButton(
    /* [in] */ ICharSequence* text)
{
    // In XL setup screen, we won't show Forget button for simplifying the UI.
    return NOERROR;
}

ECode WifiConfigUiForSetupWizardXL::SetCancelButton(
    /* [in] */ ICharSequence* text)
{
    IView::Probe(mCancelButton)->SetVisibility(IView::VISIBLE);
    // We don't want "cancel" label given from caller.
    // mCancelButton->SetText(text);
    return NOERROR;
}

ECode WifiConfigUiForSetupWizardXL::GetContext(
    /* [out] */ IContext** context)
{
    VALIDATE_NOT_NULL(context)
    AutoPtr<IActivity> activity = (IActivity*)mActivity.Get();
    *context = IContext::Probe(activity);
    REFCOUNT_ADD(*context)
    return NOERROR;
}

ECode WifiConfigUiForSetupWizardXL::SetTitle(
    /* [in] */ Int32 id)
{
    Logger::D(TAG, "Ignoring setTitle");
    return NOERROR;
}

ECode WifiConfigUiForSetupWizardXL::SetTitle(
    /* [in] */ ICharSequence* title)
{
    Logger::D(TAG, "Ignoring setTitle");
    return NOERROR;
}

ECode WifiConfigUiForSetupWizardXL::OnFocusChange(
    /* [in] */ IView* view,
    /* [in] */ Boolean hasFocus)
{
    view->SetOnFocusChangeListener(NULL);
    if (hasFocus) {
        AutoPtr<FocusRunnable> runnable = new FocusRunnable(view, this);
        Boolean res;
        mHandler->Post((IRunnable*)runnable, &res);
    }
    return NOERROR;
}

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos
