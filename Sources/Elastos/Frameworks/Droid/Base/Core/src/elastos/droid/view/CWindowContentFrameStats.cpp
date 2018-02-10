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
#include "elastos/droid/view/CWindowContentFrameStats.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace View {

CAR_OBJECT_IMPL(CWindowContentFrameStats);
CAR_INTERFACE_IMPL_2(CWindowContentFrameStats, FrameStats, IWindowContentFrameStats, IParcelable);
CWindowContentFrameStats::CWindowContentFrameStats()
{
    /* do nothing */
}

ECode CWindowContentFrameStats::constructor()
{
    return NOERROR;
}

void CWindowContentFrameStats::Init(
    /* [in] */ Int64 refreshPeriodNano,
    /* [in] */ ArrayOf<Int64>* framesPostedTimeNano,
    /* [in] */ ArrayOf<Int64>* framesPresentedTimeNano,
    /* [in] */ ArrayOf<Int64>* framesReadyTimeNano)
{
    mRefreshPeriodNano = refreshPeriodNano;
    mFramesPostedTimeNano = framesPostedTimeNano;
    mFramesPresentedTimeNano = framesPresentedTimeNano;
    mFramesReadyTimeNano = framesReadyTimeNano;
}

ECode CWindowContentFrameStats::GetFramePostedTimeNano(
    /* [in] */ Int32 index,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value);
    if (mFramesPostedTimeNano == NULL) {
        *value = 0;
        // throw new IndexOutOfBoundsException();
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    *value = (*mFramesPostedTimeNano)[index];
    return NOERROR;
}

ECode CWindowContentFrameStats::GetFrameReadyTimeNano(
    /* [in] */ Int32 index,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value);
    if (mFramesReadyTimeNano == NULL) {
        *value = 0;
        // throw new IndexOutOfBoundsException();
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    *value = (*mFramesReadyTimeNano)[index];
    return NOERROR;
}

ECode CWindowContentFrameStats::WriteToParcel(
    /* [in] */ IParcel* parcel)
{
    parcel->WriteInt64(mRefreshPeriodNano);
    parcel->WriteArrayOf((HANDLE)mFramesPostedTimeNano.Get());
    parcel->WriteArrayOf((HANDLE)mFramesPresentedTimeNano.Get());
    parcel->WriteArrayOf((HANDLE)mFramesReadyTimeNano.Get());
    return NOERROR;
}

ECode CWindowContentFrameStats::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt64(&mRefreshPeriodNano);
    source->ReadArrayOf((HANDLE*)&mFramesPostedTimeNano);
    source->ReadArrayOf((HANDLE*)&mFramesPresentedTimeNano);
    source->ReadArrayOf((HANDLE*)&mFramesReadyTimeNano);

    return NOERROR;
}

ECode CWindowContentFrameStats::ToString(
    /* [out] */ String* str)
{
    StringBuilder builder;
    builder.Append("WindowContentFrameStats[");
    Int32 count = 0;
    GetFrameCount(&count);
    builder.Append("frameCount:");
    builder.Append(count);
    builder.Append(", fromTimeNano:");
    Int64 value = 0;
    GetStartTimeNano(&value);
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
