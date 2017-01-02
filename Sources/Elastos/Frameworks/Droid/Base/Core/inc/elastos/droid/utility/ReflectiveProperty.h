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

#ifndef __ELASTOS_DROID_UTILITY_PROPERTYHELPER_H__
#define __ELASTOS_DROID_UTILITY_PROPERTYHELPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Core_server.h"

namespace Elastos {
namespace Droid {
namespace Utility {

class ReflectiveProperty
    : public IReflectiveProperty
    , public ElRefBase
{
public:
    CAR_INTERFACE_DECL()

    ReflectiveProperty(
        /* [in] */ ClassID clsType,
        /* [in] */ CarDataType dataType,
        /* [in] */ const String& name);

    CARAPI IsReadOnly(
        /* [out] */ Boolean* readOnly);

    CARAPI Set(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* value);

    CARAPI Get(
        /* [in] */ IInterface* obj,
        /* [out] */ IInterface** rst);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI GetType(
        /* [out] */ ClassID* info);

    CARAPI IsWriteOnly(
        /* [out] */ Boolean* writeOnly);

    CARAPI ForbiddenOperate(
        /* [out] */ Boolean* forbidden);

private:
    const static String PREFIX_GET;
    const static String PREFIX_IS;
    const static String PREFIX_SET;
    const static String TAG;
    AutoPtr<IMethodInfo> mSetter;
    AutoPtr<IMethodInfo> mGetter;
    String mName;
    ClassID mClsId;
    Boolean mReadOnly;
    Boolean mWriteOnly;
    Boolean mForbiddenOperate;
    CarDataType mDataType;
};
} // namespace Utility
} // namepsace Droid
} // namespace Elastos

#endif
