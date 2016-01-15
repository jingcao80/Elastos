
#include "elastos/droid/wifi/CScanSettings.h"

using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL_2(CScanSettings, Object, IParcelable, IScanSettings)

CAR_OBJECT_IMPL(CScanSettings)

ECode CScanSettings::constructor()
{
    return NOERROR;
}

ECode CScanSettings::constructor(
    /* [in] */ IScanSettings* source)
{
    AutoPtr<ICollection> channelSet;
    source->GetChannelSet((ICollection**)&channelSet);
    if (channelSet != NULL) {
        CArrayList::New(channelSet, (ICollection**)&mChannelSet);
    }

    return NOERROR;
}

ECode CScanSettings::GetChannelSet(
    /* [out] */ ICollection** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mChannelSet;
    return NOERROR;
}

ECode CScanSettings::SetChannelSet(
    /* [in] */ ICollection* channelSet)
{
    mChannelSet = channelSet;
    return NOERROR;
}

/** check for validity */
ECode CScanSettings::IsValid(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IIterator> iter;
    mChannelSet->GetIterator((IIterator**)&iter);
    Boolean bNext;
    iter->HasNext(&bNext);
    for (; bNext; iter->HasNext(&bNext)) {
        AutoPtr<IWifiChannel> channel;
        iter->GetNext((IInterface**)&channel);
        Boolean bFlag;
        channel->IsValid(&bFlag);
        if (!bFlag) {
            *result = FALSE;
            return NOERROR;
        }
    }

    *result = TRUE;

    return NOERROR;
}

/** implement Parcelable interface */
//@Override
ECode CScanSettings::DescribeContents(
    /* [out] */ Int32* contents)
{
    VALIDATE_NOT_NULL(contents);
    *contents = 0;
    return NOERROR;
}

/** implement Parcelable interface */
//@Override
ECode CScanSettings::WriteToParcel(
    /* [in] */ IParcel* out,
    /* [in] */ Int32 flags)
{
    Int32 size;
    out->WriteInt32(mChannelSet == NULL ? 0 : (mChannelSet->GetSize(&size), size));
    if (mChannelSet != NULL) {
        AutoPtr<IIterator> iter;
        mChannelSet->GetIterator((IIterator**)&iter);
        Boolean bNext;
        iter->HasNext(&bNext);
        for (; bNext; iter->HasNext(&bNext)) {
            AutoPtr<IWifiChannel> channel;
            iter->GetNext((IInterface**)&channel);
            assert(0);
            // TODO
            // channel->WriteToParcel(out, flags);
        }
    }

    return NOERROR;
}

ECode CScanSettings::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CScanSettings::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
