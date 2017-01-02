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

#include "elastos/droid/teleservice/phone/ImsUtil.h"

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

Boolean ImsUtil::sImsPhoneSupported = FALSE;

AutoPtr<PhoneGlobals> ImsUtil::GetApp()
{
    if (mApp != NULL) {
        sImsPhoneSupported = TRUE;
        return mApp;
    }
    else {
        AutoPtr<PhoneGlobals> app;
        PhoneGlobals::GetInstance((PhoneGlobals**)&app);
        sImsPhoneSupported = TRUE;
        return app;
    }
}

//AutoPtr<PhoneGlobals> ImsUtil::mApp = GetApp();

Boolean ImsUtil::IsImsPhoneSupported()
{
    GetApp();
    return sImsPhoneSupported;
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos
