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

#include "CPhoneNumberFormatter.h"
#include "PhoneNumberFormatter.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Util {

CAR_INTERFACE_IMPL(CPhoneNumberFormatter, Singleton, IPhoneNumberFormatter)

CAR_SINGLETON_IMPL(CPhoneNumberFormatter)

ECode CPhoneNumberFormatter::SetPhoneNumberFormattingTextWatcher(
    /* [in] */ IContext* context,
    /* [in] */ ITextView* textView)
{
    PhoneNumberFormatter::SetPhoneNumberFormattingTextWatcher(context, textView);
    return NOERROR;
}

} // Util
} // Common
} // Contacts
} // Apps
} // Elastos
