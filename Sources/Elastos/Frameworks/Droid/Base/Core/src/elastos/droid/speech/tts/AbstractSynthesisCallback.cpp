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

#include "elastos/droid/speech/tts/AbstractSynthesisCallback.h"
#include "elastos/droid/speech/tts/TextToSpeech.h"

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

/******************
 * AbstractSynthesisCallback
 *******************************************************************************************************/

CAR_INTERFACE_IMPL(AbstractSynthesisCallback, Object, ISynthesisCallback);

AbstractSynthesisCallback::AbstractSynthesisCallback()
{}

AbstractSynthesisCallback::~AbstractSynthesisCallback()
{}

ECode AbstractSynthesisCallback::constructor()
{
    return NOERROR;
}

ECode AbstractSynthesisCallback::constructor(
    /* [in] */ Boolean clientIsUsingV2)
{
    mClientIsUsingV2 = clientIsUsingV2;

    return NOERROR;
}

ECode AbstractSynthesisCallback::ErrorCodeOnStop(
    /* [out] */ Int32* ret)
{
    *ret = mClientIsUsingV2 ? TextToSpeech::STOPPED : TextToSpeech::TTS_ERROR;

    return NOERROR;
}

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos
