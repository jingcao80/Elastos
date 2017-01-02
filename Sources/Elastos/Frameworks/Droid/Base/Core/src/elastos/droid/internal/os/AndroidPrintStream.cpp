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

#include "elastos/droid/internal/os/AndroidPrintStream.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

AndroidPrintStream::AndroidPrintStream(
    /* [in] */ Int32 priority,
    /* [in] */ const String& tag)
    : mPriority(priority)
    , mTag(tag)
{
    if (tag == NULL) {
        // throw new NullPointerException("tag");
        assert(0);
    }
}

void AndroidPrintStream::Log(
    /* [in] */ const String& line)
{
    Logger::Println(mPriority, mTag, line);
}

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos
