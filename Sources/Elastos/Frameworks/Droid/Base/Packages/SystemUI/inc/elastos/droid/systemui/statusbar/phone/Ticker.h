#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PHONE_TICKER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PHONE_TICKER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/List.h>
#include "elastos/droid/os/Runnable.h"

using Elastos::Utility::Etl::List;
using Elastos::Core::ICharSequence;
using Elastos::Core::IRunnable;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Text::IStaticLayout;
using Elastos::Droid::Text::ITextPaint;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ITextSwitcher;
using Elastos::Droid::Widget::IImageSwitcher;
using Elastos::Droid::StatusBar::IStatusBarNotification;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

class Ticker
{
private:
    class Segment : public ElRefBase
    {
    public:
        Segment(
            /* [in] */ IStatusBarNotification* n,
            /* [in] */ IDrawable* icon,
            /* [in] */ ICharSequence* text,
            /* [in] */ Ticker* host);

        AutoPtr<IStaticLayout> GetLayout(
            /* [in] */ ICharSequence* substr);

        AutoPtr<ICharSequence> Rtrim(
            /* [in] */ ICharSequence* substr,
            /* [in] */ Int32 start,
            /* [in] */ Int32 end);

        /** returns null if there is no more text */
        AutoPtr<ICharSequence> GetText();

        /** returns null if there is no more text */
        AutoPtr<ICharSequence> Advance();

    public:
        AutoPtr<IStatusBarNotification> mNotification;
        AutoPtr<IDrawable> mIcon;
        AutoPtr<ICharSequence> mText;
        Int32 mCurrent;
        Int32 mNext;
        Boolean mFirst;
        Ticker* mHost;
    };

    class  AdvanceTickerRunnable : public Runnable
    {
    public:
        AdvanceTickerRunnable(
            /* [in] */ Ticker* ticker);

        virtual CARAPI Run();
    private:
        Ticker* mHost;
    };

public:
    static Boolean IsGraphicOrEmoji(
        /* [in] */ Char32 c);

    Ticker();

    Ticker(
        /* [in] */ IContext* context,
        /* [in] */ IView* sb);

    virtual CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid) = 0;

    CARAPI AddEntry(
        /* [in] */ IStatusBarNotification* n);

    CARAPI RemoveEntry(
        /* [in] */ IStatusBarNotification* n);

    CARAPI Halt();

    CARAPI ReflowText();

    virtual CARAPI TickerStarting() = 0;

    virtual CARAPI TickerDone() = 0;

    virtual CARAPI TickerHalting() = 0;

protected:
    CARAPI Init();

    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IView* sb);
private:
    CARAPI_(void) ScheduleAdvance();

private:
    static const Int32 TICKER_SEGMENT_DELAY;

    AutoPtr<IContext> mContext;
    AutoPtr<IHandler> mHandler;// = new Handler();
    AutoPtr<IRunnable> mAdvanceTicker;
    List<AutoPtr<Segment> > mSegments;// = new ArrayList();
    AutoPtr<ITextPaint> mPaint;
    AutoPtr<IView> mTickerView;
    AutoPtr<IImageSwitcher> mIconSwitcher;
    AutoPtr<ITextSwitcher> mTextSwitcher;
    Float mIconScale;
};

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PHONE_TICKER_H__
