
#include "CZoomActivity.h"

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AnimationsDemo {

CZoomActivity::MyListener::MyListener(
    /* [in] */ CZoomActivity* host,
    /* [in] */ IView* view)
    : mHost(host)
    , mView(view)
{
}

PInterface CZoomActivity::MyListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IViewOnClickListener) {
        return this;
    }

    return NULL;
}

UInt32 CZoomActivity::MyListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CZoomActivity::MyListener::Release()
{
    return ElRefBase::Release();
}

ECode CZoomActivity::MyListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)this) {
        *pIID = EIID_IViewOnClickListener;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CZoomActivity::MyListener::OnClick(
        /* [in] */ IView* newView)
{
    Int32 id = 0;
    newView->GetId(&id);
    if (id == R::id::thumb_button_1) {
        mHost->ZoomImageFromThumb(mView, R::drawable::image1);
    }
    else if (id == R::id::thumb_button_2) {
        mHost->ZoomImageFromThumb(mView, R::drawable::image2);
    }
    else if (id == R::id::expanded_image) {
        if (mCurrentAnimator != NULL) {
            mCurrentAnimator.cancel();
        }

        // Animate the four positioning/sizing properties in parallel, back to their
        // original values.
        AnimatorSet set = new AnimatorSet();
        set
                .play(ObjectAnimator.ofFloat(expandedImageView, View.X, startBounds.left))
                .with(ObjectAnimator.ofFloat(expandedImageView, View.Y, startBounds.top))
                .with(ObjectAnimator
                        .ofFloat(expandedImageView, View.SCALE_X, startScaleFinal))
                .with(ObjectAnimator
                        .ofFloat(expandedImageView, View.SCALE_Y, startScaleFinal));
        set.setDuration(mShortAnimationDuration);
        set.setInterpolator(new DecelerateInterpolator());
        set.addListener(new AnimatorListenerAdapter() {
            @Override
            public void onAnimationEnd(Animator animation) {
                thumbView.setAlpha(1f);
                expandedImageView.setVisibility(View.GONE);
                mCurrentAnimator = NULL;
            }

            @Override
            public void onAnimationCancel(Animator animation) {
                thumbView.setAlpha(1f);
                expandedImageView.setVisibility(View.GONE);
                mCurrentAnimator = NULL;
            }
        });
        set.start();
        mCurrentAnimator = set;
    }

    return NOERROR;
}

CZoomActivity::AnimatorListenerAdapter::AnimatorListenerAdapter(
    /* [in] */ IView* view)
    : mView(view)
{
}

UInt32 CZoomActivity::AnimatorListenerAdapter::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CZoomActivity::AnimatorListenerAdapter::Release()
{
    return ElRefBase::Release();
}

PInterface CZoomActivity::AnimatorListenerAdapter::Probe(
    /* [in] */ REIID riid)
{
    if(riid == EIID_IInterface) {
        return (IInterface*)this;
    } else if (riid == EIID_IAnimatorListener) {
        return this;
    }
    return NULL;
}

ECode CZoomActivity::AnimatorListenerAdapter::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    assert(pIID != NULL);
    if (pObject == (IInterface*)this) {
        *pIID = EIID_IAnimatorListener;
    } else {
        return E_INVALID_ARGUMENT;
    }

    return NOERROR;
}

ECode CZoomActivity::AnimatorListenerAdapter::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

ECode CZoomActivity::AnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    return mView->SetVisibility(IView::GONE);;
}

ECode CZoomActivity::AnimatorListenerAdapter::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

ECode CZoomActivity::AnimatorListenerAdapter::OnAnimationRepeat(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

CZoomActivity::CZoomActivity()
    : mShortAnimationDuration(0)
{
}

ECode CZoomActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::activity_zoom);

    // Hook up clicks on the thumbnail views.
    AutoPtr<IView> thumb1View = FindViewById(R::id::thumb_button_1);
    AutoPtr<MyListener> l = new MyListener(this, thumb1View);
    IViewOnClickListener* clickListener = (IViewOnClickListener*)(l->Probe(EIID_IViewOnClickListener));
    thumb1View->SetOnClickListener(clickListener);

    AutoPtr<IView> thumb2View = FindViewById(R::id::thumb_button_2);
    AutoPtr<MyListener> l2 = new MyListener(this, thumb2View);
    IViewOnClickListener* clickListener2 = (IViewOnClickListener*)(l2->Probe(EIID_IViewOnClickListener));
    thumb2View->SetOnClickListener(clickListener2);

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    // Retrieve and cache the system's default "short" animation time.
    return res->GetInteger(Elastos::Droid::R::integer::config_shortAnimTime, &mShortAnimationDuration);
}

ECode CZoomActivity::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Int32 id = 0;
    item->GetItemId(&id);
    switch (id) {
        case android.R.id.home: {
            // Navigate "up" the demo structure to the launchpad activity.
            // See http://developer.android.com/design/patterns/navigation.html for more.
//                NavUtils.navigateUpTo(this, new Intent(this, MainActivity.class));
            AutoPtr<IIntent> intent;
            CIntent::New((IIntent**)&intent);
            intent->SetClassName(String("AnimationsDemo"), String("AnimationsDemo.CMainActivity"));
            StartActivity(intent);
            Finish();
            *res = TRUE;
            return NOERROR:
        }
    }

    return Activity::OnOptionsItemSelected(item, res);
}

