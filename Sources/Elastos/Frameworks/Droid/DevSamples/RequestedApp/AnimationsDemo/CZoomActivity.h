
#ifndef __CZOOMACTIVITY_H__
#define __CZOOMACTIVITY_H__

#include "_CZoomActivity.h"
#include "elastos/droid/app/Activity.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Animation::IAnimator;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AnimationsDemo {

/**
 * A sample showing how to zoom an image thumbnail to full-screen, by animating the bounds of the
 * zoomed image from the thumbnail bounds to the screen bounds.
 *
 * <p>In this sample, the user can touch one of two images. Touching an image zooms it in, covering
 * the entire activity content area. Touching the zoomed-in image hides it.</p>
 */
class CZoomActivity : public Activity
{
private:
    class MyListener
        : public IViewOnClickListener
        , public ElRefBase
    {
    public:
        MyListener(
            /* [in] */ CZoomActivity* host,
            /* [in] */ IView* view);

        CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        AutoPtr<CZoomActivity> mHost;
        AutoPtr<IView> mView;
    };

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

protected:
    CZoomActivity();

    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* res);

private:
    /**
     * "Zooms" in a thumbnail view by assigning the high resolution image to a hidden "zoomed-in"
     * image view and animating its bounds to fit the entire activity content area. More
     * specifically:
     *
     * <ol>
     *   <li>Assign the high-res image to the hidden "zoomed-in" (expanded) image view.</li>
     *   <li>Calculate the starting and ending bounds for the expanded view.</li>
     *   <li>Animate each of four positioning/sizing properties (X, Y, SCALE_X, SCALE_Y)
     *       simultaneously, from the starting bounds to the ending bounds.</li>
     *   <li>Zoom back out by running the reverse animation on click.</li>
     * </ol>
     *
     * @param thumbView  The thumbnail view to zoom in.
     * @param imageResId The high-resolution version of the image represented by the thumbnail.
     */
    CARAPI_(void) ZoomImageFromThumb(
        /* [in] */ IView* thumbView,
        /* [in] */ Int32 imageResId);

private:
    /**
     * Hold a reference to the current animator, so that it can be canceled mid-way.
     */
    AutoPtr<IAnimator> mCurrentAnimator;

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

#endif  // __CZOOMACTIVITY_H__
