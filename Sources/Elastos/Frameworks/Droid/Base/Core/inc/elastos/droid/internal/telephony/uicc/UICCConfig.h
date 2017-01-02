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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_UICCCONFIG_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_UICCCONFIG_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

/**
  * A class that stores various UICC Settings/values.
  * @hide
  */
class UICCConfig
    : public Object
    , public IUICCConfig
{
public:
    CAR_INTERFACE_DECL();

    UICCConfig();

    /**
      * A method to get the stored Imsi.
      * @hide
      */
    virtual CARAPI GetImsi(
        /* [out] */ String* result);

    /**
      * A method to set the stored Imsi.
      * @hide
      */
    virtual CARAPI SetImsi(
        /* [in] */ const String& lImsi);

    /**
      * A method to get the stored MncLength.
      * @hide
      */
    virtual CARAPI GetMncLength(
        /* [out] */ Int32* result);

    /**
      * A method to set the stored MncLength.
      * @hide
      */
    virtual CARAPI SetMncLength(
        /* [in] */ Int32 lMncLength);

private:
    CARAPI_(void) Logd(
        /* [in] */ const String& sLog);

    CARAPI_(void) Loge(
        /* [in] */ const String& sLog);

private:
    /*const*/ String PREFERENCE_NAME;
    /*const*/ String TAG;
    /*const*/ Boolean LOG_DEBUG;
    String mImsi;
    Int32 mMncLength;
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_UICCCONFIG_H__

