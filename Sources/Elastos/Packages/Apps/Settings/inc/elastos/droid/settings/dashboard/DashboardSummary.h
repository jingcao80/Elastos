
#ifndef __ELASTOS_DROID_SETTINGS_DASHBOARD_DASHBOARDSUMMARY_H__
#define __ELASTOS_DROID_SETTINGS_DASHBOARD_DASHBOARDSUMMARY_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/settings/dashboard/CDashboardTile.h"
#include "elastos/droid/app/Fragment.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::App::Fragment;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Dashboard {

class DashboardSummary
    : public Fragment
{
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("DashboardSummary::MyHandler")

        MyHandler(
            /* [in] */ DashboardSummary* host);

        ~MyHandler();

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        DashboardSummary* mHost;
    };

    class HomePackageReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("DashboardSummary::HomePackageReceiver")

        HomePackageReceiver(
            /* [in] */ DashboardSummary* host);

        ~HomePackageReceiver();

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        DashboardSummary* mHost;
    };

public:
    DashboardSummary();

    ~DashboardSummary();

    CARAPI constructor();

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** view);

private:
    CARAPI_(void) RebuildUI(
        /* [in] */ IContext* context);

    CARAPI_(void) UpdateTileView(
        /* [in] */ IContext* context,
        /* [in] */ IResources* res,
        /* [in] */ CDashboardTile* tile,
        /* [in] */ IImageView* tileIcon,
        /* [in] */ ITextView* tileTextView,
        /* [in] */ ITextView* statusTextView);

    CARAPI_(void) SendRebuildUI();

public:
    static const String TAG;

    AutoPtr<ILayoutInflater> mLayoutInflater;
    AutoPtr<IViewGroup> mDashboard;

    static const Int32 MSG_REBUILD_UI = 1;
    AutoPtr<MyHandler> mHandler;

    AutoPtr<HomePackageReceiver> mHomePackageReceiver;
};

} // namespace Dashboard
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_DASHBOARD_DASHBOARDSUMMARY_H__
