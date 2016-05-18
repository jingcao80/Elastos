#ifndef __ELASTOS_DROID_GESTURE_INSTANCE_H__
#define __ELASTOS_DROID_GESTURE_INSTANCE_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.Gesture.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Gesture::IGesture;
using Elastos::Droid::Gesture::IInstance;

namespace Elastos {
namespace Droid {
namespace Gesture {
/**
 * An instance represents a sample if the label is available or a query if the
 * label is null.
 */
class Instance
    : public Object
    , public IInstance
{
public:
    CAR_INTERFACE_DECL()

    Instance();

    virtual ~Instance();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int64 id,
        /* [in] */ ArrayOf<Float>* sample,
        /* [in] */ const String& sampleName);

    /**
     * create a learning instance for a single stroke gesture
     *
     * @param gesture
     * @param label
     * @return the instance
     */
    static CARAPI_(AutoPtr<Instance>) CreateInstance(
        /* [in] */ Int32 sequenceType,
        /* [in] */ Int32 orientationType,
        /* [in] */ IGesture* gesture,
        /* [in] */ const String& label);

    CARAPI CreateInstance(
        /* [in] */ Int32 sequenceType,
        /* [in] */ Int32 orientationType,
        /* [in] */ IGesture* gesture,
        /* [in] */ const String& label,
        /* [out] */ IInstance** obj);

    static CARAPI SpatialSampler(
        /* [in] */ IGesture* gesture,
        /* [out, callee] */ ArrayOf<Float>** sampler);

    static CARAPI_(AutoPtr<ArrayOf<Float> >) TemporalSampler(
        /* [in] */ Int32 orientationType,
        /* [in] */ IGesture* gesture);

    CARAPI Normalize();

public:
    // the feature vector
    AutoPtr<ArrayOf<Float> > mVector;

    // the label can be null
    String mLabel;

    // the id of the instance
    Int64 mId;

private:
    const static Int32 SEQUENCE_SAMPLE_SIZE = 16;

    const static Int32 PATCH_SAMPLE_SIZE = 16;

    const static AutoPtr<ArrayOf<Float> > ORIENTATIONS;
};

} // namespace Gesture
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GESTURE_INSTANCE_H__
