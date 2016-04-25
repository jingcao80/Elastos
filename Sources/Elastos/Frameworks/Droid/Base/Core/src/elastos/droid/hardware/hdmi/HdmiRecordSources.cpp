
#include "elastos/droid/hardware/hdmi/HdmiRecordSources.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Hdmi {

CAR_INTERFACE_IMPL(HdmiRecordSources::RecordSource, Object, IRecordSource)

HdmiRecordSources::RecordSource::RecordSource()
{
}

ECode HdmiRecordSources::RecordSource::constructor(
    /* [in] */ Int32 sourceType,
    /* [in] */ Int32 extraDataSize)
{
    mSourceType = sourceType;
    mExtraDataSize = extraDataSize;
    return NOERROR;
}

ECode HdmiRecordSources::RecordSource::GetDataSize(
    /* [in] */ Boolean includeType,
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);

    *size = includeType ? mExtraDataSize + 1 : mExtraDataSize;
    return NOERROR;
}

ECode HdmiRecordSources::RecordSource::ToByteArray(
    /* [in] */ Boolean includeType,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 index,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    if (includeType) {
        // 1 to 8 bytes (depends on source).
        // {[Record Source Type]} |
        // {[Record Source Type] [Digital Service Identification]} |
        // {[Record Source Type] [Analogue Broadcast Type] [Analogue Frequency]
        // [Broadcast System]} |
        // {[Record Source Type] [External Plug]} |
        // {[Record Source Type] [External Physical Address]}
        // The first byte is used for record source type.
        (*data)[index++] = (Byte)mSourceType;
    }
    Int32 tmp;
    ExtraParamToByteArray(data, index, &tmp);
    return GetDataSize(includeType, result);
}

const Int32 HdmiRecordSources::OwnSource::EXTRA_DATA_SIZE = 0;

CAR_INTERFACE_IMPL(HdmiRecordSources::OwnSource, RecordSource, IHdmiRecordSourcesOwnSource)

ECode HdmiRecordSources::OwnSource::ExtraParamToByteArray(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 index,
    /* [out] */ Int32* array)
{
    VALIDATE_NOT_NULL(array);

    *array = 0;
    return NOERROR;
}

HdmiRecordSources::OwnSource::OwnSource()
{
    RecordSource::constructor(HdmiRecordSources::RECORD_SOURCE_TYPE_OWN_SOURCE,
            EXTRA_DATA_SIZE);
}

CAR_INTERFACE_IMPL_2(HdmiRecordSources::AribData, Object, IHdmiRecordSourcesAribData,
        IHdmiRecordSourcesDigitalServiceIdentification)

HdmiRecordSources::AribData::AribData(
    /* [in] */ Int32 transportStreamId,
    /* [in] */ Int32 serviceId,
    /* [in] */ Int32 originalNetworkId)
    : mTransportStreamId(transportStreamId)
    , mServiceId(serviceId)
    , mOriginalNetworkId(originalNetworkId)
{
}

