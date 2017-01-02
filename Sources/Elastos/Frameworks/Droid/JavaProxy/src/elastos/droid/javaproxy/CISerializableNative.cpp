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

#include "elastos/droid/javaproxy/CISerializableNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::IO::EIID_ISerializable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CISerializableNative::TAG("CISerializableNative");

CAR_INTERFACE_IMPL_3(CISerializableNative, Object, ISerializable, IParcelable, IBinder)

CAR_OBJECT_IMPL(CISerializableNative)

ECode CISerializableNative::constructor()
{
    return NOERROR;
}

ECode CISerializableNative::constructor(
    /* [in] */ Boolean isDexClassLoader,
    /* [in] */ const String& pkgPath,
    /* [in] */ const String& optimizedDirectory,
    /* [in] */ ArrayOf<Byte>* obj)
{
    mIsDexClassLoader = isDexClassLoader;
    mPkgPath = pkgPath;
    mOptimizedDirectory = optimizedDirectory;
    mObject = obj;
    return NOERROR;
}

ECode CISerializableNative::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadBoolean(&mIsDexClassLoader);
    source->ReadString(&mPkgPath);
    source->ReadString(&mOptimizedDirectory);
    source->ReadArrayOf((Handle32*)&mObject);
    return NOERROR;
}

ECode CISerializableNative::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteBoolean(mIsDexClassLoader);
    dest->WriteString(mPkgPath);
    dest->WriteString(mOptimizedDirectory);
    dest->WriteArrayOf((Handle32)mObject.Get());
    return NOERROR;
}

ECode CISerializableNative::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

}
}
}

