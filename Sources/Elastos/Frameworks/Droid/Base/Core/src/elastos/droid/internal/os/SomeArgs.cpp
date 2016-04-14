#include "elastos/droid/internal/os/SomeArgs.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

const Int32 SomeArgs::MAX_POOL_SIZE = 10;
AutoPtr<SomeArgs> SomeArgs::sPool;
Int32 SomeArgs::sPoolSize = 0;
Object SomeArgs::sPoolLock;

const Int32 SomeArgs::WAIT_NONE;
const Int32 SomeArgs::WAIT_WAITING;
const Int32 SomeArgs::WAIT_FINISHED;

CAR_INTERFACE_IMPL(SomeArgs, Object, ISomeArgs)

SomeArgs::SomeArgs()
    : mWaitState(WAIT_NONE)
    , mArgi1(0)
    , mArgi2(0)
    , mArgi3(0)
    , mArgi4(0)
    , mArgi5(0)
    , mArgi6(0)
    , mInPool(FALSE)
{
    /* do nothing - reduce visibility */
}

AutoPtr<SomeArgs> SomeArgs::Obtain()
{
    AutoLock lock(sPoolLock);

    if (sPoolSize > 0) {
        AutoPtr<SomeArgs> args = sPool;
        sPool = sPool->mNext;
        args->mNext = NULL;
        args->mInPool = FALSE;
        --sPoolSize;
        return args;
    }
    else {
        AutoPtr<SomeArgs> args =  new SomeArgs();
        return args;
    }
}

ECode SomeArgs::Recycle()
{
    if (mInPool) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mWaitState != WAIT_NONE) {
        return NOERROR;
    }

    AutoLock lock(sPoolLock);
    Clear();
    if (sPoolSize < MAX_POOL_SIZE) {
        mNext = sPool;
        mInPool = TRUE;
        sPool = this;
        ++sPoolSize;
    }
    return NOERROR;
}

void SomeArgs::Clear()
{
    mArg1 = NULL;
    mArg2 = NULL;
    mArg3 = NULL;
    mArg4 = NULL;
    mArg5 = NULL;
    mArg6 = NULL;
    mArgi1 = 0;
    mArgi2 = 0;
    mArgi3 = 0;
    mArgi4 = 0;
    mArgi5 = 0;
    mArgi6 = 0;
}

ECode SomeArgs::GetObjectArg(
    /* [in] */ Int32 index,
    /* [out] */ IInterface** arg)
{
    VALIDATE_NOT_NULL(arg)
    switch (index) {
        case 1:
            *arg = mArg1;
            REFCOUNT_ADD(*arg);
            break;
        case 2:
            *arg = mArg2;
            REFCOUNT_ADD(*arg);
            break;
        case 3:
            *arg = mArg3;
            REFCOUNT_ADD(*arg);
            break;
        case 4:
            *arg = mArg4;
            REFCOUNT_ADD(*arg);
            break;
        case 5:
            *arg = mArg5;
            REFCOUNT_ADD(*arg);
            break;
        case 6:
            *arg = mArg6;
            REFCOUNT_ADD(*arg);
            break;
        default:
            Logger::E("SomeArgs", "GetObjectArg invalid index!");
    }

    return NOERROR;
}

ECode SomeArgs::SetObjectArg(
    /* [in] */ Int32 index,
    /* [in] */ IInterface* arg)
{
    switch (index) {
        case 1:
            mArg1 = arg;
            break;
        case 2:
            mArg2 = arg;
            break;
        case 3:
            mArg3 = arg;
            break;
        case 4:
            mArg4 = arg;
            break;
        case 5:
            mArg5 = arg;
            break;
        case 6:
            mArg6 = arg;
            break;
        default:
            Logger::E("SomeArgs", "SetObjectArg invalid index!");
    }
    return NOERROR;
}

ECode SomeArgs::GetInt32Arg(
    /* [in] */ Int32 index,
    /* [out] */ Int32* arg)
{
    VALIDATE_NOT_NULL(arg)
    switch (index) {
        case 1:
            *arg = mArgi1;
            break;
        case 2:
            *arg = mArgi2;
            break;
        case 3:
            *arg = mArgi3;
            break;
        case 4:
            *arg = mArgi4;
            break;
        case 5:
            *arg = mArgi5;
            break;
        case 6:
            *arg = mArgi6;
            break;
        default:
            Logger::E("SomeArgs", "GetInt32Arg invalid index!");
    }
    return NOERROR;
}

ECode SomeArgs::SetInt32Arg(
    /* [in] */ Int32 index,
    /* [in] */ Int32 arg)
{
    switch (index) {
        case 1:
            mArgi1 = arg;
            break;
        case 2:
            mArgi2 = arg;
            break;
        case 3:
            mArgi3 = arg;
            break;
        case 4:
            mArgi4 = arg;
            break;
        case 5:
            mArgi5 = arg;
            break;
        case 6:
            mArgi6 = arg;
            break;
        default:
            Logger::E("SomeArgs", "SetInt32Arg invalid index!");
    }
    return NOERROR;
}

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos
