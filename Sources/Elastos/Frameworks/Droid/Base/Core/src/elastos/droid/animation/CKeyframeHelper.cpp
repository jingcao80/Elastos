
#include "elastos/droid/animation/CKeyframeHelper.h"

namespace Elastos {
namespace Droid {
namespace Animation {

CAR_INTERFACE_IMPL(CKeyframeHelper, Singleton, IKeyframeHelper)
CAR_SINGLETON_IMPL(CKeyframeHelper)

ECode CKeyframeHelper::OfInt32(
    /* [in] */ Float fraction,
    /* [in] */ Int32 value,
    /* [out] */ IKeyframe** obj)
{
    return Keyframe::OfInt32(fraction, value, obj);
}

ECode CKeyframeHelper::OfInt32(
    /* [in] */ Float fraction,
    /* [out] */ IKeyframe** obj)
{
    return Keyframe::OfInt32(fraction, obj);
}

ECode CKeyframeHelper::OfFloat(
    /* [in] */ Float fraction,
    /* [in] */ Float value,
    /* [out] */ IKeyframe** obj)
{
    return Keyframe::OfFloat(fraction, value, obj);
}

ECode CKeyframeHelper::OfFloat(
    /* [in] */ Float fraction,
    /* [out] */ IKeyframe** obj)
{
    return Keyframe::OfFloat(fraction, obj);
}

ECode CKeyframeHelper::OfObject(
    /* [in] */ Float fraction,
    /* [in] */ IInterface* value,
    /* [out] */ IKeyframe** obj)
{
    return Keyframe::OfObject(fraction, value, obj);
}

ECode CKeyframeHelper::OfObject(
    /* [in] */ Float fraction,
    /* [out] */ IKeyframe** obj)
{
    return Keyframe::OfObject(fraction, obj);
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos
