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

using Elastos::Droid::Utility::IFastImmutableArraySet;
using Elastos::Droid::Utility::ISparseArray;

/**
 * Cache for incoming message. It caches {@link HdmiCecMessage} with source address and opcode
 * as a key.
 *
 * <p>Note that whenever a device is removed it should call {@link #FlushMessagesFrom(Int32)}
 * to clean up messages come from the device.
 */
final class HdmiCecMessageCache {
    private static const FastImmutableArraySet<Integer> CACHEABLE_OPCODES =
            new FastImmutableArraySet<>(new Integer[] {
                    Constants.MESSAGE_SET_OSD_NAME,
                    Constants.MESSAGE_REPORT_PHYSICAL_ADDRESS,
                    Constants.MESSAGE_DEVICE_VENDOR_ID,
                    Constants.MESSAGE_CEC_VERSION,
            });

    // It's like [Source Logical Address, [Opcode, HdmiCecMessage]].
    private final SparseArray<SparseArray<HdmiCecMessage>> mCache = new SparseArray<>();

    HdmiCecMessageCache() {
    }

    /**
     * Return a {@link HdmiCecMessage} corresponding to the given {@code address} and
     * {@code opcode}.
     *
     * @param address a logical address of source device
     * @param opcode opcode of message
     * @return NULL if has no {@link HdmiCecMessage} matched to the given {@code address} and {code
     *         opcode}
     */
    public HdmiCecMessage GetMessage(Int32 address, Int32 opcode) {
        SparseArray<HdmiCecMessage> messages = mCache->Get(address);
        if (messages == NULL) {
            return NULL;
        }

        return messages->Get(opcode);
    }

    /**
     * Flush all {@link HdmiCecMessage}s sent from the given {@code address}.
     *
     * @param address a logical address of source device
     */
    CARAPI FlushMessagesFrom(Int32 address) {
        mCache->Remove(address);
    }

    /**
     * Flush all cached {@link HdmiCecMessage}s.
     */
    CARAPI FlushAll() {
        mCache->Clear();
    }

    /**
     * Cache incoming {@link HdmiCecMessage}. If opcode of message is not listed on
     * cacheable opcodes list, just ignore it.
     *
     * @param message a {@link HdmiCecMessage} to be cached
     */
    CARAPI CacheMessage(HdmiCecMessage message) {
        Int32 opcode = message->GetOpcode();
        if (!IsCacheable(opcode)) {
            return;
        }

        Int32 source = message->GetSource();
        SparseArray<HdmiCecMessage> messages = mCache->Get(source);
        if (messages == NULL) {
            messages = new SparseArray<>();
            mCache->Put(source, messages);
        }
        messages->Put(opcode, message);
    }

    private Boolean IsCacheable(Int32 opcode) {
        return CACHEABLE_OPCODES->Contains(opcode);
    }
}
