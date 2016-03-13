
#include "elastos/droid/systemui/egg/CLLandHelper.h"
#include "elastos/droid/systemui/egg/LLand.h"

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Egg{

CAR_SINGLETON_IMPL(CLLandHelper)
CAR_INTERFACE_IMPL(CLLandHelper, Singleton, ILLandHelper)

ECode CLLandHelper::L(
    /* [in] */ const String& s,
    /* [in] */ ArrayOf<IInterface* >* objects)
{
    LLand::L(s, objects);
    return NOERROR;
}

ECode CLLandHelper::Lerp(
    /* [in] */ Float x,
    /* [in] */ Float a,
    /* [in] */ Float b,
    /* [out] */ Float* f)
{
    return LLand::Lerp(x, a, b, f);
}

ECode CLLandHelper::Rlerp(
    /* [in] */ Float v,
    /* [in] */ Float a,
    /* [in] */ Float b,
    /* [out] */ Float* f)
{
    return LLand::Rlerp(v, a, b, f);
}

ECode CLLandHelper::Clamp(
    /* [in] */ Float f,
    /* [out] */ Float* res)
{
    return LLand::Clamp(f, res);
}

ECode CLLandHelper::Frand(
    /* [out] */ Float* f)
{
    return LLand::Frand(f);
}

ECode CLLandHelper::Frand(
    /* [in] */ Float a,
    /* [in] */ Float b,
    /* [out] */ Float* f)
{
    return LLand::Frand(a, b, f);
}

ECode CLLandHelper::Irand(
    /* [in] */ Int32 a,
    /* [in] */ Int32 b,
    /* [out] */ Int32* f)
{
    return LLand::Irand(a, b, f);
}

} // namespace Egg
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos