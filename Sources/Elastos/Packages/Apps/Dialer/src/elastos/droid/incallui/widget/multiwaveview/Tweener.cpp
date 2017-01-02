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

#include "Elastos.Droid.Animation.h"
#include "Elastos.CoreLibrary.Core.h"
#include "elastos/droid/incallui/widget/multiwaveview/Tweener.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::IPropertyValuesHolder;
using Elastos::Droid::Animation::IPropertyValuesHolderHelper;
using Elastos::Droid::Animation::CPropertyValuesHolderHelper;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Core::IArrayOf;
using Elastos::Core::IInteger32;
using Elastos::Core::IFloat;
using Elastos::Core::INumber;
using Elastos::Core::IString;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::EIID_IFloat;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InCallUI {
namespace Widget {
namespace MultiwaveView {

//=====================================================================
//               Tweener::TweenerAnimatorListenerAdapter
//=====================================================================
ECode Tweener::TweenerAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    Tweener::Remove(animation);
    return NOERROR;
}

ECode Tweener::TweenerAnimatorListenerAdapter::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    Tweener::Remove(animation);
    return NOERROR;
}


//=====================================================================
//               Tweener
//=====================================================================
const String Tweener::TAG(String("Tweener"));
const Boolean Tweener::DEBUG = FALSE;
AutoPtr<IHashMap> Tweener::sTweens =  Init_sTweens();
AutoPtr<IAnimatorListener> Tweener::sCleanupListener = new Tweener::TweenerAnimatorListenerAdapter();

AutoPtr<IHashMap> Tweener::Init_sTweens()
{
    AutoPtr<IHashMap> map;
    CHashMap::New((IHashMap**)&map);
    return map;
}

Tweener::Tweener(
    /* [in] */ IObjectAnimator* anim)
    : mAnimator(anim)
{}

void Tweener::Remove(
    /* [in] */ IAnimator* animator)
{
    AutoPtr<ISet> st;
    sTweens->GetEntrySet((ISet**)&st);
    AutoPtr<IIterator> iter;
    st->GetIterator((IIterator**)&iter);
    Boolean hasNext = FALSE;
    while ((iter->HasNext(&hasNext), hasNext)) {
        AutoPtr<IInterface> node;
        iter->GetNext((IInterface**)&node);
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(node);
        AutoPtr<IInterface> val;
        entry->GetValue((IInterface**)&val);
        Tweener* cval = (Tweener*)IObject::Probe(val);
        if (IAnimator::Probe(cval->mAnimator) == animator) {
            if (DEBUG) {
                AutoPtr<IInterface> k;
                entry->GetKey((IInterface**)&k);
                AutoPtr<IInterface> p;
                sTweens->Get(k, (IInterface**)&p);
                Int32 s = 0;
                sTweens->GetSize(&s);
                Logger::V(TAG, "Removing tweener %s sTweens.size() = %d", TO_CSTR(p), s);
            }
            iter->Remove();
            break; // an animator can only be attached to one object
        }
    }
}

ECode Tweener::To(
    /* [in] */ IInterface* object,
    /* [in] */ Int64 duration,
    /* [in] */ ArrayOf<IInterface*>* vars,
    /* [out] */ Tweener** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Int64 delay = 0;
    AutoPtr<IAnimatorUpdateListener> updateListener;
    AutoPtr<IAnimatorListener> listener;
    AutoPtr<ITimeInterpolator> interpolator;

    // Iterate through arguments and discover properties to animate
    AutoPtr<IArrayList> props;
    CArrayList::New(vars->GetLength()/2, (IArrayList**)&props);
    for (Int32 i = 0; i < vars->GetLength(); i += 2) {
        if (IString::Probe((*vars)[i]) == NULL) {
            //throw new IllegalArgumentException("Key must be a string: " + vars[i]);
            Logger::E(TAG, "Key must be a string: %s", TO_CSTR((*vars)[i]));
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        String key;
        ICharSequence::Probe((*vars)[i])->ToString(&key);
        AutoPtr<IInterface> value = (*vars)[i + 1];

        if (key.Equals("simultaneousTween")) {
            // TODO
        }
        else if (key.Equals("ease")) {
            interpolator = ITimeInterpolator::Probe(value); // TODO: multiple interpolators?
        }
        else if (key.Equals("onUpdate") || key.Equals("onUpdateListener")) {
            updateListener = IAnimatorUpdateListener::Probe(value);
        }
        else if (key.Equals("onComplete") || key.Equals("onCompleteListener")) {
            listener = IAnimatorListener::Probe(value);
        }
        else if (key.Equals("delay")) {
            INumber::Probe(value)->Int64Value(&delay);
        }
        else if (key.Equals("syncWith")) {
            // TODO
        }
        else if (IArrayOf::Probe(value) != NULL) {
            IArrayOf* arrObj = IArrayOf::Probe(value);
            InterfaceID iid;
            arrObj->GetTypeId(&iid);
            if (iid == EIID_IFloat) {
                Float f0, f1;
                AutoPtr<IInterface> obj;
                arrObj->Get(0, (IInterface**)&obj);
                IFloat::Probe(obj)->GetValue(&f0);
                obj = NULL;
                arrObj->Get(1, (IInterface**)&obj);
                IFloat::Probe(obj)->GetValue(&f1);
                AutoPtr< ArrayOf<Float> > arr = ArrayOf<Float>::Alloc(2);
                (*arr)[0] = f0;
                (*arr)[1] = f1;
                AutoPtr<IPropertyValuesHolderHelper> hlp;
                CPropertyValuesHolderHelper::AcquireSingleton((IPropertyValuesHolderHelper**)&hlp);
                AutoPtr<IPropertyValuesHolder> pv;
                hlp->OfFloat(key, arr, (IPropertyValuesHolder**)&pv);
                props->Add(pv);
            }
            else if (iid == EIID_IInteger32) {
                Int32 i0, i1;
                AutoPtr<IInterface> obj;
                arrObj->Get(0, (IInterface**)&obj);
                IInteger32::Probe(obj)->GetValue(&i0);
                obj = NULL;
                arrObj->Get(1, (IInterface**)&obj);
                IInteger32::Probe(obj)->GetValue(&i1);
                AutoPtr< ArrayOf<Int32> > arr = ArrayOf<Int32>::Alloc(2);
                (*arr)[0] = i0;
                (*arr)[1] = i1;
                AutoPtr<IPropertyValuesHolderHelper> hlp;
                CPropertyValuesHolderHelper::AcquireSingleton((IPropertyValuesHolderHelper**)&hlp);
                AutoPtr<IPropertyValuesHolder> pv;
                hlp->OfInt32(key, arr, (IPropertyValuesHolder**)&pv);
                props->Add(pv);
            }
        }
        else if (INumber::Probe(value) != NULL) {
            Float floatValue = 0.0f;
            INumber::Probe(value)->FloatValue(&floatValue);
            AutoPtr<IPropertyValuesHolderHelper> hlp;
            CPropertyValuesHolderHelper::AcquireSingleton((IPropertyValuesHolderHelper**)&hlp);
            AutoPtr<ArrayOf<Float> > arr = ArrayOf<Float>::Alloc(1);
            (*arr)[0] = floatValue;
            AutoPtr<IPropertyValuesHolder> pv;
            hlp->OfFloat(key, arr, (IPropertyValuesHolder**)&pv);
            props->Add(pv);
        }
        else {
            // throw new IllegalArgumentException(
            //         "Bad argument for key \"" + key + "\" with value " + value.getClass());
            Logger::E(TAG, "Bad argument for key \"%s\" with value %s", key.string(), TO_CSTR(value));
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    // Re-use existing tween, if present
    AutoPtr<IInterface> pVal;
    sTweens->Get(object, (IInterface**)&pVal);
    AutoPtr<Tweener> tween = (Tweener*)IObject::Probe(pVal);
    AutoPtr<IObjectAnimator> anim;
    if (tween == NULL) {
        AutoPtr<ArrayOf<IPropertyValuesHolder*> > outArr;
        props->ToArray((ArrayOf<IInterface*>**)&outArr);
        AutoPtr<IObjectAnimatorHelper> helper;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
        helper->OfPropertyValuesHolder(object, outArr, (IObjectAnimator**)&anim);
        tween = new Tweener(anim);
        sTweens->Put(object, (IObject*)tween);
        if (DEBUG) {
            Logger::V(TAG, "Added new Tweener %s", TO_CSTR(tween));
        }
    }
    else {
        anim = tween->mAnimator;
        AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, object);
        Replace(props, args); // Cancel all animators for given object
    }

    if (interpolator != NULL) {
        IAnimator::Probe(anim)->SetInterpolator(interpolator);
    }

    // Update animation with properties discovered in loop above
    IAnimator::Probe(anim)->SetStartDelay(delay);
    IAnimator::Probe(anim)->SetDuration(duration);
    if (updateListener != NULL) {
        IValueAnimator::Probe(anim)->RemoveAllUpdateListeners(); // There should be only one
        IValueAnimator::Probe(anim)->AddUpdateListener(updateListener);
    }
    if (listener != NULL) {
        IAnimator::Probe(anim)->RemoveAllListeners(); // There should be only one.
        IAnimator::Probe(anim)->AddListener(listener);
    }
    IAnimator::Probe(anim)->AddListener(sCleanupListener);

    *result = tween;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Tweener::From(
    /* [in] */ IInterface* object,
    /* [in] */ Int64 duration,
    /* [in] */ ArrayOf<IInterface*>* vars,
    /* [out] */ Tweener** result)
{
    VALIDATE_NOT_NULL(result)
    // TODO:  for v of vars
    //            toVars[v] = object[v]
    //            object[v] = vars[v]
    return Tweener::To(object, duration, vars, result);
}

void Tweener::Reset()
{
    if (DEBUG) {
        Logger::V(TAG, "Reset()");
        Int32 s = 0;
        sTweens->GetSize(&s);
        if (s > 0) {
            Logger::V(TAG, "Cleaning up %d animations", s);
        }
    }
    sTweens->Clear();
}

void Tweener::Replace(
    /* [in] */ IArrayList* props,
    /* [in] */ ArrayOf<IInterface*>* args)
{
    Int32 s = args->GetLength();
    for (Int32 i = 0; i < s; i++) {
        AutoPtr<IInterface> killobject = (*args)[i];
        AutoPtr<IInterface> p;
        sTweens->Get(killobject, (IInterface**)&p);
        Tweener* tween = (Tweener*)IObject::Probe(p);
        if (tween != NULL) {
            IAnimator::Probe(tween->mAnimator)->Cancel();
            if (props != NULL) {
                AutoPtr<ArrayOf<IPropertyValuesHolder*> > outArr;
                props->ToArray((ArrayOf<IInterface*>**)&outArr);
                IValueAnimator::Probe(tween->mAnimator)->SetValues(outArr);
            }
            else {
                sTweens->Remove((IObject*)tween);
            }
        }
    }
}

} // namespace MultiwaveView
} // namespace Widget
} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

