
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/gesture/Learner.h"

namespace Elastos {
namespace Droid {
namespace Gesture {

CAR_INTERFACE_IMPL(Learner, Object, ILearner);

Learner::Learner()
{}

Learner::~Learner()
{}

ECode Learner::AddInstance(
    /* [in] */ IInstance *instance)
{
    mInstances->Add(instance);
    return NOERROR;
}

AutoPtr<IArrayList> Learner::GetInstances()
{
    return mInstances;
}

ECode Learner::RemoveInstance(
    /* [in] */ Int64 id)
{
    AutoPtr<IArrayList> instances = mInstances;
    Int32 size;

    instances->GetSize(&size);
    for (Int32 i = 0;  i < size;  i++) {
        AutoPtr<IInstance> instance;

        instances->Get(i, (IInterface **)&instance);

        /* TODO WAITING
        if (id == instance->mId) {
            instances->Remove(i);
            return NOERROR;
        }
        */
    }
    return NOERROR;
}

ECode Learner::RemoveInstances(
    /* [in] */ const String& name)
{
    AutoPtr<IArrayList> instances = mInstances;
    Int32 size;

    instances->GetSize(&size);
    for (Int32 i = 0;  i < size;  i++) {
        AutoPtr<IInstance> instance;

        instances->Get(i, (IInterface **)&instance);

        /* TODO WAITING
        if ((instance->mLabel.IsNull() && name.IsNull())
                || (!instance->mLabel.IsNull() && instance->mLabel.Equals(name))) {
            toDelete->PushBack(instance);
            it = instances->Erase(it);
        */
    }
    return NOERROR;
}

ECode Learner::GetInstances(
    /* [in] */ IArrayList** instances)
{
    *instances = mInstances;
    REFCOUNT_ADD(mInstances);
    return NOERROR;
}

CARAPI_(AutoPtr<IArrayList>) Classify(
    /* [in] */ Int32 sequenceType,
    /* [in] */ Int32 orientationType,
    /* [in] */ ArrayOf<Float> vec)
{
    return NULL;
}

ECode Learner::Classify(
    /* [in] */ Int32 sequenceType,
    /* [in] */ Int32 orientationType,
    /* [in] */ ArrayOf<Float> *vec,
    /* [out] */ IArrayList** predictions)
{
    return  NOERROR;
}

} // namespace Gesture
} // namespace Droid
} // namespace Elastos
