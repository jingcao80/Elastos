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

#ifndef __ELASTOS_DROID_PHONE_COMMON_PHONECONSTANTS_H__
#define __ELASTOS_DROID_PHONE_COMMON_PHONECONSTANTS_H__

#include "Elastos.Droid.Phone.Common.h"

namespace Elastos {
namespace Droid {
namespace Phone {
namespace Common {

/**
 * Constants related to phone operations.
 */
class PhoneConstants
{
public:
    /** Copied from PhoneApp. See comments in Phone app for more detail. */
    // TODO: Change phone app to rely on this also.
    static const String EXTRA_CALL_ORIGIN;
};

} // namespace Common
} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_COMMON_PHONECONSTANTS_H__
