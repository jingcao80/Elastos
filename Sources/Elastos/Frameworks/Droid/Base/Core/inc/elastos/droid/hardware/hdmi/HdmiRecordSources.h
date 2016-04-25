
#ifndef __ELASTOS_DROID_HARDWARE_HDMI_HDMIRECORDSOURCES_H__
#define __ELASTOS_DROID_HARDWARE_HDMI_HDMIRECORDSOURCES_H__

#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Hdmi {

class HdmiRecordSources
    : public Object
    , public IHdmiRecordSources
{
private:
    class ChannelIdentifier;

public:
    class RecordSource
        : public Object
        , public IRecordSource
    {
    public:
        CAR_INTERFACE_DECL()

        RecordSource();

        CARAPI constructor(
            /* [in] */ Int32 sourceType,
            /* [in] */ Int32 extraDataSize);

        virtual ~RecordSource() {}

        virtual CARAPI ExtraParamToByteArray(
            /* [in] */ ArrayOf<Byte>* data,
            /* [in] */ Int32 index,
            /* [out] */ Int32* array) = 0;

        CARAPI GetDataSize(
            /* [in] */ Boolean includeType,
            /* [out] */ Int32* size);

        CARAPI ToByteArray(
            /* [in] */ Boolean includeType,
            /* [in] */ ArrayOf<Byte>* data,
            /* [in] */ Int32 index,
            /* [out] */ Int32* result);

    public:
        Int32 mSourceType;
        Int32 mExtraDataSize;
    };

    class OwnSource
        : public RecordSource
        , public IHdmiRecordSourcesOwnSource
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI ExtraParamToByteArray(
            /* [in] */ ArrayOf<Byte>* data,
            /* [in] */ Int32 index,
            /* [out] */ Int32* array);

    private:
        friend class HdmiRecordSources;
        OwnSource();

    private:
        static const Int32 EXTRA_DATA_SIZE;
    };

    class AribData
        : public Object
        , public IHdmiRecordSourcesAribData
        , public IHdmiRecordSourcesDigitalServiceIdentification
    {
    public:
        CAR_INTERFACE_DECL()

        AribData(
            /* [in] */ Int32 transportStreamId,
            /* [in] */ Int32 serviceId,
            /* [in] */ Int32 originalNetworkId);

        CARAPI ToByteArray(
            /* [in] */ ArrayOf<Byte>* data,
            /* [in] */ Int32 index,
            /* [out] */ Int32* result);

    private:
        /** The transport_stream_ID of the transport stream carrying the required service */
        Int32 mTransportStreamId;
        /** The service_ID of the required service */
        Int32 mServiceId;
        /**
         * The original_network_ID of the network carrying the transport stream for the required
         * service
         */
        Int32 mOriginalNetworkId;
    };

    class AtscData
        : public Object
        , public IHdmiRecordSourcesAtscData
        , public IHdmiRecordSourcesDigitalServiceIdentification
    {
    public:
        CAR_INTERFACE_DECL()

        AtscData(
            /* [in] */ Int32 transportStreamId,
            /* [in] */ Int32 programNumber);

        CARAPI ToByteArray(
            /* [in] */ ArrayOf<Byte>* data,
            /* [in] */ Int32 index,
            /* [out] */ Int32* result);

    private:
        /** The transport_stream_ID of the transport stream carrying the required service */
        Int32 mTransportStreamId;
        /** The Program_number of the required service */
        Int32 mProgramNumber;
    };

    class DvbData
        : public Object
        , public IHdmiRecordSourcesDvbData
        , public IHdmiRecordSourcesDigitalServiceIdentification
    {
    public:
        CAR_INTERFACE_DECL()

        DvbData(
            /* [in] */ Int32 transportStreamId,
            /* [in] */ Int32 serviceId,
            /* [in] */ Int32 originalNetworkId);

        CARAPI ToByteArray(
            /* [in] */ ArrayOf<Byte>* data,
            /* [in] */ Int32 index,
            /* [out] */ Int32* result);

    private:
        /** The transport_stream_ID of the transport stream carrying the required service */
        Int32 mTransportStreamId;
        /** The service_ID of the required service */
        Int32 mServiceId;
        /**
         * The original_network_ID of the network carrying the transport stream for the required
         * service
         */
        Int32 mOriginalNetworkId;
    };

    class DigitalChannelData
        : public Object
        , public IHdmiRecordSourcesDigitalChannelData
        , public IHdmiRecordSourcesDigitalServiceIdentification
    {
    public:
        CAR_INTERFACE_DECL()

        static CARAPI OfTwoNumbers(
            /* [in] */ Int32 majorNumber,
            /* [in] */ Int32 minorNumber,
            /* [out] */ IHdmiRecordSourcesDigitalChannelData** data);

        static CARAPI OfOneNumber(
            /* [in] */ Int32 number,
            /* [out] */ IHdmiRecordSourcesDigitalChannelData** data);

        CARAPI ToByteArray(
            /* [in] */ ArrayOf<Byte>* data,
            /* [in] */ Int32 index,
            /* [out] */ Int32* result);

    private:
        DigitalChannelData(
            /* [in] */ ChannelIdentifier* id);

    private:
        /** Identifies the logical or virtual channel number of a service. */
        AutoPtr<ChannelIdentifier> mChannelIdentifier;
    };

    class DigitalServiceSource
        : public RecordSource
        , public IHdmiRecordSourcesDigitalServiceSource
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI ExtraParamToByteArray(
            /* [in] */ ArrayOf<Byte>* data,
            /* [in] */ Int32 index,
            /* [out] */ Int32* array);
    private:
        DigitalServiceSource(
            /* [in] */ Int32 identificatinoMethod,
            /* [in] */ Int32 broadcastSystem,
            /* [in] */ IHdmiRecordSourcesDigitalServiceIdentification* identification);

    private:
        friend class HdmiRecordSources;

        /** Indicates that a service is identified by digital service IDs. */
        static const Int32 DIGITAL_SERVICE_IDENTIFIED_BY_DIGITAL_ID;
        /** Indicates that a service is identified by a logical or virtual channel number. */
        static const Int32 DIGITAL_SERVICE_IDENTIFIED_BY_CHANNEL;

        /**
         * Type of identification. It should be one of DIGITAL_SERVICE_IDENTIFIED_BY_DIGITAL_ID and
         * DIGITAL_SERVICE_IDENTIFIED_BY_CHANNEL
         */
        Int32 mIdentificationMethod;
        /**
         * Indicates the Digital Broadcast System of required service. This is present irrespective
         * of the state of [Service Identification Method].
         */
        Int32 mBroadcastSystem;

        /**
         * Extra parameter for digital service identification.
         */
        AutoPtr<IHdmiRecordSourcesDigitalServiceIdentification> mIdentification;
    };

    class AnalogueServiceSource
        : public RecordSource
        , public IHdmiRecordSourcesAnalogueServiceSource
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI ExtraParamToByteArray(
            /* [in] */ ArrayOf<Byte>* data,
            /* [in] */ Int32 index,
            /* [out] */ Int32* array);

    private:
        friend class HdmiRecordSources;

        AnalogueServiceSource(
            /* [in] */ Int32 broadcastType,
            /* [in] */ Int32 frequency,
            /* [in] */ Int32 broadcastSystem);

    private:
        /** Indicates the Analogue broadcast type. */
        Int32 mBroadcastType;
        /** Used to specify the frequency used by an analogue tuner. 0x0000<N<0xFFFF. */
        Int32 mFrequency;
        /**
         * This specifies information about the color system, the sound carrier and the
         * IF-frequency.
         */
        Int32 mBroadcastSystem;
    };

    class ExternalPlugData
        : public RecordSource
        , public IHdmiRecordSourcesExternalPlugData
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI ExtraParamToByteArray(
            /* [in] */ ArrayOf<Byte>* data,
            /* [in] */ Int32 index,
            /* [out] */ Int32* array);

    private:
        friend class HdmiRecordSources;

        ExternalPlugData(
            /* [in] */ Int32 plugNumber);

    private:
        /** External Plug number on the Recording Device. */
        Int32 mPlugNumber;
    };

    class ExternalPhysicalAddress
        : public RecordSource
        , public IHdmiRecordSourcesExternalPhysicalAddress
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI ExtraParamToByteArray(
            /* [in] */ ArrayOf<Byte>* data,
            /* [in] */ Int32 index,
            /* [out] */ Int32* array);

    private:
        friend class HdmiRecordSources;

        ExternalPhysicalAddress(
            /* [in] */ Int32 physicalAddress);

    private:
        Int32 mPhysicalAddress;
    };

