
#include "CCrossfadeActivity.h"
#include "R.h"
#include <R.h>

using Elastos::Droid::Animation::EIID_IAnimatorListener;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::View::IViewPropertyAnimator;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AnimationsDemo {

CCrossfadeActivity::AnimatorListenerAdapter::AnimatorListenerAdapter(
    /* [in] */ IView* view)
    : mView(view)
{
}

UInt32 CCrossfadeActivity::AnimatorListenerAdapter::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CCrossfadeActivity::AnimatorListenerAdapter::Release()
{
    return ElRefBase::Release();
}

PInterface CCrossfadeActivity::AnimatorListenerAdapter::Probe(
    /* [in] */ REIID riid)
{
    if(riid == EIID_IInterface) {
        return (IInterface*)this;
    } else if (riid == EIID_IAnimatorListener) {
        return this;
    }
    return NULL;
}

ECode CCrossfadeActivity::AnimatorListenerAdapter::GetInterfaceID(
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

ECode CCrossfadeActivity::AnimatorListenerAdapter::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

ECode CCrossfadeActivity::AnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    return mView->SetVisibility(IView::GONE);;
}

ECode CCrossfadeActivity::AnimatorListenerAdapter::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

ECode CCrossfadeActivity::AnimatorListenerAdapter::OnAnimationRepeat(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

CCrossfadeActivity::CCrossfadeActivity()
    : mContentLoaded(FALSE)
    , mShortAnimationDuration(0)
{
}

ECode CCrossfadeActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::activity_crossfade);

    mContentView = FindViewById(R::id::content);
    mLoadingView = FindViewById(R::id::loading_spinner);

    // Initially hide the content view.
    mContentView->SetVisibility(IView::GONE);

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    // Retrieve and cache the system's default "short" animation time.
    return res->GetInteger(Elastos::Droid::R::integer::config_shortAnimTime, &mShortAnimationDuration);
}

ECode CCrossfadeActivity::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Activity::OnCreateOptionsMenu(menu, res);

    AutoPtr<IMenuInflater> inflater;
    GetMenuInflater((IMenuInflater**)&inflater);
    inflater->Inflate(R::menu::activity_crossfade, menu);
    *res = TRUE;
    return NOERROR;
}

ECode CCrossfadeActivity::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Int32 id = 0;
    item->GetItemId(&id);
    switch (id) {
        case Elastos::Droid::R::id::home: {
            // Navigate "up" the demo structure to the launchpad activity.
            // See http://developer.android.com/design/patterns/navigation.html for more.
//                NavUtils.navigateUpTo(this, new Intent(this, MainActivity.class));
            AutoPtr<IIntent> intent;
            CIntent::New((IIntent**)&intent);
            intent->SetClassName(String("AnimationsDemo"), String("AnimationsDemo.CMainActivity"));
            StartActivity(intent);
            Finish();
            *res = TRUE;
            return NOERROR;
        }

        case R::id::action_toggle: {
            // Toggle whether content is loaded.
            mContentLoaded = !mContentLoaded;
            ShowContentOrLoadingIndicator(mContentLoaded);
            *res = TRUE;
            return NOERROR;
        }
    }

    return Activity::OnOptionsItemSelected(item, res);
}

void CCrossfadeActivity::ShowContentOrLoadingIndicator(
    /* [in] */ Boolean contentLoaded)
{
    // Decide which view to hide and which to show.
    AutoPtr<IView> showView = contentLoaded ? mContentView : mLoadingView;
    AutoPtr<IView> hideView = contentLoaded ? mLoadingView : mContentView;

    // Set the "show" view to 0% opacity but visible, so that it is visible
    // (but fully transparent) during the animation.
    showView->SetAlpha(0.f);
    showView->SetVisibility(IView::VISIBLE);

    // Animate the "show" view to 100% opacity, and clear any animation listener set on
    // the view. Remember that listeners are not limited to the specific animation
    // describes in the chained method calls. Listeners are set on the
    // ViewPropertyAnimator object for the view, which persists across several
    // animations.
    AutoPtr<IViewPropertyAnimator> showAnimator;
    showView->Animate((IViewPropertyAnimator**)&showAnimator);
    showAnimator->Alpha(1.f);
    showAnimator->SetDuration(mShortAnimationDuration);
    showAnimator->SetListener(NULL);
    showAnimator->Release();

    // Animate the "hide" view to 0% opacity. After the animation ends, set its visibility
    // to GONE as an optimization step (it won't participate in layout passes, etc.)
    hideView->Animate((IViewPropertyAnimator**)&showAnimator);
    showAnimator->Alpha(0.f);
    showAnimator->SetDuration(mShortAnimationDuration);
    showAnimator->SetListener(new AnimatorListenerAdapter(hideView));
}

} // namespace AnimationsDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
