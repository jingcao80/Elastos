#ifndef __ELASTOS_DROID_SETTINGS_CDREAMSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_CDREAMSETTINGS_H__

#include "_Elastos_Droid_Settings_CDreamSettings.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"
#include "elastos/droid/settings/DreamBackend.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/widget/ArrayAdapter.h"

using Elastos::Droid::Settings::DreamBackend;
using Elastos::Droid::Settings::Widget::ISwitchBar;
using Elastos::Droid::Settings::Widget::ISwitchBarOnSwitchChangeListener;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IOnMenuItemClickListener;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::ArrayAdapter;
using Elastos::Droid::Widget::ISwitch;

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CDreamSettings)
    , public SettingsPreferenceFragment
{
public:
    class InnerListener
        : public Object
        , public ISwitchBarOnSwitchChangeListener
    {
    public:
        TO_STRING_IMPL("CDreamSettings::InnerListener")

        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ CDreamSettings* host);

        //@Override
        CARAPI OnSwitchChanged(
            /* [in] */ ISwitch* switchView,
            /* [in] */ Boolean isChecked);

    private:
        CDreamSettings* mHost;
    };

private:
    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CDreamSettings::MyRunnable")

        MyRunnable(
            /* [in] */ CDreamSettings* host,
            /* [in] */ Int32 id);

        //@Override
        CARAPI Run();

    private:
        CDreamSettings* mHost;
        Int32 mId;
    };

    class OnMenuItemClickListener
        : public Object
        , public IOnMenuItemClickListener
    {
    public:
        TO_STRING_IMPL("CDreamSettings::OnMenuItemClickListener")

        CAR_INTERFACE_DECL()

        OnMenuItemClickListener(
            /* [in] */ IRunnable* onClick);

        //@Override
        CARAPI OnMenuItemClick(
            /* [in] */ IMenuItem* item,
            /* [out] */ Boolean* result);

    private:
        AutoPtr<IRunnable> mOnClick;
    };

    class DialogInterfaceOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        TO_STRING_IMPL("CDreamSettings::DialogInterfaceOnClickListener")

        CAR_INTERFACE_DECL()

        DialogInterfaceOnClickListener(
            /* [in] */ CDreamSettings* host);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 item);

    private:
        CDreamSettings* mHost;
    };

    class DreamInfoAdapter
        : public ArrayAdapter
    {
    public:
        TO_STRING_IMPL("CDreamSettings::DreamInfoAdapter");

        DreamInfoAdapter(
            /* [in] */ CDreamSettings* host,
            /* [in] */ IContext* context);

        //@Override
        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** result);

        CARAPI_(void) Activate(
            /* [in] */ DreamBackend::DreamInfo* dreamInfo);

    private:
        CARAPI_(AutoPtr<IView>) CreateDreamInfoRow(
            /* [in] */ IViewGroup* parent);

        CARAPI_(AutoPtr<DreamBackend::DreamInfo>) GetCurrentSelection();

    private:
        AutoPtr<ILayoutInflater> mInflater;
        CDreamSettings* mHost;
    };

    class MyViewOnTouchListener
        : public Object
        , public IViewOnTouchListener
    {
    public:
        TO_STRING_IMPL("CDreamSettings::MyViewOnTouchListener")

        CAR_INTERFACE_DECL()

        MyViewOnTouchListener(
            /* [in] */ DreamInfoAdapter* host,
            /* [in] */ IView* row);

        //@Override
        CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

    private:
        DreamInfoAdapter* mHost;
        AutoPtr<IView> mRow;
    };

    class DreamInfoAdapterOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CDreamSettings::DreamInfoAdapterOnClickListener")

        CAR_INTERFACE_DECL()

        DreamInfoAdapterOnClickListener(
            /* [in] */ CDreamSettings* host,
            /* [in] */ DreamInfoAdapter* owner,
            /* [in] */ IView* row,
            /* [in] */ Int32 id);

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CDreamSettings* mHost;
        DreamInfoAdapter* mOwner;
        AutoPtr<IView> mRow;
        Int32 mId;
    };

    class PackageReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("CDreamSettings::PackageReceiver")

        PackageReceiver(
            /* [in] */ CDreamSettings* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CDreamSettings* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CDreamSettings();

    ~CDreamSettings();

    CARAPI constructor();

    //@Override
    CARAPI_(Int32) GetHelpResource();

    //@Override
    CARAPI OnAttach(
        /* [in] */ IActivity* activity);

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnSwitchChanged(
        /* [in] */ ISwitch* switchView,
        /* [in] */ Boolean isChecked);

    //@Override
    CARAPI OnStart();

    //@Override
    CARAPI OnDestroyView();

    //@Override
    CARAPI OnActivityCreated(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [in] */ IMenuInflater* inflater);

    //@Override
    CARAPI OnCreateDialog(
        /* [in] */ Int32 dialogId,
        /* [out] */ IDialog** result);

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnResume();

    static CARAPI_(Int32) GetSummaryResource(
        /* [in] */ IContext* context);

    static CARAPI_(AutoPtr<ICharSequence>) GetSummaryTextWithDreamName(
        /* [in] */ IContext* context);

private:
    CARAPI_(AutoPtr<IMenuItem>) CreateMenuItem(
        /* [in] */ IMenu* menu,
        /* [in] */ Int32 titleRes,
        /* [in] */ Int32 actionEnum,
        /* [in] */ Boolean isEnabled,
        /* [in] */ IRunnable* onClick);

    CARAPI_(AutoPtr<IDialog>) CreateWhenToDreamDialog();

    CARAPI_(void) RefreshFromBackend();

    static CARAPI_(void) Logd(
        /* [in] */ const String& msg,
        /* [in] */ ArrayOf<IInterface*>* args);

public:
    static const Boolean DEBUG = FALSE;

private:
    static const String TAG;// = DreamSettings.class->GetSimpleName();
    static const Int32 DIALOG_WHEN_TO_DREAM;// = 1;
    static const String PACKAGE_SCHEME;// = "package";

    AutoPtr<PackageReceiver> mPackageReceiver;// = new PackageReceiver();

    IContext* mContext;
    AutoPtr<DreamBackend> mBackend;
    AutoPtr<DreamInfoAdapter> mAdapter;
    AutoPtr<ISwitchBar> mSwitchBar;
    AutoPtr< ArrayOf<IMenuItem*> > mMenuItemsWhenEnabled;
    Boolean mRefreshing;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CDREAMSETTINGS_H__