#ifndef __ELASTOS_DROID_INTERNAL_OS_SOME_ARGS_H__
#define __ELASTOS_DROID_INTERNAL_OS_SOME_ARGS_H__

#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

class SomeArgs
    : public Object
    , public ISomeArgs
{
public:
    CAR_INTERFACE_DECL()

    static CARAPI_(AutoPtr<SomeArgs>) Obtain();

    CARAPI GetObjectArg(
        /* [in] */ Int32 index,
        /* [out] */ IInterface** arg);

    CARAPI SetObjectArg(
        /* [in] */ Int32 index,
        /* [in] */ IInterface* arg);

    CARAPI GetInt32Arg(
        /* [in] */ Int32 index,
        /* [out] */ Int32* arg);

    CARAPI SetInt32Arg(
        /* [in] */ Int32 index,
        /* [in] */ Int32 arg);

    CARAPI Recycle();

private:
    CARAPI_(void) Clear();

    SomeArgs();

public:
    static const Int32 WAIT_NONE = 0;
    static const Int32 WAIT_WAITING = 1;
    static const Int32 WAIT_FINISHED = 2;

    Int32 mWaitState;
    AutoPtr<IInterface> mArg1;
    AutoPtr<IInterface> mArg2;
    AutoPtr<IInterface> mArg3;
    AutoPtr<IInterface> mArg4;
    AutoPtr<IInterface> mArg5;
    AutoPtr<IInterface> mArg6;

    Int32 mArgi1;
    Int32 mArgi2;
    Int32 mArgi3;
    Int32 mArgi4;
    Int32 mArgi5;
    Int32 mArgi6;

private:
    static const Int32 MAX_POOL_SIZE;

    static AutoPtr<SomeArgs> sPool;
    static Int32 sPoolSize;
    static Object sPoolLock;

    AutoPtr<SomeArgs> mNext;

    Boolean mInPool;
};

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Internal::Os::SomeArgs, IInterface)

#endif //__ELASTOS_DROID_INTERNAL_OS_SOME_ARGS_H__
