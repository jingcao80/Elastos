#ifndef __ELASTOS_DROID_SETTINGS_NOTIFICATION_CNOTIFICATIONSTATION_H__
#define __ELASTOS_DROID_SETTINGS_NOTIFICATION_CNOTIFICATIONSTATION_H__

#include "Elastos.Droid.Service.h"
#include "_Elastos_Droid_Settings_Notification_CNotificationStation.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"
#include "elastos/droid/service/notification/NotificationListenerService.h"
#include "elastos/droid/widget/ArrayAdapter.h"

using Elastos::Droid::Settings::SettingsPreferenceFragment;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IINotificationManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Service::Notification::NotificationListenerService;
using Elastos::Droid::Service::Notification::INotificationListenerService;
using Elastos::Droid::Service::Notification::IStatusBarNotification;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::ArrayAdapter;
using Elastos::Core::IComparator;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

CarClass(CNotificationStation)
    , public SettingsPreferenceFragment
{
private:
    class HistoricalNotificationInfo
        : public Object
    {
    public:
        TO_STRING_IMPL("CNotificationStation::HistoricalNotificationInfo")

        String mPkg;
        AutoPtr<IDrawable> mPkgicon;
        AutoPtr<ICharSequence> mPkgname;
        AutoPtr<IDrawable> mIcon;
        AutoPtr<ICharSequence> mTitle;
        Int32 mPriority;
        Int32 mUser;
        Int64 mTimestamp;
        Boolean mActive;
    };

    class NotificationHistoryAdapter
        : public ArrayAdapter
    {
    public:
        TO_STRING_IMPL("CNotificationStation::NotificationHistoryAdapter")

        NotificationHistoryAdapter(
            /* [in] */ CNotificationStation* host);

        CARAPI constructor(
            /* [in] */ IContext* context);

        //@Override
        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** result);

    private:
        CARAPI_(AutoPtr<IView>) CreateRow(
            /* [in] */ IViewGroup* parent);

    private:
        AutoPtr<ILayoutInflater> mInflater;
        CNotificationStation* mHost;
    };

    class MyOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CNotificationStation::MyOnClickListener")

        CAR_INTERFACE_DECL()

        MyOnClickListener(
            /* [in] */ CNotificationStation* host,
            /* [in] */ HistoricalNotificationInfo* info);

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CNotificationStation* mHost;
        AutoPtr<HistoricalNotificationInfo> mInfo;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CNotificationStation::MyRunnable")

        MyRunnable(
            /* [in] */ CNotificationStation* host);

        //@Override
        CARAPI Run();

    private:
        CNotificationStation* mHost;
    };

    class MyNotificationListenerService
        : public NotificationListenerService
    {
    public:
        TO_STRING_IMPL("CNotificationStation::MyNotificationListenerService")

        MyNotificationListenerService();

        CARAPI constructor(
            /* [in] */ CNotificationStation* host);

        //@Override
        CARAPI OnNotificationPosted(
            /* [in] */ IStatusBarNotification* notification);

        //@Override
        CARAPI OnNotificationRemoved(
            /* [in] */ IStatusBarNotification* notification);

    private:
        CNotificationStation* mHost;
    };

    class MyComparator
        : public Object
        , public IComparator
    {
    public:
        TO_STRING_IMPL("CNotificationStation::MyComparator")

        CAR_INTERFACE_DECL()

        //@Override
        CARAPI Compare(
            /* [in] */ IInterface* _lhs,
            /* [in] */ IInterface* _rhs,
            /* [out] */ Int32* result);
    };

public:
    CAR_OBJECT_DECL()

    CNotificationStation();

    ~CNotificationStation();

    CARAPI constructor();

    //@Override
    CARAPI OnAttach(
        /* [in] */ IActivity* activity);

    //@Override
    CARAPI OnDetach();

    //@Override
    CARAPI OnActivityCreated(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnResume();

private:
    CARAPI_(void) RefreshList();

    static CARAPI_(void) Logd(
        /* [in] */ const String& msg,
        /* [in] */ ArrayOf<IInterface*>* args);

    CARAPI_(AutoPtr<IList>) /*List<HistoricalNotificationInfo>*/ LoadNotifications();

    CARAPI_(AutoPtr<IResources>) GetResourcesForUserPackage(
        /* [in] */ const String& pkg,
        /* [in] */ Int32 userId);

    CARAPI_(AutoPtr<IDrawable>) LoadPackageIconDrawable(
        /* [in] */ const String& pkg,
        /* [in] */ Int32 userId);

    CARAPI_(AutoPtr<ICharSequence>) LoadPackageName(
        /* [in] */ const String& pkg);

    CARAPI_(AutoPtr<IDrawable>) LoadIconDrawable(
        /* [in] */ const String& pkg,
        /* [in] */ Int32 userId,
        /* [in] */ Int32 resId);

    CARAPI_(void) StartApplicationDetailsActivity(
        /* [in] */ const String& packageName);

private:
    static const String TAG;

    static const Boolean DEBUG;

    AutoPtr<IPackageManager> mPm;
    AutoPtr<IINotificationManager> mNoMan;

    AutoPtr<Runnable> mRefreshListRunnable;

    AutoPtr<MyNotificationListenerService> mListener;

    AutoPtr<NotificationHistoryAdapter> mAdapter;
    IContext* mContext;

    AutoPtr<MyComparator> mNotificationSorter;
};

} // namespace NOtification
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_NOTIFICATION_CNOTIFICATIONSTATION_H__