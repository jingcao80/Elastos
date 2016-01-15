#include "elastos/droid/systemui/statusbar/CCommandQueue.h"
#include <elastos/core/StringBuilder.h>
#include "elastos/droid/statusbar/CStatusBarIcon.h"

using Elastos::Core::CString;
using Elastos::Core::StringBuilder;
using Elastos::Core::ICharSequence;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::StatusBar::CStatusBarIcon;

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
const Int32 CCommandQueue::MSG_ADD_NOTIFICATION           = 2 << MSG_SHIFT;
const Int32 CCommandQueue::MSG_UPDATE_NOTIFICATION        = 3 << MSG_SHIFT;
const Int32 CCommandQueue::MSG_REMOVE_NOTIFICATION        = 4 << MSG_SHIFT;
const Int32 CCommandQueue::MSG_DISABLE                    = 5 << MSG_SHIFT;
const Int32 CCommandQueue::MSG_EXPAND_NOTIFICATIONS       = 6 << MSG_SHIFT;
const Int32 CCommandQueue::MSG_COLLAPSE_PANELS            = 7 << MSG_SHIFT;
const Int32 CCommandQueue::MSG_EXPAND_SETTINGS            = 8 << MSG_SHIFT;
const Int32 CCommandQueue::MSG_SET_SYSTEMUI_VISIBILITY    = 9 << MSG_SHIFT;
const Int32 CCommandQueue::MSG_TOP_APP_WINDOW_CHANGED     = 10 << MSG_SHIFT;
const Int32 CCommandQueue::MSG_SHOW_IME_BUTTON            = 11 << MSG_SHIFT;
const Int32 CCommandQueue::MSG_SET_HARD_KEYBOARD_STATUS   = 12 << MSG_SHIFT;
const Int32 CCommandQueue::MSG_TOGGLE_RECENT_APPS         = 13 << MSG_SHIFT;
const Int32 CCommandQueue::MSG_PRELOAD_RECENT_APPS        = 14 << MSG_SHIFT;
const Int32 CCommandQueue::MSG_CANCEL_PRELOAD_RECENT_APPS = 15 << MSG_SHIFT;
const Int32 CCommandQueue::MSG_SET_NAVIGATION_ICON_HINTS  = 16 << MSG_SHIFT;

ECode CCommandQueue::constructor(
    /* [in] */ ICommandQueueCallbacks* callbacks)
{
    mCallbacks = callbacks;
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

    String iconPackage;
    icon->GetIconPackage(&iconPackage);
    Int32 iconId, iconLevel, number;
    icon->GetIconId(&iconId);
    icon->GetIconLevel(&iconLevel);
    icon->GetNumber(&number);
    AutoPtr<ICharSequence> seq;
    icon->GetContentDescription((ICharSequence**)&seq);
    AutoPtr<IUserHandle> user;
    icon->GetUser((IUserHandle**)&user);
    Boolean visible;
    icon->GetVisible(&visible);

    AutoPtr<IStatusBarIcon> cloneIcon;
    CStatusBarIcon::New(iconPackage, user,
        iconId, iconLevel, number, seq, (IStatusBarIcon**)&cloneIcon);
    cloneIcon->SetVisible(visible);

    Int32 what = MSG_ICON | index;
    mHandler->RemoveMessages(what);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(what, OP_SET_ICON, 0, cloneIcon, (IMessage**)&msg);
    Boolean result;
    return mHandler->SendMessage(msg, &result);
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

ECode CCommandQueue::AddNotification(
    /* [in] */ IBinder* key,
    /* [in] */ IStatusBarNotification* notification)
{
    AutoLock lock(mListLock);

    AutoPtr<NotificationQueueEntry> ne = new NotificationQueueEntry();
    ne->mKey = key;
    ne->mNotification = notification;

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_ADD_NOTIFICATION, ne, (IMessage**)&msg);
    Boolean result;
    return mHandler->SendMessage(msg, &result);
}

