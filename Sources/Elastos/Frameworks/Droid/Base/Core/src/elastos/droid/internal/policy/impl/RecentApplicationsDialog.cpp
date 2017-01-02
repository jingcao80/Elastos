//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/internal/policy/impl/RecentApplicationsDialog.h"
#include "elastos/droid/internal/policy/impl/CPhoneWindowManager.h"
#include "elastos/droid/internal/policy/impl/IconUtilities.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/R.h"
#include "elastos/droid/view/SoundEffectConstants.h"
#include "elastos/droid/content/CIntent.h"

using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::R;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::View::SoundEffectConstants;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Core::CString;
using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::IList;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

//=====================================================================
//               RecentApplicationsDialog::InnerListener
//=====================================================================
CAR_INTERFACE_IMPL(RecentApplicationsDialog::InnerListener, Object, IViewOnClickListener)

RecentApplicationsDialog::InnerListener::InnerListener(
    /* [in] */ RecentApplicationsDialog* host)
    : mHost(host)
{}

ECode RecentApplicationsDialog::InnerListener::OnClick(
    /* [in] */ IView* view)
{
    return mHost->OnClick(view);
}

//=====================================================================
//               RecentApplicationsDialog::MyBroadcastReceiver
//=====================================================================
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

//=====================================================================
//               RecentApplicationsDialog::CleanupRunnable
//=====================================================================
CAR_INTERFACE_IMPL(RecentApplicationsDialog::CleanupRunnable, Object, IRunnable)

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
            IView* view = IView::Probe(icon);
            view->SetTag(NULL);
        }
    }
    return NOERROR;
}

//=====================================================================
//                       RecentApplicationsDialog
//=====================================================================


const String RecentApplicationsDialog::TAG("RecentApplicationsDialog");
const Boolean RecentApplicationsDialog::DBG_FORCE_EMPTY_LIST = FALSE;
AutoPtr<IStatusBarManager> RecentApplicationsDialog::sStatusBar;
const Int32 RecentApplicationsDialog::NUM_BUTTONS = 8;
const Int32 RecentApplicationsDialog::MAX_RECENT_TASKS = 16;//NUM_BUTTONS * 2;    // allow for some discards

RecentApplicationsDialog::RecentApplicationsDialog()
{
    mIcons = ArrayOf<ITextView*>::Alloc(NUM_BUTTONS);
    CHandler::New((IHandler**)&mHandler);
    mCleanup = new CleanupRunnable(this);
    mBroadcastReceiver = new MyBroadcastReceiver(this);
}

ECode RecentApplicationsDialog::constructor(
    /* [in] */ IContext* ctx)
{
    return Init(ctx);
}

ECode RecentApplicationsDialog::Init(
    /* [in] */ IContext* context)
{
    return Dialog::constructor(context, R::style::Theme_Dialog_RecentApplications);
}

ECode RecentApplicationsDialog::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Dialog::OnCreate(savedInstanceState);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);

    if (sStatusBar == NULL) {
        AutoPtr<IInterface> obj;
        context->GetSystemService(IContext::STATUS_BAR_SERVICE, (IInterface**)&obj);
        sStatusBar = IStatusBarManager::Probe(obj.Get());
    }

    Boolean bval;
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
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
    IViewGroupLayoutParams* vgParams = IViewGroupLayoutParams::Probe(params);
    vgParams->SetWidth(IViewGroupLayoutParams::MATCH_PARENT);
    vgParams->SetHeight(IViewGroupLayoutParams::MATCH_PARENT);
    window->SetAttributes(params);
    window->SetFlags(0, IWindowManagerLayoutParams::FLAG_DIM_BEHIND);

    AutoPtr<IView> tempView;
    FindViewById(R::id::button0, (IView**)&tempView);
    assert(tempView.Get() != NULL);
    mIcons->Set(0, ITextView::Probe(tempView.Get()));
    FindViewById(R::id::button1, (IView**)&tempView);
    assert(tempView.Get() != NULL);
    mIcons->Set(1, ITextView::Probe(tempView.Get()));
    FindViewById(R::id::button2, (IView**)&tempView);
    assert(tempView.Get() != NULL);
    mIcons->Set(2, ITextView::Probe(tempView.Get()));
    FindViewById(R::id::button3, (IView**)&tempView);
    assert(tempView.Get() != NULL);
    mIcons->Set(3, ITextView::Probe(tempView.Get()));
    FindViewById(R::id::button4, (IView**)&tempView);
    assert(tempView.Get() != NULL);
    mIcons->Set(4, ITextView::Probe(tempView.Get()));
    FindViewById(R::id::button5, (IView**)&tempView);
    assert(tempView.Get() != NULL);
    mIcons->Set(5, ITextView::Probe(tempView.Get()));
    FindViewById(R::id::button6, (IView**)&tempView);
    assert(tempView.Get() != NULL);
    mIcons->Set(6, ITextView::Probe(tempView.Get()));
    FindViewById(R::id::button7, (IView**)&tempView);
    assert(tempView.Get() != NULL);
    mIcons->Set(7, ITextView::Probe(tempView.Get()));

    FindViewById(R::id::no_applications_message, (IView**)&mNoAppsText);

    AutoPtr<InnerListener> listener = new InnerListener(this);
    for (Int32 i = 0; i < mIcons->GetLength(); ++i) {
        AutoPtr<ITextView> icon = (*mIcons)[i];
        if (icon) {
            IView* v = IView::Probe(icon);
            v->SetOnClickListener(listener);
        }
    }
    return NOERROR;
}

