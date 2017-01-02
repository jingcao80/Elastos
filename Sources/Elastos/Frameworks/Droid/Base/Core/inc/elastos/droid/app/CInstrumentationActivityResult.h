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

#ifndef __ELASTOS_DROID_APP_CINSTRUMENTATIONACTIVITYRESULT_H__
#define __ELASTOS_DROID_APP_CINSTRUMENTATIONACTIVITYRESULT_H__

#include "Elastos.Droid.Content.h"
#include "_Elastos_Droid_App_CInstrumentationActivityResult.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CInstrumentationActivityResult)
    , public Object
    , public IInstrumentationActivityResult
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CInstrumentationActivityResult();

    /**
     * Create a new activity result.  See {@link Activity#setResult} for
     * more information.
     *
     * @param resultCode The result code to propagate back to the
     * originating activity, often RESULT_CANCELED or RESULT_OK
     * @param resultData The data to propagate back to the originating
     * activity.
     */
    CARAPI constructor(
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* resultData);

    /**
     * Retrieve the result code contained in this result.
     */
    CARAPI GetResultCode(
        /* [out] */ Int32* resultCode);

    /**
     * Retrieve the data contained in this result.
     */
    CARAPI GetResultData(
        /* [out] */ IIntent** resultData);

private:
    Int32 mResultCode;
    AutoPtr<IIntent> mResultData;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CINSTRUMENTATIONACTIVITYRESULT_H__