void CZoomActivity::ZoomImageFromThumb(
    /* [in] */ IView* thumbView,
    /* [in] */ Int32 imageResId)
{
    // If there's an animation in progress, cancel it immediately and proceed with this one.
    if (mCurrentAnimator != NULL) {
        mCurrentAnimator->Cancel();
    }

    // Load the high-resolution "zoomed-in" image.
    AutoPtr<IImageView> expandedImageView = IImageView::Probe(FindViewById(R::id::expanded_image));
    expandedImageView->SetImageResource(imageResId);

    // Calculate the starting and ending bounds for the zoomed-in image. This step
    // involves lots of math. Yay, math.
    AutoPtr<IRect> startBounds;
    CRect::New((IRect**)&startBounds);
    AutoPtr<IRect> finalBounds;
    CRect::New((IRect**)&finalBounds);
    AutoPtr<IPoint> globalOffset;
    CPoint::New((IPoint**)&globalOffset);

    // The start bounds are the global visible rectangle of the thumbnail, and the
    // final bounds are the global visible rectangle of the container view. Also
    // set the container view's offset as the origin for the bounds, since that's
    // the origin for the positioning animation properties (X, Y).
    thumbView->GetGlobalVisibleRect(startBounds);

    FindViewById(R::id::container)->GetGlobalVisibleRect(finalBounds, globalOffset);
    Int32 x = 0, y = 0;
    globalOffset->GetX(&x);
    globalOffset->GetY(&y);
    startBounds->Offset(-x, -y);
    finalBounds->Offset(-x, -y);

    // Adjust the start bounds to be the same aspect ratio as the final bounds using the
    // "center crop" technique. This prevents undesirable stretching during the animation.
    // Also calculate the start scaling factor (the end scaling factor is always 1.0).
    Float startScale;
    Int32 w = 0, h = 0;
    finalBounds->GetWidth(&w);
    finalBounds->GetHeight(&h);
    Int32 w2 = 0, h2 = 0;
    startBounds->GetWidth(&w2);
    startBounds->GetHeight(&h2);
    if ((Float) (w / h) > (Float) (w2 / h2)) {
        // Extend start bounds horizontally
        startScale = (Float) (h2 / h);
        Float startWidth = startScale * w;
        Float deltaWidth = (startWidth - w2) / 2;

        Int32 l = 0, r = 0;
        startBounds->GetLeft(&l);
        startBounds->SetLeft(l - deltaWidth);
        startBounds->GetRight(&r);
        startBounds->SetRight(r + deltaWidth);
    } else {
        // Extend start bounds vertically
        startScale = (Float) w2 / w;
        Float startHeight = startScale * h;
        Float deltaHeight = (startHeight - h2) / 2;

        Int32 t = 0, b = 0;
        startBounds->GetTop(&t);
        startBounds->SetTop(t - deltaHeight);

        startBounds->GetBottom(&b);
        startBounds->SetBottom(b + deltaHeight);
    }

    // Hide the thumbnail and show the zoomed-in view. When the animation begins,
    // it will position the zoomed-in view in the place of the thumbnail.
    thumbView->SetAlpha(0.f);
    expandedImageView->SetVisibility(IView::VISIBLE);

    // Set the pivot point for SCALE_X and SCALE_Y transformations to the top-left corner of
    // the zoomed-in view (the default is the center of the view).
    expandedImageView->SetPivotX(0.f);
    expandedImageView->SetPivotY(0.f);

    // Construct and run the parallel animation of the four translation and scale properties
    // (X, Y, SCALE_X, and SCALE_Y).
    AutoPtr<IAnimatorSet> set;
    CAnimatorSet::New((IAnimatorSet**)&set);

    AutoPtr<IAnimatorSetBuilder> builder;
    AutoPtr<IObjectAnimatorHelper> helper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
    AutoPtr<IObjectAnimator> animator;
    helper->OfFloat(expandedImageView, View.X, startBounds.left,
                    finalBounds.left, (IObjectAnimator**)&animator);

    set->Play(animator, (IAnimatorSetBuilder**)&builder);

    builder.with(ObjectAnimator.ofFloat(expandedImageView, View.Y, startBounds.top,
            finalBounds.top))
    builder.with(ObjectAnimator.ofFloat(expandedImageView, View.SCALE_X, startScale, 1.f))
    builder.with(ObjectAnimator.ofFloat(expandedImageView, View.SCALE_Y, startScale, 1.f));

    set->SetDuration(mShortAnimationDuration);
    set->SetInterpolator(new DecelerateInterpolator());
    set->AddListener(new AnimatorListenerAdapter() {
        @Override
        public void onAnimationEnd(Animator animation) {
            mCurrentAnimator = NULL;
        }

        @Override
        public void onAnimationCancel(Animator animation) {
            mCurrentAnimator = NULL;
        }
    });

    set->Start();
    mCurrentAnimator = set;

    // Upon clicking the zoomed-in image, it should zoom back down to the original bounds
    // and show the thumbnail instead of the expanded image.
    Float startScaleFinal = startScale;
    AutoPtr<MyListener> l = new MyListener(this, expandedImageView);
    IViewOnClickListener* clickListener = (IViewOnClickListener*)(l->Probe(EIID_IViewOnClickListener));
    thumb1View->SetOnClickListener(clickListener);
    expandedImageView->SetOnClickListener(clickListener);
}

} // namespace AnimationsDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
