#ifndef __ELASTOS_DROID_SETTINGS_COWNERINFOSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_COWNERINFOSETTINGS_H__

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Widget.h"
#include "_Elastos_Droid_Settings_COwnerInfoSettings.h"
#include "elastos/droid/app/Fragment.h"

using Elastos::Droid::App::Fragment;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(COwnerInfoSettings)
    , public Fragment
{
private:
    class OnCheckedChangeListener
        : public Object
        , public ICompoundButtonOnCheckedChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        OnCheckedChangeListener(
            /* [in] */ COwnerInfoSettings* host);

        ~OnCheckedChangeListener();

        CARAPI OnCheckedChanged(
            /* [in] */ ICompoundButton* buttonView,
            /* [in] */ Boolean isChecked);

    private:
        COwnerInfoSettings* mHost;
    };

public:
    CAR_OBJECT_DECL()

    COwnerInfoSettings();

    ~COwnerInfoSettings();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** result);

    //@Override
    CARAPI OnPause();

protected:
    virtual CARAPI_(void) SaveChanges();

private:
    CARAPI_(void) InitView();

public:
    static const String EXTRA_SHOW_NICKNAME;

private:
    AutoPtr<IView> mView;
    AutoPtr<ICheckBox> mCheckbox;
    Int32 mUserId;
    AutoPtr<ILockPatternUtils> mLockPatternUtils;
    AutoPtr<IEditText> mOwnerInfo;
    AutoPtr<IEditText> mNickname;
    Boolean mShowNickname;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_COWNERINFOSETTINGS_H__