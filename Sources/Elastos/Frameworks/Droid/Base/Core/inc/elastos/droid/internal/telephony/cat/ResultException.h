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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_RESULTEXCEPTION_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_RESULTEXCEPTION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

/**
  * Class for errors in the Result object.
  *
  * {@hide}
  */
class ResultException
    : public Object
    , public IResultException
{
public:
    CAR_INTERFACE_DECL();

    ResultException();

    CARAPI constructor(
        /* [in] */ ResultCode result);

    CARAPI constructor(
        /* [in] */ ResultCode result,
        /* [in] */ const String& explanation);

    CARAPI constructor(
        /* [in] */ ResultCode result,
        /* [in] */ Int32 additionalInfo);

    CARAPI constructor(
        /* [in] */ ResultCode result,
        /* [in] */ Int32 additionalInfo,
        /* [in] */ const String& explanation);

    virtual CARAPI Result(
        /* [out] */ ResultCode* result);

    virtual CARAPI HasAdditionalInfo(
        /* [out] */ Boolean* result);

    virtual CARAPI AdditionalInfo(
        /* [out] */ Int32* result);

    virtual CARAPI Explanation(
        /* [out] */ String* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

private:
    ResultCode mResult;
    Int32 mAdditionalInfo;
    String mExplanation;
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_RESULTEXCEPTION_H__