private:
    class ChannelIdentifier
        : public Object
    {
    private:
        friend class HdmiRecordSources::DigitalChannelData;

        ChannelIdentifier(
            /* [in] */ Int32 format,
            /* [in] */ Int32 majorNumber,
            /* [in] */ Int32 minorNumer);


        CARAPI ToByteArray(
            /* [in] */ ArrayOf<Byte>* data,
            /* [in] */ Int32 index,
            /* [out] */ Int32* result);

    private:
        /** Identifies Channel Format */
        Int32 mChannelNumberFormat;
        /**
         * Major Channel Number (if Channel Number Format is 2-part). If format is
         * CHANNEL_NUMBER_FORMAT_1_PART, this will be ignored(0).
         */
        Int32 mMajorChannelNumber;
        /**
         * 1-part Channel Number, or a Minor Channel Number (if Channel Number Format is 2-part).
         */
        Int32 mMinorChannelNumber;
    };

public:
    CAR_INTERFACE_DECL()

    virtual ~HdmiRecordSources() {}

    CARAPI constructor();

    // ---------------------------------------------------------------------------------------------
    // ---- Own source -----------------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------
    /**
     * Create {@link OwnSource} of own source.
     */
    static CARAPI OfOwnSource(
        /* [out] */ IHdmiRecordSourcesOwnSource** source);

    /**
     * Create {@link DigitalServiceSource} with channel type.
     *
     * @param broadcastSystem digital broadcast system. It should be one of
     *            <ul>
     *            <li>{@link #DIGITAL_BROADCAST_TYPE_ARIB}
     *            <li>{@link #DIGITAL_BROADCAST_TYPE_ATSC}
     *            <li>{@link #DIGITAL_BROADCAST_TYPE_DVB}
     *            <li>{@link #DIGITAL_BROADCAST_TYPE_ARIB_BS}
     *            <li>{@link #DIGITAL_BROADCAST_TYPE_ARIB_CS}
     *            <li>{@link #DIGITAL_BROADCAST_TYPE_ARIB_T}
     *            <li>{@link #DIGITAL_BROADCAST_TYPE_ATSC_CABLE}
     *            <li>{@link #DIGITAL_BROADCAST_TYPE_ATSC_SATELLITE}
     *            <li>{@link #DIGITAL_BROADCAST_TYPE_ATSC_TERRESTRIAL}
     *            <li>{@link #DIGITAL_BROADCAST_TYPE_DVB_C}
     *            <li>{@link #DIGITAL_BROADCAST_TYPE_DVB_S}
     *            <li>{@link #DIGITAL_BROADCAST_TYPE_DVB_S2}
     *            <li>{@link #DIGITAL_BROADCAST_TYPE_DVB_T}
     *            </ul>
     * @hide
     */
    static CARAPI OfDigitalChannelId(
        /* [in] */ Int32 broadcastSystem,
        /* [in] */ IHdmiRecordSourcesDigitalChannelData* data,
        /* [out] */ IHdmiRecordSourcesDigitalServiceSource** source);

    /**
     * Create {@link DigitalServiceSource} of ARIB type.
     *
     * @param aribType ARIB type. It should be one of
     *            <ul>
     *            <li>{@link #DIGITAL_BROADCAST_TYPE_ARIB}
     *            <li>{@link #DIGITAL_BROADCAST_TYPE_ARIB_BS}
     *            <li>{@link #DIGITAL_BROADCAST_TYPE_ARIB_CS}
     *            <li>{@link #DIGITAL_BROADCAST_TYPE_ARIB_T}
     *            </ul>
     * @hide
     */
    //@Nullable
    static CARAPI OfArib(
        /* [in] */ Int32 aribType,
        /* [in] */ IHdmiRecordSourcesAribData* data,
        /* [out] */ IHdmiRecordSourcesDigitalServiceSource** source);

    /**
     * Create {@link DigitalServiceSource} of ATSC type.
     *
     * @param atscType ATSC type. It should be one of
     *            <ul>
     *            <li>{@link #DIGITAL_BROADCAST_TYPE_ATSC}
     *            <li>{@link #DIGITAL_BROADCAST_TYPE_ATSC_CABLE}
     *            <li>{@link #DIGITAL_BROADCAST_TYPE_ATSC_SATELLITE}
     *            <li>{@link #DIGITAL_BROADCAST_TYPE_ATSC_TERRESTRIAL}
     *            </ul>
     * @hide
     */
    //@Nullable
    static CARAPI OfAtsc(
        /* [in] */ Int32 atscType,
        /* [in] */ IHdmiRecordSourcesAtscData* data,
        /* [out] */ IHdmiRecordSourcesDigitalServiceSource** source);

    /**
     * Create {@link DigitalServiceSource} of ATSC type.
     *
     * @param dvbType DVB type. It should be one of
     *            <ul>
     *            <li>{@link #DIGITAL_BROADCAST_TYPE_DVB}
     *            <li>{@link #DIGITAL_BROADCAST_TYPE_DVB_C}
     *            <li>{@link #DIGITAL_BROADCAST_TYPE_DVB_S}
     *            <li>{@link #DIGITAL_BROADCAST_TYPE_DVB_S2}
     *            <li>{@link #DIGITAL_BROADCAST_TYPE_DVB_T}
     *            </ul>
     * @hide
     */
    //@Nullable
    static CARAPI OfDvb(
        /* [in] */ Int32 dvbType,
        /* [in] */ IHdmiRecordSourcesDvbData* data,
        /* [out] */ IHdmiRecordSourcesDigitalServiceSource** source);

    /**
     * Create {@link AnalogueServiceSource} of analogue service.
     *
     * @param broadcastType
     * @param frequency
     * @param broadcastSystem
     * @hide
     */
    //@Nullable
    static CARAPI OfAnalogue(
        /* [in] */ Int32 broadcastType,
        /* [in] */ Int32 frequency,
        /* [in] */ Int32 broadcastSystem,
        /* [out] */ IHdmiRecordSourcesAnalogueServiceSource** source);

    // ---------------------------------------------------------------------------------------------
    // ---- External plug data ---------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------
    /**
     * Create {@link ExternalPlugData} of external plug type.
     *
     * @param plugNumber plug number. It should be in range of [1, 255]
     * @hide
     */
    static CARAPI OfExternalPlug(
        /* [in] */ Int32 plugNumber,
        /* [out] */ IHdmiRecordSourcesExternalPlugData** data);

    // ---------------------------------------------------------------------------------------------
    // ---- External physical address --------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------
    /**
     * Create {@link ExternalPhysicalAddress} of external physical address.
     *
     * @param physicalAddress
     * @hide
     */
    static CARAPI OfExternalPhysicalAddress(
        /* [in] */ Int32 physicalAddress,
        /* [out] */ IHdmiRecordSourcesExternalPhysicalAddress** address);

    /**
     * Check the byte array of record source.
     * @hide
     */
    //@SystemApi
    static CARAPI CheckRecordSource(
        /* [in] */ ArrayOf<Byte>* recordSource,
        /* [out] */ Boolean* result);

