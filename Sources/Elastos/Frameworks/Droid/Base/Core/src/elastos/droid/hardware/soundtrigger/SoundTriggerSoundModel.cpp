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

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/hardware/soundtrigger/SoundTriggerSoundModel.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Soundtrigger {

CAR_INTERFACE_IMPL(SoundTriggerSoundModel, Object, ISoundTriggerSoundModel)

SoundTriggerSoundModel::SoundTriggerSoundModel()
    : mUuid(NULL)
    , mType(0)
    , mVendorUuid(NULL)
    , mData(NULL)
{
}

SoundTriggerSoundModel::~SoundTriggerSoundModel()
{
}

ECode SoundTriggerSoundModel::constructor()
{
    return NOERROR;
}

ECode SoundTriggerSoundModel::constructor(
    /* [in] */ IUUID* uuid,
    /* [in] */ IUUID* vendorUuid,
    /* [in] */ Int32 type,
    /* [in] */ ArrayOf<Byte>* data)
{
    mUuid = uuid;
    mVendorUuid = vendorUuid;
    mType = type;
    mData = data;
    return NOERROR;
}

ECode SoundTriggerSoundModel::GetUuid(
    /* [out] */ IUUID** uuid)
{
    VALIDATE_NOT_NULL(uuid);

    *uuid = mUuid;
    REFCOUNT_ADD(*uuid);
    return NOERROR;
}

ECode SoundTriggerSoundModel::GetType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);

    *type = mType;
    return NOERROR;
}

ECode SoundTriggerSoundModel::GetVendorUuid(
    /* [out] */ IUUID** uuid)
{
    VALIDATE_NOT_NULL(uuid);

    *uuid = mVendorUuid;
    REFCOUNT_ADD(*uuid);
    return NOERROR;
}

ECode SoundTriggerSoundModel::Getdata(
    /* [out, callee] */ ArrayOf<Byte>** data)
{
    VALIDATE_NOT_NULL(data);

    *data = mData;
    REFCOUNT_ADD(*data);
    return NOERROR;
}

} //Soundtrigger
} //Hardware
} //Droid
} //Elastos
