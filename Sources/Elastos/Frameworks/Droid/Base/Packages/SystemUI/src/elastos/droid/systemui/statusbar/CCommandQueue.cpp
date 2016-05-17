
#include "elastos/droid/systemui/statusbar/CCommandQueue.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Internal::StatusBar::CStatusBarIcon;
using Elastos::Droid::Internal::StatusBar::EIID_IIStatusBar;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandlerCallback;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Core::AutoLock;
using Elastos::Core::CString;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

const Int32 CCommandQueue::INDEX_MASK = 0xffff;
const Int32 CCommandQueue::MSG_SHIFT  = 16;
const Int32 CCommandQueue::MSG_MASK   = 0xffff << MSG_SHIFT;

const Int32 CCommandQueue::OP_SET_ICON    = 1;
const Int32 CCommandQueue::OP_REMOVE_ICON = 2;

const Int32 CCommandQueue::MSG_ICON                       = 1 << MSG_SHIFT;
const Int32 CCommandQueue::MSG_DISABLE                    = 2 << MSG_SHIFT;
const Int32 CCommandQueue::MSG_EXPAND_NOTIFICATIONS       = 3 << MSG_SHIFT;
const Int32 CCommandQueue::MSG_COLLAPSE_PANELS            = 4 << MSG_SHIFT;
const Int32 CCommandQueue::MSG_EXPAND_SETTINGS            = 5 << MSG_SHIFT;
const Int32 CCommandQueue::MSG_SET_SYSTEMUI_VISIBILITY    = 6 << MSG_SHIFT;
const Int32 CCommandQueue::MSG_TOP_APP_WINDOW_CHANGED     = 7 << MSG_SHIFT;
const Int32 CCommandQueue::MSG_SHOW_IME_BUTTON            = 8 << MSG_SHIFT;
const Int32 CCommandQueue::MSG_TOGGLE_RECENT_APPS         = 9 << MSG_SHIFT;
const Int32 CCommandQueue::MSG_PRELOAD_RECENT_APPS        = 10 << MSG_SHIFT;
const Int32 CCommandQueue::MSG_CANCEL_PRELOAD_RECENT_APPS = 11 << MSG_SHIFT;
const Int32 CCommandQueue::MSG_SET_WINDOW_STATE           = 12 << MSG_SHIFT;
const Int32 CCommandQueue::MSG_SHOW_RECENT_APPS           = 13 << MSG_SHIFT;
const Int32 CCommandQueue::MSG_HIDE_RECENT_APPS           = 14 << MSG_SHIFT;
const Int32 CCommandQueue::MSG_BUZZ_BEEP_BLINKED          = 15 << MSG_SHIFT;
const Int32 CCommandQueue::MSG_NOTIFICATION_LIGHT_OFF     = 16 << MSG_SHIFT;
const Int32 CCommandQueue::MSG_NOTIFICATION_LIGHT_PULSE   = 17 << MSG_SHIFT;
const String CCommandQueue::SHOW_IME_SWITCHER_KEY("showImeSwitcherKey");

CAR_OBJECT_IMPL(CCommandQueue);
CAR_INTERFACE_IMPL_3(CCommandQueue, Object, ICommandQueue, IIStatusBar, IBinder);
ECode CCommandQueue::constructor(
    /* [in] */ ICommandQueueCallbacks* callbacks,
    /* [in] */ IStatusBarIconList* list)
{
    mCallbacks = callbacks;
    mList = list;
    mHandler = new MyHandler(this);
    return NOERROR;
}

ECode CCommandQueue::SetIconList(
    /* [in] */ IStatusBarIconList* list)
{
    mList = list;
    return NOERROR;
}

ECode CCommandQueue::SetIcon(
    /* [in] */ Int32 index,
    /* [in] */ IStatusBarIcon* icon)
{
    AutoLock lock(mListLock);
    Int32 what = MSG_ICON | index;
    mHandler->RemoveMessages(what);
    AutoPtr<IStatusBarIcon> c;
    icon->Clone((IStatusBarIcon**)&c);
    AutoPtr<IMessage> m;
    mHandler->ObtainMessage(what, OP_SET_ICON, 0, c, (IMessage**)&m);
    m->SendToTarget();
    return NOERROR;
}

