
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/widget/ShareActionProvider.h"
#include "elastos/droid/widget/CActivityChooserModelHelper.h"
#include "elastos/droid/widget/CActivityChooserView.h"
#include "elastos/droid/utility/CTypedValue.h"
#include "elastos/droid/R.h"

#include <elastos/core/Math.h>

using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::View::EIID_IOnMenuItemClickListener;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::EIID_IActionProvider;
using Elastos::Droid::Widget::CActivityChooserModelHelper;
using Elastos::Droid::Widget::CActivityChooserView;

using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Widget {

//============================================================================
//        ShareActionProvider::ShareActivityChooserModelPolicy
//============================================================================
CAR_INTERFACE_IMPL(ShareActionProvider::ShareActivityChooserModelPolicy, Object, IOnChooseActivityListener)

ShareActionProvider::ShareActivityChooserModelPolicy::ShareActivityChooserModelPolicy(
    /* [in] */ ShareActionProvider* host)
    : mHost(host)
{
}

ECode ShareActionProvider::ShareActivityChooserModelPolicy::OnChooseActivity(
    /* [in] */  IActivityChooserModel* host,
    /* [in] */  IIntent* intent,
    /* [out] */  Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    if (mHost->mOnShareTargetSelectedListener != NULL) {
        return mHost->mOnShareTargetSelectedListener->OnShareTargetSelected(
            (IShareActionProvider*)mHost, intent, res);
    }
    *res = FALSE;
    return NOERROR;
}

//============================================================================
//        ShareActionProvider::ShareMenuItemOnMenuItemClickListener
//============================================================================
CAR_INTERFACE_IMPL(ShareActionProvider::ShareMenuItemOnMenuItemClickListener, Object, IOnMenuItemClickListener)

ShareActionProvider::ShareMenuItemOnMenuItemClickListener::ShareMenuItemOnMenuItemClickListener(
    /* [in] */ ShareActionProvider* host)
    : mHost(host)
{
}

ECode ShareActionProvider::ShareMenuItemOnMenuItemClickListener::OnMenuItemClick(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* isConsumed)
{
    VALIDATE_NOT_NULL(isConsumed)

    AutoPtr<IActivityChooserModelHelper> helper;
    CActivityChooserModelHelper::AcquireSingleton((IActivityChooserModelHelper**)&helper);
    AutoPtr<IActivityChooserModel> dataModel;
    helper->Get(mHost->mContext, mHost->mShareHistoryFileName, (IActivityChooserModel**)&dataModel);
    Int32 itemId = 0;
    item->GetItemId(&itemId);
    AutoPtr<IIntent> launchIntent;
    dataModel->ChooseActivity(itemId, (IIntent**)&launchIntent);
    if (launchIntent != NULL) {
        String action;
        launchIntent->GetAction(&action);
        if (IIntent::ACTION_SEND.Equals(action) ||
                IIntent::ACTION_SEND_MULTIPLE.Equals(action)) {
            launchIntent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_DOCUMENT |
                    IIntent::FLAG_ACTIVITY_MULTIPLE_TASK);
        }
        mHost->mContext->StartActivity(launchIntent);
    }
    *isConsumed = TRUE;
    return NOERROR;
}

//===============================================================
// ShareActionProvider::
//===============================================================
const Int32 ShareActionProvider::DEFAULT_INITIAL_ACTIVITY_COUNT = 4;

const String ShareActionProvider::DEFAULT_SHARE_HISTORY_FILE_NAME("share_history.xml");

CAR_INTERFACE_IMPL(ShareActionProvider, ActionProvider, IShareActionProvider)

ShareActionProvider::ShareActionProvider()
    : mMaxShownActivityCount(DEFAULT_INITIAL_ACTIVITY_COUNT)
    , mShareHistoryFileName(DEFAULT_SHARE_HISTORY_FILE_NAME)
{
    mOnMenuItemClickListener = new ShareMenuItemOnMenuItemClickListener(this);
}

ECode ShareActionProvider::constructor(
    /* [in] */ IContext* context)
{
    ActionProvider::constructor(context);
    mContext = context;
    return NOERROR;
}

ECode ShareActionProvider::SetOnShareTargetSelectedListener(
    /* [in] */ IOnShareTargetSelectedListener* listener)
{
    mOnShareTargetSelectedListener = listener;
    SetActivityChooserPolicyIfNeeded();
    return NOERROR;
}

ECode ShareActionProvider::OnCreateActionView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)

    // Create the view and set its data model.
    AutoPtr<IActivityChooserView> activityChooserView;
    CActivityChooserView::New(mContext, (IActivityChooserView**)&activityChooserView);
    Boolean bEdMod = FALSE;
    if (!(IView::Probe(activityChooserView)->IsInEditMode(&bEdMod), bEdMod)) {
        AutoPtr<IActivityChooserModelHelper> helper;
        CActivityChooserModelHelper::AcquireSingleton((IActivityChooserModelHelper**)&helper);
        AutoPtr<IActivityChooserModel> dataModel;
        helper->Get(mContext, mShareHistoryFileName, (IActivityChooserModel**)&dataModel);
        activityChooserView->SetActivityChooserModel(dataModel);
    }

    // Lookup and set the expand action icon.
    AutoPtr<ITypedValue> outTypedValue;
    CTypedValue::New((ITypedValue**)&outTypedValue);
    AutoPtr<IResourcesTheme> theme;
    mContext->GetTheme((IResourcesTheme**)&theme);
    Boolean res = FALSE;
    theme->ResolveAttribute(R::attr::actionModeShareDrawable, outTypedValue, TRUE, &res);

    Int32 id = 0;
    outTypedValue->GetResourceId(&id);
    AutoPtr<IDrawable> drawable;
    mContext->GetDrawable(id, (IDrawable**)&drawable);
    activityChooserView->SetExpandActivityOverflowButtonDrawable(drawable);
    activityChooserView->SetProvider(this);

    // Set content description.
    activityChooserView->SetDefaultActionButtonContentDescription(R::string::shareactionprovider_share_with_application);
    activityChooserView->SetExpandActivityOverflowButtonContentDescription(R::string::shareactionprovider_share_with);

    *view = IView::Probe(activityChooserView);
    REFCOUNT_ADD(*view)
    return NOERROR;
}

