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
        AutoPtr<IInterface> obj;
        instances->Get(i, (IInterface**)&obj);
        IInstance* instance = IInstance::Probe(obj);

        assert(0 && "TODO");
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
        AutoPtr<IInterface> obj;
        instances->Get(i, (IInterface**)&obj);
        IInstance* instance = IInstance::Probe(obj);

        assert(0 && "TODO");
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
    /* [out] */ IArrayList** instances)
{
    VALIDATE_NOT_NULL(instances)
    *instances = mInstances;
    REFCOUNT_ADD(*instances);
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
