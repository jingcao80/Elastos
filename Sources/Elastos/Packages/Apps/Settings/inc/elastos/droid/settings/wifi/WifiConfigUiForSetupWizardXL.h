#ifndef __ELASTOS_DROID_SETTINGS_WIFI_WIFICONFIGUIFORSETUPWIZARDXL_H__
#define __ELASTOS_DROID_SETTINGS_WIFI_WIFICONFIGUIFORSETUPWIZARDXL_H__

#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/settings/wifi/WifiConfigController.h"

using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IViewOnFocusChangeListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::Widget::IButton;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

/**
 * Shows simplified UI for configuring a wifi network. Used only in SetupWizard for XLarge
 * screen.
 */
class WifiConfigUiForSetupWizardXL
    : public Object
    , public IWifiConfigUiForSetupWizardXL
    , public IWifiConfigUiBase
{
private:
    class InnerListener
        : public Object
        , public IViewOnFocusChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("WifiConfigUiForSetupWizardXL::InnerListener")

        InnerListener(
            /* [in] */ WifiConfigUiForSetupWizardXL* host);

        //@Override
        CARAPI OnFocusChange(
            /* [in] */ IView* view,
            /* [in] */ Boolean hasFocus);

    private:
        WifiConfigUiForSetupWizardXL* mHost;
    };

    class FocusRunnable
        : public Runnable
    {
    public:
        FocusRunnable(
            /* [in] */ IView* viewToBeFocused,
            /* [in] */ WifiConfigUiForSetupWizardXL* host);

        ~FocusRunnable();

        //@Override
        CARAPI Run();

    public:
        AutoPtr<IView> mViewToBeFocused;

    private:
        WifiConfigUiForSetupWizardXL* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    /**
     * @param activity Activity which creates this object.
     * @param parent Parent ViewGroup (typically some layout) holding a view object created by
     * this object
     * @param accessPoint target AccessPoint to be configured.
     * @param edit
     */
    WifiConfigUiForSetupWizardXL(
        /* [in] */ IWifiSettingsForSetupWizardXL* activity,
        /* [in] */ IViewGroup* parent,
        /* [in] */ IAccessPoint* accessPoint,
        /* [in] */ Boolean edit);

    ~WifiConfigUiForSetupWizardXL();

    /**
     * @param editViewId must be EditView
     */
    CARAPI RequestFocusAndShowKeyboard(
        /* [in] */ Int32 editViewId);

    CARAPI_(AutoPtr<IView>) GetView();

    CARAPI_(AutoPtr<IAccessPoint>) GetAccessPoint();

    //@Override
    CARAPI GetController(
        /* [out] */ IWifiConfigController** controller);

    //@Override
    CARAPI IsEdit(
        /* [out] */ Boolean* res);

    //@Override
    CARAPI GetLayoutInflater(
        /* [out] */ ILayoutInflater** inflater);

    //@Override
    CARAPI GetSubmitButton(
        /* [out] */ IButton** button);

    //@Override
    CARAPI GetForgetButton(
        /* [out] */ IButton** button);

    //@Override
    CARAPI GetCancelButton(
        /* [out] */ IButton** button);

    //@Override
    CARAPI SetSubmitButton(
        /* [in] */ ICharSequence* text);

    //@Override
    CARAPI SetForgetButton(
        /* [in] */ ICharSequence* text);

    //@Override
    CARAPI SetCancelButton(
        /* [in] */ ICharSequence* text);

    //@Override
    CARAPI GetContext(
        /* [out] */ IContext** context);

    //@Override
    CARAPI SetTitle(
        /* [in] */ Int32 id);

    //@Override
    CARAPI SetTitle(
        /* [in] */ ICharSequence* title);

    //@Override
    CARAPI OnFocusChange(
        /* [in] */ IView* view,
        /* [in] */ Boolean hasFocus);

private:
    static const String TAG;

    AutoPtr<IButton> mConnectButton;
    AutoPtr<IButton> mCancelButton;

    AutoPtr<IWifiSettingsForSetupWizardXL> mActivity;
    AutoPtr<IView> mView;
    AutoPtr<WifiConfigController> mController;
    AutoPtr<IAccessPoint> mAccessPoint;
    Boolean mEdit;
    AutoPtr<IHandler> mHandler;

    AutoPtr<IInputMethodManager> mInputMethodManager;

    AutoPtr<ILayoutInflater> mInflater;
};

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIFI_WIFICONFIGUIFORSETUPWIZARDXL_H__
