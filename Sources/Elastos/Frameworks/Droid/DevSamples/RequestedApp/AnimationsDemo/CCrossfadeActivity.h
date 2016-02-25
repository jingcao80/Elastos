
#ifndef __CCROSSFADEACTIVITY_H__
#define __CCROSSFADEACTIVITY_H__

#include "_CCrossfadeActivity.h"
#include "elastos/droid/app/Activity.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::IAnimator;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AnimationsDemo {

/**
 * This sample demonstrates cross-fading between two overlapping views.
 *
 * <p>In this sample, the two overlapping views are a loading indicator and some text content. The
 * active view is toggled by touching the toggle button in the action bar. In real-world
 * applications, this toggle would occur as soon as content was available. Note that if content is
 * immediately available, a loading spinner shouldn't be presented and there should be no
 * animation.</p>
 */
class CCrossfadeActivity : public Activity
{
private:
    class AnimatorListenerAdapter
        : public ElRefBase
        , public IAnimatorListener
    {
    public:
        AnimatorListenerAdapter(
            /* [in] */ IView* view);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI_(PInterface) Probe(
            /* [in] */ REIID riid);

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        virtual CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

        virtual CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        virtual CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

        virtual CARAPI OnAnimationRepeat(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<IView> mView;
    };

public:
    CCrossfadeActivity();

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* res);

    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* res);

private:
    /**
     * Cross-fades between {@link #mContentView} and {@link #mLoadingView}.
     */
    CARAPI_(void) ShowContentOrLoadingIndicator(
        /* [in] */ Boolean contentLoaded);

private:
    /**
     * The flag indicating whether content is loaded (text is shown) or not (loading spinner is
     * shown).
     */
    Boolean mContentLoaded;

    /**
     * The view (or view group) containing the content. This is one of two overlapping views.
     */
    AutoPtr<IView> mContentView;

    /**
     * The view containing the loading indicator. This is the other of two overlapping views.
     */
    AutoPtr<IView> mLoadingView;

    /**
     * The system "short" animation time duration, in milliseconds. This duration is ideal for
     * subtle animations or animations that occur very frequently.
     */
    Int32 mShortAnimationDuration;
};

} // namespace AnimationsDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif //__CCROSSFADEACTIVITY_H__
