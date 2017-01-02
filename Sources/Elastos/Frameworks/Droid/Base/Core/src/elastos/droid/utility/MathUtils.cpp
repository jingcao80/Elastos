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

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/utility/MathUtils.h"

using Elastos::Utility::CRandom;

namespace Elastos {
namespace Droid {
namespace Utility {

static AutoPtr<IRandom> InitRandom()
{
    AutoPtr<IRandom> random;
    CRandom::New((IRandom**)&random);
    return random;
}

const AutoPtr<IRandom> MathUtils::sRandom = InitRandom();
const Float MathUtils::DEG_TO_RAD = 3.1415926f / 180.0f;
const Float MathUtils::RAD_TO_DEG = 180.0f / 3.1415926f;

Int32 MathUtils::Random(Int32 howbig)
{
    Float nf;
    sRandom->NextFloat(&nf);
    return (Int32) (nf * howbig);
}

Int32 MathUtils::Random(Int32 howsmall, Int32 howbig)
{
    if (howsmall >= howbig) return howsmall;
    Float nf;
    sRandom->NextFloat(&nf);
    return (Int32) (nf * (howbig - howsmall) + howsmall);
}

Float MathUtils::Random(Float howbig)
{
    Float nf;
    sRandom->NextFloat(&nf);
    return nf * howbig;
}

Float MathUtils::Random(Float howsmall, Float howbig)
{
    if (howsmall >= howbig) return howsmall;
    Float nf;
    sRandom->NextFloat(&nf);
    return nf * (howbig - howsmall) + howsmall;
}

void MathUtils::RandomSeed(Int64 seed)
{
    sRandom->SetSeed(seed);
}

} // namespace Utility
} // namepsace Droid
} // namespace Elastos
