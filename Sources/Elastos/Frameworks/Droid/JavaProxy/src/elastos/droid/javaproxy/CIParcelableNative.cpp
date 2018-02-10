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

#include "elastos/droid/javaproxy/CIParcelableNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIParcelableNative::TAG("CIParcelableNative");

CAR_INTERFACE_IMPL_2(CIParcelableNative, Object, IParcelable, IBinder)

CAR_OBJECT_IMPL(CIParcelableNative)

ECode CIParcelableNative::constructor()
{
    return NOERROR;
}

ECode CIParcelableNative::constructor(
    /* [in] */ const String& pkgPath,
    /* [in] */ ArrayOf<Byte>* obj)
{
    mPkgPath = pkgPath;
    mObject = obj;
    return NOERROR;
}

String CIParcelableNative::GetPackagePath()
{
    return mPkgPath;
}

ECode CIParcelableNative::GetObject(
    /* [out, callee] */ ArrayOf<Byte>** obj)
{
    assert(obj != NULL);
    *obj = mObject;
    if (*obj) (*obj)->AddRef();
    return NOERROR;
}

ECode CIParcelableNative::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadString(&mPkgPath);
    source->ReadArrayOf((HANDLE*)&mObject);
    return NOERROR;
}

ECode CIParcelableNative::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mPkgPath);
    dest->WriteArrayOf((HANDLE)mObject.Get());
    return NOERROR;
}

ECode CIParcelableNative::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

}
}
}

