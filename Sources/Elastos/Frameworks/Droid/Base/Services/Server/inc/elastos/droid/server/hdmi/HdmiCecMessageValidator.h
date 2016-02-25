
#ifndef __ELASTOS_DROID_SERVER_HDMI_HDMICECMESSAGEVALIDATOR_H__
#define __ELASTOS_DROID_SERVER_HDMI_HDMICECMESSAGEVALIDATOR_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Utility::ISparseArray;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * A helper class to validates {@link HdmiCecMessage}.
 */
class HdmiCecMessageValidator
    : public Object
{
private:
    class ValidationInfo
        : public Object
    {
    public:
        ValidationInfo(
            /* [in] */ IHdmiCecMessageValidatorParameterValidator* validator,
            /* [in] */ Int32 type);

        AutoPtr<IHdmiCecMessageValidatorParameterValidator> mParameterValidator;

        const Int32 mAddressType;
    };

    class FixedLengthValidator
        : public Object
        , public IHdmiCecMessageValidatorParameterValidator
    {
    public:
        CAR_INTERFACE_DECL()

        FixedLengthValidator(
            /* [in] */ Int32 length);

        // @Override
        CARAPI IsValid(
            /* [in] */ ArrayOf<Byte>* params,
            /* [out] */ Boolean* result);

    private:
        const Int32 mLength;
    };

    class VariableLengthValidator
        : public Object
        , public IHdmiCecMessageValidatorParameterValidator
    {
    public:
        CAR_INTERFACE_DECL()

        VariableLengthValidator(
            /* [in] */ Int32 minLength,
            /* [in] */ Int32 maxLength);

        // @Override
        CARAPI IsValid(
            /* [in] */ ArrayOf<Byte>* params,
            /* [out] */ Boolean* result);

    private:
        const Int32 mMinLength;

        const Int32 mMaxLength;
    };

    class PhysicalAddressValidator
        : public Object
        , public IHdmiCecMessageValidatorParameterValidator
    {
    public:
        CAR_INTERFACE_DECL()

        // @Override
        CARAPI IsValid(
            /* [in] */ ArrayOf<Byte>* params,
            /* [out] */ Boolean* result);
    };

    class ReportPhysicalAddressValidator
        : public Object
        , public IHdmiCecMessageValidatorParameterValidator
    {
    public:
        CAR_INTERFACE_DECL()

        // @Override
        CARAPI IsValid(
            /* [in] */ ArrayOf<Byte>* params,
            /* [out] */ Boolean* result);
    };

    class RoutingChangeValidator
        : public Object
        , public IHdmiCecMessageValidatorParameterValidator
    {
    public:
        CAR_INTERFACE_DECL()

        // @Override
        CARAPI IsValid(
            /* [in] */ ArrayOf<Byte>* params,
            /* [out] */ Boolean* result);
    };

public:
    HdmiCecMessageValidator();

    CARAPI constructor(
        /* [in] */ IHdmiControlService* service);

    CARAPI IsValid(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Int32* result);

    /**
     * Check if the given type is valid. A valid type is one of the actual logical device types
     * defined in the standard ({@link HdmiDeviceInfo#DEVICE_TV},
     * {@link HdmiDeviceInfo#DEVICE_PLAYBACK}, {@link HdmiDeviceInfo#DEVICE_TUNER},
     * {@link HdmiDeviceInfo#DEVICE_RECORDER}, and
     * {@link HdmiDeviceInfo#DEVICE_AUDIO_SYSTEM}).
     *
     * @param type device type
     * @return true if the given type is valid
     */
    static CARAPI IsValidType(
        /* [in] */ Int32 type,
        /* [out] */ Boolean* result);

private:
    CARAPI AddValidationInfo(
        /* [in] */ Int32 opcode,
        /* [in] */ IHdmiCecMessageValidatorParameterValidator* validator,
        /* [in] */ Int32 addrType);

    CARAPI IsValidPhysicalAddress(
        /* [in] */ ArrayOf<Byte>* params,
        /* [in] */ Int32 offset,
        /* [out] */ Boolean* result);

public:
    AutoPtr<ISparseArray> mValidationInfo;

    static const Int32 OK;

    static const Int32 ERROR_SOURCE;

    static const Int32 ERROR_DESTINATION;

    static const Int32 ERROR_PARAMETER;

private:
    static const String TAG;

    AutoPtr<IHdmiControlService> mService;

    // Only the direct addressing is allowed.
    static const Int32 DEST_DIRECT;

    // Only the broadcast addressing is allowed.
    static const Int32 DEST_BROADCAST;

    // Both the direct and the broadcast addressing are allowed.
    static const Int32 DEST_ALL;

    // True if the messages from address 15 (unregistered) are allowed.
    static const Int32 SRC_UNREGISTERED;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_HDMICECMESSAGEVALIDATOR_H__
