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

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/IsimFileHandler.h"

#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                           IsimFileHandler
//=====================================================================
CAR_INTERFACE_IMPL(IsimFileHandler, IccFileHandler, IIsimFileHandler);

const String IsimFileHandler::LOGTAG("IsimFH");

IsimFileHandler::IsimFileHandler()
{
}

ECode IsimFileHandler::constructor(
    /* [in] */ IUiccCardApplication* app,
    /* [in] */ const String& aid,
    /* [in] */ ICommandsInterface* ci)
{
    return IccFileHandler::constructor(app, aid, ci);
}

String IsimFileHandler::GetEFPath(
    /* [in] */ Int32 efid)
{
    switch(efid) {
    case EF_IMPI:
    case EF_IMPU:
    case EF_DOMAIN:
    case EF_IST:
    case EF_PCSCF:
        return MF_SIM + DF_ADF;
    }
    String path = GetCommonIccEFPath(efid);
    return path;
}

ECode IsimFileHandler::Logd(
    /* [in] */ const String& msg)
{
    Logger::D(LOGTAG, msg);
    return NOERROR;
}

ECode IsimFileHandler::Loge(
    /* [in] */ const String& msg)
{
    Logger::E(LOGTAG, msg);
    return NOERROR;
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
