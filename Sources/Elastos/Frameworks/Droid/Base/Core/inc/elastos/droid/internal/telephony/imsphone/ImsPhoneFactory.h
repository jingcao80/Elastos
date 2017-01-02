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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_IMSPHONEFACTORY_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_IMSPHONEFACTORY_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::IPhoneNotifier;
using Elastos::Droid::Internal::Telephony::IPhone;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace ImsPhone {

/**
  * {@hide}
  */
class ImsPhoneFactory
    : public Object
{
public:
    CAR_INTERFACE_DECL();

    /**
      * Makes a {@link ImsPhone} object.
      * @param context {@code Context} needed to create a Phone object
      * @param phoneNotifier {@code PhoneNotifier} needed to create a Phone
      *      object
      * @return the {@code ImsPhone} object
      */
    static CARAPI_(AutoPtr<IImsPhone>) MakePhone(
        /* [in] */ IContext* context,
        /* [in] */ IPhoneNotifier* phoneNotifier,
        /* [in] */ IPhone* defaultPhone);
};

} // namespace Imsphone
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_IMSPHONEFACTORY_H__

