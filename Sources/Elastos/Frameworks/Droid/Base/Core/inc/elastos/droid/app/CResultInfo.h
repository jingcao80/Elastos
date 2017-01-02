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

#ifndef __ELASTOS_DROID_APP_CRESULTINFO_H__
#define __ELASTOS_DROID_APP_CRESULTINFO_H__

#include "_Elastos_Droid_App_CResultInfo.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CResultInfo)
    , public Object
    , public IResultInfo
    , public IParcelable
{
public:
    CResultInfo();

    virtual ~CResultInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& resultWho,
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

    CARAPI GetResultWho(
        /* [out] */ String* who);

    CARAPI SetResultWho(
        /* [in] */ const String& who);

    CARAPI GetRequestCode(
        /* [out] */ Int32* code);

    CARAPI SetRequestCode(
        /* [in] */ Int32 code);

    CARAPI GetResultCode(
        /* [out] */ Int32* code);

    CARAPI SetResultCode(
        /* [in] */ Int32 code);

    CARAPI GetData(
        /* [out] */ IIntent** data);

    CARAPI SetData(
        /* [in] */ IIntent* data);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

public:
    String mResultWho;
    Int32 mRequestCode;
    Int32 mResultCode;
    AutoPtr<IIntent> mData;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CRESULTINFO_H__
