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

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/CWindowAnimationFrameStats.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace View {

CAR_OBJECT_IMPL(CWindowAnimationFrameStats);
CAR_INTERFACE_IMPL_2(CWindowAnimationFrameStats, FrameStats, IWindowAnimationFrameStats, IParcelable);
CWindowAnimationFrameStats::CWindowAnimationFrameStats()
{
    /* do nothing */
}

ECode CWindowAnimationFrameStats::constructor()
{
    return NOERROR;
}

void CWindowAnimationFrameStats::Init(
    /* [in] */ Int64 refreshPeriodNano,
    /* [in] */ ArrayOf<Int64>* framesPresentedTimeNano)
{
    mRefreshPeriodNano = refreshPeriodNano;
    mFramesPresentedTimeNano = framesPresentedTimeNano;
}

ECode CWindowAnimationFrameStats::WriteToParcel(
    /* [in] */ IParcel* parcel)
{
    parcel->WriteInt64(mRefreshPeriodNano);
    parcel->WriteArrayOf((HANDLE)mFramesPresentedTimeNano.Get());
    return NOERROR;
}

ECode CWindowAnimationFrameStats::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt64(&mRefreshPeriodNano);
    source->ReadArrayOf((HANDLE*)(&mFramesPresentedTimeNano));
    return NOERROR;
}

ECode CWindowAnimationFrameStats::ToString(
    /* [out] */ String* str)
{
    StringBuilder builder;
    builder.Append("CWindowAnimationFrameStats[");
    Int32 count = 0;
    GetFrameCount(&count);
    builder.Append("frameCount:");
    builder.Append(count);
    Int64 value = 0;
    GetStartTimeNano(&value);
    builder.Append(", fromTimeNano:");
    builder.Append(value);
    builder.Append(", toTimeNano:");
    GetEndTimeNano(&value);
    builder.Append(value);
    builder.AppendChar(']');
    return builder.ToString(str);
}

} // namespace View
} // namepsace Droid
} // namespace Elastos
