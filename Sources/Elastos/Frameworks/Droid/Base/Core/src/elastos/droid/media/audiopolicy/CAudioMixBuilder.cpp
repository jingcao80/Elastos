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

#include "elastos/droid/media/audiopolicy/CAudioMixBuilder.h"
#include "elastos/droid/media/audiopolicy/CAudioMix.h"
#include "elastos/droid/media/AudioSystem.h"
#include "elastos/droid/media/CAudioFormatBuilder.h"

using Elastos::Droid::Media::CAudioFormatBuilder;

namespace Elastos {
namespace Droid {
namespace Media {
namespace AudioPolicy {

CAR_INTERFACE_IMPL(CAudioMixBuilder, Object, IAudioMixBuilder)

CAR_OBJECT_IMPL(CAudioMixBuilder)

CAudioMixBuilder::CAudioMixBuilder()
    : mRouteFlags(0)
{}

CAudioMixBuilder::~CAudioMixBuilder()
{
}

ECode CAudioMixBuilder::constructor()
{
    return NOERROR;
}

ECode CAudioMixBuilder::constructor(
    /* [in] */ IAudioMixingRule * rule)
{
    if (rule == NULL) {
        // throw new IllegalArgumentException("Illegal null AudioMixingRule argument");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mRule = rule;
    return NOERROR;
}

ECode CAudioMixBuilder::SetMixingRule(
    /* [in] */ IAudioMixingRule * rule,
    /* [out] */ IAudioMixBuilder** builder)
{
    VALIDATE_NOT_NULL(builder)
    if (rule == NULL) {
        // throw new IllegalArgumentException("Illegal null AudioMixingRule argument");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mRule = rule;
    *builder = this;
    REFCOUNT_ADD(*builder)
    return NOERROR;
}

ECode CAudioMixBuilder::SetFormat(
    /* [in] */ IAudioFormat * format,
    /* [out] */ IAudioMixBuilder** builder)
{
    VALIDATE_NOT_NULL(builder)
    if (format == NULL) {
        // throw new IllegalArgumentException("Illegal null AudioFormat argument");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mFormat = format;
    *builder = this;
    REFCOUNT_ADD(*builder)
    return NOERROR;
}

ECode CAudioMixBuilder::SetRouteFlags(
    /* [in] */ Int32 routeFlags,
    /* [out] */ IAudioMixBuilder** builder)
{
    VALIDATE_NOT_NULL(builder)
    if (routeFlags == 0) {
        // throw new IllegalArgumentException("Illegal empty route flags");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if ((routeFlags & (IAudioMix::ROUTE_FLAG_LOOP_BACK | IAudioMix::ROUTE_FLAG_RENDER)) == 0) {
        // throw new IllegalArgumentException("Invalid route flags 0x"
        //     + Integer.toHexString(routeFlags) + "when creating an AudioMix");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mRouteFlags = routeFlags;
    *builder = this;
    REFCOUNT_ADD(*builder)
    return NOERROR;
}

ECode CAudioMixBuilder::Build(
    /* [out] */ IAudioMix** result)
{
    VALIDATE_NOT_NULL(result)
    if (mRule == NULL) {
        // throw new IllegalArgumentException("Illegal null AudioMixingRule");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mRouteFlags == 0) {
        // no route flags set, use default
        mRouteFlags = IAudioMix::ROUTE_FLAG_RENDER;
    }
    if (mFormat == NULL) {
        Int32 rate;
        AudioSystem::GetPrimaryOutputSamplingRate(&rate);
        if (rate <= 0) {
            rate = 44100;
        }
        AutoPtr<IAudioFormatBuilder> builder;
        CAudioFormatBuilder::New((IAudioFormatBuilder**)&builder);
        builder->SetSampleRate(rate);
        builder->Build((IAudioFormat**)&mFormat);
    }
    return CAudioMix::New(mRule, mFormat, mRouteFlags, result);
}


} // namespace AudioPolicy
} // namespace Media
} // namepsace Droid
} // namespace Elastos
