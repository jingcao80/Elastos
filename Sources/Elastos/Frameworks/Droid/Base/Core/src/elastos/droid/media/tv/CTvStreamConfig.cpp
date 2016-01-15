#include "elastos/droid/media/tv/CTvStreamConfig.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

String CTvStreamConfig::TAG("TvStreamConfig");

CAR_INTERFACE_IMPL_2(CTvStreamConfig, Object, ITvStreamConfig, IParcelable)

CAR_OBJECT_IMPL(CTvStreamConfig)

CTvStreamConfig::CTvStreamConfig()
    : mStreamId(0)
    , mType(0)
    , mMaxWidth(0)
    , mMaxHeight(0)
    , mGeneration(0)
{
}

CTvStreamConfig::~CTvStreamConfig()
{
}

ECode CTvStreamConfig::constructor()
{
    return NOERROR;
}

ECode CTvStreamConfig::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mStreamId);
    source->ReadInt32(&mType);
    source->ReadInt32(&mMaxWidth);
    source->ReadInt32(&mMaxHeight);
    source->ReadInt32(&mGeneration);
    return NOERROR;
}

ECode CTvStreamConfig::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mStreamId);
    dest->WriteInt32(mType);
    dest->WriteInt32(mMaxWidth);
    dest->WriteInt32(mMaxHeight);
    dest->WriteInt32(mGeneration);
    return NOERROR;
}

ECode CTvStreamConfig::GetStreamId(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mStreamId;
    return NOERROR;
}

ECode CTvStreamConfig::GetType(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mType;
    return NOERROR;
}

ECode CTvStreamConfig::GetMaxWidth(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMaxWidth;
    return NOERROR;
}

ECode CTvStreamConfig::GetMaxHeight(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMaxHeight;
    return NOERROR;
}

ECode CTvStreamConfig::GetGeneration(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mGeneration;
    return NOERROR;
}

ECode CTvStreamConfig::ToString(
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result)

    StringBuilder b;
    b.Append("TvStreamConfig {");
    b.Append("mStreamId=");
    b.Append(mStreamId);
    b.Append(";");
    b.Append("mType=");
    b.Append(mType);
    b.Append(";");
    b.Append("mGeneration=");
    b.Append(mGeneration);
    b.Append("}");
    return b.ToString(result);
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
