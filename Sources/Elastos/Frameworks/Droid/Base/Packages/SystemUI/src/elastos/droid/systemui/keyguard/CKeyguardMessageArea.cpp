
#include "elastos/droid/systemui/keyguard/CKeyguardMessageArea.h"
#include "elastos/droid/systemui/keyguard/KeyguardUpdateMonitor.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CAR_INTERFACE_IMPL(CKeyguardMessageArea::Helper, Object, ISecurityMessageDisplay)

ECode CKeyguardMessageArea::Helper::constructor(
    /* [in] */ IView* v)
{
    AutoPtr<IView> view;
    v->FindViewById(R::id::keyguard_message_area, (IView**)&view);
    mMessageArea = (CKeyguardMessageArea*)IKeyguardMessageArea::Probe(view);
    if (mMessageArea == NULL) {
        //throw new RuntimeException("Can't find keyguard_message_area in " + v.getClass());
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode CKeyguardMessageArea::Helper::SetMessage(
    /* [in] */ ICharSequence* msg,
    /* [in] */ Boolean important)
{
    if (!TextUtils::IsEmpty(msg) && important) {
        mMessageArea->mMessage = msg;
        mMessageArea->SecurityMessageChanged();
    }
    return NOERROR;
}

ECode CKeyguardMessageArea::Helper::SetMessage(
    /* [in] */ Int32 resId,
    /* [in] */ Boolean important)
{
    if (resId != 0 && important) {
        AutoPtr<IContext> context;
        mMessageArea->GetContext((IContext**)&context);
        AutoPtr<IResources> resources;
        context->GetResources((IResources**)&resources);
        resources->GetText(resId, (ICharSequence**)&(mMessageArea->mMessage));
        mMessageArea->SecurityMessageChanged();
    }
    return NOERROR;
}

ECode CKeyguardMessageArea::Helper::SetMessage(
    /* [in] */ Int32 resId,
    /* [in] */ Boolean important,
    /* [in] */ ArrayOf<IInterface*>* formatArgs)
{
    if (resId != 0 && important) {
        AutoPtr<IContext> context;
        mMessageArea->GetContext((IContext**)&context);
        String str;
        context->GetString(resId, formatArgs, &str);
        mMessageArea->mMessage = CoreUtils::Convert(str);
        mMessageArea->SecurityMessageChanged();
    }
    return NOERROR;
}

ECode CKeyguardMessageArea::Helper::ShowBouncer(
    /* [in] */ Int32 duration)
{
    mMessageArea->HideMessage(duration, FALSE);
    mMessageArea->mShowingBouncer = TRUE;
    return NOERROR;
}

ECode CKeyguardMessageArea::Helper::HideBouncer(
    /* [in] */ Int32 duration)
{
    mMessageArea->ShowMessage(duration);
    mMessageArea->mShowingBouncer = FALSE;
    return NOERROR;
}

ECode CKeyguardMessageArea::Helper::SetTimeout(
    /* [in] */ Int32 timeoutMs)
{
    mMessageArea->mTimeout = timeoutMs;
    return NOERROR;
}

ECode CKeyguardMessageArea::MyRunnable::Run()
{
    mHost->mMessage = NULL;
    mHost->mShowingMessage = FALSE;
    if (mHost->mShowingBouncer) {
        mHost->HideMessage(FADE_DURATION, TRUE);
    }
    else {
        mHost->Update();
    }
    return NOERROR;
}

CKeyguardMessageArea::AnnounceRunnable::AnnounceRunnable(
    /* [in] */ IView* host,
    /* [in] */ ICharSequence* textToAnnounce)
{
    IWeakReferenceSource::Probe(host)->GetWeakReference((IWeakReference**)&mHost);
    mTextToAnnounce = textToAnnounce;
}

ECode CKeyguardMessageArea::AnnounceRunnable::Run()
{
    AutoPtr<IInterface> obj;
    mHost->Resolve(EIID_IInterface, (IInterface**)&obj);
    AutoPtr<IView> host = IView::Probe(obj);
    if (host != NULL) {
        host->AnnounceForAccessibility(mTextToAnnounce);
    }
    return NOERROR;
}

ECode CKeyguardMessageArea::MyKeyguardUpdateMonitorCallback::OnScreenTurnedOff(
    /* [in] */ Int32 why)
{
    return mHost->SetSelected(FALSE);
}

ECode CKeyguardMessageArea::MyKeyguardUpdateMonitorCallback::OnScreenTurnedOn()
{
    return mHost->SetSelected(TRUE);
}

ECode CKeyguardMessageArea::MyAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    return mHost->Update();
}

const Int32 CKeyguardMessageArea::FADE_DURATION = 750;

const AutoPtr<IInterface> CKeyguardMessageArea::ANNOUNCE_TOKEN;

const Int64 CKeyguardMessageArea::ANNOUNCEMENT_DELAY = 250;

const Int32 CKeyguardMessageArea::SECURITY_MESSAGE_DURATION = 5000;

const String CKeyguardMessageArea::TAG("KeyguardMessageArea");

CAR_OBJECT_IMPL(CKeyguardMessageArea)

CAR_INTERFACE_IMPL(CKeyguardMessageArea, TextView, IKeyguardMessageArea)

CKeyguardMessageArea::CKeyguardMessageArea()
    : mShowingBouncer(FALSE)
    , mTimeout(SECURITY_MESSAGE_DURATION)
    , mShowingMessage(FALSE)
{
    mClearMessageRunnable = new MyRunnable(this);

    mInfoCallback = new MyKeyguardUpdateMonitorCallback(this);
}

ECode CKeyguardMessageArea::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CKeyguardMessageArea::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    TextView::constructor(context, attrs);
    SetLayerType(LAYER_TYPE_HARDWARE, NULL); // work around nested unclipped SaveLayer bug

    CLockPatternUtils::New(context, (ILockPatternUtils**)&mLockPatternUtils);

    // Registering this callback immediately updates the battery state, among other things.
    AutoPtr<IContext> _context;
    GetContext((IContext**)&_context);
    mUpdateMonitor = KeyguardUpdateMonitor::GetInstance(_context);
    mUpdateMonitor->RegisterCallback(mInfoCallback);
    AutoPtr<ILooper> looper = Looper::GetMyLooper();
    CHandler::New(looper, (IHandler**)&mHandler);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    String str;
    resources->GetString(R::string::kg_text_message_separator, &str);
    mSeparator = CoreUtils::Convert(str);

    return Update();
}

