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

#include "elastos/droid/media/audiopolicy/CAudioMix.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace AudioPolicy {

CAR_INTERFACE_IMPL(CAudioMix, Object, IAudioMix)

CAR_OBJECT_IMPL(CAudioMix)

CAudioMix::CAudioMix()
    : mRouteFlags(0)
{}

CAudioMix::~CAudioMix()
{
}

ECode CAudioMix::constructor(
    /* [in] */ IAudioMixingRule * rule,
    /* [in] */ IAudioFormat * format,
    /* [in] */ Int32 routeFlags)
{
    mRule = rule;
    mFormat = format;
    mRouteFlags = routeFlags;
    return NOERROR;
}

Int32 CAudioMix::GetRouteFlags()
{
    return mRouteFlags;
}

AutoPtr<IAudioFormat> CAudioMix::GetFormat()
{
    return mFormat;
}

AutoPtr<IAudioMixingRule> CAudioMix::GetRule()
{
    return mRule;
}


} // namespace AudioPolicy
} // namespace Media
} // namepsace Droid
} // namespace Elastos
