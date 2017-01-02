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

#include "elastos/droid/gesture/Prediction.h"

namespace Elastos {
namespace Droid {
namespace Gesture {

CAR_INTERFACE_IMPL(Prediction, Object, IPrediction);

Prediction::Prediction()
{}

Prediction::~Prediction()
{}

ECode Prediction::constructor(
    /* [in] */ const String& label,
    /* [in] */ Double predictionScore)
{
    mName = label;
    mScore = predictionScore;
    return NOERROR;
}

ECode Prediction::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    *name = mName;
    return NOERROR;
}

ECode Prediction::GetScore(
    /* [out] */ Double* score)
{
    VALIDATE_NOT_NULL(score);

    *score = mScore;
    return NOERROR;
}

} // namespace Gesture
} // namespace Droid
} // namespace Elastos