ECode HdmiRecordSources::AribData::ToByteArray(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 index,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = HdmiRecordSources::ThreeFieldsToSixBytes(mTransportStreamId, mServiceId,
            mOriginalNetworkId, data, index);
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(HdmiRecordSources::AtscData, Object, IHdmiRecordSourcesAtscData,
        IHdmiRecordSourcesDigitalServiceIdentification)

HdmiRecordSources::AtscData::AtscData(
    /* [in] */ Int32 transportStreamId,
    /* [in] */ Int32 programNumber)
    : mTransportStreamId(transportStreamId)
    , mProgramNumber(programNumber)
{
}

ECode HdmiRecordSources::AtscData::ToByteArray(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 index,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = HdmiRecordSources::ThreeFieldsToSixBytes(mTransportStreamId,
            mProgramNumber, 0, data, index);
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(HdmiRecordSources::DvbData, Object, IHdmiRecordSourcesDvbData,
        IHdmiRecordSourcesDigitalServiceIdentification)

HdmiRecordSources::DvbData::DvbData(
    /* [in] */ Int32 transportStreamId,
    /* [in] */ Int32 serviceId,
    /* [in] */ Int32 originalNetworkId)
    : mTransportStreamId(transportStreamId)
    , mServiceId(serviceId)
    , mOriginalNetworkId(originalNetworkId)
{
}

ECode HdmiRecordSources::DvbData::ToByteArray(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 index,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = HdmiRecordSources::ThreeFieldsToSixBytes(mTransportStreamId, mServiceId,
            mOriginalNetworkId, data, index);
    return NOERROR;
}

HdmiRecordSources::ChannelIdentifier::ChannelIdentifier(
    /* [in] */ Int32 format,
    /* [in] */ Int32 majorNumber,
    /* [in] */ Int32 minorNumer)
    : mChannelNumberFormat(format)
    , mMajorChannelNumber(majorNumber)
    , mMinorChannelNumber(minorNumer)
{
}

ECode HdmiRecordSources::ChannelIdentifier::ToByteArray(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 index,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    // The first 6 bits for format, the 10 bits for major number.
    (*data)[index] = (Byte) ((mChannelNumberFormat << 2) | ((mMajorChannelNumber >> 8) & 0x3));
    (*data)[index + 1] = (Byte) (mMajorChannelNumber & 0xFF);
    // Minor number uses the next 16 bits.
    HdmiRecordSources::Int16ToByteArray((Int16) mMinorChannelNumber, data, index + 2);
    *result = 4;
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(HdmiRecordSources::DigitalChannelData, Object, IHdmiRecordSourcesDigitalChannelData,
        IHdmiRecordSourcesDigitalServiceIdentification)


HdmiRecordSources::DigitalChannelData::DigitalChannelData(
    /* [in] */ ChannelIdentifier* id)
    : mChannelIdentifier(id)
{
}

ECode HdmiRecordSources::DigitalChannelData::OfTwoNumbers(
    /* [in] */ Int32 majorNumber,
    /* [in] */ Int32 minorNumber,
    /* [out] */ IHdmiRecordSourcesDigitalChannelData** data)
{
    VALIDATE_NOT_NULL(data);

    AutoPtr<ChannelIdentifier> iden = new ChannelIdentifier(CHANNEL_NUMBER_FORMAT_2_PART,
            majorNumber, minorNumber);
    AutoPtr<IHdmiRecordSourcesDigitalChannelData> res = new DigitalChannelData(iden);
    *data = res;
    REFCOUNT_ADD(*data);
    return NOERROR;
}

ECode HdmiRecordSources::DigitalChannelData::OfOneNumber(
    /* [in] */ Int32 number,
    /* [out] */ IHdmiRecordSourcesDigitalChannelData** data)
{
    VALIDATE_NOT_NULL(data);

    AutoPtr<ChannelIdentifier> iden = new ChannelIdentifier(CHANNEL_NUMBER_FORMAT_1_PART,
            0, number);
    AutoPtr<IHdmiRecordSourcesDigitalChannelData> res =  new DigitalChannelData(iden);
    *data = res;
    REFCOUNT_ADD(*data);
    return NOERROR;
}

ECode HdmiRecordSources::DigitalChannelData::ToByteArray(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 index,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 res;
    mChannelIdentifier->ToByteArray(data, index, &res);
    // The last 2 bytes is reserved for future use.
    (*data)[index + 4] = 0;
    (*data)[index + 5] = 0;
    *result = 6;
    return NOERROR;
}

const Int32 HdmiRecordSources::DigitalServiceSource::DIGITAL_SERVICE_IDENTIFIED_BY_DIGITAL_ID = 0;
const Int32 HdmiRecordSources::DigitalServiceSource::DIGITAL_SERVICE_IDENTIFIED_BY_CHANNEL = 1;

CAR_INTERFACE_IMPL(HdmiRecordSources::DigitalServiceSource, RecordSource,
        IHdmiRecordSourcesDigitalServiceSource)

HdmiRecordSources::DigitalServiceSource::DigitalServiceSource(
    /* [in] */ Int32 identificatinoMethod,
    /* [in] */ Int32 broadcastSystem,
    /* [in] */ IHdmiRecordSourcesDigitalServiceIdentification* identification)
    : mIdentificationMethod(identificatinoMethod)
    , mBroadcastSystem(broadcastSystem)
    , mIdentification(identification)
{
    RecordSource::constructor(RECORD_SOURCE_TYPE_DIGITAL_SERVICE, EXTRA_DATA_SIZE);
}

ECode HdmiRecordSources::DigitalServiceSource::ExtraParamToByteArray(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 index,
    /* [out] */ Int32* array)
{
    VALIDATE_NOT_NULL(array);

    (*data)[index] = (Byte)((mIdentificationMethod << 7) | (mBroadcastSystem & 0x7F));
    Int32 res;
    mIdentification->ToByteArray(data, index + 1, &res);
    *array = EXTRA_DATA_SIZE;
    return NOERROR;
}

CAR_INTERFACE_IMPL(HdmiRecordSources::AnalogueServiceSource, RecordSource,
        IHdmiRecordSourcesAnalogueServiceSource)

HdmiRecordSources::AnalogueServiceSource::AnalogueServiceSource(
    /* [in] */ Int32 broadcastType,
    /* [in] */ Int32 frequency,
    /* [in] */ Int32 broadcastSystem)
    : mBroadcastType(broadcastType)
    , mFrequency(frequency)
    , mBroadcastSystem(broadcastSystem)
{
    RecordSource::constructor(HdmiRecordSources::RECORD_SOURCE_TYPE_ANALOGUE_SERVICE,
            EXTRA_DATA_SIZE);
}

ECode HdmiRecordSources::AnalogueServiceSource::ExtraParamToByteArray(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 index,
    /* [out] */ Int32* array)
{
    VALIDATE_NOT_NULL(array);

    // [Analogue Broadcast Type] - 1 byte
    (*data)[index] = (Byte)mBroadcastType;
    // [Analogue Frequency] - 2 bytes
    Int16ToByteArray((Int16)mFrequency, data, index + 1);
    // [Broadcast System] - 1 byte
    (*data)[index + 3] = (Byte) mBroadcastSystem;
    *array = EXTRA_DATA_SIZE;
    return NOERROR;
}

CAR_INTERFACE_IMPL(HdmiRecordSources::ExternalPlugData, RecordSource,
        IHdmiRecordSourcesExternalPlugData)

HdmiRecordSources::ExternalPlugData::ExternalPlugData(
    /* [in] */ Int32 plugNumber)
    : mPlugNumber(plugNumber)
{
    RecordSource::constructor(HdmiRecordSources::RECORD_SOURCE_TYPE_EXTERNAL_PLUG,
        EXTRA_DATA_SIZE);
}

HdmiRecordSources::ExternalPlugData::ExtraParamToByteArray(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 index,
    /* [out] */ Int32* array)
{
    VALIDATE_NOT_NULL(array);

    (*data)[index] = (Byte)mPlugNumber;
    *array = EXTRA_DATA_SIZE;
    return NOERROR;
}

CAR_INTERFACE_IMPL(HdmiRecordSources::ExternalPhysicalAddress, RecordSource,
        IHdmiRecordSourcesExternalPhysicalAddress)

HdmiRecordSources::ExternalPhysicalAddress::ExternalPhysicalAddress(
    /* [in] */ Int32 physicalAddress)
    : mPhysicalAddress(physicalAddress)
{
    RecordSource::constructor(HdmiRecordSources::RECORD_SOURCE_TYPE_EXTERNAL_PHYSICAL_ADDRESS,
            EXTRA_DATA_SIZE);
}

ECode HdmiRecordSources::ExternalPhysicalAddress::ExtraParamToByteArray(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 index,
    /* [out] */ Int32* array)
{
    VALIDATE_NOT_NULL(array);

    Int16ToByteArray((Int16)mPhysicalAddress, data, index);
    *array = EXTRA_DATA_SIZE;
    return NOERROR;
}

const String HdmiRecordSources::TAG("HdmiRecordSources");

/** Record source type for "Own Source". */
const Int32 HdmiRecordSources::RECORD_SOURCE_TYPE_OWN_SOURCE;
/** Record source type for "Digital Service". */
const Int32 HdmiRecordSources::RECORD_SOURCE_TYPE_DIGITAL_SERVICE;
/** Record source type for "Analogue Service". */
const Int32 HdmiRecordSources::RECORD_SOURCE_TYPE_ANALOGUE_SERVICE;
/** Record source type for "Exteranl Plug". */
const Int32 HdmiRecordSources::RECORD_SOURCE_TYPE_EXTERNAL_PLUG;
/** Record source type for "External Physical Address". */
const Int32 HdmiRecordSources::RECORD_SOURCE_TYPE_EXTERNAL_PHYSICAL_ADDRESS;

/** Channel number formats. */
static const Int32 CHANNEL_NUMBER_FORMAT_1_PART = 0x01;
static const Int32 CHANNEL_NUMBER_FORMAT_2_PART = 0x02;

CAR_INTERFACE_IMPL(HdmiRecordSources, Object, IHdmiRecordSources)

ECode HdmiRecordSources::OfOwnSource(
    /* [out] */ IHdmiRecordSourcesOwnSource** source)
{
    VALIDATE_NOT_NULL(source)

    AutoPtr<IHdmiRecordSourcesOwnSource> sour = new OwnSource();
    *source = sour;
    REFCOUNT_ADD(*source);
    return NOERROR;
}

ECode HdmiRecordSources::OfDigitalChannelId(
    /* [in] */ Int32 broadcastSystem,
    /* [in] */ IHdmiRecordSourcesDigitalChannelData* data,
    /* [out] */ IHdmiRecordSourcesDigitalServiceSource** source)
{
    VALIDATE_NOT_NULL(source);
    *source = NULL;

    if (data == NULL) {
        //throw new IllegalArgumentException("data should not be null.");
        Slogger::E(TAG, "data should not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    switch (broadcastSystem) {
        case IHdmiRecordSources::DIGITAL_BROADCAST_TYPE_ARIB:
        case IHdmiRecordSources::DIGITAL_BROADCAST_TYPE_ATSC:
        case IHdmiRecordSources::DIGITAL_BROADCAST_TYPE_DVB:
        case IHdmiRecordSources::DIGITAL_BROADCAST_TYPE_ARIB_BS:
        case IHdmiRecordSources::DIGITAL_BROADCAST_TYPE_ARIB_CS:
        case IHdmiRecordSources::DIGITAL_BROADCAST_TYPE_ARIB_T:
        case IHdmiRecordSources::DIGITAL_BROADCAST_TYPE_ATSC_CABLE:
        case IHdmiRecordSources::DIGITAL_BROADCAST_TYPE_ATSC_SATELLITE:
        case IHdmiRecordSources::DIGITAL_BROADCAST_TYPE_ATSC_TERRESTRIAL:
        case IHdmiRecordSources::DIGITAL_BROADCAST_TYPE_DVB_C:
        case IHdmiRecordSources::DIGITAL_BROADCAST_TYPE_DVB_S:
        case IHdmiRecordSources::DIGITAL_BROADCAST_TYPE_DVB_S2:
        case IHdmiRecordSources::DIGITAL_BROADCAST_TYPE_DVB_T:
        {
            AutoPtr<IHdmiRecordSourcesDigitalServiceSource> res = new DigitalServiceSource(
                    DigitalServiceSource::DIGITAL_SERVICE_IDENTIFIED_BY_CHANNEL,
                    broadcastSystem, IHdmiRecordSourcesDigitalServiceIdentification::Probe(data));
            *source = res;
            REFCOUNT_ADD(*source);
            return NOERROR;
        }
        default:
        {
            Slogger::W(TAG, "Invalid broadcast type:%d", broadcastSystem);
            // throw new IllegalArgumentException(
            //         "Invalid broadcast system value:%d", broadcastSystem);
            Slogger::E(TAG, "Invalid broadcast system value:%d", broadcastSystem);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode HdmiRecordSources::OfArib(
    /* [in] */ Int32 aribType,
    /* [in] */ IHdmiRecordSourcesAribData* data,
    /* [out] */ IHdmiRecordSourcesDigitalServiceSource** source)
{
    VALIDATE_NOT_NULL(source);
    *source = NULL;

    if (data == NULL) {
        //throw new IllegalArgumentException("data should not be null.");
        Slogger::E(TAG, "data should not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    switch (aribType) {
        case IHdmiRecordSources::DIGITAL_BROADCAST_TYPE_ARIB:
        case IHdmiRecordSources::DIGITAL_BROADCAST_TYPE_ARIB_BS:
        case IHdmiRecordSources::DIGITAL_BROADCAST_TYPE_ARIB_CS:
        case IHdmiRecordSources::DIGITAL_BROADCAST_TYPE_ARIB_T:
        {
            AutoPtr<IHdmiRecordSourcesDigitalServiceSource> res = new DigitalServiceSource(
                    DigitalServiceSource::DIGITAL_SERVICE_IDENTIFIED_BY_DIGITAL_ID,
                    aribType, IHdmiRecordSourcesDigitalServiceIdentification::Probe(data));
            *source = res;
            REFCOUNT_ADD(*source);
            return NOERROR;
        }
        default:
        {
            Slogger::W(TAG, "Invalid ARIB type:%d", aribType);
            //throw new IllegalArgumentException("type should not be null.");
            Slogger::E(TAG, "type should not be null.");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode HdmiRecordSources::OfAtsc(
    /* [in] */ Int32 atscType,
    /* [in] */ IHdmiRecordSourcesAtscData* data,
    /* [out] */ IHdmiRecordSourcesDigitalServiceSource** source)
{
    VALIDATE_NOT_NULL(source);
    *source = NULL;

    if (data == NULL) {
        //throw new IllegalArgumentException("data should not be null.");
        Slogger::E(TAG, "data should not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    switch (atscType) {
        case IHdmiRecordSources::DIGITAL_BROADCAST_TYPE_ATSC:
        case IHdmiRecordSources::DIGITAL_BROADCAST_TYPE_ATSC_CABLE:
        case IHdmiRecordSources::DIGITAL_BROADCAST_TYPE_ATSC_SATELLITE:
        case IHdmiRecordSources::DIGITAL_BROADCAST_TYPE_ATSC_TERRESTRIAL:
        {
            AutoPtr<IHdmiRecordSourcesDigitalServiceSource> res = new DigitalServiceSource(
                    DigitalServiceSource::DIGITAL_SERVICE_IDENTIFIED_BY_DIGITAL_ID,
                    atscType, IHdmiRecordSourcesDigitalServiceIdentification::Probe(data));
            *source = res;
            REFCOUNT_ADD(*source);
            return NOERROR;
        }
        default:
        {
            Slogger::W(TAG, "Invalid ATSC type:%d", atscType);
            //throw new IllegalArgumentException("Invalid ATSC type:" + atscType);
            Slogger::E(TAG, "Invalid ATSC type:%d", atscType);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode HdmiRecordSources::OfDvb(
    /* [in] */ Int32 dvbType,
    /* [in] */ IHdmiRecordSourcesDvbData* data,
    /* [out] */ IHdmiRecordSourcesDigitalServiceSource** source)
{
    VALIDATE_NOT_NULL(source);
    *source = NULL;

    if (data == NULL) {
        //throw new IllegalArgumentException("data should not be null.");
        Slogger::E(TAG, "data should not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    switch (dvbType) {
        case IHdmiRecordSources::DIGITAL_BROADCAST_TYPE_DVB:
        case IHdmiRecordSources::DIGITAL_BROADCAST_TYPE_DVB_C:
        case IHdmiRecordSources::DIGITAL_BROADCAST_TYPE_DVB_S:
        case IHdmiRecordSources::DIGITAL_BROADCAST_TYPE_DVB_S2:
        case IHdmiRecordSources::DIGITAL_BROADCAST_TYPE_DVB_T:
        {
            AutoPtr<IHdmiRecordSourcesDigitalServiceSource> res = new DigitalServiceSource(
                    DigitalServiceSource::DIGITAL_SERVICE_IDENTIFIED_BY_DIGITAL_ID,
                    dvbType, IHdmiRecordSourcesDigitalServiceIdentification::Probe(data));
            *source = res;
            REFCOUNT_ADD(*source);
            return NOERROR;
        }
        default:
        {
            Slogger::W(TAG, "Invalid DVB type:%d", dvbType);
            //throw new IllegalArgumentException("Invalid DVB type:" + dvbType);
            Slogger::E(TAG, "Invalid ATSC type:%d", dvbType);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode HdmiRecordSources::OfAnalogue(
    /* [in] */ Int32 broadcastType,
    /* [in] */ Int32 frequency,
    /* [in] */ Int32 broadcastSystem,
    /* [out] */ IHdmiRecordSourcesAnalogueServiceSource** source)
{
    VALIDATE_NOT_NULL(source);
    *source = NULL;

    if (broadcastType < ANALOGUE_BROADCAST_TYPE_CABLE
            || broadcastType > ANALOGUE_BROADCAST_TYPE_TERRESTRIAL) {
        Slogger::W(TAG, "Invalid Broadcast type:%d", broadcastType);
        //throw new IllegalArgumentException("Invalid Broadcast type:" + broadcastType);
        Slogger::E(TAG, "Invalid Broadcast type:%d",broadcastType);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (frequency < 0 || frequency > 0xFFFF) {
        Slogger::W(TAG, "Invalid frequency value[0x0000-0xFFFF]:%d", frequency);
        // throw new IllegalArgumentException(
        //         "Invalid frequency value[0x0000-0xFFFF]:" + frequency);
        Slogger::E(TAG, "Invalid frequency value[0x0000-0xFFFF]:%d" + frequency);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (broadcastSystem < BROADCAST_SYSTEM_PAL_BG
            || broadcastSystem > BROADCAST_SYSTEM_PAL_OTHER_SYSTEM) {

        Slogger::W(TAG, "Invalid Broadcast system:%d", broadcastSystem);
        // throw new IllegalArgumentException(
        //         "Invalid Broadcast system:" + broadcastSystem);
        Slogger::E(TAG, "Invalid Broadcast system:%d", broadcastSystem);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IHdmiRecordSourcesAnalogueServiceSource> res =
            new AnalogueServiceSource(broadcastType, frequency, broadcastSystem);
    *source = res;
    REFCOUNT_ADD(*source);
    return NOERROR;
}

ECode HdmiRecordSources::OfExternalPlug(
    /* [in] */ Int32 plugNumber,
    /* [out] */ IHdmiRecordSourcesExternalPlugData** data)
{
    VALIDATE_NOT_NULL(data);
    *data = NULL;

    if (plugNumber < 1 || plugNumber > 255) {
        Slogger::W(TAG, "Invalid plug number[1-255] %d", plugNumber);
        //throw new IllegalArgumentException("Invalid plug number[1-255]" + plugNumber);
        Slogger::E(TAG, "Invalid plug number[1-255] %d" + plugNumber);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IHdmiRecordSourcesExternalPlugData> res = new ExternalPlugData(plugNumber);
    *data = res;
    REFCOUNT_ADD(*data);
    return NOERROR;
}

ECode HdmiRecordSources::OfExternalPhysicalAddress(
    /* [in] */ Int32 physicalAddress,
    /* [out] */ IHdmiRecordSourcesExternalPhysicalAddress** address)
{
    VALIDATE_NOT_NULL(address);
    *address = NULL;

    if ((physicalAddress & ~0xFFFF) != 0) {
        Slogger::W(TAG, "Invalid physical address: %d" + physicalAddress);
        //throw new IllegalArgumentException("Invalid physical address:" + physicalAddress);
        Slogger::E(TAG, "Invalid physical address: %d" + physicalAddress);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IHdmiRecordSourcesExternalPhysicalAddress> res = new ExternalPhysicalAddress(physicalAddress);
    *address = res;
    REFCOUNT_ADD(*address);
    return NOERROR;
}

Int32 HdmiRecordSources::ThreeFieldsToSixBytes(
    /* [in] */ Int32 first,
    /* [in] */ Int32 second,
    /* [in] */ Int32 third,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 index)
{
    Int16ToByteArray((Int16)first, data, index);
    Int16ToByteArray((Int16)second, data, index + 2);
    Int16ToByteArray((Int16)third, data, index + 4);
    return 6;
}

Int32 HdmiRecordSources::Int16ToByteArray(
    /* [in] */ Int16 value,
    /* [in] */ ArrayOf<Byte>* byteArray,
    /* [in] */ Int32 index)
{
    (*byteArray)[index] = (Byte)((value >> 8) & 0xFF);
    (*byteArray)[index + 1] = (Byte)(value & 0xFF);
    return 2;
}

ECode HdmiRecordSources::CheckRecordSource(
    /* [in] */ ArrayOf<Byte>* recordSource,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    Int32 recordSourceType = (*recordSource)[0];
    Int32 extraDataSize = recordSource->GetLength() - 1;
    switch (recordSourceType) {
        case HdmiRecordSources::RECORD_SOURCE_TYPE_OWN_SOURCE:
            *result = extraDataSize == OwnSource::EXTRA_DATA_SIZE;
            return NOERROR;
        case HdmiRecordSources::RECORD_SOURCE_TYPE_DIGITAL_SERVICE:
            *result = extraDataSize == DigitalServiceSource::EXTRA_DATA_SIZE;
            return NOERROR;
        case HdmiRecordSources::RECORD_SOURCE_TYPE_ANALOGUE_SERVICE:
            *result = extraDataSize == AnalogueServiceSource::EXTRA_DATA_SIZE;
            return NOERROR;
        case HdmiRecordSources::RECORD_SOURCE_TYPE_EXTERNAL_PLUG:
            *result = extraDataSize == ExternalPlugData::EXTRA_DATA_SIZE;
            return NOERROR;
        case HdmiRecordSources::RECORD_SOURCE_TYPE_EXTERNAL_PHYSICAL_ADDRESS:
            *result = extraDataSize == ExternalPhysicalAddress::EXTRA_DATA_SIZE;
            return NOERROR;
        default:
            *result = FALSE;
            return NOERROR;
    }
    return NOERROR;
}

} // namespace Hdmi
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos
