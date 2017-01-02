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

#ifndef __ELASTOS_DROID_GESTURE_INSTANCELEARNER_H__
#define __ELASTOS_DROID_GESTURE_INSTANCELEARNER_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/gesture/Learner.h"

using Elastos::Core::IComparator;

namespace Elastos {
namespace Droid {
namespace Gesture {

class InstanceLearner
    : public Learner
    , public IInstanceLearner
{
private:
    class PredictionComparator
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI Compare(
            /* [in] */ IInterface* object1,
            /* [in] */ IInterface* object2,
            /* [out] */ Int32* result);
    };

public:
    CAR_INTERFACE_DECL()

    InstanceLearner();

    virtual ~InstanceLearner();

    CARAPI constructor();

    CARAPI GetInstances(
        /* [in] */ IArrayList** instances);

    CARAPI_(AutoPtr<IArrayList>) Classify(
        /* [in] */ Int32 sequenceType,
        /* [in] */ Int32 orientationType,
        /* [in] */ ArrayOf<Float> *vec);

    CARAPI AddInstance(
        /* [in] */ IInstance *instance);

    CARAPI Classify(
        /* [in] */ Int32 sequenceType,
        /* [in] */ Int32 orientationType,
        /* [in] */ ArrayOf<Float> *vec,
        /* [out] */ IArrayList** predictions);
public:
    AutoPtr<IComparator> mComparator;
};

} // namespace Gesture
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GESTURE_INSTANCELEARNER_H__
