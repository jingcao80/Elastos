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

#include "InetUnixAddress.h"
#include "OsConstants.h"

using Elastos::Droid::System::OsConstants;

namespace Elastos {
namespace Net {


/**
 * Constructs an AF_UNIX InetAddress for the given path.
 */
ECode InetUnixAddress::constructor(
    /* [in] */ const String& path)
{
    return InetAddress::constructor(OsConstants::_AF_UNIX, path.GetBytes(), String(NULL));
}

/**
 * Constructs an AF_UNIX InetAddress for the given path.
 */
ECode InetUnixAddress::constructor(
    /* [in] */ ArrayOf<Byte> * path)
{
    return InetAddress::constructor(OsConstants::_AF_UNIX, path, String(NULL));
}

/**
 * Returns a string form of this InetAddress.
 */
ECode InetUnixAddress::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("InetUnixAddress[") + String(*mIpAddress) + "]";
    return NOERROR;
}

} // namespace Net
} // namespace Elastos