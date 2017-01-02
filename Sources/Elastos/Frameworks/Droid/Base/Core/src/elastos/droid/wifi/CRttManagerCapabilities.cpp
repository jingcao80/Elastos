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

#include "elastos/droid/wifi/CRttManagerCapabilities.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL(CRttManagerCapabilities, Object, IRttManagerCapabilities)

CAR_OBJECT_IMPL(CRttManagerCapabilities)

ECode CRttManagerCapabilities::constructor()
{
    mSupportedType = 0;
    mSupportedPeerType = 0;
    return NOERROR;
}

ECode CRttManagerCapabilities::GetSupportedType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSupportedType;
    return NOERROR;
}

ECode CRttManagerCapabilities::SetSupportedType(
    /* [in] */ Int32 supportedType)
{
    mSupportedType = supportedType;
    return NOERROR;
}

ECode CRttManagerCapabilities::GetSupportedPeerType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSupportedPeerType;
    return NOERROR;
}

ECode CRttManagerCapabilities::SetSupportedPeerType(
    /* [in] */ Int32 supportedPeerType)
{
    mSupportedPeerType = supportedPeerType;
    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
