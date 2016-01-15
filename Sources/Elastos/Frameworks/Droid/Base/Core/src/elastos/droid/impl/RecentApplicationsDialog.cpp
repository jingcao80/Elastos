#include "elastos/droid/impl/RecentApplicationsDialog.h"
#include "elastos/droid/impl/CPhoneWindowManager.h"
#include "elastos/droid/impl/IconUtilities.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/R.h"
#include "elastos/droid/view/SoundEffectConstants.h"
#include "elastos/droid/content/CIntent.h"

using Elastos::Core::CString;
using Elastos::Droid::R;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::View::SoundEffectConstants;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

const String RecentApplicationsDialog::TAG("RecentApplicationsDialog");
const Boolean RecentApplicationsDialog::DBG_FORCE_EMPTY_LIST = FALSE;

AutoPtr<IStatusBarManager> RecentApplicationsDialog::sStatusBar;

const Int32 RecentApplicationsDialog::NUM_BUTTONS = 8;
const Int32 RecentApplicationsDialog::MAX_RECENT_TASKS = 16;//NUM_BUTTONS * 2;    // allow for some discards

RecentApplicationsDialog::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ RecentApplicationsDialog* host)
    : mHost(host)
{
}

ECode RecentApplicationsDialog::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(intent);

    String action;
    intent->GetAction(&action);
    if (action.Equals(IIntent::ACTION_CLOSE_SYSTEM_DIALOGS)) {
        String reason;
        intent->GetStringExtra(CPhoneWindowManager::SYSTEM_DIALOG_REASON_KEY, &reason);
        if (!CPhoneWindowManager::SYSTEM_DIALOG_REASON_RECENT_APPS.Equals(reason)) {
            mHost->Dismiss();
        }
    }

    return NOERROR;
}

RecentApplicationsDialog::CleanupRunnable::CleanupRunnable(
   /* [in] */ RecentApplicationsDialog* host)
    : mHost(host)
{
}

