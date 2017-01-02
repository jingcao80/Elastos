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

#ifndef __ELASTOS_DROID_SERVER_HDMI_HDMIMHLCONTROLLERSTUB_H__
#define __ELASTOS_DROID_SERVER_HDMI_HDMIMHLCONTROLLERSTUB_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>
#include <Elastos.Droid.Hardware.h>

using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Hardware::Hdmi::IHdmiPortInfo;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

class HdmiControlService;
class HdmiMhlLocalDeviceStub;
/**
 * A handler class for MHL control command. It converts user's command into MHL command and pass it
 * to MHL HAL layer.
 * <p>
 * It can be created only by {@link HdmiMhlControllerStub#create}.
 */
class HdmiMhlControllerStub
    : public Object
{
public:
    // Returns true if MHL controller is initialized and ready to use.
    CARAPI IsReady(
        /* [out] */ Boolean* result);

    static CARAPI Create(
        /* [in] */ IHdmiControlService* service,
        /* [out] */ HdmiMhlControllerStub** result);

    CARAPI GetPortInfos(
        /* [out, callee] */ ArrayOf<IHdmiPortInfo*>** result);

    /**
     * Return {@link HdmiMhlLocalDeviceStub} matched with the given port id.
     *
     * @return null if has no matched port id
     */
    CARAPI GetLocalDevice(
        /* [in] */ Int32 portId,
        /* [out] */ HdmiMhlLocalDeviceStub** result);

    /**
     * Return {@link HdmiMhlLocalDeviceStub} matched with the given device id.
     *
     * @return null if has no matched id
     */
    CARAPI GetLocalDeviceById(
        /* [in] */ Int32 deviceId,
        /* [out] */ HdmiMhlLocalDeviceStub** result);

    CARAPI GetAllLocalDevices(
        /* [out] */ ISparseArray** result);

    /**
     * Remove a {@link HdmiMhlLocalDeviceStub} matched with the given port id.
     *
     * @return removed {@link HdmiMhlLocalDeviceStub}. Return null if no matched port id.
     */
    CARAPI RemoveLocalDevice(
        /* [in] */ Int32 portId,
        /* [out] */ HdmiMhlLocalDeviceStub** result);

    /**
     * Add a new {@link HdmiMhlLocalDeviceStub}.
     *
     * @return old {@link HdmiMhlLocalDeviceStub} having same port id
     */
    CARAPI AddLocalDevice(
        /* [in] */ HdmiMhlLocalDeviceStub* device,
        /* [out] */ HdmiMhlLocalDeviceStub** result);

    CARAPI ClearAllLocalDevices();

    CARAPI SendVendorCommand(
        /* [in] */ Int32 portId,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [in] */ ArrayOf<Byte>* data);

    CARAPI SetOption(
        /* [in] */ Int32 flag,
        /* [in] */ Int32 value);

    /**
     * Get the MHL version supported by underlying hardware port of the given {@code portId}.
     * MHL specification version 2.0 returns 0x20, 3.0 will return 0x30 respectively.
     * The return value is stored in 'version'. Return INVALID_VERSION if MHL hardware layer
     * is not ready.
     */
    CARAPI GetMhlVersion(
        /* [in] */ Int32 portId,
        /* [out] */ Int32* result);

    /**
     * Get MHL version of a device which is connected to a port of the given {@code portId}.
     * MHL specification version 2.0 returns 0x20, 3.0 will return 0x30 respectively.
     * The return value is stored in 'version'.
     */
    CARAPI GetPeerMhlVersion(
        /* [in] */ Int32 portId,
        /* [out] */ Int32* result);

    /**
     * Get the bit flags describing the features supported by the system. Refer to feature support
     * flag register info in MHL specification.
     */
    CARAPI GetSupportedFeatures(
        /* [in] */ Int32 portId,
        /* [out] */ Int32* result);

    /**
     * Get the bit flags describing the roles which ECBUS device can play. Refer to the
     * ECBUS_DEV_ROLES Register info MHL3.0 specification
     */
    CARAPI GetEcbusDeviceRoles(
        /* [in] */ Int32 portId,
        /* [out] */ Int32* result);

private:
    // Private constructor. Use HdmiMhlControllerStub.create().
    HdmiMhlControllerStub(
        /* [in] */ IHdmiControlService* service);

    static CARAPI_(AutoPtr<ISparseArray>) InitArray();

private:
    static const AutoPtr<ISparseArray> mLocalDevices;

    static const AutoPtr<ArrayOf<IHdmiPortInfo*> > EMPTY_PORT_INFO;

    static const Int32 INVALID_MHL_VERSION;

    static const Int32 NO_SUPPORTED_FEATURES;

    static const Int32 INVALID_DEVICE_ROLES;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_HDMIMHLCONTROLLERSTUB_H__
