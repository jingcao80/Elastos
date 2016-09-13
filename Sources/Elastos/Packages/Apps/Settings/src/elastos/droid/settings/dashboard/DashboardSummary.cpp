
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/settings/dashboard/DashboardSummary.h"
#include "elastos/droid/settings/CSettingsActivity.h"
#include "elastos/droid/settings/dashboard/CDashboardCategory.h"
#include "elastos/droid/settings/dashboard/CDashboardTileView.h"
#include "elastos/droid/text/TextUtils.h"
#include "../R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Widget::IFrameLayout;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Dashboard {

const String DashboardSummary::TAG("DashboardSummary");
const Int32 DashboardSummary::MSG_REBUILD_UI;

//===============================================================================
//                  DashboardSummary::MyHandler
//===============================================================================

DashboardSummary::MyHandler::MyHandler(
    /* [in] */ DashboardSummary* host)
    : mHost(host)
{}

DashboardSummary::MyHandler::~MyHandler()
{}

ECode DashboardSummary::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MSG_REBUILD_UI: {
            AutoPtr<IActivity> act;
            mHost->GetActivity((IActivity**)&act);
            mHost->RebuildUI(IContext::Probe(act));
        } break;
    }

    return NOERROR;
}

//===============================================================================
//                  DashboardSummary::HomePackageReceiver
//===============================================================================

DashboardSummary::HomePackageReceiver::HomePackageReceiver(
    /* [in] */ DashboardSummary* host)
    : mHost(host)
{}

DashboardSummary::HomePackageReceiver::~HomePackageReceiver()
{}

ECode DashboardSummary::HomePackageReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mHost->RebuildUI(context);
    return NOERROR;
}

//===============================================================================
//                  DashboardSummary::HomePackageReceiver
//===============================================================================

DashboardSummary::DashboardSummary()
{
    mHandler = new MyHandler(this);
    mHandler->constructor();

    mHomePackageReceiver = new HomePackageReceiver(this);
}

DashboardSummary::~DashboardSummary()
{}

ECode DashboardSummary::constructor()
{
    return Fragment::constructor();
}

ECode DashboardSummary::OnResume()
{
    Fragment::OnResume();

    SendRebuildUI();

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IIntent::ACTION_PACKAGE_ADDED, (IIntentFilter**)&filter);

    filter->AddAction(IIntent::ACTION_PACKAGE_REMOVED);
    filter->AddAction(IIntent::ACTION_PACKAGE_CHANGED);
    filter->AddAction(IIntent::ACTION_PACKAGE_REPLACED);
    filter->AddDataScheme(String("package"));
    AutoPtr<IActivity> act;
    GetActivity((IActivity**)&act);
    AutoPtr<IIntent> intent;
    IContext::Probe(act)->RegisterReceiver(mHomePackageReceiver, filter, (IIntent**)&intent);
    return NOERROR;
}

ECode DashboardSummary::OnPause()
{
    Fragment::OnPause();

    AutoPtr<IActivity> act;
    GetActivity((IActivity**)&act);
    IContext::Probe(act)->UnregisterReceiver(mHomePackageReceiver);
    return NOERROR;
}

ECode DashboardSummary::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    mLayoutInflater = inflater;

    AutoPtr<IView> rootView;
    inflater->Inflate(R::layout::dashboard, container, FALSE, (IView**)&rootView);
    AutoPtr<IView> _view;
    rootView->FindViewById(R::id::dashboard_container, (IView**)&_view);
    mDashboard = IViewGroup::Probe(_view);

    *view = rootView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

void DashboardSummary::RebuildUI(
    /* [in] */ IContext* context)
{
    Boolean result;
    if (IsAdded(&result), !result) {
        Logger::W(TAG, "Cannot build the DashboardSummary UI yet as the Fragment is not added");
        return;
    }

    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 start;
    sys->GetCurrentTimeMillis(&start);
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);

    mDashboard->RemoveAllViews();

    AutoPtr<IList> categories;
    ((CSettingsActivity*)IActivity::Probe(context))->GetDashboardCategories(TRUE, (IList**)&categories);

    Int32 count;
    categories->GetSize(&count);

    for (Int32 n = 0; n < count; n++) {
        AutoPtr<IInterface> obj;
        categories->Get(n, (IInterface**)&obj);
        CDashboardCategory* category = (CDashboardCategory*)IObject::Probe(obj);

        AutoPtr<IView> categoryView;
        mLayoutInflater->Inflate(R::layout::dashboard_category, mDashboard,
                FALSE, (IView**)&categoryView);

        AutoPtr<IView> view;
        categoryView->FindViewById(R::id::category_title, (IView**)&view);
        AutoPtr<ITextView> categoryLabel = ITextView::Probe(view);
        categoryLabel->SetText(category->GetTitle(res));

        view = NULL;
        categoryView->FindViewById(R::id::category_content, (IView**)&view);
        AutoPtr<IViewGroup> categoryContent = IViewGroup::Probe(view);

        const Int32 tilesCount = category->GetTilesCount();
        for (Int32 i = 0; i < tilesCount; i++) {
            AutoPtr<CDashboardTile> tile = category->GetTile(i);

            AutoPtr<IFrameLayout> layout;
            CDashboardTileView::New(context, (IFrameLayout**)&layout);
            AutoPtr<CDashboardTileView> tileView = (CDashboardTileView*)layout.Get();
            UpdateTileView(context, res, tile, tileView->GetImageView(),
                    tileView->GetTitleTextView(), tileView->GetStatusTextView());

            tileView->SetTile(tile);

            categoryContent->AddView(tileView);
        }

        // Add the category
        mDashboard->AddView(categoryView);
    }

    Int64 now;
    sys->GetCurrentTimeMillis(&now);

    Int64 delta = now - start;
    Logger::D(TAG, "rebuildUI took: %lld ms", delta);
}

void DashboardSummary::UpdateTileView(
    /* [in] */ IContext* context,
    /* [in] */ IResources* res,
    /* [in] */ CDashboardTile* tile,
    /* [in] */ IImageView* tileIcon,
    /* [in] */ ITextView* tileTextView,
    /* [in] */ ITextView* statusTextView)
{
    if (tile->mIconRes > 0) {
        tileIcon->SetImageResource(tile->mIconRes);
    }
    else {
        tileIcon->SetImageDrawable(NULL);
        IView::Probe(tileIcon)->SetBackground(NULL);
    }

    tileTextView->SetText(tile->GetTitle(res));

    AutoPtr<ICharSequence> summary = tile->GetSummary(res);
    if (!TextUtils::IsEmpty(summary)) {
        IView::Probe(statusTextView)->SetVisibility(IView::VISIBLE);
        statusTextView->SetText(summary);
    }
    else {
        IView::Probe(statusTextView)->SetVisibility(IView::GONE);
    }
}

void DashboardSummary::SendRebuildUI()
{
    Boolean res;
    if (mHandler->HasMessages(MSG_REBUILD_UI, &res), !res) {
        mHandler->SendEmptyMessage(MSG_REBUILD_UI, &res);
    }
}

} // namespace Dashboard
} // namespace Settings
} // namespace Droid
} // namespace Elastos
