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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCSERVICETABLE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCSERVICETABLE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// import android.telephony.Rlog;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

/**
  * Wrapper class for an ICC EF containing a bit field of enabled services.
  */
class IccServiceTable
    : public Object
    , public IIccServiceTable
{
public:
    CAR_INTERFACE_DECL();

    IccServiceTable();

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* table);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

protected:
    // Get the class name to use for log strings
    virtual CARAPI_(String) GetTag() = 0;

    // Get the array of enums to use for toString
    virtual CARAPI_(AutoPtr<ArrayOf<IInterface*> >) GetValues() = 0;

    /**
      * Returns if the specified service is available.
      * @param service the service number as a zero-based offset (the enum ordinal)
      * @return true if the service is available; false otherwise
      */
    virtual CARAPI_(Boolean) IsAvailable(
        /* [in] */ Int32 service);

protected:
    /*const*/ AutoPtr< ArrayOf<Byte> > mServiceTable;
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCSERVICETABLE_H__

