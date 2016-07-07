#ifndef __ELASTOS_DROID_SETTINGS_CLOCALEPICKER_H__
#define __ELASTOS_DROID_SETTINGS_CLOCALEPICKER_H__

#include "elastos/droid/app/ListFragment.h"
#include "_Elastos_Droid_Settings_CLocalePicker.h"
#include "elastos/droid/internal/app/LocalePicker.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Internal::App::LocalePicker;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CLocalePicker)
    , public LocalePicker
    , public ILocaleSelectionListener
    , public IDialogCreatable
{
private:
    class OnCreateDialogRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        OnCreateDialogRunnable(
            /* [in] */ CLocalePicker* host,
            /* [in] */ IActivity* activity,
            /* [in] */ Int32 dialogId);

        ~OnCreateDialogRunnable();

        //@Override
        CARAPI Run();

    private:
        CLocalePicker* mHost;
        AutoPtr<IActivity> mActivity;
        Int32 mDialogId;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CLocalePicker();

    ~CLocalePicker();

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
    CARAPI OnLocaleSelected(
        /* [in] */ ILocale* locale);

    //@Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    CARAPI OnCreateDialog(
        /* [in] */ Int32 dialogId,
        /* [out] */ IDialog** dialog);

protected:
    CARAPI_(void) ShowDialog(
        /* [in] */ Int32 dialogId);

    CARAPI_(void) RemoveDialog(
        /* [in] */ Int32 dialogId);

private:
    static const String TAG;

    AutoPtr<ISettingsDialogFragment> mDialogFragment;
    static const Int32 DLG_SHOW_GLOBAL_WARNING;
    static const String SAVE_TARGET_LOCALE;

    AutoPtr<ILocale> mTargetLocale;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CLOCALEPICKER_H__