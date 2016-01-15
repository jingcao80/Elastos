
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/animation/CAnimationUtils.h"
#include "elastos/droid/view/animation/AnimationUtils.h"

namespace Elastos {
namespace Droid {
namespace View {
namespace Animation {

CAR_INTERFACE_IMPL(CAnimationUtils, Singleton, IAnimationUtils)

CAR_SINGLETON_IMPL(CAnimationUtils)

CAnimationUtils::CurrentAnimationTimeMillis(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time);
    return AnimationUtils::CurrentAnimationTimeMillis(time);
}

CAnimationUtils::LoadAnimation(
    /* [in] */ IContext* context,
    /* [in] */ Int32 id,
    /* [out] */ IAnimation** animation)
{
    VALIDATE_NOT_NULL(animation);
    return AnimationUtils::LoadAnimation(context, id, animation);
}

CAnimationUtils::LoadLayoutAnimation(
    /* [in] */ IContext* context,
    /* [in] */ Int32 id,
    /* [out] */ ILayoutAnimationController** controller)
{
    VALIDATE_NOT_NULL(controller);
    return AnimationUtils::LoadLayoutAnimation(context, id, controller);
}

CAnimationUtils::MakeInAnimation(
    /* [in] */ IContext* context,
    /* [in] */ Boolean fromLeft,
    /* [out] */ IAnimation** animation)
{
    VALIDATE_NOT_NULL(animation);
    return AnimationUtils::MakeInAnimation(context, fromLeft, animation);
}

CAnimationUtils::MakeOutAnimation(
    /* [in] */ IContext* context,
    /* [in] */ Boolean toRight,
    /* [out] */ IAnimation** animation)
{
    VALIDATE_NOT_NULL(animation);
    return AnimationUtils::MakeOutAnimation(context, toRight, animation);
}

CAnimationUtils::MakeInChildBottomAnimation(
    /* [in] */ IContext* context,
    /* [out] */ IAnimation** animation)
{
    VALIDATE_NOT_NULL(animation);
    return AnimationUtils::MakeInChildBottomAnimation(context, animation);
}

CAnimationUtils::LoadInterpolator(
    /* [in] */ IContext* context,
    /* [in] */ Int32 id,
    /* [out] */ IInterpolator** interpolator)
{
    VALIDATE_NOT_NULL(interpolator);
    return AnimationUtils::LoadInterpolator(context, id, interpolator);
}

CAnimationUtils::LoadInterpolator(
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme,
    /* [in] */ Int32 id,
    /* [out] */ IInterpolator** interpolator)
{
    VALIDATE_NOT_NULL(interpolator);
    return AnimationUtils::LoadInterpolator(res, theme, id, interpolator);
}

}   //namespace Animation
}   //namespace View
}   //namespace Droid
}   //namespace Elastos