ECode CCommandQueue::UpdateNotification(
    /* [in] */ IBinder* key,
    /* [in] */ IStatusBarNotification* notification)
{
    AutoLock lock(mListLock);

    AutoPtr<NotificationQueueEntry> ne = new NotificationQueueEntry();
    ne->mKey = key;
    ne->mNotification = notification;

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_UPDATE_NOTIFICATION, ne, (IMessage**)&msg);
    Boolean result;
    return mHandler->SendMessage(msg, &result);
}

ECode CCommandQueue::RemoveNotification(
    /* [in] */ IBinder* key)
{
    AutoLock lock(mListLock);

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_REMOVE_NOTIFICATION, key, (IMessage**)&msg);
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
    /* [in] */ Int32 backDisposition)
{
    AutoLock lock(mListLock);

    mHandler->RemoveMessages(MSG_SHOW_IME_BUTTON);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_SHOW_IME_BUTTON,
        vis, backDisposition, token, (IMessage**)&msg);
    Boolean result;
    return mHandler->SendMessage(msg, &result);
}

ECode CCommandQueue::SetHardKeyboardStatus(
    /* [in] */ Boolean available,
    /* [in] */ Boolean enabled)
{
    AutoLock lock(mListLock);

    mHandler->RemoveMessages(MSG_SET_HARD_KEYBOARD_STATUS);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_SET_HARD_KEYBOARD_STATUS,
        available ? 1 : 0, enabled ? 1 : 0, (IMessage**)&msg);
    Boolean result;
    return mHandler->SendMessage(msg, &result);
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

ECode CCommandQueue::SetNavigationIconHints(
    /* [in] */ Int32 hints)
{
    AutoLock lock(mListLock);

    mHandler->RemoveMessages(MSG_SET_NAVIGATION_ICON_HINTS);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_SET_NAVIGATION_ICON_HINTS, hints, 0, (IMessage**)&msg);
    Boolean result;
    return mHandler->SendMessage(msg, &result);
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
CAR_INTERFACE_IMPL(CCommandQueue::NotificationQueueEntry, IInterface)

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
        case CCommandQueue::MSG_ADD_NOTIFICATION: {
            NotificationQueueEntry* ne = (NotificationQueueEntry*)obj.Get();
            mHost->mCallbacks->AddNotification(ne->mKey, ne->mNotification);
            break;
        }
        case CCommandQueue::MSG_UPDATE_NOTIFICATION: {
            NotificationQueueEntry* ne = (NotificationQueueEntry*)obj.Get();
            mHost->mCallbacks->UpdateNotification(ne->mKey, ne->mNotification);
            break;
        }
        case CCommandQueue::MSG_REMOVE_NOTIFICATION: {
            mHost->mCallbacks->RemoveNotification(IBinder::Probe(obj));
            break;
        }
        case CCommandQueue::MSG_DISABLE:
            mHost->mCallbacks->Disable(arg1);
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
        case CCommandQueue::MSG_SHOW_IME_BUTTON:
            mHost->mCallbacks->SetImeWindowStatus(IBinder::Probe(obj), arg1, arg2);
            break;
        case CCommandQueue::MSG_SET_HARD_KEYBOARD_STATUS:
            mHost->mCallbacks->SetHardKeyboardStatus(arg1 != 0, arg2 != 0);
            break;
        case CCommandQueue::MSG_TOGGLE_RECENT_APPS:
            mHost->mCallbacks->ToggleRecentApps();
            break;
        case CCommandQueue::MSG_PRELOAD_RECENT_APPS:
            mHost->mCallbacks->PreloadRecentApps();
            break;
        case CCommandQueue::MSG_CANCEL_PRELOAD_RECENT_APPS:
            mHost->mCallbacks->CancelPreloadRecentApps();
            break;
        case CCommandQueue::MSG_SET_NAVIGATION_ICON_HINTS:
            mHost->mCallbacks->SetNavigationIconHints(arg1);
            break;
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

}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
