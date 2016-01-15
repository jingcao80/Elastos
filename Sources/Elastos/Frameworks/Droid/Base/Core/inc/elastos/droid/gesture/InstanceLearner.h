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
        CAR_INTERFACE_DECL();

        CARAPI Compare(
            /* [in] */ IInterface* object1,
            /* [in] */ IInterface* object2,
            /* [out] */ Int32* result);
    };

public:
    CAR_INTERFACE_DECL();

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