ECode RecentApplicationsDialog::CleanupRunnable::Run()
{
    // dump extra memory we're hanging on to
    for (Int32 i = 0; i < mHost->mIcons->GetLength(); ++i) {
        AutoPtr<ITextView> icon = (*mHost->mIcons)[i];
        if (icon) {
            icon->SetCompoundDrawables(NULL, NULL, NULL, NULL);
            icon->SetTag(NULL);
        }
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL_LIGHT(RecentApplicationsDialog::RecentTag, IInterface)

RecentApplicationsDialog::RecentApplicationsDialog()
{
    mIcons = ArrayOf<ITextView*>::Alloc(NUM_BUTTONS);
    CHandler::New((IHandler**)&mHandler);
    mCleanup = new CleanupRunnable(this);
    mBroadcastReceiver = new MyBroadcastReceiver(this);
}

ECode RecentApplicationsDialog::Init(
    /* [in] */ IContext* context)
{
    return Dialog::Init(context, R::style::Theme_Dialog_RecentApplications);
}

void RecentApplicationsDialog::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Dialog::OnCreate(savedInstanceState);

    AutoPtr<IContext> context = GetContext();

    if (sStatusBar == NULL) {
        AutoPtr<IInterface> obj;
        context->GetSystemService(IContext::STATUS_BAR_SERVICE, (IInterface**)&obj);
        sStatusBar = IStatusBarManager::Probe(obj.Get());
    }

    Boolean bval;
    AutoPtr<IWindow> window = GetWindow();
    window->RequestFeature(IWindow::FEATURE_NO_TITLE, &bval);
    window->SetType(IWindowManagerLayoutParams::TYPE_RECENTS_OVERLAY);
    window->SetFlags(
        IWindowManagerLayoutParams::FLAG_ALT_FOCUSABLE_IM,
        IWindowManagerLayoutParams::FLAG_ALT_FOCUSABLE_IM);
    AutoPtr<ICharSequence> seq;
    CString::New(String("Recents"), (ICharSequence**)&seq);
    window->SetTitle(seq);

    SetContentView(R::layout::recent_apps_dialog);

    AutoPtr<IWindowManagerLayoutParams> params;
    window->GetAttributes((IWindowManagerLayoutParams**)&params);
    params->SetWidth(IWindowManagerLayoutParams::MATCH_PARENT);
    params->SetHeight(IWindowManagerLayoutParams::MATCH_PARENT);
    window->SetAttributes(params);
    window->SetFlags(0, IWindowManagerLayoutParams::FLAG_DIM_BEHIND);

    AutoPtr<IView> tempView;
    tempView = FindViewById(R::id::button0);
    assert(tempView.Get() != NULL);
    mIcons->Set(0, ITextView::Probe(tempView.Get()));
    tempView = FindViewById(R::id::button1);
    assert(tempView.Get() != NULL);
    mIcons->Set(1, ITextView::Probe(tempView.Get()));
    tempView = FindViewById(R::id::button2);
    assert(tempView.Get() != NULL);
    mIcons->Set(2, ITextView::Probe(tempView.Get()));
    tempView = FindViewById(R::id::button3);
    assert(tempView.Get() != NULL);
    mIcons->Set(3, ITextView::Probe(tempView.Get()));
    tempView = FindViewById(R::id::button4);
    assert(tempView.Get() != NULL);
    mIcons->Set(4, ITextView::Probe(tempView.Get()));
    tempView = FindViewById(R::id::button5);
    assert(tempView.Get() != NULL);
    mIcons->Set(5, ITextView::Probe(tempView.Get()));
    tempView = FindViewById(R::id::button6);
    assert(tempView.Get() != NULL);
    mIcons->Set(6, ITextView::Probe(tempView.Get()));
    tempView = FindViewById(R::id::button7);
    assert(tempView.Get() != NULL);
    mIcons->Set(7, ITextView::Probe(tempView.Get()));

    mNoAppsText = FindViewById(R::id::no_applications_message);

    for (Int32 i = 0; i < mIcons->GetLength(); ++i) {
        AutoPtr<ITextView> icon = (*mIcons)[i];
        if (icon) {
            icon->SetOnClickListener(THIS_PROBE(IViewOnClickListener));
        }
    }
}

Boolean RecentApplicationsDialog::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event)
{
    if (keyCode == IKeyEvent::KEYCODE_TAB) {
        // Ignore all meta keys other than SHIFT.  The app switch key could be a
        // fallback action chorded with ALT, META or even CTRL depending on the key map.
        // DPad navigation is handled by the ViewRoot elsewhere.
        Boolean backward;
        event->IsShiftPressed(&backward);
        Int32 numIcons = mIcons->GetLength();
        Int32 numButtons = 0;
        Int32 visible;
        for (Int32 i = 0; i < numIcons; ++i) {
            AutoPtr<ITextView> icon = (*mIcons)[i];
            icon->GetVisibility(&visible);
            if (visible == IView::VISIBLE) {
                numButtons += 1;
            }
            else {
                break;
            }
        }

        if (numButtons != 0) {
            Boolean hasFocus;
            Int32 nextFocus = backward ? numButtons - 1 : 0;
            for (Int32 i = 0; i < numButtons; i++) {
                AutoPtr<ITextView> icon = (*mIcons)[i];
                if (icon->HasFocus(&hasFocus), hasFocus) {
                    if (backward) {
                        nextFocus = (i + numButtons - 1) % numButtons;
                    }
                    else {
                        nextFocus = (i + 1) % numButtons;
                    }
                    break;
                }
            }

            Int32 direction = backward ? IView::FOCUS_BACKWARD : IView::FOCUS_FORWARD;
            AutoPtr<ITextView> icon = (*mIcons)[nextFocus];
            Boolean bval;
            if (icon->RequestFocus(direction, &bval), bval) {
                icon->PlaySoundEffect(
                    SoundEffectConstants::GetContantForFocusDirection(direction));
            }
        }

        // The dialog always handles the key to prevent the ViewRoot from
        // performing the default navigation itself.
        return TRUE;
    }

    return Dialog::OnKeyDown(keyCode, event);
}