ECode ShareActionProvider::HasSubMenu(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = TRUE;
    return NOERROR;
}

ECode ShareActionProvider::OnPrepareSubMenu(
    /* [in] */ ISubMenu* subMenu)
{
    // Clear since the order of items may change.
    IMenu::Probe(subMenu)->Clear();

    AutoPtr<IActivityChooserModelHelper> helper;
    CActivityChooserModelHelper::AcquireSingleton((IActivityChooserModelHelper**)&helper);
    AutoPtr<IActivityChooserModel> dataModel;
    helper->Get(mContext, mShareHistoryFileName, (IActivityChooserModel**)&dataModel);

    AutoPtr<IPackageManager> packageManager;
    mContext->GetPackageManager((IPackageManager**)&packageManager);

    Int32 expandedActivityCount = 0;
    dataModel->GetActivityCount(&expandedActivityCount);
    Int32 collapsedActivityCount = Elastos::Core::Math::Min(expandedActivityCount, mMaxShownActivityCount);

    // Populate the sub-menu with a sub set of the activities.
    for (Int32 i = 0; i < collapsedActivityCount; i++) {
        AutoPtr<IResolveInfo> activity;
        dataModel->GetActivity(i, (IResolveInfo**)&activity);
        AutoPtr<ICharSequence> csq;
        activity->LoadLabel(packageManager, (ICharSequence**)&csq);
        AutoPtr<IMenuItem> item;
        IMenu::Probe(subMenu)->Add(0, i, i, csq,(IMenuItem**)&item);
        AutoPtr<IDrawable> icon;
        activity->LoadIcon(packageManager, (IDrawable**)&icon);
        item->SetIcon(icon);
        item->SetOnMenuItemClickListener(mOnMenuItemClickListener);
    }

    if (collapsedActivityCount < expandedActivityCount) {
        // Add a sub-menu for showing all activities as a list item.
        String str;
        mContext->GetString(R::string::activity_chooser_view_see_all, &str);
        AutoPtr<ICharSequence> strcsq;
        CString::New(str, (ICharSequence**)&strcsq);
        AutoPtr<ISubMenu> expandedSubMenu;
        IMenu::Probe(subMenu)->AddSubMenu(IMenu::NONE, collapsedActivityCount, collapsedActivityCount, strcsq, (ISubMenu**)&expandedSubMenu);

        for (Int32 i = 0; i < expandedActivityCount; i++) {
            AutoPtr<IResolveInfo> activity;
            dataModel->GetActivity(i, (IResolveInfo**)&activity);
            AutoPtr<ICharSequence> lebel;
            activity->LoadLabel(packageManager, (ICharSequence**)&lebel);
            AutoPtr<IMenuItem> itemMenu;
            IMenu::Probe(expandedSubMenu)->Add(0, i, i, lebel,(IMenuItem**)&itemMenu);
            AutoPtr<IDrawable> drawable;
            activity->LoadIcon(packageManager, (IDrawable**)&drawable);
            itemMenu->SetIcon(drawable);
            itemMenu->SetOnMenuItemClickListener(mOnMenuItemClickListener);
        }
    }
    return NOERROR;
}

ECode ShareActionProvider::SetShareHistoryFileName(
    /* [in] */ const String& shareHistoryFile)
{
    mShareHistoryFileName = shareHistoryFile;
    SetActivityChooserPolicyIfNeeded();
    return NOERROR;
}

ECode ShareActionProvider::SetShareIntent(
    /* [in] */ IIntent* shareIntent)
{
    if (shareIntent != NULL) {
        String action;
        shareIntent->GetAction(&action);
        if (IIntent::ACTION_SEND.Equals(action) || IIntent::ACTION_SEND_MULTIPLE.Equals(action)) {
            shareIntent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_DOCUMENT |
                    IIntent::FLAG_ACTIVITY_MULTIPLE_TASK);
        }
    }
    AutoPtr<IActivityChooserModelHelper> helper;
    CActivityChooserModelHelper::AcquireSingleton((IActivityChooserModelHelper**)&helper);
    AutoPtr<IActivityChooserModel> dataModel;
    helper->Get(mContext, mShareHistoryFileName, (IActivityChooserModel**)&dataModel);
    dataModel->SetIntent(shareIntent);
    return NOERROR;
}

void ShareActionProvider::SetActivityChooserPolicyIfNeeded()
{
    if (mOnShareTargetSelectedListener == NULL) {
        return;
    }
    if (mOnChooseActivityListener == NULL) {
        mOnChooseActivityListener = new ShareActivityChooserModelPolicy(this);
    }
    AutoPtr<IActivityChooserModelHelper> helper;
    CActivityChooserModelHelper::AcquireSingleton((IActivityChooserModelHelper**)&helper);
    AutoPtr<IActivityChooserModel> dataModel;
    helper->Get(mContext, mShareHistoryFileName, (IActivityChooserModel**)&dataModel);
    dataModel->SetOnChooseActivityListener(mOnChooseActivityListener);
    return;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
