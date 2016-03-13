
#include "elastos/droid/systemui/statusbar/phone/DemoStatusIcons.h"
#include "elastos/droid/systemui/statusbar/policy/LocationControllerImpl.h"
#include "elastos/droid/systemui/statusbar/CStatusBarIconView.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "../../R.h"
#include <elastos/droid/R.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::StatusBar::CStatusBarIcon;
using Elastos::Droid::Internal::StatusBar::IStatusBarIcon;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::SystemUI::StatusBar::Policy::LocationControllerImpl;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::Widget::CLinearLayoutLayoutParams;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CAR_INTERFACE_IMPL_2(DemoStatusIcons, LinearLayout, IDemoStatusIcons, IDemoMode);
DemoStatusIcons::DemoStatusIcons(
    /* [in] */ ILinearLayout* statusIcons,
    /* [in] */ Int32 iconSize)
    : mIconSize(0)
    , mDemoMode(FALSE)
{
    AutoPtr<IContext> ctx;
    IView::Probe(statusIcons)->GetContext((IContext**)&ctx);
    LinearLayout::constructor(ctx);
    mStatusIcons = statusIcons;
    mIconSize = iconSize;

    AutoPtr<IViewGroupLayoutParams> vp;
    IView::Probe(mStatusIcons)->GetLayoutParams((IViewGroupLayoutParams**)&vp);
    SetLayoutParams(vp);
    Int32 orientation = 0;
    mStatusIcons->GetOrientation(&orientation);
    SetOrientation(orientation);
    SetGravity(IGravity::CENTER_VERTICAL); // no LL.getGravity()
    AutoPtr<IViewParent> parent;
    IView::Probe(mStatusIcons)->GetParent((IViewParent**)&parent);
    AutoPtr<IViewGroup> p = IViewGroup::Probe(parent);

    Int32 index = 0;
    p->IndexOfChild(IView::Probe(mStatusIcons), &index);
    p->AddView(THIS_PROBE(IView), index);
}

ECode DemoStatusIcons::DispatchDemoCommand(
    /* [in] */ const String& command,
    /* [in] */ IBundle* args)
{
    if (!mDemoMode && command.Equals(COMMAND_ENTER)) {
        mDemoMode = TRUE;
        IView::Probe(mStatusIcons)->SetVisibility(IView::GONE);
        SetVisibility(IView::VISIBLE);
    }
    else if (mDemoMode && command.Equals(COMMAND_EXIT)) {
        mDemoMode = FALSE;
        IView::Probe(mStatusIcons)->SetVisibility(IView::VISIBLE);
        SetVisibility(IView::GONE);
    }
    else if (mDemoMode && command.Equals(COMMAND_STATUS)) {
        String volume;
        args->GetString(String("volume"), &volume);
        if (volume != NULL) {
            Int32 iconId = volume.Equals("vibrate") ? R::drawable::stat_sys_ringer_vibrate
                    : 0;
            UpdateSlot(String("volume"), String(NULL), iconId);
        }
        String zen;
        args->GetString(String("zen"), &zen);
        if (zen != NULL) {
            Int32 iconId = zen.Equals("important") ? R::drawable::stat_sys_zen_important
                    : zen.Equals("none") ? R::drawable::stat_sys_zen_none
                    : 0;
            UpdateSlot(String("zen"), String(NULL), iconId);
        }
        String bt;
        args->GetString(String("bluetooth"), &bt);
        if (bt != NULL) {
            Int32 iconId = bt.Equals("disconnected") ? R::drawable::stat_sys_data_bluetooth
                    : bt.Equals("connected") ? R::drawable::stat_sys_data_bluetooth_connected
                    : 0;
            UpdateSlot(String("bluetooth"), String(NULL), iconId);
        }
        String location;
        args->GetString(String("location"), &location);
        if (location != NULL) {
            Int32 iconId = location.Equals("show") ? LocationControllerImpl::LOCATION_STATUS_ICON_ID
                    : 0;
            UpdateSlot(LocationControllerImpl::LOCATION_STATUS_ICON_PLACEHOLDER, String(NULL), iconId);
        }
        String alarm;
        args->GetString(String("alarm"), &alarm);
        if (alarm != NULL) {
            Int32 iconId = alarm.Equals("show") ? R::drawable::stat_sys_alarm
                    : 0;
            UpdateSlot(String("alarm_clock"), String(NULL), iconId);
        }
        String sync;
        args->GetString(String("sync"), &sync);
        if (sync != NULL) {
            Int32 iconId = sync.Equals("show") ? R::drawable::stat_sys_sync
                    : 0;
            UpdateSlot(String("sync_active"), String(NULL), iconId);
        }
        String tty;
        args->GetString(String("tty"), &tty);
        if (tty != NULL) {
            Int32 iconId = tty.Equals("show") ? R::drawable::stat_sys_tty_mode
                    : 0;
            UpdateSlot(String("tty"), String(NULL), iconId);
        }
        String eri;
        args->GetString(String("eri"), &eri);
        if (eri != NULL) {
            Int32 iconId = eri.Equals("show") ? R::drawable::stat_sys_roaming_cdma_0
                    : 0;
            UpdateSlot(String("cdma_eri"), String(NULL), iconId);
        }
        String mute;
        args->GetString(String("mute"), &mute);
        if (mute != NULL) {
            Int32 iconId = mute.Equals("show") ? Elastos::Droid::R::drawable::stat_notify_call_mute
                    : 0;
            UpdateSlot(String("mute"), String(NULL), iconId);
        }
        String speakerphone;
        args->GetString(String("speakerphone"), &speakerphone);
        if (speakerphone != NULL) {
            Int32 iconId = speakerphone.Equals("show") ? Elastos::Droid::R::drawable::stat_sys_speakerphone
                    : 0;
            UpdateSlot(String("speakerphone"), String(NULL), iconId);
        }
        String cast;
        args->GetString(String("cast"), &cast);
        if (cast != NULL) {
            Int32 iconId = cast.Equals("cast") ? R::drawable::stat_sys_cast : 0;
            UpdateSlot(String("cast"), String(NULL), iconId);
        }
    }
    return NOERROR;
}

