#include "elastos/droid/systemui/volume/ZenToast.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.CoreLibrary.Core.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/R.h"
#include "../R.h"

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
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
using Elastos::Core::CString;
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
        Int32 v;
        intent->GetInt32Extra(ZenToast::EXTRA_ZEN, ISettingsGlobal::ZEN_MODE_IMPORTANT_INTERRUPTIONS, &v);
        const Int32 zen = v;
        String str;
        intent->GetStringExtra(ZenToast::EXTRA_TEXT, &str);
        const String text = str;
        mHost->HandleShow(zen, text);
    }
    else if (ZenToast::ACTION_HIDE.Equals(action)) {
        mHost->HandleHide();
    }
    return NOERROR;
}

//===================================
// ZenToast::MyListener
//===================================

CAR_INTERFACE_IMPL(ZenToast::MyListener, Object, IViewOnAttachStateChangeListener)

ZenToast::MyListener::MyListener(
    /* [in] */ ITextView* message,
    /* [in] */ ZenToast* host)
    : mMessage(message)
    , mHost(host)
{}

ECode ZenToast::MyListener::OnViewAttachedToWindow(
    /* [in] */ IView* v)
{
    AutoPtr<ICharSequence> text;
    mMessage->GetText((ICharSequence**)&text);
    mHost->mZenToast->AnnounceForAccessibility(text);
    return NOERROR;
}

ECode ZenToast::MyListener::OnViewDetachedFromWindow(
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

ZenToast::ZenToast(
    /* [in] */ IContext* context)
    : mContext(context)
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&obj);
    mWindowManager = IWindowManager::Probe(obj);
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(ACTION_SHOW);
    filter->AddAction(ACTION_HIDE);
    AutoPtr<IUserHandleHelper> uhh;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhh);
    AutoPtr<IUserHandle> uh;
    uhh->GetALL((IUserHandle**)&uh);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiverAsUser(mReceiver, uh, filter, String(NULL), mHandler, (IIntent**)&intent);
    mHandler = new MyHandler(this);
    mReceiver = new MyReceiver(this);
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
    Int32 _iconRes;
    switch (zen) {
        case ISettingsGlobal::ZEN_MODE_NO_INTERRUPTIONS:
            mContext->GetString(R::string::zen_no_interruptions, &text);
            _iconRes = R::drawable::ic_zen_none;
            break;
        case ISettingsGlobal::ZEN_MODE_IMPORTANT_INTERRUPTIONS:
            mContext->GetString(R::string::zen_important_interruptions, &text);
            _iconRes = R::drawable::ic_zen_important;
            break;
        default:
            return;
    }
    const Int32 iconRes = _iconRes;
    if (!overrideText.IsNull()) {
        text = overrideText;
    }
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    AutoPtr<IWindowManagerLayoutParams> params;
    CWindowManagerLayoutParams::New((IWindowManagerLayoutParams**)&params);

    //TODO
    // params.height = IWindowManagerLayoutParams::WRAP_CONTENT;

    Int32 width;
    res->GetDimensionPixelSize(R::dimen::zen_toast_width, &width);

    // params.width = width;

    params->SetFormat(IPixelFormat::TRANSLUCENT);
    params->SetWindowAnimations(R::style::ZenToastAnimations);
    params->SetType(IWindowManagerLayoutParams::TYPE_STATUS_BAR_PANEL);
    AutoPtr<ICharSequence> title;
    CString::New(String("ZenToast"), (ICharSequence**)&title);
    params->SetTitle(title);
    params->SetFlags(IWindowManagerLayoutParams::FLAG_KEEP_SCREEN_ON
        | IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE
        | IWindowManagerLayoutParams::FLAG_NOT_TOUCHABLE);
    params->SetGravity(IGravity::CENTER);
    String packageName;
    mContext->GetPackageName(&packageName);
    params->SetPackageName(packageName);

    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(mContext, (ILayoutInflater**)&inflater);
    inflater->Inflate(R::layout::zen_toast, NULL, (IView**)&mZenToast);

    AutoPtr<IView> v1;
    mZenToast->FindViewById(Elastos::Droid::R::id::message, (IView**)&v1);
    const AutoPtr<ITextView> message = ITextView::Probe(v1);
    AutoPtr<ICharSequence> cs;
    CString::New(text, (ICharSequence**)&cs);
    message->SetText(cs);

    AutoPtr<IView> v2;
    mZenToast->FindViewById(R::id::icon, (IView**)&v2);
    const AutoPtr<IImageView> icon = IImageView::Probe(v2);
    icon->SetImageResource(iconRes);
    AutoPtr<MyListener> listener = new MyListener(message, this);
    mZenToast->AddOnAttachStateChangeListener(listener);
    IViewManager::Probe(mWindowManager)->AddView(mZenToast, IViewGroupLayoutParams::Probe(params));
    Int32 animDuration;
    res->GetInteger(R::integer::zen_toast_animation_duration, &animDuration);
    Int32 visibleDuration;
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