private:
    HdmiRecordSources() {}

    // ---------------------------------------------------------------------------------------------
    // ------- Helper methods ----------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------
    static CARAPI_(Int32) ThreeFieldsToSixBytes(
        /* [in] */ Int32 first,
        /* [in] */ Int32 second,
        /* [in] */ Int32 third,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 index);

    static CARAPI_(Int32) Int16ToByteArray(
        /* [in] */ Int16 value,
        /* [in] */ ArrayOf<Byte>* byteArray,
        /* [in] */ Int32 index);

private:
    static const String TAG;

    /** Record source type for "Own Source". */
    static const Int32 RECORD_SOURCE_TYPE_OWN_SOURCE = 1;
    /** Record source type for "Digital Service". */
    static const Int32 RECORD_SOURCE_TYPE_DIGITAL_SERVICE = 2;
    /** Record source type for "Analogue Service". */
    static const Int32 RECORD_SOURCE_TYPE_ANALOGUE_SERVICE = 3;
    /** Record source type for "Exteranl Plug". */
    static const Int32 RECORD_SOURCE_TYPE_EXTERNAL_PLUG = 4;
    /** Record source type for "External Physical Address". */
    static const Int32 RECORD_SOURCE_TYPE_EXTERNAL_PHYSICAL_ADDRESS = 5;

    /** Channel number formats. */
    static const Int32 CHANNEL_NUMBER_FORMAT_1_PART;
    static const Int32 CHANNEL_NUMBER_FORMAT_2_PART;
};

} //Hdmi
} //Hardware
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_HARDWARE_HDMI_HDMIRECORDSOURCES_H__
