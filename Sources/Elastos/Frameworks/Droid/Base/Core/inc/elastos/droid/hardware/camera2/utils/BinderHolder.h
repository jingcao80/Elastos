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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_BINDERHOLDER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_BINDERHOLDER_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

class BinderHolder
    : public Object
    , public IBinderHolder
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    BinderHolder() {}

    virtual ~BinderHolder() {}

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IBinder* binder);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    //@Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI GetBinder(
        /* [out] */ IBinder** binder);

    CARAPI SetBinder(
        /* [in] */ IBinder* binder);

    // public static final Parcelable.Creator<BinderHolder> CREATOR =
    //          new Parcelable.Creator<BinderHolder>() {
    //      @Override
    //      public BinderHolder createFromParcel(Parcel in) {
    //          return new BinderHolder(in);
    //      }

    //      @Override
    //      public BinderHolder[] newArray(int size) {
    //          return new BinderHolder[size];
    //      }
    // };

private:
    CARAPI constructor(
        /* [in] */ IParcel* _in);

private:
    AutoPtr<IBinder> mBinder;
};

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_BINDERHOLDER_H__
