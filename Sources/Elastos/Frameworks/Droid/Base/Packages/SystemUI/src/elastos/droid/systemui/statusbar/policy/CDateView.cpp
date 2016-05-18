
#include "elastos/droid/systemui/statusbar/policy/CDateView.h"
#include "../../R.h"
#include <elastos/droid/text/format/DateFormat.h>

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Text::Format::DateFormat;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Core::CSystem;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::ISystem;
using Elastos::Utility::CDate;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ILocaleHelper;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CDateView::DateViewBroadcastReceiver::DateViewBroadcastReceiver(
    /* [in] */ CDateView* host)
    : mHost(host)
{}

ECode CDateView::DateViewBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_TIME_TICK.Equals(action)
            || IIntent::ACTION_TIME_CHANGED.Equals(action)
            || IIntent::ACTION_TIMEZONE_CHANGED.Equals(action)
            || IIntent::ACTION_LOCALE_CHANGED.Equals(action)) {
        if (IIntent::ACTION_LOCALE_CHANGED.Equals(action)
                || IIntent::ACTION_TIMEZONE_CHANGED.Equals(action)) {
            // need to get a fresh date format
            mHost->mDateFormat = NULL;
        }
        mHost->UpdateClock();
    }
    return NOERROR;
}

const String CDateView::TAG("DateView");
CAR_INTERFACE_IMPL(CDateView, TextView, IDateView)
CDateView::CDateView()
{
    mIntentReceiver = new DateViewBroadcastReceiver(this);
    CDate::New((IDate**)&mCurrentTime);
}

ECode CDateView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    TextView::constructor(context, attrs);

    AutoPtr<IResourcesTheme> rt;
    context->GetTheme((IResourcesTheme**)&rt);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
        const_cast<Int32 *>(R::styleable::DateView),
        ArraySize(R::styleable::DateView));
    AutoPtr<ITypedArray> a;

    rt->ObtainStyledAttributes(attrs, attrIds,
            0, 0, (ITypedArray**)&a);

    // try {
    a->GetString(R::styleable::DateView_datePattern, &mDatePattern);
    // } finally {
    a->Recycle();
    // }
    if (mDatePattern == NULL) {
        AutoPtr<IContext> c;
        GetContext((IContext**)&c);
        c->GetString(R::string::system_ui_date_pattern, &mDatePattern);
    }
    return NOERROR;
}

ECode CDateView::OnAttachedToWindow()
{
    TextView::OnAttachedToWindow();

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_TIME_TICK);
    filter->AddAction(IIntent::ACTION_TIME_CHANGED);
    filter->AddAction(IIntent::ACTION_TIMEZONE_CHANGED);
    filter->AddAction(IIntent::ACTION_LOCALE_CHANGED);

    AutoPtr<IContext> c;
    GetContext((IContext**)&c);
    AutoPtr<IIntent> i;
    c->RegisterReceiver(mIntentReceiver, filter, String(NULL), NULL, (IIntent**)&i);

    UpdateClock();
    return NOERROR;
}

ECode CDateView::OnDetachedFromWindow()
{
    TextView::OnDetachedFromWindow();

    mDateFormat = NULL; // reload the locale next time
    AutoPtr<IContext> c;
    GetContext((IContext**)&c);
    c->UnregisterReceiver(mIntentReceiver);
    return NOERROR;
}

void CDateView::UpdateClock()
{
    if (mDateFormat == NULL) {
        AutoPtr<ILocaleHelper> helper;
        CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
        AutoPtr<ILocale> l;
        helper->GetDefault((ILocale**)&l);
        String fmt = DateFormat::GetBestDateTimePattern(l, mDatePattern);
        CSimpleDateFormat::New(fmt, l, (ISimpleDateFormat**)&mDateFormat);
    }

    AutoPtr<ISystem> s;
    CSystem::AcquireSingleton((ISystem**)&s);
    Int64 t;
    s->GetCurrentTimeMillis(&t);
    mCurrentTime->SetTime(t);

    String text;
    IDateFormat::Probe(mDateFormat)->Format(mCurrentTime, &text);
    if (!text.Equals(mLastText)) {
        AutoPtr<ICharSequence> cs;
        CString::New(text, (ICharSequence**)&cs);
        SetText(cs);
        mLastText = text;
    }
}

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
