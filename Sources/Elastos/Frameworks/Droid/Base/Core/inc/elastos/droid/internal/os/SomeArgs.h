//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_INTERNAL_OS_SOME_ARGS_H__
#define __ELASTOS_DROID_INTERNAL_OS_SOME_ARGS_H__

#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

class ECO_PUBLIC SomeArgs
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
    ECO_LOCAL CARAPI_(void) Clear();

    ECO_LOCAL SomeArgs();

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
