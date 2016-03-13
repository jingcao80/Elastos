#include "elastos/droid/internal/telephony/CSmsRawData.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL_2(CSmsRawData, Object, ISmsRawData, IParcelable)

CAR_OBJECT_IMPL(CSmsRawData)

CSmsRawData::CSmsRawData()
{
}

CSmsRawData::~CSmsRawData()
{
}

ECode CSmsRawData::constructor()
{
    return NOERROR;
}

ECode CSmsRawData::constructor(
    /* [in] */ ArrayOf<Byte>* data)
{
    mData = data;
    return NOERROR;
}

ECode CSmsRawData::GetBytes(
    /* [out, callee] */ ArrayOf<Byte>** bytes)
{
    VALIDATE_NOT_NULL(bytes)
    *bytes = mData;
    REFCOUNT_ADD(*bytes)
    return NOERROR;
}

ECode CSmsRawData::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadArrayOf((Handle32*)(&mData));
    return NOERROR;
}

ECode CSmsRawData::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteArrayOf((Handle32)mData.Get());
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos