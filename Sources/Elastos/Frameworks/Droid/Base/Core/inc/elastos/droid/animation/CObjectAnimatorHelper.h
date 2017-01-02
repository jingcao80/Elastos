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

#ifndef __ELASTOS_DROID_ANIMATION_COBJECTANIMATORHELPER_H__
#define __ELASTOS_DROID_ANIMATION_COBJECTANIMATORHELPER_H__

#include "_Elastos_Droid_Animation_CObjectAnimatorHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;
using Elastos::Droid::Utility::IProperty;

namespace Elastos {
namespace Droid {
namespace Animation {

CarClass(CObjectAnimatorHelper)
    , public Singleton
    , public IObjectAnimatorHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI OfInt32(
        /* [in] */ IInterface* target,
        /* [in] */ const String& propertyName,
        /* [in] */ ArrayOf<Int32>* values,
        /* [out] */ IObjectAnimator** anim);

    CARAPI OfInt32(
        /* [in] */ IInterface* target,
        /* [in] */ IProperty* property,
        /* [in] */ ArrayOf<Int32>* values,
        /* [out] */ IObjectAnimator** anim);

    CARAPI OfFloat(
        /* [in] */ IInterface* target,
        /* [in] */ const String& propertyName,
        /* [in] */ ArrayOf<Float>* values,
        /* [out] */ IObjectAnimator** anim);

    CARAPI OfFloat(
        /* [in] */ IInterface* target,
        /* [in] */ IProperty* property,
        /* [in] */ ArrayOf<Float>* values,
        /* [out] */ IObjectAnimator** anim);

    CARAPI OfObject(
        /* [in] */ IInterface* target,
        /* [in] */ const String& propertyName,
        /* [in] */ ITypeEvaluator* evaluator,
        /* [in] */ ArrayOf<IInterface*>* values,
        /* [out] */ IObjectAnimator** anim);

    CARAPI OfObject(
        /* [in] */ IInterface* target,
        /* [in] */ IProperty* property,
        /* [in] */ ITypeEvaluator* evaluator,
        /* [in] */ ArrayOf<IInterface*>* values,
        /* [out] */ IObjectAnimator** anim);

    CARAPI OfPropertyValuesHolder(
        /* [in] */ IInterface* target,
        /* [in] */ ArrayOf<IPropertyValuesHolder*>* values,
        /* [out] */ IObjectAnimator** anim);
};

} // namespace Animation
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_ANIMATION_COBJECTANIMATORHELPER_H__
