
#include "elastos/droid/launcher2/LauncherAnimatorUpdateListener.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.CoreLibrary.Core.h"
#include "R.h"

using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Core::IFloat;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(LauncherAnimatorUpdateListener, Object, IAnimatorUpdateListener);

ECode LauncherAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> obj;
    animation->GetAnimatedValue((IInterface**)&obj);
    AutoPtr<IFloat> fObj = IFloat::Probe(obj);
    Float b;
    fObj->GetValue(&b);
    Float a = 1.0f - b;
    return OnAnimationUpdate(a, b);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos