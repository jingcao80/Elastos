
#ifndef __ELASTOS_DROID_SERVER_HDMI_HDMICECMESSAGE_H__
#define __ELASTOS_DROID_SERVER_HDMI_HDMICECMESSAGE_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * A class to encapsulate HDMI-CEC message used for the devices connected via
 * HDMI cable to communicate with one another. A message is defined by its
 * source and destination address, command (or opcode), and optional parameters.
 */
class HdmiCecMessage
    : public Object
    , public IHdmiCecMessage
{
public:
    CAR_INTERFACE_DECL()

    HdmiCecMessage();

    /**
     * Constructor.
     */
    CARAPI constructor(
        /* [in] */ Int32 source,
        /* [in] */ Int32 destination,
        /* [in] */ Int32 opcode,
        /* [in] */ ArrayOf<Byte>* params);

    static CARAPI GetEMPTY_PARAM(
            /* [out, callee] */ ArrayOf<Byte>** result);

    /**
     * Return the source address field of the message. It is the logical address
     * of the device which generated the message.
     *
     * @return source address
     */
    CARAPI GetSource(
        /* [out] */ Int32* result);

    /**
     * Return the destination address field of the message. It is the logical address
     * of the device to which the message is sent.
     *
     * @return destination address
     */
    CARAPI GetDestination(
        /* [out] */ Int32* result);

    /**
     * Return the opcode field of the message. It is the type of the message that
     * tells the destination device what to do.
     *
     * @return opcode
     */
    CARAPI GetOpcode(
        /* [out] */ Int32* result);

    /**
     * Return the parameter field of the message. The contents of parameter varies
     * from opcode to opcode, and is used together with opcode to describe
     * the action for the destination device to take.
     *
     * @return parameter
     */
    CARAPI GetParams(
        /* [out, callee] */ ArrayOf<Byte>** result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

private:
    static CARAPI OpcodeToString(
        /* [in] */ Int32 opcode,
        /* [out] */ String* result);

private:
    static AutoPtr<ArrayOf<Byte> > EMPTY_PARAM;

    Int32 mSource;

    Int32 mDestination;

    Int32 mOpcode;

    AutoPtr<ArrayOf<Byte> > mParams;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_HDMICECMESSAGE_H__
