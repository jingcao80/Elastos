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

#ifndef  __ELASTOS_DROID_PHONE_CARRIERLOGO_H__
#define  __ELASTOS_DROID_PHONE_CARRIERLOGO_H__

#include "_Elastos.Droid.TeleService.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * Utility class to look up carrier logo resource IDs.
 */
class CarrierLogo
    : public Object
{
public:
    TO_STRING_IMPL("CarrierLogo")

    static CARAPI_(Int32) GetLogo(
        /* [in] */ const String& name);

private:
    /** This class is never instantiated. */
    CarrierLogo();

    static CARAPI_(AutoPtr<IMap>) GetLogoMap();

private:
    static AutoPtr<IMap> sLogoMap;

};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CARRIERLOGO_H__