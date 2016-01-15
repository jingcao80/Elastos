
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/internal/app/HeavyWeightSwitcherActivity.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/utility/CTypedValue.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CAR_INTERFACE_IMPL(HeavyWeightSwitcherActivity::SwitchOldListener, Object, IViewOnClickListener)

ECode HeavyWeightSwitcherActivity::SwitchOldListener::OnClick(
    /* [in] */ IView* v)
{
    ActivityManagerNative::GetDefault()->MoveTaskToFront(mHost->mCurTask, 0, NULL);
    mHost->Finish();
    return NOERROR;
}

CAR_INTERFACE_IMPL(HeavyWeightSwitcherActivity::SwitchNewListener, Object, IViewOnClickListener)

ECode HeavyWeightSwitcherActivity::SwitchNewListener::OnClick(
    /* [in] */ IView* v)
{
    ActivityManagerNative::GetDefault()->FinishHeavyWeightApp();

    ECode ec;
    if (mHost->mHasResult) {
        ec = mHost->StartIntentSenderForResult(mHost->mStartIntent, -1, NULL,
                IIntent::FLAG_ACTIVITY_FORWARD_RESULT,
                IIntent::FLAG_ACTIVITY_FORWARD_RESULT, 0);
    }
    else {
        ec = mHost->StartIntentSenderForResult(mHost->mStartIntent, -1, NULL, 0, 0, 0);
    }

    if (ec == (ECode)E_SEND_INTENT_EXCEPTION) {
       Logger::W("HeavyWeightSwitcherActivity", "Failure starting");
    }
    mHost->Finish();
    return NOERROR;
}

CAR_INTERFACE_IMPL(HeavyWeightSwitcherActivity::CancelListener, Object, IViewOnClickListener)

ECode HeavyWeightSwitcherActivity::CancelListener::OnClick(
    /* [in] */ IView* v)
{
    mHost->Finish();
    return NOERROR;
}

CAR_INTERFACE_IMPL(HeavyWeightSwitcherActivity, Activity, IHeavyWeightSwitcherActivity)

HeavyWeightSwitcherActivity::HeavyWeightSwitcherActivity()
    : mStartIntent(NULL)
    , mHasResult(FALSE)
    , mCurApp(String(NULL))
    , mCurTask(0)
    , mNewApp(String(NULL))
    , mSwitchOldListener(new SwitchOldListener(this))
    , mSwitchNewListener(new SwitchNewListener(this))
    , mCancelListener(new CancelListener(this))
{}

ECode HeavyWeightSwitcherActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);

    Boolean supported;
    RequestWindowFeature(IWindow::FEATURE_LEFT_ICON, &supported);

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    AutoPtr<IParcelable> parcelable;
    intent->GetParcelableExtra(KEY_INTENT, (IParcelable**)&parcelable);
    mStartIntent = IIntentSender::Probe(parcelable);
    intent->GetBooleanExtra(IHeavyWeightSwitcherActivity::KEY_HAS_RESULT, FALSE, &mHasResult);
    intent->GetStringExtra(IHeavyWeightSwitcherActivity::KEY_CUR_APP, &mCurApp);
    intent->GetInt32Extra(IHeavyWeightSwitcherActivity::KEY_CUR_TASK, 0, &mCurTask);
    intent->GetStringExtra(IHeavyWeightSwitcherActivity::KEY_NEW_APP, &mNewApp);

    SetContentView(R::layout::heavy_weight_switcher);

    SetIconAndText(R::id::old_app_icon, R::id::old_app_action, R::id::old_app_description,
            mCurApp, R::string::old_app_action, R::string::old_app_description);
    SetIconAndText(R::id::new_app_icon, R::id::new_app_action, R::id::new_app_description,
            mNewApp, R::string::new_app_action, R::string::new_app_description);

    AutoPtr<IView> button;
    FindViewById(R::id::switch_old, (IView**)&button);
    button->SetOnClickListener(mSwitchOldListener);
    button = NULL;
    FindViewById(R::id::switch_new, (IView**)&button);
    button->SetOnClickListener(mSwitchNewListener);
    button = NULL;
    FindViewById(R::id::cancel, (IView**)&button);
    button->SetOnClickListener(mCancelListener);

    AutoPtr<ITypedValue> out;
    CTypedValue::New((ITypedValue**)&out);
    AutoPtr<IResourcesTheme> theme;
    GetTheme((IResourcesTheme**)&theme);
    Boolean founded;
    theme->ResolveAttribute(R::attr::alertDialogIcon, out, TRUE, &founded);
    Int32 resId;
    out->GetResourceId(&resId);
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    window->SetFeatureDrawableResource(IWindow::FEATURE_LEFT_ICON,resId);
    return NOERROR;
}

ECode HeavyWeightSwitcherActivity::SetText(
    /* [in] */ Int32 id,
    /* [in] */ ICharSequence* text)
{
    AutoPtr<ITextView> tView;
    FindViewById(id, (IView**)(ITextView**)&tView);
    tView->SetText(text);
    return NOERROR;
}

ECode HeavyWeightSwitcherActivity::SetDrawable(
    /* [in] */ Int32 id,
    /* [in] */ IDrawable* dr)
{
    if (dr != NULL) {
        AutoPtr<IView> imageView;
        FindViewById(id, (IView**)&imageView);
        IImageView::Probe(imageView)->SetImageDrawable(dr);
    }
    return NOERROR;
}

ECode HeavyWeightSwitcherActivity::SetIconAndText(
    /* [in] */ Int32 iconId,
    /* [in] */ Int32 actionId,
    /* [in] */ Int32 descriptionId,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 actionStr,
    /* [in] */ Int32 descriptionStr)
{
    AutoPtr<ICharSequence> appName;
    CString::New(String(""), (ICharSequence**)&appName);
    AutoPtr<IDrawable> appIcon;
    if (mCurApp != NULL) {
        AutoPtr<IPackageManager> pkgManager;
        GetPackageManager((IPackageManager**)&pkgManager);
        AutoPtr<IApplicationInfo> info;
        pkgManager->GetApplicationInfo(packageName, 0, (IApplicationInfo**)&info);
        if (info) {
            appName = NULL;
            IPackageItemInfo::Probe(info)->LoadLabel(pkgManager, (ICharSequence**)&appName);
            IPackageItemInfo::Probe(info)->LoadIcon(pkgManager, (IDrawable**)&appIcon);
        }
    }

    SetDrawable(iconId, appIcon);
    String str;
    AutoPtr<ArrayOf<IInterface*> > appNameArray= ArrayOf<IInterface*>::Alloc(1);
    appNameArray->Set(0, appName);
    GetString(actionStr, appNameArray, &str);
    AutoPtr<ICharSequence> strSequence;
    CString::New(str, (ICharSequence**)&strSequence);
    SetText(actionId, strSequence);
    AutoPtr<ICharSequence> txt;
    GetText(descriptionStr, (ICharSequence**)&txt);
    SetText(descriptionId, txt);
    return NOERROR;
}

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos
