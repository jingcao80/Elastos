/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.server.hdmi;

using Elastos::Droid::Hardware::Hdmi::IHdmiPortInfo;
using Elastos::Droid::Utility::ISparseArray;

using Elastos::Droid::Server::Ihdmi.HdmiControlService.SendMessageCallback;

/**
 * A handler class for MHL control command. It converts user's command into MHL command and pass it
 * to MHL HAL layer.
 * <p>
 * It can be created only by {@link HdmiMhlControllerStub#create}.
 */
final class HdmiMhlControllerStub {

    private static const SparseArray<HdmiMhlLocalDeviceStub> mLocalDevices = new SparseArray<>();
    private static const HdmiPortInfo[] EMPTY_PORT_INFO = new HdmiPortInfo[0];
    private static const Int32 INVALID_MHL_VERSION = 0;
    private static const Int32 NO_SUPPORTED_FEATURES = 0;
    private static const Int32 INVALID_DEVICE_ROLES = 0;

    // Private constructor. Use HdmiMhlControllerStub->Create().
    private HdmiMhlControllerStub(HdmiControlService service) {
    }

    // Returns TRUE if MHL controller is initialized and ready to use.
    Boolean IsReady() {
        return FALSE;
    }

    static HdmiMhlControllerStub Create(HdmiControlService service) {
        return new HdmiMhlControllerStub(service);
    }

    HdmiPortInfo[] GetPortInfos() {
        return EMPTY_PORT_INFO;
    }

    /**
     * Return {@link HdmiMhlLocalDeviceStub} matched with the given port id.
     *
     * @return NULL if has no matched port id
     */
    HdmiMhlLocalDeviceStub GetLocalDevice(Int32 portId) {
        return NULL;
    }

    /**
     * Return {@link HdmiMhlLocalDeviceStub} matched with the given device id.
     *
     * @return NULL if has no matched id
     */
    HdmiMhlLocalDeviceStub GetLocalDeviceById(Int32 deviceId) {
        return NULL;
    }

    SparseArray<HdmiMhlLocalDeviceStub> GetAllLocalDevices() {
        return mLocalDevices;
    }

    /**
     * Remove a {@link HdmiMhlLocalDeviceStub} matched with the given port id.
     *
     * @return removed {@link HdmiMhlLocalDeviceStub}. Return NULL if no matched port id.
     */
    HdmiMhlLocalDeviceStub RemoveLocalDevice(Int32 portId) {
        return NULL;
    }

    /**
     * Add a new {@link HdmiMhlLocalDeviceStub}.
     *
     * @return old {@link HdmiMhlLocalDeviceStub} having same port id
     */
    HdmiMhlLocalDeviceStub AddLocalDevice(HdmiMhlLocalDeviceStub device) {
        return NULL;
    }

    void ClearAllLocalDevices() {
    }

    void SendVendorCommand(Int32 portId, Int32 offset, Int32 length, Byte[] data) {
    }

    void SetOption(Int32 flag, Int32 value) {
    }

    /**
     * Get the MHL version supported by underlying hardware port of the given {@code portId}.
     * MHL specification version 2.0 returns 0x20, 3.0 will return 0x30 respectively.
     * The return value is stored in 'version'. Return INVALID_VERSION if MHL hardware layer
     * is not ready.
     */
    Int32 GetMhlVersion(Int32 portId) {
        return INVALID_MHL_VERSION;
    }

    /**
     * Get MHL version of a device which is connected to a port of the given {@code portId}.
     * MHL specification version 2.0 returns 0x20, 3.0 will return 0x30 respectively.
     * The return value is stored in 'version'.
     */
    Int32 GetPeerMhlVersion(Int32 portId) {
        return INVALID_MHL_VERSION;
    }

    /**
     * Get the bit flags describing the features supported by the system. Refer to feature support
     * flag register info in MHL specification.
     */
    Int32 GetSupportedFeatures(Int32 portId) {
        return NO_SUPPORTED_FEATURES;
    }

    /**
     * Get the bit flags describing the roles which ECBUS device can play. Refer to the
     * ECBUS_DEV_ROLES Register info MHL3.0 specification
     */
    Int32 GetEcbusDeviceRoles(Int32 portId) {
        return INVALID_DEVICE_ROLES;
    }
}