ECode CCommandQueue::RemoveIcon(
    /* [in] */ Int32 index)
{
    AutoLock lock(mListLock);

    Int32 what = MSG_ICON | index;
    mHandler->RemoveMessages(what);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(what, OP_REMOVE_ICON, 0, (IMessage**)&msg);
    Boolean result;
    return mHandler->SendMessage(msg, &result);
}

ECode CCommandQueue::Disable(
    /* [in] */ Int32 state)
{
    AutoLock lock(mListLock);

    mHandler->RemoveMessages(MSG_DISABLE);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_DISABLE, 0, 0, (IMessage**)&msg);
    Boolean result;
    return mHandler->SendMessage(msg, &result);
}

ECode CCommandQueue::AnimateExpandNotificationsPanel()
{
    AutoLock lock(mListLock);

    mHandler->RemoveMessages(MSG_EXPAND_NOTIFICATIONS);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_EXPAND_NOTIFICATIONS, 0, 0, (IMessage**)&msg);
    Boolean result;
    return mHandler->SendMessage(msg, &result);
}

ECode CCommandQueue::AnimateExpandSettingsPanel()
{
    AutoLock lock(mListLock);

    mHandler->RemoveMessages(MSG_EXPAND_SETTINGS);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_EXPAND_SETTINGS, 0, 0, (IMessage**)&msg);
    Boolean result;
    return mHandler->SendMessage(msg, &result);
}

ECode CCommandQueue::AnimateCollapsePanels()
{
    AutoLock lock(mListLock);

    mHandler->RemoveMessages(MSG_COLLAPSE_PANELS);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_COLLAPSE_PANELS, 0, 0, (IMessage**)&msg);
    Boolean result;
    return mHandler->SendMessage(msg, &result);
}

ECode CCommandQueue::SetSystemUiVisibility(
    /* [in] */ Int32 vis,
    /* [in] */ Int32 mask)
{
    AutoLock lock(mListLock);

    mHandler->RemoveMessages(MSG_SET_SYSTEMUI_VISIBILITY);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_SET_SYSTEMUI_VISIBILITY,
        vis, mask, (IMessage**)&msg);
    Boolean result;
    return mHandler->SendMessage(msg, &result);
}

ECode CCommandQueue::TopAppWindowChanged(
    /* [in] */ Boolean menuVisible)
{
    AutoLock lock(mListLock);

    mHandler->RemoveMessages(MSG_TOP_APP_WINDOW_CHANGED);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_TOP_APP_WINDOW_CHANGED,
        menuVisible ? 1 : 0, 0, (IMessage**)&msg);
    Boolean result;
    return mHandler->SendMessage(msg, &result);
}

ECode CCommandQueue::SetImeWindowStatus(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 vis,
    /* [in] */ Int32 backDisposition,
    /* [in] */ Boolean showImeSwitcher)
{
    AutoLock lock(mListLock);

    mHandler->RemoveMessages(MSG_SHOW_IME_BUTTON);

    AutoPtr<IMessage> m;
    mHandler->ObtainMessage(MSG_SHOW_IME_BUTTON, vis, backDisposition, token, (IMessage**)&m);
    AutoPtr<IBundle> data;
    m->GetData((IBundle**)&data);
    data->PutBoolean(SHOW_IME_SWITCHER_KEY, showImeSwitcher);
    m->SendToTarget();
    return NOERROR;
}

ECode CCommandQueue::ShowRecentApps(
    /* [in] */ Boolean triggeredFromAltTab)
{
    AutoLock lock(mListLock);
    mHandler->RemoveMessages(MSG_SHOW_RECENT_APPS);

    AutoPtr<IMessage> m;
    mHandler->ObtainMessage(MSG_SHOW_RECENT_APPS,
            triggeredFromAltTab ? 1 : 0, 0, NULL, (IMessage**)&m);
    m->SendToTarget();
    return NOERROR;
}