ECode RecentApplicationsDialog::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
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
            IView* v = IView::Probe(icon);
            v->GetVisibility(&visible);
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
                IView* v = IView::Probe(icon);
                if (v->HasFocus(&hasFocus), hasFocus) {
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
            IView* v = IView::Probe(icon);
            Boolean bval;
            if (v->RequestFocus(direction, &bval), bval) {
                v->PlaySoundEffect(
                    SoundEffectConstants::GetContantForFocusDirection(direction));
            }
        }

        // The dialog always handles the key to prevent the ViewRoot from
        // performing the default navigation itself.
        *result = TRUE;
    }

    Dialog::OnKeyDown(keyCode, event, result);
    return NOERROR;
}

ECode RecentApplicationsDialog::DismissAndSwitch()
{
    Int32 numIcons = mIcons->GetLength();
    AutoPtr<RecentTag> tag = NULL;
    Int32 visible;
    Boolean hasFocus;
    for (Int32 i = 0; i < numIcons; i++) {
        AutoPtr<ITextView> icon = (*mIcons)[i];
        IView* v = IView::Probe(icon);
        v->GetVisibility(&visible);
        if (visible != IView::VISIBLE) {
            break;
        }

        if (i == 0 || (v->HasFocus(&hasFocus), hasFocus)) {
            AutoPtr<IInterface> obj;
            v->GetTag((IInterface**)&obj);
            tag = (RecentTag*)(IObject::Probe(obj.Get()));
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
            AutoPtr<IView> v = IView::Probe(icon);
            v->GetTag((IInterface**)&obj);
            AutoPtr<RecentTag> tag = (RecentTag*)(IObject::Probe(obj.Get()));
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
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
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
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            context->StartActivity(intent);
        // } catch (ActivityNotFoundException e) {
        //     Log.w("Recent", "Unable to launch recent task", e);
        // }
    }
}

ECode RecentApplicationsDialog::OnStart()
{
    Dialog::OnStart();

    ReloadButtons();
    if (sStatusBar != NULL) {
        sStatusBar->Disable(IStatusBarManager::DISABLE_EXPAND);
    }

    // receive broadcasts
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IIntent> stikyIntent;
    context->RegisterReceiver(mBroadcastReceiver, mBroadcastIntentFilter, (IIntent**)&stikyIntent);

    mHandler->RemoveCallbacks(mCleanup);
    return NOERROR;
}

ECode RecentApplicationsDialog::OnStop()
{
    Dialog::OnStop();

    if (sStatusBar != NULL) {
        sStatusBar->Disable(IStatusBarManager::DISABLE_NONE);
    }

    // stop receiving broadcasts
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    context->UnregisterReceiver(mBroadcastReceiver);

    Boolean result;
    mHandler->PostDelayed(mCleanup, 100, &result);
    return NOERROR;
}

void RecentApplicationsDialog::ReloadButtons()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IInterface> tmpObj;
    context->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&tmpObj);
    AutoPtr<IActivityManager> am = IActivityManager::Probe(tmpObj);

    AutoPtr<IList> recentTasks;
    am->GetRecentTasks(MAX_RECENT_TASKS, IActivityManager::RECENT_IGNORE_UNAVAILABLE,
        (IList**)&recentTasks);

    AutoPtr<IActivityInfo> homeInfo;
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&intent);
    intent->AddCategory(IIntent::CATEGORY_HOME);
    intent->ResolveActivityInfo(pm, 0, (IActivityInfo**)&homeInfo);

    AutoPtr<IconUtilities> iconUtilities = new IconUtilities(context);

    // Performance note:  Our android performance guide says to prefer Iterator when
    // using a List class, but because we know that getRecentTasks() always returns
    // an ArrayList<>, we'll use a simple index instead.

    AutoPtr<IIterator> iterator;
    recentTasks->GetIterator((IIterator**)&iterator);

    Int32 flags = 0;
    Int32 i = 0;
    Int32 index = 0;
    Boolean hasNext = FALSE;
    while ((iterator->HasNext(&hasNext), hasNext) && (index < NUM_BUTTONS)) {
        AutoPtr<IInterface> obj;
        iterator->GetNext((IInterface**)&obj);
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
            AutoPtr<IPackageItemInfo> pii = IPackageItemInfo::Probe(homeInfo);
            pii->GetPackageName(&pkgName);
            pii->GetName(&name);
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
            AutoPtr<IPackageItemInfo> aiPii = IPackageItemInfo::Probe(activityInfo);
            AutoPtr<ICharSequence> seq;
            aiPii->LoadLabel(pm, (ICharSequence**)&seq);
            String title;
            seq->ToString(&title);

            AutoPtr<IDrawable> icon;
            aiPii->LoadIcon(pm, (IDrawable**)&icon);

            if (!title.IsNull() && title.GetLength() > 0 && icon != NULL) {
                AutoPtr<ITextView> tv = (*mIcons)[index];
                IView* v = IView::Probe(tv);
                tv->SetText(seq);
                icon = iconUtilities->CreateIconDrawable(icon);
                tv->SetCompoundDrawables(NULL, icon, NULL, NULL);
                AutoPtr<RecentTag> tag = new RecentTag();
                tag->mInfo = info;
                tag->mIntent = newIntent;
                v->SetTag(TO_IINTERFACE(tag));
                v->SetVisibility(IView::VISIBLE);
                v->SetPressed(FALSE);
                v->ClearFocus();
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
        IView* v = IView::Probe(icon);
        v->SetVisibility(IView::GONE);
    }
}


} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos
