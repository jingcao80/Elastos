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
#include "elastos/droid/media/session/CPlaybackStateCustomAction.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Objects.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Objects;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

CAR_INTERFACE_IMPL_2(CPlaybackStateCustomAction, Object, IPlaybackStateCustomAction, IParcelable)

CAR_OBJECT_IMPL(CPlaybackStateCustomAction)

CPlaybackStateCustomAction::CPlaybackStateCustomAction()
    : mIcon(0)
{
}

CPlaybackStateCustomAction::~CPlaybackStateCustomAction()
{
}

ECode CPlaybackStateCustomAction::constructor(
    /* [in] */ const String& action,
    /* [in] */ ICharSequence * name,
    /* [in] */ Int32 icon,
    /* [in] */ IBundle * extras)
{
    mAction = action;
    mName = name;
    mIcon = icon;
    mExtras = extras;
    return NOERROR;
}

ECode CPlaybackStateCustomAction::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadString(&mAction);
    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((HANDLE*)&obj);
    mName = ICharSequence::Probe(obj);
    source->ReadInt32(&mIcon);
    obj = NULL;
    source->ReadInterfacePtr((HANDLE*)&obj);
    mExtras = IBundle::Probe(obj);
    return NOERROR;
}

ECode CPlaybackStateCustomAction::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mAction);
    dest->WriteInterfacePtr(mName);
    dest->WriteInt32(mIcon);
    dest->WriteInterfacePtr(mExtras);
    return NOERROR;
}

ECode CPlaybackStateCustomAction::GetAction(
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAction;
    return NOERROR;
}

ECode CPlaybackStateCustomAction::GetName(
    /* [out] */ ICharSequence ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mName;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CPlaybackStateCustomAction::GetIcon(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIcon;
    return NOERROR;
}

ECode CPlaybackStateCustomAction::GetExtras(
    /* [out] */ IBundle ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mExtras;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CPlaybackStateCustomAction::ToString(
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result)
    StringBuilder sb("Action:");
    sb.Append(mAction);
    String str;
    mName->ToString(&str);
    sb.Append(" mName=");
    sb.Append(str);
    sb.Append(", mIcon=");
    sb.Append(mIcon);
    // mExtras->ToString(&str);
    // sb.Append(", mExtras=");
    // sb.Append(str);
    return sb.ToString(result);
}

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos
