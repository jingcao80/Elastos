
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/R.h"
#include "elastos/droid/text/format/CFormatter.h"
#include "elastos/droid/text/format/DateUtils.h"
#include "elastos/droid/widget/Chronometer.h"
#include "elastos/core/AutoLock.h"

using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Text::Format::DateUtils;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::IView;

using Elastos::Core::AutoLock;
using Elastos::Core::CString;
using Elastos::Core::IAppendable;
using Elastos::Utility::CFormatter;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;

namespace Elastos {
namespace Droid {
namespace Widget {

const String Chronometer::TAG("Chronometer");
const Int32 Chronometer::TICK_WHAT;

CAR_INTERFACE_IMPL(Chronometer, Object, IChronometer)

Chronometer::InnerHandler::InnerHandler(
    /* [in] */ Chronometer* host)
    : mHost(host)
{
    assert(mHost);
}

ECode Chronometer::InnerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);
    if (mHost->mRunning) {
        mHost->UpdateText(SystemClock::GetElapsedRealtime());
        mHost->DispatchChronometerTick();

        Boolean result;
        SendEmptyMessageDelayed(Chronometer::TICK_WHAT, 1000, &result);
    }
    return NOERROR;
}

Chronometer::Chronometer()
   : mBase(0)
   , mVisible(FALSE)
   , mStarted(FALSE)
   , mRunning(FALSE)
   , mLogged(FALSE)
   , mRecycle(8)
{
    mFormatterArgs = ArrayOf<IInterface*>::Alloc(1);
    mHandler = new InnerHandler(this);
}

ECode Chronometer::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL, 0);
}

ECode Chronometer::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode Chronometer::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return constructor(context, attrs, defStyle, 0);
}

ECode Chronometer::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle,
    /* [in] */ Int32 defStyleRes)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(attrs);

    TextView::constructor(context, attrs, defStyle, defStyleRes);
    AutoPtr< ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::Chronometer);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, defStyle, defStyleRes, (ITypedArray**)&a);

    String str;
    a->GetString(R::styleable::Chronometer_format, &str);
    SetFormat(str);
    a->Recycle();

    Init();
    return NOERROR;
}

void Chronometer::Init()
{
    mBase = SystemClock::GetElapsedRealtime();
    UpdateText(mBase);
}

ECode Chronometer::SetBase(
    /* [in] */ Int64 base)
{
    mBase = base;
    DispatchChronometerTick();
    UpdateText(SystemClock::GetElapsedRealtime());
    return NOERROR;
}

ECode Chronometer::GetBase(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mBase;
    return NOERROR;
}

ECode Chronometer::SetFormat(
    /* [in] */ const String& format)
{
    mFormat = format;
    if (!format.IsNull() && mFormatBuilder == NULL) {
        mFormatBuilder = new StringBuilder(format.GetByteLength() * 2);
    }
    return NOERROR;
}

ECode Chronometer::GetFormat(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mFormat;
    return NOERROR;
}

ECode Chronometer::SetOnChronometerTickListener(
    /* [in] */ IOnChronometerTickListener* listener)
{
    VALIDATE_NOT_NULL(listener);
    mOnChronometerTickListener = listener;
    return NOERROR;
}

ECode Chronometer::GetOnChronometerTickListener(
    /* [out] */ IOnChronometerTickListener** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOnChronometerTickListener;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode Chronometer::Start()
{
    mStarted = TRUE;
    UpdateRunning();
    return NOERROR;
}

ECode Chronometer::Stop()
{
    mStarted = FALSE;
    UpdateRunning();
    return NOERROR;
}

ECode Chronometer::SetStarted(
    /* [in] */ Boolean started)
{
    mStarted = started;
    UpdateRunning();
    return NOERROR;
}

ECode Chronometer::OnDetachedFromWindow()
{
    TextView::OnDetachedFromWindow();
    mVisible = FALSE;
    UpdateRunning();
    return NOERROR;
}

void Chronometer::OnWindowVisibilityChanged(
    /* [in] */ Int32 visibility)
{
    TextView::OnWindowVisibilityChanged(visibility);
    mVisible = visibility == IView::VISIBLE;
    UpdateRunning();
}

//synchronized
void Chronometer::UpdateText(
    /* [in] */ Int64 now)
{
    AutoLock lock(this);
    Int64 seconds = now - mBase;
    seconds /= 1000;
    String sRecycle;
    mRecycle.ToString(&sRecycle);
    String text = DateUtils::FormatElapsedTime(sRecycle, seconds);

    if (mFormat != NULL) {
        AutoPtr<ILocale> loc;
        AutoPtr<ILocaleHelper> helper;
        CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
        helper->GetDefault((ILocale**)&loc);

        Boolean res;
        loc->Equals(TO_IINTERFACE(mFormatterLocale), &res);
        if (mFormatter == NULL || !res) {
            mFormatterLocale = loc;
            CFormatter::New(IAppendable::Probe(mFormatBuilder), loc, (IFormatter**)&mFormatter);
        }
        mFormatBuilder->SetLength(0);
        AutoPtr<ICharSequence> charSequence;
        CString::New(text, (ICharSequence**)&charSequence);
        (*mFormatterArgs)[0] = TO_IINTERFACE(charSequence);
        //try {
            mFormatter->Format(mFormat, mFormatterArgs);
            mFormatBuilder->ToString(&text);
        /*} catch (IllegalFormatException ex) {
            if (!mLogged) {
                Log.w(TAG, "Illegal format string: " + mFormat);
                mLogged = TRUE;
            }
        }*/
    }
    AutoPtr<ICharSequence> seq;
    CString::New(text, (ICharSequence**)&seq);
    SetText(seq);
}

void Chronometer::UpdateRunning()
{
    Boolean running = mVisible && mStarted;
    if (running != mRunning) {
        if (running) {
            UpdateText(SystemClock::GetElapsedRealtime());
            DispatchChronometerTick();
            Boolean result;
            mHandler->SendEmptyMessageDelayed(TICK_WHAT, 1000, &result);
        }
        else {
            mHandler->RemoveMessages(TICK_WHAT);
        }
        mRunning = running;
    }
}

ECode Chronometer::DispatchChronometerTick()
{
    if (mOnChronometerTickListener != NULL) {
        mOnChronometerTickListener->OnChronometerTick(this);
    }
    return NOERROR;
}

ECode Chronometer::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    VALIDATE_NOT_NULL(event);
    TextView::OnInitializeAccessibilityEvent(event);
    String classNameStr("Chronometer");
    AutoPtr<ICharSequence> className;
    FAIL_RETURN(CString::New(classNameStr, (ICharSequence**)&className));

    IAccessibilityRecord* record = IAccessibilityRecord::Probe(event);
    record->SetClassName(className);
    return NOERROR;
}

ECode Chronometer::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    VALIDATE_NOT_NULL(info);
    TextView::OnInitializeAccessibilityNodeInfo(info);
    String classNameStr("Chronometer");
    AutoPtr<ICharSequence> className;
    FAIL_RETURN(CString::New(classNameStr, (ICharSequence**)&className));
    info->SetClassName(className);
    return NOERROR;
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos

