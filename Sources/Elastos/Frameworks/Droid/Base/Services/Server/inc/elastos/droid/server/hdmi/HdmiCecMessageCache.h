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

#ifndef __ELASTOS_DROID_SERVER_HDMI_HDMICECMESSAGECACHE_H__
#define __ELASTOS_DROID_SERVER_HDMI_HDMICECMESSAGECACHE_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::IFastImmutableArraySet;
using Elastos::Droid::Utility::ISparseArray;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Cache for incoming message. It caches {@link HdmiCecMessage} with source address and opcode
 * as a key.
 *
 * <p>Note that whenever a device is removed it should call {@link #flushMessagesFrom(int)}
 * to clean up messages come from the device.
 */
class HdmiCecMessageCache
    : public Object
{
public:
    HdmiCecMessageCache();

    CARAPI constructor();

    /**
     * Return a {@link HdmiCecMessage} corresponding to the given {@code address} and
     * {@code opcode}.
     *
     * @param address a logical address of source device
     * @param opcode opcode of message
     * @return null if has no {@link HdmiCecMessage} matched to the given {@code address} and {code
     *         opcode}
     */
    CARAPI GetMessage(
        /* [in] */ Int32 address,
        /* [in] */ Int32 opcode,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Flush all {@link HdmiCecMessage}s sent from the given {@code address}.
     *
     * @param address a logical address of source device
     */
    CARAPI FlushMessagesFrom(
        /* [in] */ Int32 address);

    /**
     * Flush all cached {@link HdmiCecMessage}s.
     */
    CARAPI FlushAll();

    /**
     * Cache incoming {@link HdmiCecMessage}. If opcode of message is not listed on
     * cacheable opcodes list, just ignore it.
     *
     * @param message a {@link HdmiCecMessage} to be cached
     */
    CARAPI CacheMessage(
        /* [in] */ IHdmiCecMessage* message);

private:
    CARAPI_(Boolean) IsCacheable(
        /* [in] */ Int32 opcode);

    static AutoPtr<IFastImmutableArraySet> InitCACHEABLE_OPCODES();

private:
    static const AutoPtr<IFastImmutableArraySet> CACHEABLE_OPCODES;

    // It's like [Source Logical Address, [Opcode, HdmiCecMessage]].
    AutoPtr<ISparseArray> mCache;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_HDMICECMESSAGECACHE_H__