ECode RecentApplicationsDialog::DismissAndSwitch()
{
    Int32 numIcons = mIcons->GetLength();
    AutoPtr<RecentTag> tag = NULL;
    Int32 visible;
    Boolean hasFocus;
    for (Int32 i = 0; i < numIcons; i++) {
        AutoPtr<ITextView> icon = (*mIcons)[i];
        icon->GetVisibility(&visible);
        if (visible != IView::VISIBLE) {
            break;
        }

        if (i == 0 || (icon->HasFocus(&hasFocus), hasFocus)) {
            AutoPtr<IInterface> obj;
            icon->GetTag((IInterface**)&obj);
            tag = (RecentTag*)obj.Get();
            if (hasFocus) {
                break;
            }
        }
    }

    if (tag != NULL) {
        SwitchTo(tag);
    }
    Dismiss();
    return NOERROR;
}

ECode RecentApplicationsDialog::OnClick(
    /* [in] */ IView* view)
{
    Int32 numIcons = mIcons->GetLength();
    for (Int32 i = 0; i < numIcons; i++) {
        AutoPtr<ITextView> icon = (*mIcons)[i];
        if (icon.Get() == ITextView::Probe(view)) {
            AutoPtr<IInterface> obj;
            icon->GetTag((IInterface**)&obj);
            AutoPtr<RecentTag> tag = (RecentTag*)obj.Get();
            SwitchTo(tag);
            break;
        }
    }
    Dismiss();

    return NOERROR;
}


void RecentApplicationsDialog::SwitchTo(
    /* [in] */ RecentTag* tag)
{
    assert(tag != NULL);

    Int32 id;
    tag->mInfo->GetId(&id);
    AutoPtr<IIntent> intent;
    tag->mInfo->GetBaseIntent((IIntent**)&intent);
    if (id >= 0) {
        AutoPtr<IContext> context = GetContext();
        AutoPtr<IInterface> tmpObj;
        context->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&tmpObj);
        AutoPtr<IActivityManager> am = IActivityManager::Probe(tmpObj.Get());

        // This is an active task; it should just go to the foreground.
        am->MoveTaskToFront(id, IActivityManager::MOVE_TASK_WITH_HOME);
    }
    else if (intent != NULL) {
        intent->AddFlags(
            IIntent::FLAG_ACTIVITY_LAUNCHED_FROM_HISTORY
            | IIntent::FLAG_ACTIVITY_TASK_ON_HOME);
        // try {
            AutoPtr<IContext> context = GetContext();
            context->StartActivity(intent);
        // } catch (ActivityNotFoundException e) {
        //     Log.w("Recent", "Unable to launch recent task", e);
        // }
    }
}

void RecentApplicationsDialog::OnStart()
{
    Dialog::OnStart();

    ReloadButtons();
    if (sStatusBar != NULL) {
        sStatusBar->Disable(IStatusBarManager::DISABLE_EXPAND);
    }

    // receive broadcasts
    AutoPtr<IContext> context = GetContext();
    AutoPtr<IIntent> stikyIntent;
    context->RegisterReceiver(mBroadcastReceiver, mBroadcastIntentFilter, (IIntent**)&stikyIntent);

    mHandler->RemoveCallbacks(mCleanup);
}

void RecentApplicationsDialog::OnStop()
{
    Dialog::OnStop();

    if (sStatusBar != NULL) {
        sStatusBar->Disable(IStatusBarManager::DISABLE_NONE);
    }

    // stop receiving broadcasts
    AutoPtr<IContext> context = GetContext();
    context->UnregisterReceiver(mBroadcastReceiver);

    Boolean result;
    mHandler->PostDelayed(mCleanup, 100, &result);
}

