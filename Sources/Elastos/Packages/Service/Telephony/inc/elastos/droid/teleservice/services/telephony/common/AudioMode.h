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

#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_COMMON_AUDIOMODE_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_COMMON_AUDIOMODE_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/core/StringBuffer.h>

using Elastos::Core::Object;
using Elastos::Core::StringBuffer;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {
namespace Common {

/**
 * Container class for audio modes.
 */
class AudioMode
    : public Object
{
public:
    TO_STRING_IMPL("AudioMode")

    static CARAPI_(String) ToString(
        /* [in] */ Int32 mode);

private:
    static CARAPI_(void) ListAppend(
        /* [in] */ StringBuffer* buffer,
        /* [in] */ const String& str);

public:
    // These can be used as a bit mask
    static Int32 EARPIECE;
    static Int32 BLUETOOTH;
    static Int32 WIRED_HEADSET;
    static Int32 SPEAKER;

    static Int32 WIRED_OR_EARPIECE;
    static Int32 ALL_MODES;
};

} // namespace Common
} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_COMMON_AUDIOMODE_H__