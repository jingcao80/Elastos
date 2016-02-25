
#ifndef __ELASTOS_DROID_SERVER_HDMI_HDMIUTILS_H__
#define __ELASTOS_DROID_SERVER_HDMI_HDMIUTILS_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Utility::ISlog;
using Elastos::Droid::Utility::ISparseArray;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Various utilities to handle HDMI CEC messages.
 */
class HdmiUtils
    : public Object
{
public:
    /**
     * Check if the given logical address is valid. A logical address is valid
     * if it is one allocated for an actual device which allows communication
     * with other logical devices.
     *
     * @param address logical address
     * @return true if the given address is valid
     */
    static CARAPI IsValidAddress(
        /* [in] */ Int32 address,
        /* [out] */ Boolean* result);

    /**
     * Return the device type for the given logical address.
     *
     * @param address logical address
     * @return device type for the given logical address; DEVICE_INACTIVE
     *         if the address is not valid.
     */
    static CARAPI GetTypeFromAddress(
        /* [in] */ Int32 address,
        /* [out] */ Int32* result);

    /**
     * Return the default device name for a logical address. This is the name
     * by which the logical device is known to others until a name is
     * set explicitly using HdmiCecService.setOsdName.
     *
     * @param address logical address
     * @return default device name; empty string if the address is not valid
     */
    static CARAPI GetDefaultDeviceName(
        /* [in] */ Int32 address,
        /* [out] */ String* result);

    /**
     * Verify if the given address is for the given device type.  If not it will throw
     * {@link IllegalArgumentException}.
     *
     * @param logicalAddress the logical address to verify
     * @param deviceType the device type to check
     * @throw IllegalArgumentException
     */
    static CARAPI VerifyAddressType(
        /* [in] */ Int32 logicalAddress,
        /* [in] */ Int32 deviceType);

    /**
     * Check if the given CEC message come from the given address.
     *
     * @param cmd the CEC message to check
     * @param expectedAddress the expected source address of the given message
     * @param tag the tag of caller module (for log message)
     * @return true if the CEC message comes from the given address
     */
    static CARAPI CheckCommandSource(
        /* [in] */ IHdmiCecMessage* cmd,
        /* [in] */ Int32 expectedAddress,
        /* [in] */ const String& tag,
        /* [out] */ Boolean* result);

    /**
     * Parse the parameter block of CEC message as [System Audio Status].
     *
     * @param cmd the CEC message to parse
     * @return true if the given parameter has [ON] value
     */
    static CARAPI ParseCommandParamSystemAudioStatus(
        /* [in] */ IHdmiCecMessage* cmd,
        /* [out] */ Boolean* result);

    /**
     * Convert integer array to list of {@link Integer}.
     *
     * <p>The result is immutable.
     *
     * @param is integer array
     * @return {@link List} instance containing the elements in the given array
     */
    static CARAPI AsImmutableList(
        /* [in] */ ArrayOf<Int32>* is,
        /* [out] */ IList** result);

    /**
     * Assemble two bytes into single integer value.
     *
     * @param data to be assembled
     * @return assembled value
     */
    static CARAPI TwoBytesToInt(
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ Int32* result);

    /**
     * Assemble two bytes into single integer value.
     *
     * @param data to be assembled
     * @param offset offset to the data to convert in the array
     * @return assembled value
     */
    static CARAPI TwoBytesToInt(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset,
        /* [out] */ Int32* result);

    /**
     * Assemble three bytes into single integer value.
     *
     * @param data to be assembled
     * @return assembled value
     */
    static CARAPI ThreeBytesToInt(
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ Int32* result);

    static CARAPI SparseArrayToList(
        /* [in] */ ISparseArray* array,
        /* [out] */ IList** result);

    static CARAPI MergeToUnmodifiableList(
        /* [in] */ IList* a,
        /* [in] */ IList* b,
        /* [out] */ IList** result);

    /**
     * See if the new path is affecting the active path.
     *
     * @param activePath current active path
     * @param newPath new path
     * @return true if the new path changes the current active path
     */
    static CARAPI IsAffectingActiveRoutingPath(
        /* [in] */ Int32 activePath,
        /* [in] */ Int32 newPath,
        /* [out] */ Boolean* result);

    /**
     * See if the new path is in the active path.
     *
     * @param activePath current active path
     * @param newPath new path
     * @return true if the new path in the active routing path
     */
    static CARAPI IsInActiveRoutingPath(
        /* [in] */ Int32 activePath,
        /* [in] */ Int32 newPath,
        /* [out] */ Boolean* result);

    /**
     * Clone {@link HdmiDeviceInfo} with new power status.
     */
    static CARAPI CloneHdmiDeviceInfo(
        /* [in] */ IHdmiDeviceInfo* info,
        /* [in] */ Int32 newPowerStatus,
        /* [out] */ IHdmiDeviceInfo** result);

private:
    HdmiUtils();

    static AutoPtr<ArrayOf<Int32> > InitADDRESS_TO_TYPE();

    static AutoPtr<ArrayOf<String> > InitDEFAULT_NAMES();

private:
    static const AutoPtr<ArrayOf<Int32> > ADDRESS_TO_TYPE;

    static const AutoPtr<ArrayOf<String> > DEFAULT_NAMES;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_HDMIUTILS_H__
