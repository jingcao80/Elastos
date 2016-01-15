
#include "elastos/droid/internal/widget/multiwaveview/Tweener.h"
#include "elastos/droid/animation/CPropertyValuesHolderHelper.h"
#include "elastos/droid/animation/CObjectAnimatorHelper.h"

#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::IPropertyValuesHolderHelper;
using Elastos::Droid::Animation::CPropertyValuesHolderHelper;
using Elastos::Droid::Animation::IPropertyValuesHolder;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::IValueAnimator;

using Elastos::Core::INumber;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {
namespace Multiwaveview {

const String Tweener::TAG(String("Tweener"));
const Boolean Tweener::DEBUG = FALSE;

AutoPtr<IHashMap> Tweener::sTweens;

AutoPtr<IAnimatorListener> Tweener::mCleanupListener = new TweenerAnimatorListenerAdapter();
//=====================================================================
//               Tweener::TweenerAnimatorListenerAdapter::
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
//               Tweener::
//=====================================================================
CAR_INTERFACE_IMPL(Tweener, Object, ITweener)

Tweener::Tweener(
    /* [in] */ IObjectAnimator* anim)
{
    CHashMap::New((IHashMap**)&sTweens);
    mAnimator = anim;
}

void Tweener::Remove(
    /* [in] */ IAnimator* animator)
{
    AutoPtr<ISet> st;
    sTweens->GetEntrySet((ISet**)&st);
    AutoPtr<IIterator> iter;
    st->GetIterator((IIterator**)&iter);
    Boolean bHasNxt = FALSE;
    while ((iter->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> node;
        iter->GetNext((IInterface**)&node);
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(node);
        AutoPtr<IInterface> val;
        entry->GetValue((IInterface**)&val);
        AutoPtr<Tweener> cval = (Tweener*)ITweener::Probe(val);
        if (IInterface::Probe(cval->mAnimator) == IInterface::Probe(animator)) {
            if (DEBUG) {
                AutoPtr<IInterface> k;
                entry->GetKey((IInterface**)&k);
                AutoPtr<IInterface> p;
                sTweens->Get(k, (IInterface**)&p);
                Int32 s = 0;
                sTweens->GetSize(&s);
                Logger::V(TAG, "Removing tweener %p sTweens.size() = %d", p.Get(), s);
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
    /* [out] */ ITweener** result)
{
    VALIDATE_NOT_NULL(result)

    Int64 delay = 0;
    AutoPtr<IAnimatorUpdateListener> updateListener;
    AutoPtr<IAnimatorListener> listener;
    AutoPtr<ITimeInterpolator> interpolator;

    // Iterate through arguments and discover properties to animate
    AutoPtr<IArrayList> props;
    CArrayList::New(vars->GetLength()/2, (IArrayList**)&props);
    for (Int32 i = 0; i < vars->GetLength(); i += 2) {
        if (ICharSequence::Probe((*vars)[i]) == NULL) {
            //throw new IllegalArgumentException("Key must be a string: " + vars[i]);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        AutoPtr<ICharSequence> cs = ICharSequence::Probe((*vars)[i]);
        String key;
        cs->ToString(&key);
        AutoPtr<IInterface> value = (*vars)[i+1];
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
            AutoPtr<INumber> val = INumber::Probe(value);
            val->Int64Value(&delay);
        }
        else if (key.Equals("syncWith")) {
            // TODO
        }
        // else if (value instanceof float[]) {
        //     props.add(PropertyValuesHolder.ofFloat(key,
        //             ((float[])value)[0], ((float[])value)[1]));
        // }
        // else if (value instanceof int[]) {
        //     props.add(PropertyValuesHolder.ofInt(key,
        //             ((int[])value)[0], ((int[])value)[1]));
        // }
        else if (INumber::Probe(value) != NULL) {
            AutoPtr<INumber> val = INumber::Probe(value);
            Float floatValue = 0.0f;
            val->FloatValue(&floatValue);
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
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    // Re-use existing tween, if present
    AutoPtr<IInterface> pVal;
    sTweens->Get(object, (IInterface**)&pVal);
    AutoPtr<Tweener> tween = (Tweener*)ITweener::Probe(pVal);
    AutoPtr<IObjectAnimator> anim;
    if (tween == NULL) {
        Int32 s = 0;
        props->GetSize(&s);
        AutoPtr<ArrayOf<IPropertyValuesHolder*> > arr = ArrayOf<IPropertyValuesHolder*>::Alloc(s);
        AutoPtr<ArrayOf<IPropertyValuesHolder*> > outArr;
        props->ToArray((ArrayOf<IInterface*>*)arr.Get(), (ArrayOf<IInterface*>**)&outArr);
        AutoPtr<IObjectAnimatorHelper> helper;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
        helper->OfPropertyValuesHolder(object, outArr, (IObjectAnimator**)&anim);
        tween = new Tweener(anim);
        sTweens->Put(object, ITweener::Probe(tween));
        if (DEBUG) {
            Logger::V(TAG, "Added new Tweener %p", tween.Get());
        }
    }
    else {
        anim = tween->mAnimator;
        AutoPtr<ArrayOf<IInterface*> > pObjArr = ArrayOf<IInterface*>::Alloc(1);
        (*pObjArr)[0] = object;
        Replace(props, pObjArr); // Cancel all animators for given object
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
    IAnimator::Probe(anim)->AddListener(mCleanupListener);

    *result = tween;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Tweener::From(
    /* [in] */ IInterface* object,
    /* [in] */ Int64 duration,
    /* [in] */ ArrayOf<IInterface*>* vars,
    /* [out] */ ITweener** result)
{
    VALIDATE_NOT_NULL(result)
    // TODO:  for v of vars
    //            toVars[v] = object[v]
    //            object[v] = vars[v]
    return Tweener::To(object, duration, vars, result);
}

ECode Tweener::Reset()
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
    return NOERROR;
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
        AutoPtr<Tweener> tween = (Tweener*)ITweener::Probe(p);
        if (tween != NULL) {
            IAnimator::Probe(tween->mAnimator)->Cancel();
            if (props != NULL) {
                Int32 s = 0;
                props->GetSize(&s);
                AutoPtr<ArrayOf<IPropertyValuesHolder*> > arr = ArrayOf<IPropertyValuesHolder*>::Alloc(s);
                AutoPtr<ArrayOf<IPropertyValuesHolder*> > outArr;
                props->ToArray((ArrayOf<IInterface*>*)arr.Get(), (ArrayOf<IInterface*>**)&outArr);
                IValueAnimator::Probe(tween->mAnimator)->SetValues(outArr);
            }
            else {
                sTweens->Remove(ITweener::Probe(tween));
            }
        }
    }
}

} // namespace Multiwaveview
} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos
