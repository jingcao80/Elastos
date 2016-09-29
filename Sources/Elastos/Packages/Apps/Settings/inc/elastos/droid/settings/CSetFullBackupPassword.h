#ifndef __ELASTOS_DROID_SETTINGS_CSETFULLBACKUPPASSWORD_H__
#define __ELASTOS_DROID_SETTINGS_CSETFULLBACKUPPASSWORD_H__

#include "Elastos.Droid.Widget.h"
#include "_Elastos_Droid_Settings_CSetFullBackupPassword.h"
#include "elastos/droid/app/Activity.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::Backup::IIBackupManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CSetFullBackupPassword)
    , public Activity
{
public:
    class MyOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CSetFullBackupPassword::MyOnClickListener")

        CAR_INTERFACE_DECL()

        MyOnClickListener(
            /* [in] */ CSetFullBackupPassword* host);

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CSetFullBackupPassword* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CSetFullBackupPassword();

    ~CSetFullBackupPassword();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

private:
    CARAPI_(Boolean) SetBackupPassword(
        /* [in] */ const String& currentPw,
        /* [in] */ const String& newPw);

public:
    static const String TAG;// = "CSetFullBackupPassword";

    AutoPtr<IIBackupManager> mBackupManager;
    AutoPtr<ITextView> mCurrentPw;
    AutoPtr<ITextView> mNewPw;
    AutoPtr<ITextView> mConfirmNewPw;
    AutoPtr<IButton> mCancel;
    AutoPtr<IButton> mSet;

    AutoPtr<IViewOnClickListener> mButtonListener;// = new MyOnClickListener(this);
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CSETFULLBACKUPPASSWORD_H__