ECode CKeyguardMessageArea::OnFinishInflate()
{
    Boolean screenOn;
    KeyguardUpdateMonitor::GetInstance(mContext)->IsScreenOn(&screenOn);
    return SetSelected(screenOn); // This is required to ensure marquee works
}

ECode CKeyguardMessageArea::SecurityMessageChanged()
{
    SetAlpha(1.0f);
    mShowingMessage = TRUE;
    Update();
    mHandler->RemoveCallbacks(mClearMessageRunnable);
    Boolean res;
    if (mTimeout > 0) {
        mHandler->PostDelayed(mClearMessageRunnable, mTimeout, &res);
    }
    mHandler->RemoveCallbacksAndMessages(ANNOUNCE_TOKEN);
    AutoPtr<ICharSequence> text;
    GetText((ICharSequence**)&text);
    AutoPtr<IRunnable> r = new AnnounceRunnable(this, text);

    return mHandler->PostAtTime(r, ANNOUNCE_TOKEN,
           (SystemClock::GetUptimeMillis() + ANNOUNCEMENT_DELAY), &res);
}

ECode CKeyguardMessageArea::Update()
{
    assert(0);
    //MutableInt icon = new MutableInt(0);
    AutoPtr<ICharSequence> status;
    GetCurrentMessage((ICharSequence**)&status);
    assert(0);
    //SetCompoundDrawablesWithIntrinsicBounds(icon.value, 0, 0, 0);
    return SetText(status);
}

ECode CKeyguardMessageArea::GetCurrentMessage(
    /* [out] */ ICharSequence** cchar)
{
    VALIDATE_NOT_NULL(cchar)

    *cchar = mShowingMessage ? mMessage : NULL;
    REFCOUNT_ADD(*cchar);
    return NOERROR;
}

void CKeyguardMessageArea::HideMessage(
    /* [in] */ Int32 duration,
    /* [in] */ Boolean thenUpdate)
{
    if (duration > 0) {
        AutoPtr<IObjectAnimatorHelper> helper;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
        AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
        (*array)[0] = 0.0f;
        AutoPtr<IObjectAnimator> tmp;
        helper->OfFloat(TO_IINTERFACE(this), String("alpha"), array,
                (IObjectAnimator**)&tmp);
        AutoPtr<IAnimator> anim = IAnimator::Probe(tmp);
        anim->SetDuration(duration);
        if (thenUpdate) {
            AutoPtr<IAnimatorListener> lis = new MyAnimatorListenerAdapter(this);
            anim->AddListener(lis);
        }
        anim->Start();
    }
    else {
        SetAlpha(0.0f);
        if (thenUpdate) {
            Update();
        }
    }
}

void CKeyguardMessageArea::ShowMessage(
    /* [in] */ Int32 duration)
{
    if (duration > 0) {
        AutoPtr<IObjectAnimatorHelper> helper;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
        AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
        (*array)[0] = 1.0f;
        AutoPtr<IObjectAnimator> tmp;
        helper->OfFloat(TO_IINTERFACE(this), String("alpha"), array,
                (IObjectAnimator**)&tmp);
        AutoPtr<IAnimator> anim = IAnimator::Probe(tmp);
        anim->SetDuration(duration);
        anim->Start();
    }
    else {
        SetAlpha(1.0f);
    }
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
