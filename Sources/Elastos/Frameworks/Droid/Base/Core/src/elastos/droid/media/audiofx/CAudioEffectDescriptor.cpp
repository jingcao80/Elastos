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

#include "elastos/droid/media/audiofx/CAudioEffectDescriptor.h"

using Elastos::Utility::IUUIDHelper;
using Elastos::Utility::CUUIDHelper;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

CAudioEffectDescriptor::CAudioEffectDescriptor()
    : mConnectMode(String(NULL))
    , mName(String(NULL))
    , mImplementor(String(NULL))
{}

CAR_INTERFACE_IMPL(CAudioEffectDescriptor, Object, IAudioEffectDescriptor)

CAR_OBJECT_IMPL(CAudioEffectDescriptor)

ECode CAudioEffectDescriptor::constructor(
    /* [in] */ const String& type,
    /* [in] */ const String& uuid,
    /* [in] */ const String& connectMode,
    /* [in] */ const String& name,
    /* [in] */ const String& implementor)
{
    mConnectMode = connectMode;
    mName = name;
    mImplementor = implementor;

    AutoPtr<IUUIDHelper> helper;
    CUUIDHelper::AcquireSingleton((IUUIDHelper**)&helper);
    helper->FromString(type,(IUUID**)&mType);
    helper->FromString(uuid,(IUUID**)&mUuid);
    return NOERROR;
}

ECode CAudioEffectDescriptor::GetType(
    /* [out] */ IUUID** type)
{
    VALIDATE_NOT_NULL(type);
    *type = mType;
    REFCOUNT_ADD(*type);
    return NOERROR;
}

ECode CAudioEffectDescriptor::SetType(
    /* [in] */ IUUID* type)
{
    VALIDATE_NOT_NULL(type);
    mType = type;
    return NOERROR;
}

ECode CAudioEffectDescriptor::GetUuid(
    /* [out] */ IUUID** uuid)
{
    VALIDATE_NOT_NULL(uuid);
    *uuid = mUuid;
    REFCOUNT_ADD(*uuid);
    return NOERROR;
}

ECode CAudioEffectDescriptor::SetUuid(
    /* [in] */ IUUID* uuid)
{
    VALIDATE_NOT_NULL(uuid);
    mUuid = uuid;
    return NOERROR;
}

ECode CAudioEffectDescriptor::GetConnectMode(
    /* [out] */ String* connectMode)
{
    VALIDATE_NOT_NULL(connectMode);
    *connectMode = mConnectMode;
    return NOERROR;
}

ECode CAudioEffectDescriptor::SetConnectMode(
    /* [in] */ const String& connectMode)
{
    mConnectMode = connectMode;
    return NOERROR;
}

ECode CAudioEffectDescriptor::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    return NOERROR;
}

ECode CAudioEffectDescriptor::SetName(
    /* [in] */ const String& name)
{
    mName = name;
    return NOERROR;
}

ECode CAudioEffectDescriptor::GetImplementor(
    /* [out] */ String* implementor)
{
    VALIDATE_NOT_NULL(implementor);
    *implementor = mImplementor;
    return NOERROR;
}

ECode CAudioEffectDescriptor::SetImplementor(
    /* [in] */ const String& implementor)
{
    mImplementor = implementor;
    return NOERROR;
}

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos
