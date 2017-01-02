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

#ifndef __ELASTOS_DROID_GESTURE_LEARNER_H__
#define __ELASTOS_DROID_GESTURE_LEARNER_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/gesture/Instance.h"
#include "elastos/core/Object.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Utility::IArrayList;
using Elastos::Droid::Gesture::ILearner;
using Elastos::Droid::Gesture::IInstance;


namespace Elastos {
namespace Droid {
namespace Gesture {
/**
 * The abstract class of a gesture learner
 */
class Learner
    : public Object
    , public ILearner
{
public:
    CAR_INTERFACE_DECL()

    Learner();

    virtual ~Learner();

    /**
     * Add an instance to the learner
     *
     * @param instance
     */
    CARAPI AddInstance(
        /* [in] */ IInstance *instance);

    /**
     * Retrieve all the instances
     *
     * @return instances
     */
    CARAPI_(AutoPtr<IArrayList>) GetInstances();

    CARAPI GetInstances(
        /* [in] */ IArrayList** instances);

    /**
     * Remove an instance based on its id
     *
     * @param id
     */
    CARAPI RemoveInstance(
        /* [in] */ Int64 id);

    /**
     * Remove all the instances of a category
     *
     * @param name the category name
     */
    CARAPI RemoveInstances(
        /* [in] */ const String& name);

    CARAPI_(AutoPtr<IArrayList>) Classify(
        /* [in] */ Int32 sequenceType,
        /* [in] */ Int32 orientationType,
        /* [in] */ ArrayOf<Float> *vec);

    CARAPI Classify(
        /* [in] */ Int32 sequenceType,
        /* [in] */ Int32 orientationType,
        /* [in] */ ArrayOf<Float> *vec,
        /* [out] */ IArrayList** predictions);
public:
    AutoPtr<IArrayList> mInstances;
};

} // namespace Gesture
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GESTURE_LEARNER_H__
