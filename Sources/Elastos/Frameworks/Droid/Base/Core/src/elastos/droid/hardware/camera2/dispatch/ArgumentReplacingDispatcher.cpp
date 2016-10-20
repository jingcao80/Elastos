
#include "elastos/droid/hardware/camera2/dispatch/ArgumentReplacingDispatcher.h"
#include "elastos/droid/internal/utility/Preconditions.h"

using Elastos::Droid::Internal::Utility::Preconditions;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Dispatch {

CAR_INTERFACE_IMPL_2(ArgumentReplacingDispatcher, Object, IArgumentReplacingDispatcher, IDispatchable)

ArgumentReplacingDispatcher::ArgumentReplacingDispatcher()
{
}

ECode ArgumentReplacingDispatcher::constructor()
{
    return NOERROR;
}

ECode ArgumentReplacingDispatcher::constructor(
    /* [in] */ IDispatchable* target,
    /* [in] */ Int32 argumentIndex,
    /* [in] */ IInterface* replaceWith)
{
    FAIL_RETURN(Preconditions::CheckNotNull(target, String("target must not be null")))
    FAIL_RETURN(Preconditions::CheckArgumentNonnegative(argumentIndex,
            String("argumentIndex must not be negative")))
    FAIL_RETURN(Preconditions::CheckNotNull(replaceWith, String("replaceWith must not be null")))

    mTarget = target;
    mArgumentIndex = argumentIndex;
    mReplaceWith = replaceWith;
    return NOERROR;
}

ECode ArgumentReplacingDispatcher::Dispatch(
    /* [in] */ IMethodInfo* method,
    /* [in] */ IArgumentList* args)
{
    assert(0);
    // AutoPtr<ArrayOf<IInterface*> > _args;
    // if (args->GetLength() > mArgumentIndex) {
    //     // don't change in-place since it can affect upstream dispatches
    //     _args = ArrayCopy(args);
    //     _args->Set(mArgumentIndex ,mReplaceWith);
    // }

    // return mTarget->Dispatch(method, _args);
    return NOERROR;
}

AutoPtr<ArrayOf<IInterface*> > ArgumentReplacingDispatcher::ArrayCopy(
    /* [in] */ ArrayOf<IInterface*>* array)
{
    Int32 length = array->GetLength();
    AutoPtr<ArrayOf<IInterface*> > newArray = ArrayOf<IInterface*>::Alloc(length);
    for (Int32 i = 0; i < length; ++i) {
        newArray->Set(i, (*array)[i]);
    }
    return newArray;
}

} // namespace Dispatch
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
