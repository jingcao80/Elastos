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

#ifndef __ELASTOS_DROID_NET_CWIFILINKQUALITYINFO_H__
#define __ELASTOS_DROID_NET_CWIFILINKQUALITYINFO_H__

#include "_Elastos_Droid_Net_CWifiLinkQualityInfo.h"
#include "elastos/droid/net/WifiLinkQualityInfo.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 *  Class that represents useful attributes of wifi network links
 *  such as the upload/download throughput or error rate etc.
 *  @hide
 */
CarClass(CWifiLinkQualityInfo)
    , public WifiLinkQualityInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_CWIFILINKQUALITYINFO_H__
