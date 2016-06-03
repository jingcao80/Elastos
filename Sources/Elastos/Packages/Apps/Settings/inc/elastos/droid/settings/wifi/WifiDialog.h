#ifndef __ELASTOS_DROID_SETTINGS_WIFI_WIFIDIALOG_H__
#define __ELASTOS_DROID_SETTINGS_WIFI_WIFIDIALOG_H__

#include "elastos/droid/app/AlertDialog.h"
#include "elastos/droid/settings/wifi/WifiConfigController.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::App::AlertDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IButton;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

class WifiDialog
    : public AlertDialog
    , public IWifiConfigUiBase
{
public:
    CAR_INTERFACE_DECL();

    WifiDialog();

    ~WifiDialog();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IDialogInterfaceOnClickListener* listener,
        /* [in] */ AccessPoint* accessPoint,
        /* [in] */ Boolean edit,
        /* [in] */ Boolean hideSubmitButton);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IDialogInterfaceOnClickListener* listener,
        /* [in] */ AccessPoint* accessPoint,
        /* [in] */ Boolean edit);

    //@Override
    CARAPI GetController(
        /* [out] */ IWifiConfigController** controller);

    //@Override
    CARAPI IsEdit(
        /* [out] */ Boolean* result);

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

    // the under method use to fix compile error
    // @Override
    CARAPI GetContext(
        /* [out] */ IContext** context);

    //@Override
    CARAPI GetLayoutInflater(
        /* [out] */ ILayoutInflater** inflater);

    //@Override
    CARAPI SetTitle(
        /* [in] */ Int32 id);

    //@Override
    CARAPI SetTitle(
        /* [in] */ ICharSequence* title);

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

public:
    static const Int32 BUTTON_SUBMIT;
    static const Int32 BUTTON_FORGET;

private:
    Boolean mEdit;
    AutoPtr<IDialogInterfaceOnClickListener> mListener;
    AutoPtr<AccessPoint> mAccessPoint;

    AutoPtr<IView> mView;
    AutoPtr<WifiConfigController> mController;
    Boolean mHideSubmitButton;
};

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIFI_WIFIDIALOG_H__