ECode CCommandQueue::HideRecentApps(
    /* [in] */ Boolean triggeredFromAltTab,
    /* [in] */ Boolean triggeredFromHomeKey)
{
    AutoLock lock(mListLock);
    mHandler->RemoveMessages(MSG_HIDE_RECENT_APPS);

    AutoPtr<IMessage> m;
    mHandler->ObtainMessage(MSG_HIDE_RECENT_APPS,
            triggeredFromAltTab ? 1 : 0, triggeredFromHomeKey ? 1 : 0,
            NULL, (IMessage**)&m);
    m->SendToTarget();
    return NOERROR;
}

ECode CCommandQueue::ToggleRecentApps()
{
    AutoLock lock(mListLock);

    mHandler->RemoveMessages(MSG_TOGGLE_RECENT_APPS);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_TOGGLE_RECENT_APPS, 0, 0, (IMessage**)&msg);
    Boolean result;
    return mHandler->SendMessage(msg, &result);
}

ECode CCommandQueue::PreloadRecentApps()
{
    AutoLock lock(mListLock);

    mHandler->RemoveMessages(MSG_PRELOAD_RECENT_APPS);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_PRELOAD_RECENT_APPS, 0, 0, (IMessage**)&msg);
    Boolean result;
    return mHandler->SendMessage(msg, &result);
}

ECode CCommandQueue::CancelPreloadRecentApps()
{
    AutoLock lock(mListLock);

    mHandler->RemoveMessages(MSG_CANCEL_PRELOAD_RECENT_APPS);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_CANCEL_PRELOAD_RECENT_APPS, 0, 0, (IMessage**)&msg);
    Boolean result;
    return mHandler->SendMessage(msg, &result);
}

ECode CCommandQueue::SetWindowState(
    /* [in] */ Int32 window,
    /* [in] */ Int32 state)
{
    AutoLock lock(mListLock);
    // don't coalesce these
    AutoPtr<IMessage> m;
    mHandler->ObtainMessage(MSG_SET_WINDOW_STATE, window, state, NULL, (IMessage**)&m);
    m->SendToTarget();
    return NOERROR;
}

ECode CCommandQueue::BuzzBeepBlinked()
{
    AutoLock lock(mListLock);
    mHandler->RemoveMessages(MSG_BUZZ_BEEP_BLINKED);
    Boolean tmp = FALSE;
    mHandler->SendEmptyMessage(MSG_BUZZ_BEEP_BLINKED, &tmp);
    return NOERROR;
}

ECode CCommandQueue::NotificationLightOff()
{
    AutoLock lock(mListLock);
    Boolean tmp = FALSE;
    mHandler->SendEmptyMessage(MSG_NOTIFICATION_LIGHT_OFF, &tmp);
    return NOERROR;
}

ECode CCommandQueue::NotificationLightPulse(
    /* [in] */ Int32 argb,
    /* [in] */ Int32 onMillis,
    /* [in] */ Int32 offMillis)
{
    AutoLock lock(mListLock);
    AutoPtr<IInteger32> obj;
    CInteger32::New(argb, (IInteger32**)&obj);
    AutoPtr<IMessage> m;
    mHandler->ObtainMessage(MSG_NOTIFICATION_LIGHT_PULSE, onMillis, offMillis, obj, (IMessage**)&m);
    m->SendToTarget();
    return NOERROR;
}

ECode CCommandQueue::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    StringBuilder sb("CCommandQueue: icons count=");

    Int32 count = 0;
    if (mList != NULL) {
        mList->Size(&count);
    }
    sb += count;
    *info = sb.ToString();
    return NOERROR;
}

