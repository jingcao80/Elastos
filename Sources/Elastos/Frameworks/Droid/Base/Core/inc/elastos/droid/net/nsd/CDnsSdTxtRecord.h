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

#ifndef __ELASTOS_DROID_NET_NSD_CDNSSDTXTRECORD_H__
#define __ELASTOS_DROID_NET_NSD_CDNSSDTXTRECORD_H__

#include "_Elastos_Droid_Net_Nsd_CDnsSdTxtRecord.h"
#include "elastos/droid/net/nsd/DnsSdTxtRecord.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Nsd {

/**
 * This class handles TXT record data for DNS based service discovery as specified at
 * http://tools.ietf.org/html/draft-cheshire-dnsext-dns-sd-11
 *
 * DNS-SD specifies that a TXT record corresponding to an SRV record consist of
 * a packed array of bytes, each preceded by a length byte. Each string
 * is an attribute-value pair.
 *
 * The DnsSdTxtRecord object stores the entire TXT data as a single byte array, traversing it
 * as need be to implement its various methods.
 * @hide
 *
 */
CarClass(CDnsSdTxtRecord)
    , public DnsSdTxtRecord
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Nsd
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_NSD_CDNSSDTXTRECORD_H__
