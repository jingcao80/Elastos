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

#include "Elastos.Droid.Os.h"
#include "elastos/droid/hardware/camera2/utils/BinderHolder.h"
#include <elastos/core/StringBuffer.h>

using Elastos::Core::StringBuffer;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

CAR_INTERFACE_IMPL_2(BinderHolder, Object, IBinderHolder, IParcelable)

ECode BinderHolder::constructor()
{
    return NOERROR;
}

ECode BinderHolder::constructor(
    /* [in] */ IBinder* binder)
{
    mBinder = binder;
    return NOERROR;
}

ECode BinderHolder::constructor(
    /* [in] */ IParcel* _in)
{
    assert(0);
    //return _in->readStrongBinder((IBinder**)&mBinder);
    return NOERROR;
}

ECode BinderHolder::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    assert(0);
    //return dest->WriteStrongBinder(mBinder);
    return NOERROR;
}

ECode BinderHolder::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(0);
    //return source->ReadStrongBinder((IBinder**)&mBinder);
    return NOERROR;
}

ECode BinderHolder::GetBinder(
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder);

    *binder = mBinder;
    REFCOUNT_ADD(*binder);
    return NOERROR;
}

ECode BinderHolder::SetBinder(
    /* [in] */ IBinder* binder)
{
    mBinder = binder;
    return NOERROR;
}

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