void DemoStatusIcons::UpdateSlot(
    /* [in] */ const String& slot,
    /* [in] */ const String& iconPkg,
    /* [in] */ Int32 iconId)
{
    if (!mDemoMode) return;
    Int32 removeIndex = -1, count = 0;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        AutoPtr<IStatusBarIconView> v = IStatusBarIconView::Probe(child);

        AutoPtr<IInterface> obj;
        IView::Probe(v)->GetTag((IInterface**)&obj);
        String tag;
        ICharSequence::Probe(obj)->ToString(&tag);
        if (slot.Equals(tag)) {
            if (iconId == 0) {
                removeIndex = i;
                break;
            }
            else {
                AutoPtr<IStatusBarIcon> icon;
                v->GetStatusBarIcon((IStatusBarIcon**)&icon);
                icon->SetIconPackage(iconPkg);
                icon->SetIconId(iconId);
                Boolean tmp = FALSE;
                v->Set(icon, &tmp);
                v->UpdateDrawable();
                return;
            }
        }
    }
    if (iconId == 0) {
        if (removeIndex != -1) {
            RemoveViewAt(removeIndex);
            return;
        }
    }
    AutoPtr<IStatusBarIcon> icon;
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> CURRENT;
    helper->GetCURRENT((IUserHandle**)&CURRENT);
    AutoPtr<ICharSequence> value;
    CString::New(String("Demo"), (ICharSequence**)&value);
    CStatusBarIcon::New(iconPkg, CURRENT, iconId, 0, 0, value, (IStatusBarIcon**)&icon);
    AutoPtr<IStatusBarIconView> v;
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    CStatusBarIconView::New(ctx, NULL, (IStatusBarIconView**)&v);

    value = NULL;
    CString::New(slot, (ICharSequence**)&value);
    IView::Probe(v)->SetTag(value);
    Boolean tmp = FALSE;
    v->Set(icon, &tmp);
    AutoPtr<IViewGroupLayoutParams> vp;
    CLinearLayoutLayoutParams::New(mIconSize, mIconSize, (IViewGroupLayoutParams**)&vp);
    AddView(IView::Probe(v), 0, vp);
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
