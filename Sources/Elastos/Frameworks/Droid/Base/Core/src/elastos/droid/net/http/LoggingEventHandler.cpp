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

#include "elastos/droid/net/http/LoggingEventHandler.h"
#include "elastos/droid/net/http/Headers.h"
#include "elastos/droid/net/http/HttpLog.h"
#include "elastos/droid/net/http/Request.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Net::Http::IHeaders;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Utility::ILog;

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

CAR_INTERFACE_IMPL_2(LoggingEventHandler, Object, ILoggingEventHandler, IEventHandler)

ECode LoggingEventHandler::constructor()
{
    return NOERROR;
}

ECode LoggingEventHandler::RequestSent()
{
    HttpLog::V(String("LoggingEventHandler:requestSent()"));
    return NOERROR;
}

ECode LoggingEventHandler::Status(
    /* [in] */ Int32 major_version,
    /* [in] */ Int32 minor_version,
    /* [in] */ Int32 code, /* Status-Code value */
    /* [in] */ const String& reason_phrase)
{
    if (HttpLog::LOGV) {
        HttpLog::V(String("LoggingEventHandler:status() major: ") + StringUtils::ToString(major_version) +
              String(" minor: ") + StringUtils::ToString(minor_version) +
              String(" code: ") + StringUtils::ToString(code) +
              String(" reason: ") + reason_phrase);
    }
    return NOERROR;
}

ECode LoggingEventHandler::Headers(
    /* [in] */ IHeaders* headers)
{
    if (HttpLog::LOGV) {
        HttpLog::V(String("LoggingEventHandler:headers()"));
        // HttpLog::V(headers->ToString());
    }
    return NOERROR;
}

ECode LoggingEventHandler::LocationChanged(
    /* [in] */ const String& newLocation,
    /* [in] */ Boolean permanent)
{
    if (HttpLog::LOGV) {
        HttpLog::V(String("LoggingEventHandler: locationChanged() ") + newLocation +
                  String(" permanent ") + StringUtils::BooleanToString(permanent));
    }
    return NOERROR;
}

ECode LoggingEventHandler::Data(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 len)
{
    if (HttpLog::LOGV) {
        HttpLog::V(String("LoggingEventHandler: data() ") + StringUtils::ToString(len) + String(" bytes"));
    }
    return NOERROR;
}

ECode LoggingEventHandler::EndData()
{
    if (HttpLog::LOGV) {
        HttpLog::V(String("LoggingEventHandler: endData() called"));
    }
    return NOERROR;
}

ECode LoggingEventHandler::Certificate(
    /* [in] */ ISslCertificate* certificate)
{
    if (HttpLog::LOGV) {
         HttpLog::V(String("LoggingEventHandler: certificate(): ") /*+ certificate->ToString()*/);
     }
    return NOERROR;
}

ECode LoggingEventHandler::Error(
    /* [in] */ Int32 id,
    /* [in] */ const String& description)
{
    if (HttpLog::LOGV) {
        HttpLog::V(String("LoggingEventHandler: error() called Id:") + StringUtils::ToString(id) +
                  String(" description ") + description);
    }
    return NOERROR;
}

ECode LoggingEventHandler::HandleSslErrorRequest(
    /* [in] */ ISslError* error,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (HttpLog::LOGV) {
        HttpLog::V(String("LoggingEventHandler: handleSslErrorRequest():") /*+ error->ToString()*/);
    }
    // return false so that the caller thread won't wait forever
    *result = FALSE;
    return NOERROR;
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
