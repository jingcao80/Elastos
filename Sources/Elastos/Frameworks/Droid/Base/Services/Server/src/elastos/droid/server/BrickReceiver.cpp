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

#include <Elastos.Droid.Os.h>
#include "elastos/droid/server/BrickReceiver.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Os::ISystemService;
using Elastos::Droid::Os::CSystemService;

namespace Elastos {
namespace Droid {
namespace Server {

ECode BrickReceiver::OnReceive(
    /* [in] */ IContext *context,
    /* [in] */ IIntent *intent)
{
    Slogger::W("BrickReceiver", "!!! BRICKING DEVICE !!!");
    AutoPtr<Elastos::Droid::Os::ISystemService> systemService;
    CSystemService::AcquireSingleton((Elastos::Droid::Os::ISystemService**)&systemService);
    return systemService->Start(String("brick"));
}

ECode BrickReceiver::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "BrickReceiver";
    return NOERROR;
}

} // Server
} // Droid
} // Elastos