void RecentApplicationsDialog::ReloadButtons()
{
    AutoPtr<IContext> context = GetContext();
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IInterface> tmpObj;
    context->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&tmpObj);
    AutoPtr<IActivityManager> am = IActivityManager::Probe(tmpObj.Get());

    AutoPtr<IObjectContainer> recentTasks;
    am->GetRecentTasks(MAX_RECENT_TASKS, IActivityManager::RECENT_IGNORE_UNAVAILABLE,
        (IObjectContainer**)&recentTasks);

    AutoPtr<IActivityInfo> homeInfo;
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&intent);
    intent->AddCategory(IIntent::CATEGORY_HOME);
    intent->ResolveActivityInfo(pm, 0, (IActivityInfo**)&homeInfo);

    AutoPtr<IconUtilities> iconUtilities = new IconUtilities(context);

    // Performance note:  Our android performance guide says to prefer Iterator when
    // using a List class, but because we know that getRecentTasks() always returns
    // an ArrayList<>, we'll use a simple index instead.

    AutoPtr<IObjectEnumerator> enumerator;
    recentTasks->GetObjectEnumerator((IObjectEnumerator**)&enumerator);

    Int32 flags = 0;
    Int32 i = 0;
    Int32 index = 0;
    Boolean hasNext = FALSE;
    while ((enumerator->MoveNext(&hasNext), hasNext) && (index < NUM_BUTTONS)) {
        AutoPtr<IInterface> obj;
        enumerator->Current((IInterface**)&obj);
        AutoPtr<IActivityManagerRecentTaskInfo> info = IActivityManagerRecentTaskInfo::Probe(obj.Get());

        // for debug purposes only, disallow first result to create empty lists
        if (DBG_FORCE_EMPTY_LIST && (i == 0)) {
            ++i;
            continue;
        }

        AutoPtr<IIntent> newIntent, baseIntent;
        info->GetBaseIntent((IIntent**)&baseIntent);
        CIntent::New(baseIntent, (IIntent**)&newIntent);

        AutoPtr<IComponentName> cn;
        info->GetOrigActivity((IComponentName**)&cn);
        if (cn != NULL) {
            newIntent->SetComponent(cn);
        }

        // Skip the current home activity.
        if (homeInfo != NULL) {
            String pkgName, name, intentPkgName, intentClsName;
            homeInfo->GetPackageName(&pkgName);
            homeInfo->GetName(&name);
            AutoPtr<IComponentName> cn;
            newIntent->GetComponent((IComponentName**)&cn);
            cn->GetPackageName(&intentPkgName);
            cn->GetClassName(&intentClsName);
            if (pkgName.Equals(intentPkgName) && name.Equals(intentClsName)) {
                ++i;
                continue;
            }
        }

        newIntent->GetFlags(&flags);
        newIntent->SetFlags((flags & ~IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED)
                | IIntent::FLAG_ACTIVITY_NEW_TASK);
        AutoPtr<IResolveInfo> resolveInfo;
        pm->ResolveActivity(newIntent, 0, (IResolveInfo**)&resolveInfo);
        if (resolveInfo != NULL) {
            AutoPtr<IActivityInfo> activityInfo;
            resolveInfo->GetActivityInfo((IActivityInfo**)&activityInfo);
            AutoPtr<ICharSequence> seq;
            activityInfo->LoadLabel(pm, (ICharSequence**)&seq);
            String title;
            seq->ToString(&title);

            AutoPtr<IDrawable> icon;
            activityInfo->LoadIcon(pm, (IDrawable**)&icon);

            if (!title.IsNull() && title.GetLength() > 0 && icon != NULL) {
                AutoPtr<ITextView> tv = (*mIcons)[index];
                tv->SetText(seq);
                icon = iconUtilities->CreateIconDrawable(icon);
                tv->SetCompoundDrawables(NULL, icon, NULL, NULL);
                AutoPtr<RecentTag> tag = new RecentTag();
                tag->mInfo = info;
                tag->mIntent = newIntent;
                tv->SetTag(tag);
                tv->SetVisibility(IView::VISIBLE);
                tv->SetPressed(FALSE);
                tv->ClearFocus();
                ++index;
            }
        }

        ++i;
    }

    // handle the case of "no icons to show"
    mNoAppsText->SetVisibility((index == 0) ? IView::VISIBLE : IView::GONE);

    // hide the rest
    for (; index < NUM_BUTTONS; ++index) {
        AutoPtr<ITextView> icon = (*mIcons)[index];
        icon->SetVisibility(IView::GONE);
    }
}


} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namepsace Droid
} // namespace Elastos
