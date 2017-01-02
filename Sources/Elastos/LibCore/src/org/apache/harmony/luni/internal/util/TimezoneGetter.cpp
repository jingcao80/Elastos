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

#include "org/apache/harmony/luni/internal/util/TimezoneGetter.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Luni {
namespace Internal {
namespace Util {

AutoPtr<ITimezoneGetter> TimezoneGetter::instance;
CAR_INTERFACE_IMPL(TimezoneGetter, Object, ITimezoneGetter)

AutoPtr<ITimezoneGetter> TimezoneGetter::GetInstance()
{
    return instance;
}

ECode TimezoneGetter::SetInstance(
    /* [in] */ ITimezoneGetter* getter)
{
    if (instance != NULL) {
        //throw new UnsupportedOperationException("TimezoneGetter instance already set");
        Logger::E("TimezoneGetter", "TimezoneGetter instance already set");
        assert(0);
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    instance = getter;
    return NOERROR;
}

} // namespace Util
} // namespace Internal
} // namespace Luni
} // namespace Harmony
} // namespace Apache
} // namespace Org