//=============================================================================
CCommandQueue::MyHandler::MyHandler(
    /* [in] */ CCommandQueue* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode CCommandQueue::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch(what) {
        case CCommandQueue::MSG_ICON: {
            Int32 index = what & INDEX_MASK;
            IStatusBarIcon* icon = IStatusBarIcon::Probe(obj);
            if (arg1 == OP_SET_ICON) {
                mHost->HandleSetIcon(index, icon);
            }
            else if (arg1 == OP_REMOVE_ICON) {
                mHost->HandleRemoveIcon(index);
            }
            break;
        }
        case CCommandQueue::MSG_DISABLE:
            mHost->mCallbacks->Disable(arg1, TRUE /* animate */);
            break;
        case CCommandQueue::MSG_EXPAND_NOTIFICATIONS:
            mHost->mCallbacks->AnimateExpandNotificationsPanel();
            break;
        case CCommandQueue::MSG_COLLAPSE_PANELS:
            mHost->mCallbacks->AnimateCollapsePanels(0);
            break;
        case CCommandQueue::MSG_EXPAND_SETTINGS:
            mHost->mCallbacks->AnimateExpandSettingsPanel();
            break;
        case CCommandQueue::MSG_SET_SYSTEMUI_VISIBILITY:
            mHost->mCallbacks->SetSystemUiVisibility(arg1, arg2);
            break;
        case CCommandQueue::MSG_TOP_APP_WINDOW_CHANGED:
            mHost->mCallbacks->TopAppWindowChanged(arg1 != 0);
            break;
        case CCommandQueue::MSG_SHOW_IME_BUTTON: {
            AutoPtr<IBundle> data;
            msg->GetData((IBundle**)&data);
            Boolean value = FALSE;
            data->GetBoolean(SHOW_IME_SWITCHER_KEY, FALSE, &value);
            mHost->mCallbacks->SetImeWindowStatus(IBinder::Probe(obj), arg1, arg2, value);
        }
            break;
        case CCommandQueue::MSG_SHOW_RECENT_APPS:
            mHost->mCallbacks->ShowRecentApps(arg1 != 0);
            break;
        case MSG_HIDE_RECENT_APPS:
            mHost->mCallbacks->HideRecentApps(arg1 != 0, arg2 != 0);
        case CCommandQueue::MSG_TOGGLE_RECENT_APPS:
            mHost->mCallbacks->ToggleRecentApps();
            break;
        case CCommandQueue::MSG_PRELOAD_RECENT_APPS:
            mHost->mCallbacks->PreloadRecentApps();
            break;
        case CCommandQueue::MSG_CANCEL_PRELOAD_RECENT_APPS:
            mHost->mCallbacks->CancelPreloadRecentApps();
            break;
        case MSG_SET_WINDOW_STATE:
            mHost->mCallbacks->SetWindowState(arg1, arg2);
            break;
        case MSG_BUZZ_BEEP_BLINKED:
            mHost->mCallbacks->BuzzBeepBlinked();
            break;
        case MSG_NOTIFICATION_LIGHT_OFF:
            mHost->mCallbacks->NotificationLightOff();
            break;
        case MSG_NOTIFICATION_LIGHT_PULSE: {
            AutoPtr<IInteger32> data = IInteger32::Probe(obj);
            Int32 value = 0;
            data->GetValue(&value);
            mHost->mCallbacks->NotificationLightPulse(value, arg1, arg2);
            break;
        }
    }

    return NOERROR;
}

ECode CCommandQueue::HandleSetIcon(
    /* [in] */ Int32 index,
    /* [in] */ IStatusBarIcon* icon)
{
    assert(mList != NULL);

    AutoLock lock(mListLock);

    Int32 viewIndex;
    mList->GetViewIndex(index, &viewIndex);

    AutoPtr<IStatusBarIcon> old;
    mList->GetIcon(index, (IStatusBarIcon**)&old);
    if (old == NULL) {
        mList->SetIcon(index, icon);
        String slotIndex;
        mList->GetSlot(index, &slotIndex);
        mCallbacks->AddIcon(slotIndex, index, viewIndex, icon);
    }
    else {
        mList->SetIcon(index, icon);
        String slotIndex;
        mList->GetSlot(index, &slotIndex);
        mCallbacks->UpdateIcon(slotIndex, index, viewIndex, old, icon);
    }
    return NOERROR;
}

ECode CCommandQueue::HandleRemoveIcon(
    /* [in] */ Int32 index)
{
    assert(mList != NULL);

    AutoLock lock(mListLock);

    Int32 viewIndex;
    mList->GetViewIndex(index, &viewIndex);

    AutoPtr<IStatusBarIcon> icon;
    mList->GetIcon(index, (IStatusBarIcon**)&icon);
    if (icon != NULL) {
        mList->RemoveIcon(index);
        String slotIndex;
        mList->GetSlot(index, &slotIndex);
        mCallbacks->RemoveIcon(slotIndex, index, viewIndex);
    }

    return NOERROR;
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
