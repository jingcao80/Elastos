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

#include "Elastos.Droid.Net.h"
#include "elastos/droid/content/CIntentFilterAuthorityEntry.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL_2(CIntentFilterAuthorityEntry, Object, IIntentFilterAuthorityEntry, IParcelable)

CAR_OBJECT_IMPL(CIntentFilterAuthorityEntry)

CIntentFilterAuthorityEntry::CIntentFilterAuthorityEntry()
    : mWild(FALSE)
    , mPort(0)
{}

CIntentFilterAuthorityEntry::~CIntentFilterAuthorityEntry()
{}

ECode CIntentFilterAuthorityEntry::constructor()
{
    return NOERROR;
}

ECode CIntentFilterAuthorityEntry::constructor(
    /* [in] */ const String& host,
    /* [in] */ const String& port)
{
    mOrigHost = host;
    mWild = !host.IsNullOrEmpty() && host.GetChar(0) == '*';
    mHost = mWild ? host.Substring(1) : host;
    mPort = !port.IsNull() ? StringUtils::ParseInt32(port) : -1;
    return NOERROR;
}

ECode CIntentFilterAuthorityEntry::GetHost(
    /* [out] */ String* host)
{
    VALIDATE_NOT_NULL(host)
    *host = mOrigHost;
    return NOERROR;
}

ECode CIntentFilterAuthorityEntry::GetPort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port)
    *port = mPort;
    return NOERROR;
}

ECode CIntentFilterAuthorityEntry::GetWild(
    /* [out] */ Boolean* wild)
{
    VALIDATE_NOT_NULL(wild)
    *wild = mWild;
    return NOERROR;
}

ECode CIntentFilterAuthorityEntry::Match(
    /* [in] */ IIntentFilterAuthorityEntry* otherObj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    CIntentFilterAuthorityEntry* other = (CIntentFilterAuthorityEntry*)otherObj;
    if (mWild != other->mWild) {
        return NOERROR;
    }
    if (!mHost.Equals(other->mHost)) {
        return NOERROR;
    }
    if (mPort != other->mPort) {
        return NOERROR;
    }

    *result = TRUE;
    return NOERROR;
}

ECode CIntentFilterAuthorityEntry::Match(
    /* [in] */ IUri* data,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IIntentFilter::NO_MATCH_DATA;

    String host;
    FAIL_RETURN(data->GetHost(&host))

    if (host.IsNull()) {
        return NOERROR;
    }

    if (FALSE) {
        Logger::V("IntentFilter", "Match host [%s] : [%s]", host.string(), mHost.string());
    }

    if (mWild) {
        if (host.GetLength() < mHost.GetLength()) {
            return NOERROR;
        }
        host = host.Substring(host.GetLength() - mHost.GetLength());
    }

    if (host.CompareIgnoreCase(mHost) != 0) {
        return NOERROR;
    }

    if (mPort >= 0) {
        Int32 port;
        FAIL_RETURN(data->GetPort(&port));
        if (mPort != port) {
            return NOERROR;
        }

        *result = IIntentFilter::MATCH_CATEGORY_PORT;
        return NOERROR;
    }

    *result = IIntentFilter::MATCH_CATEGORY_HOST;
    return NOERROR;
}

ECode CIntentFilterAuthorityEntry::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    FAIL_RETURN(source->ReadString(&mOrigHost))
    FAIL_RETURN(source->ReadString(&mHost))
    FAIL_RETURN(source->ReadBoolean(&mWild))
    FAIL_RETURN(source->ReadInt32(&mPort))
    return NOERROR;
}

ECode CIntentFilterAuthorityEntry::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    FAIL_RETURN(dest->WriteString(mOrigHost))
    FAIL_RETURN(dest->WriteString(mHost))
    FAIL_RETURN(dest->WriteBoolean(mWild))
    FAIL_RETURN(dest->WriteInt32(mPort))
    return NOERROR;
}

}
}
}

