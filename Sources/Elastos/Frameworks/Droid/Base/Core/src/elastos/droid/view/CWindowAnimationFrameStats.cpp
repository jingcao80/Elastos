
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
    parcel->WriteArrayOf((Handle32)mFramesPresentedTimeNano.Get());
    return NOERROR;
}

ECode CWindowAnimationFrameStats::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt64(&mRefreshPeriodNano);
    source->ReadArrayOf((Handle32*)(&mFramesPresentedTimeNano));
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
