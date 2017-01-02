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

#ifndef __ELASTOS_DROID_NET_CNETWORKSTATSHISTORYDATASTREAMUTILS_H__
#define __ELASTOS_DROID_NET_CNETWORKSTATSHISTORYDATASTREAMUTILS_H__

#include "_Elastos_Droid_Net_CNetworkStatsHistoryDataStreamUtils.h"
#include <elastos/core/Singleton.h>

using Elastos::IO::IDataInputStream;
using Elastos::IO::IDataOutputStream;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Utility methods for interacting with {@link DataInputStream} and
 * {@link DataOutputStream}, mostly dealing with writing partial arrays.
 */
CarClass(CNetworkStatsHistoryDataStreamUtils)
    , public Singleton
    , public INetworkStatsHistoryDataStreamUtils
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    // @Deprecated
    CARAPI ReadFullLongArray(
        /* [in] */ IDataInputStream* in,
        /* [out, callee] */ ArrayOf<Int64>** result);

    /**
     * Read variable-length {@link Long} using protobuf-style approach.
     */
    CARAPI ReadVarLong(
        /* [in] */ IDataInputStream* in,
        /* [out] */ Int64* result);

    /**
     * Write variable-length {@link Long} using protobuf-style approach.
     */
    CARAPI WriteVarLong(
        /* [in] */ IDataOutputStream* out,
        /* [in] */ Int64 value);

    CARAPI ReadVarLongArray(
        /* [in] */ IDataInputStream* in,
        /* [out, callee] */ ArrayOf<Int64>** result);

    CARAPI WriteVarLongArray(
        /* [in] */ IDataOutputStream* out,
        /* [in] */ ArrayOf<Int64>* values,
        /* [in] */ Int32 size);
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CNETWORKSTATSHISTORYDATASTREAMUTILS_H__
