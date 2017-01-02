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
#include "elastos/droid/internal/telephony/uicc/IccFileNotFound.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                           IccFileNotFound
//=====================================================================
CAR_INTERFACE_IMPL(IccFileNotFound, IccException, IIccFileNotFound);

IccFileNotFound::IccFileNotFound()
{
}

IccFileNotFound::IccFileNotFound(
    /* [in] */ const String& s)
    : IccException(s)
{
}

IccFileNotFound::IccFileNotFound(
    /* [in] */ Int32 ef)
    : IccException(String("ICC EF Not Found 0x") + StringUtils::ToString(ef))
{
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos


