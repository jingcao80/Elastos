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

#ifndef __ELASTOS_DROID_JAVAPROXY_CISERIALIZABLENATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CISERIALIZABLENATIVE_H__

#include "_Elastos_Droid_JavaProxy_CISerializableNative.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;
using Elastos::IO::ISerializable;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CISerializableNative)
    , public Object
    , public ISerializable
    , public IParcelable
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Boolean isDexClassLoader,
        /* [in] */ const String& pkgPath,
        /* [in] */ const String& optimizedDirectory,
        /* [in] */ ArrayOf<Byte>* obj);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel * pSource);

    CARAPI WriteToParcel(
        /* [in] */ IParcel * pDest);

    CARAPI ToString(
        /* [out] */ String* str);

public:
    Boolean mIsDexClassLoader;
    String mPkgPath;
    String mOptimizedDirectory;
    AutoPtr<ArrayOf<Byte> > mObject;

private:
    static const String TAG;
};

}
}
}

#endif // __ELASTOS_DROID_JAVAPROXY_CISERIALIZABLENATIVE_H__
