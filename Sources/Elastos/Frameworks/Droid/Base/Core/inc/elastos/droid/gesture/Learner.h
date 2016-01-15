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
    CAR_INTERFACE_DECL();

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
