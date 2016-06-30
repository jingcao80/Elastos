#include "elastos/droid/systemui/volume/ZenToast.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.CoreLibrary.Core.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/R.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::View::EIID_IViewOnAttachStateChangeListener;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::R;
using Elastos::Core::CoreUtils;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Volume {

//===================================
// ZenToast::MyHandler
//===================================

ZenToast::MyHandler::MyHandler(
    /* [in] */ ZenToast* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode ZenToast::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case ZenToast::MSG_SHOW: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->HandleShow(arg1, String(NULL));
            break;
        }
        case ZenToast::MSG_HIDE:
            mHost->HandleHide();
            break;
    }
    return NOERROR;
}

//===================================
// ZenToast::MyReceiver
//===================================

ZenToast::MyReceiver::MyReceiver(
    /* [in] */ ZenToast* host)
    : mHost(host)
{}

ECode ZenToast::MyReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (ZenToast::ACTION_SHOW.Equals(action)) {
        Int32 zen;
        intent->GetInt32Extra(ZenToast::EXTRA_ZEN, ISettingsGlobal::ZEN_MODE_IMPORTANT_INTERRUPTIONS, &zen);
        String str;
        intent->GetStringExtra(ZenToast::EXTRA_TEXT, &str);
        mHost->HandleShow(zen, str);
    }
    else if (ZenToast::ACTION_HIDE.Equals(action)) {
        mHost->HandleHide();
    }
    return NOERROR;
}

//===================================
// ZenToast::OnAttachStateChangeListener
//===================================

CAR_INTERFACE_IMPL(ZenToast::OnAttachStateChangeListener, Object, IViewOnAttachStateChangeListener)

ZenToast::OnAttachStateChangeListener::OnAttachStateChangeListener(
    /* [in] */ ITextView* message,
    /* [in] */ ZenToast* host)
    : mMessage(message)
    , mHost(host)
{}

ECode ZenToast::OnAttachStateChangeListener::OnViewAttachedToWindow(
    /* [in] */ IView* v)
{
    AutoPtr<ICharSequence> text;
    mMessage->GetText((ICharSequence**)&text);
    mHost->mZenToast->AnnounceForAccessibility(text);
    return NOERROR;
}

ECode ZenToast::OnAttachStateChangeListener::OnViewDetachedFromWindow(
    /* [in] */ IView* v)
{
    return NOERROR;
}

//===================================
// ZenToast
//===================================

const String ZenToast::ACTION_SHOW("ZenToast.SHOW");
const String ZenToast::ACTION_HIDE("ZenToast.HIDE");
const String ZenToast::EXTRA_ZEN("zen");
const String ZenToast::EXTRA_TEXT("text");

const Int32 ZenToast::MSG_SHOW;
const Int32 ZenToast::MSG_HIDE;

ECode ZenToast::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    mHandler = new MyHandler(this);
    mReceiver = new MyReceiver(this);

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&obj);
    mWindowManager = IWindowManager::Probe(obj);
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(ACTION_SHOW);
    filter->AddAction(ACTION_HIDE);
    AutoPtr<IIntent> intent;
    return mContext->RegisterReceiverAsUser(mReceiver, UserHandle::ALL, filter,
        String(NULL), mHandler, (IIntent**)&intent);
}

ECode ZenToast::Show(
    /* [in] */ Int32 zen)
{
    mHandler->RemoveMessages(MSG_HIDE);
    mHandler->RemoveMessages(MSG_SHOW);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_SHOW, zen, 0, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode ZenToast::Hide()
{
    mHandler->RemoveMessages(MSG_HIDE);
    mHandler->RemoveMessages(MSG_SHOW);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_HIDE, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

void ZenToast::HandleShow(
    /* [in] */ Int32 zen,
    /* [in] */ String overrideText)
{
    HandleHide();

    String text;
    Int32 iconRes;
    switch (zen) {
        case ISettingsGlobal::ZEN_MODE_NO_INTERRUPTIONS:
            mContext->GetString(R::string::zen_no_interruptions, &text);
            iconRes = R::drawable::ic_zen_none;
            break;
        case ISettingsGlobal::ZEN_MODE_IMPORTANT_INTERRUPTIONS:
            mContext->GetString(R::string::zen_important_interruptions, &text);
            iconRes = R::drawable::ic_zen_important;
            break;
        default:
            return;
    }

    if (!overrideText.IsNull()) {
        text = overrideText;
    }
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    AutoPtr<IWindowManagerLayoutParams> params;
    CWindowManagerLayoutParams::New((IWindowManagerLayoutParams**)&params);
    IViewGroupLayoutParams* vglp = IViewGroupLayoutParams::Probe(params);

    Int32 width;
    res->GetDimensionPixelSize(R::dimen::zen_toast_width, &width);
    vglp->SetHeight(IViewGroupLayoutParams::WRAP_CONTENT);
    vglp->SetWidth(width);

    params->SetFormat(IPixelFormat::TRANSLUCENT);
    params->SetWindowAnimations(R::style::ZenToastAnimations);
    params->SetType(IWindowManagerLayoutParams::TYPE_STATUS_BAR_PANEL);
    AutoPtr<ICharSequence> title = CoreUtils::Convert("ZenToast");
    params->SetTitle(title);
    params->SetFlags(IWindowManagerLayoutParams::FLAG_KEEP_SCREEN_ON
        | IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE
        | IWindowManagerLayoutParams::FLAG_NOT_TOUCHABLE);
    params->SetGravity(IGravity::CENTER);
    String packageName;
    mContext->GetPackageName(&packageName);
    params->SetPackageName(packageName);

    mZenToast = NULL;
    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(mContext, (ILayoutInflater**)&inflater);
    inflater->Inflate(R::layout::zen_toast, NULL, (IView**)&mZenToast);

    AutoPtr<IView> v1;
    mZenToast->FindViewById(Elastos::Droid::R::id::message, (IView**)&v1);
    AutoPtr<ITextView> message = ITextView::Probe(v1);
    AutoPtr<ICharSequence> cs = CoreUtils::Convert(text);
    message->SetText(cs);

    AutoPtr<IView> v2;
    mZenToast->FindViewById(Elastos::Droid::R::id::icon, (IView**)&v2);
    AutoPtr<IImageView> icon = IImageView::Probe(v2);
    icon->SetImageResource(iconRes);

    AutoPtr<OnAttachStateChangeListener> listener = new OnAttachStateChangeListener(message, this);
    mZenToast->AddOnAttachStateChangeListener(listener);
    IViewManager::Probe(mWindowManager)->AddView(mZenToast, vglp);
    Int32 animDuration, visibleDuration;
    res->GetInteger(R::integer::zen_toast_animation_duration, &animDuration);
    res->GetInteger(R::integer::zen_toast_visible_duration, &visibleDuration);
    Boolean result;
    mHandler->SendEmptyMessageDelayed(MSG_HIDE, animDuration + visibleDuration, &result);
}

void ZenToast::HandleHide()
{
    if (mZenToast != NULL) {
        IViewManager::Probe(mWindowManager)->RemoveView(mZenToast);
        mZenToast = NULL;
    }
}

} // namespace Volume
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos