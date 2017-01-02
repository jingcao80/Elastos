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

#include <Elastos.Droid.Content.h>
#include "elastos/droid/server/wm/WindowBinder.h"
#include <elastos/utility/logging/Slogger.h>
#include <binder/Binder.h>
#include <binder/IServiceManager.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

WindowBinder::WindowBinder()
{
    mNativeBinder = new android::BBinder();
    mNativeBinder->incStrong(NULL);
}

WindowBinder::~WindowBinder()
{
    assert(mNativeBinder != NULL);
    mNativeBinder->decStrong(NULL);
}

ECode WindowBinder::Register()
{
    assert(mNativeBinder != NULL);
    android::sp<android::IServiceManager> sm = android::defaultServiceManager();
    int res = sm->addService(android::String16(IContext::WINDOW_SERVICE.string()), mNativeBinder);
    if (res != 0) {
        Slogger::E("WindowBinder", "add service window failed");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Wm
} // namespace Server
} // namespace Droid
} // namespace Elastos