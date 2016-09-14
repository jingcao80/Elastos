
#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_TEXTPROGRESSBAR_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_TEXTPROGRESSBAR_H__

#include "elastos/droid/R.h"
#include "elastos/droid/widget/RelativeLayout.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Widget::ITextProgressBar;
using Elastos::Droid::R;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::Widget::IChronometer;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Widget::RelativeLayout;
using Elastos::Droid::Widget::IOnChronometerTickListener;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

class TextProgressBar
    : public RelativeLayout
    , public ITextProgressBar
{
private:
    class InnerListener
        : public Object
        , public IOnChronometerTickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ TextProgressBar* host);

        CARAPI OnChronometerTick(
            /* [in] */ IChronometer* chronometer);
    private:
        TextProgressBar* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    TextProgressBar();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    /**
     * Catch any interesting children when they are added.
     */
    // @Override
    CARAPI AddView(
        /* [in] */ IView* child,
        /* [in] */ Int32 index,
        /* [in] */ IViewGroupLayoutParams* params);

    /**
     * Set the expected termination time of the running {@link Chronometer}.
     * This value is used to adjust the {@link ProgressBar} against the elapsed
     * time.
     * <p>
     * Call this <b>after</b> adjusting the {@link Chronometer} base, if
     * necessary.
     *
     * @param durationBase Use the {@link SystemClock#elapsedRealtime} time
     *            base.
     */
    virtual CARAPI SetDurationBase(
        /* [in] */ Int64 durationBase);


    /**
     * Callback when {@link Chronometer} changes, indicating that we should
     * update the {@link ProgressBar} and change the layout if necessary.
     */
    virtual CARAPI OnChronometerTick(
        /* [in] */ IChronometer* chronometer);

public:
    static const String TAG;
    static const Int32 CHRONOMETER_ID = R::id::text1;
    static const Int32 PROGRESSBAR_ID = R::id::progress;
    AutoPtr<IChronometer> mChronometer;
    AutoPtr<IProgressBar> mProgressBar;
    Int64 mDurationBase;
    Int32 mDuration;
    Boolean mChronometerFollow;
    Int32 mChronometerGravity;
};

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_TEXTPROGRESSBAR_H__
