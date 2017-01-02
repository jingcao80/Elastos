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
#include "elastos/droid/media/session/CPlaybackStateCustomActionBuilder.h"
#include "elastos/droid/media/session/CPlaybackStateCustomAction.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

CAR_INTERFACE_IMPL(CPlaybackStateCustomActionBuilder, Object, IISessionCallback)

CAR_OBJECT_IMPL(CPlaybackStateCustomActionBuilder)

CPlaybackStateCustomActionBuilder::CPlaybackStateCustomActionBuilder()
    : mAction(String(NULL))
    , mIcon(0)
{
}

CPlaybackStateCustomActionBuilder::~CPlaybackStateCustomActionBuilder()
{
}

ECode CPlaybackStateCustomActionBuilder::constructor(
    /* [in] */ const String& action,
    /* [in] */ ICharSequence * name,
    /* [in] */ Int32 icon)
{
    if (action.IsEmpty()) {
        // throw new IllegalArgumentException(
        //         "You must specify an action to build a CustomAction.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String str;
    name->ToString(&str);
    if (str.IsEmpty()) {
        // throw new IllegalArgumentException(
        //         "You must specify a name to build a CustomAction.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (icon == 0) {
        // throw new IllegalArgumentException(
        //         "You must specify an icon resource id to build a CustomAction.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mAction = action;
    mName = name;
    mIcon = icon;
    return NOERROR;
}

ECode CPlaybackStateCustomActionBuilder::SetExtras(
    /* [in] */ IBundle * extras)
{
    VALIDATE_NOT_NULL(extras)
    mExtras = extras;
    return NOERROR;
}

ECode CPlaybackStateCustomActionBuilder::Build(
    /* [out] */ IPlaybackStateCustomAction ** result)
{
    VALIDATE_NOT_NULL(result)
    return CPlaybackStateCustomAction::New(mAction, mName, mIcon, mExtras, result);
}

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos
