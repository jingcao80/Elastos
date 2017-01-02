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
    static CARAPI_(Boolean) IsValidAddress(
        /* [in] */ Int32 address);

    /**
     * Return the device type for the given logical address.
     *
     * @param address logical address
     * @return device type for the given logical address; DEVICE_INACTIVE
     *         if the address is not valid.
     */
    static CARAPI_(Int32) GetTypeFromAddress(
        /* [in] */ Int32 address);

    /**
     * Return the default device name for a logical address. This is the name
     * by which the logical device is known to others until a name is
     * set explicitly using HdmiCecService.setOsdName.
     *
     * @param address logical address
     * @return default device name; empty string if the address is not valid
     */
    static CARAPI_(String) GetDefaultDeviceName(
        /* [in] */ Int32 address);

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
    static CARAPI_(Boolean) CheckCommandSource(
        /* [in] */ IHdmiCecMessage* cmd,
        /* [in] */ Int32 expectedAddress,
        /* [in] */ const String& tag);

    /**
     * Parse the parameter block of CEC message as [System Audio Status].
     *
     * @param cmd the CEC message to parse
     * @return true if the given parameter has [ON] value
     */
    static CARAPI_(Boolean) ParseCommandParamSystemAudioStatus(
        /* [in] */ IHdmiCecMessage* cmd);

    /**
     * Convert integer array to list of {@link Integer}.
     *
     * <p>The result is immutable.
     *
     * @param is integer array
     * @return {@link List} instance containing the elements in the given array
     */
    static CARAPI_(AutoPtr<IList>) AsImmutableList(
        /* [in] */ ArrayOf<Int32>* is);

    /**
     * Assemble two bytes into single integer value.
     *
     * @param data to be assembled
     * @return assembled value
     */
    static CARAPI_(Int32) TwoBytesToInt32(
        /* [in] */ ArrayOf<Byte>* data);

    /**
     * Assemble two bytes into single integer value.
     *
     * @param data to be assembled
     * @param offset offset to the data to convert in the array
     * @return assembled value
     */
    static CARAPI_(Int32) TwoBytesToInt32(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset);

    /**
     * Assemble three bytes into single integer value.
     *
     * @param data to be assembled
     * @return assembled value
     */
    static CARAPI_(Int32) ThreeBytesToInt32(
        /* [in] */ ArrayOf<Byte>* data);

    static CARAPI_(AutoPtr<IList>) SparseArrayToList(
        /* [in] */ ISparseArray* array);

    static CARAPI_(AutoPtr<IList>) MergeToUnmodifiableList(
        /* [in] */ IList* a,
        /* [in] */ IList* b);

    /**
     * See if the new path is affecting the active path.
     *
     * @param activePath current active path
     * @param newPath new path
     * @return true if the new path changes the current active path
     */
    static CARAPI_(Boolean) IsAffectingActiveRoutingPath(
        /* [in] */ Int32 activePath,
        /* [in] */ Int32 newPath);

    /**
     * See if the new path is in the active path.
     *
     * @param activePath current active path
     * @param newPath new path
     * @return true if the new path in the active routing path
     */
    static CARAPI_(Boolean) IsInActiveRoutingPath(
        /* [in] */ Int32 activePath,
        /* [in] */ Int32 newPath);

    /**
     * Clone {@link HdmiDeviceInfo} with new power status.
     */
    static CARAPI_(AutoPtr<IHdmiDeviceInfo>) CloneHdmiDeviceInfo(
        /* [in] */ IHdmiDeviceInfo* info,
        /* [in] */ Int32 newPowerStatus);

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
