
#include "elastos/droid/animation/ObjectAnimator.h"
#include "elastos/droid/animation/CObjectAnimator.h"
#include "elastos/droid/animation/PropertyValuesHolder.h"
#include "elastos/droid/animation/PathKeyframes.h"
#include "elastos/droid/animation/CArgbEvaluator.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Animation {

const String ObjectAnimator::LOGTAG("ObjectAnimator");

CAR_INTERFACE_IMPL(ObjectAnimator, ValueAnimator, IObjectAnimator);

ObjectAnimator::ObjectAnimator()
    : mAutoCancel(FALSE)
{}

ECode ObjectAnimator::SetPropertyName(
    /* [in] */ const String& propertyName)
{
    // mValues could be NULL if this is being constructed piecemeal. Just record the
    // propertyName to be used later when setValues() is called if so.
    if (mValues != NULL) {
        AutoPtr<IPropertyValuesHolder> valuesHolder = (*mValues)[0];
        String oldName;
        valuesHolder->GetPropertyName(&oldName);

        valuesHolder->SetPropertyName(propertyName);
        mValuesMap.Erase(oldName);
        mValuesMap[propertyName] = valuesHolder;
    }

    mPropertyName = propertyName;
    // New property/values/target should cause re-initialization prior to starting
    mInitialized = FALSE;

    return NOERROR;
}

ECode ObjectAnimator::SetProperty(
    /* [in] */ IProperty* property)
{
    // mValues could be NULL if this is being constructed piecemeal. Just record the
    // propertyName to be used later when setValues() is called if so.
    if (mValues != NULL) {
        AutoPtr<IPropertyValuesHolder> valuesHolder = (*mValues)[0];
        String oldName;
        valuesHolder->GetPropertyName(&oldName);
        valuesHolder->SetProperty(property);
        mValuesMap.Erase(oldName);
        mValuesMap[mPropertyName] = valuesHolder;
    }

    if (mProperty != NULL) {
        property->GetName(&mPropertyName);
    }

    mProperty = property;
    // New property/values/target should cause re-initialization prior to starting
    mInitialized = FALSE;

    return NOERROR;
}

ECode ObjectAnimator::GetPropertyName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    String propertyName;
    if (mPropertyName != NULL) {
        propertyName = mPropertyName;
    }
    else if (mProperty != NULL) {
        mProperty->GetName(&propertyName);
    }
    else if (mValues != NULL && mValues->GetLength() > 0) {
        for (Int32 i = 0; i < mValues->GetLength(); ++i) {
            if (i == 0) {
                propertyName = String("");
            }
            else {
                propertyName += String(",");
            }

            String tmp;
            (*mValues)[i]->GetPropertyName(&tmp);
            propertyName += tmp;
        }
    }
    *name = propertyName;
    return NOERROR;
}

String ObjectAnimator::GetNameForTrace()
{
    String name;
    GetPropertyName(&name);
    StringBuilder sb("animator:");
    sb += name;
    return sb.ToString();
}

ECode ObjectAnimator::constructor(
    /* [in] */ IInterface* target,
    /* [in] */ const String& propertyName)
{
    SetTarget(target);
    return SetPropertyName(propertyName);
}

ECode ObjectAnimator::constructor(
    /* [in] */ IInterface* target,
    /* [in] */ IProperty* property)
{
    SetTarget(target);
    return SetProperty(property);
}

ECode ObjectAnimator::SetInt32Values(
    /* [in] */ ArrayOf<Int32>* values)
{
    if (mValues == NULL || mValues->GetLength() == 0) {
        // No values yet - this animator is being constructed piecemeal. Init the values with
        // whatever the current propertyName is
        AutoPtr<ArrayOf<IPropertyValuesHolder*> > params =ArrayOf<IPropertyValuesHolder*>::Alloc(1);
        if (mProperty != NULL) {
            params->Set(0, PropertyValuesHolder::OfInt32(mProperty, values));
            SetValues(params);
        } else {
            params->Set(0, PropertyValuesHolder::OfInt32(mPropertyName, values));
            SetValues(params);
        }
    } else {
        ValueAnimator::SetInt32Values(values);
    }

    return NOERROR;
}

ECode ObjectAnimator::SetFloatValues(
    /* [in] */ ArrayOf<Float>* values)
{
    if (mValues == NULL || mValues->GetLength() == 0) {
        // No values yet - this animator is being constructed piecemeal. Init the values with
        // whatever the current propertyName is
        AutoPtr<ArrayOf<IPropertyValuesHolder*> > params =ArrayOf<IPropertyValuesHolder*>::Alloc(1);
        if (mProperty != NULL) {
            AutoPtr<IPropertyValuesHolder> holder = PropertyValuesHolder::OfFloat(mProperty, values);
            params->Set(0, holder);
            SetValues(params);
        }
        else {
            AutoPtr<IPropertyValuesHolder> holder = PropertyValuesHolder::OfFloat(mPropertyName, values);
            params->Set(0, holder);
            SetValues(params);
        }
    }
    else {
        ValueAnimator::SetFloatValues(values);
    }

    return NOERROR;
}

ECode ObjectAnimator::SetObjectValues(
    /* [in] */ ArrayOf<IInterface*>* values)
{
    if (mValues == NULL || mValues->GetLength() == 0) {
        // No values yet - this animator is being constructed piecemeal. Init the values with
        // whatever the current propertyName is
        AutoPtr<ArrayOf<IPropertyValuesHolder*> > params =ArrayOf<IPropertyValuesHolder*>::Alloc(1);
        if (mProperty != NULL) {
            AutoPtr<IPropertyValuesHolder> holder = PropertyValuesHolder::OfObject(mProperty, NULL, values);
            params->Set(0, holder);
            SetValues(params);
        } else {
            AutoPtr<IPropertyValuesHolder> holder = PropertyValuesHolder::OfObject(mPropertyName, NULL, values);
            params->Set(0, holder);
            SetValues(params);
        }
    } else {
        ValueAnimator::SetObjectValues(values);
    }

    return NOERROR;
}

ECode ObjectAnimator::Start()
{
    // See if any of the current active/pending animators need to be canceled
    AutoPtr<AnimationHandler> handler = (AnimationHandler*)pthread_getspecific(sAnimationHandler);
    if (handler != NULL) {
        List<AutoPtr<IValueAnimator> >::ReverseIterator it1 = handler->mAnimations.RBegin();
        for (; it1 != handler->mAnimations.REnd(); ++it1) {
            AutoPtr<IObjectAnimator> anim = IObjectAnimator::Probe(*it1);
            if (anim) {
                if (((ObjectAnimator*)anim.Get())->mAutoCancel && HasSameTargetAndProperties(IAnimator::Probe(anim))) {
                    IAnimator::Probe(anim)->Cancel();
                }
            }
        }

        List<AutoPtr<IValueAnimator> >::ReverseIterator it2 = handler->mPendingAnimations.RBegin();
        for (; it2 != handler->mPendingAnimations.REnd(); ++it2) {
            AutoPtr<IObjectAnimator> anim = IObjectAnimator::Probe(*it2);
            if (anim) {
                if (((ObjectAnimator*)anim.Get())->mAutoCancel && HasSameTargetAndProperties(IAnimator::Probe(anim))) {
                    IAnimator::Probe(anim)->Cancel();
                }
            }
        }

        List<AutoPtr<IValueAnimator> >::ReverseIterator it3 = handler->mDelayedAnims.RBegin();
        for (; it3 != handler->mDelayedAnims.REnd(); ++it3) {
            AutoPtr<IObjectAnimator> anim = IObjectAnimator::Probe(*it3);
            if (anim) {
                if (((ObjectAnimator*)anim.Get())->mAutoCancel && HasSameTargetAndProperties(IAnimator::Probe(anim))) {
                    IAnimator::Probe(anim)->Cancel();
                }
            }
        }
    }
    // if (DBG) {
    //     Log.d(LOGTAG, "Anim target, duration: " + getTarget() + ", " + getDuration());
    //     for (int i = 0; i < mValues.length; ++i) {
    //         PropertyValuesHolder pvh = mValues[i];
    //         Log.d(LOGTAG, "   Values[" + i + "]: " +
    //             pvh.getPropertyName() + ", " + pvh.mKeyframes.getValue(0) + ", " +
    //             pvh.mKeyframes.getValue(1));
    //     }
    // }
    return ValueAnimator::Start();
}

ECode ObjectAnimator::InitAnimation()
{
    if (!mInitialized) {
        // mValueType may change due to setter/getter setup; do this before calling super.init(),
        // which uses mValueType to set up the default type evaluator.
        AutoPtr<IInterface> target;
        GetTarget((IInterface**)&target);
        if (target != NULL) {
            const Int32 numValues = mValues->GetLength();
            for (Int32 i = 0; i < numValues; ++i) {
                ((PropertyValuesHolder*)((*mValues)[i]))->SetupSetterAndGetter(target);
            }
        }

        ValueAnimator::InitAnimation();
    }
    return NOERROR;
}

ECode ObjectAnimator::SetDuration(
    /* [in] */ Int64 duration)
{
    ValueAnimator::SetDuration(duration);
    return NOERROR;
}

ECode ObjectAnimator::GetTarget(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    *object = NULL;
    if (mTarget == NULL) {
        return NOERROR;
    }

    return mTarget->Resolve(EIID_IInterface, object);
}

ECode ObjectAnimator::SetTarget(
    /* [in] */ IInterface* target)
{
    AutoPtr<IInterface> oldTarget;
    GetTarget((IInterface**)&oldTarget);
    if (oldTarget != IInterface::Probe(target)) {
        mTarget = NULL;
        if (target != NULL) {
            IWeakReferenceSource::Probe(target)->GetWeakReference((IWeakReference**)&mTarget);
        }
        // New target should cause re-initialization prior to starting
        mInitialized = FALSE;
    }

    return NOERROR;
}

ECode ObjectAnimator::SetupStartValues()
{
    InitAnimation();
    AutoPtr<IInterface> target;
    GetTarget((IInterface**)&target);
    if (target != NULL) {
        Int32 numValues = mValues->GetLength();
        for (Int32 i = 0; i < numValues; ++i) {
            AutoPtr<IPropertyValuesHolder> holder = (*mValues)[i];
            ((PropertyValuesHolder*)holder.Get())->SetupStartValue(target);
        }
    }

    return NOERROR;
}

ECode ObjectAnimator::SetupEndValues()
{
    InitAnimation();

    AutoPtr<IInterface> target;
    GetTarget((IInterface**)&target);
    if (target != NULL) {
        Int32 numValues = mValues->GetLength();
        for (Int32 i = 0; i < numValues; ++i) {
            AutoPtr<IPropertyValuesHolder> holder = (*mValues)[i];
            ((PropertyValuesHolder*)holder.Get())->SetupEndValue(target);
        }
    }

    return NOERROR;
}

ECode ObjectAnimator::AnimateValue(
    /* [in] */ Float fraction)
{
    AutoPtr<IInterface> target;
    GetTarget((IInterface**)&target);
    if (mTarget != NULL && target == NULL) {
        // We lost the target reference, cancel and clean up.
        Cancel();
        return NOERROR;
    }

    ValueAnimator::AnimateValue(fraction);
    Int32 numValues = mValues->GetLength();
    for (Int32 i = 0; i < numValues; ++i) {
        AutoPtr<IPropertyValuesHolder> holder = (*mValues)[i];
        PropertyValuesHolder* ptr = (PropertyValuesHolder*)holder.Get();
        ptr->SetAnimatedValue(target);
    }
    return NOERROR;
}

ECode ObjectAnimator::Clone(
    /* [out] */ IInterface** object)
{
    AutoPtr<CObjectAnimator> newObject;
    CObjectAnimator::NewByFriend((CObjectAnimator**)&newObject);
    CloneSuperData(newObject.Get());
    CloneInternal(newObject);
    ObjectAnimator* anim = newObject;
    anim->mTarget = mTarget;
    anim->mPropertyName = mPropertyName;
    anim->mProperty = mProperty;
    *object = newObject->Probe(EIID_IObjectAnimator);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

static ECode CheckIsCarObject(
    /* [in] */ IInterface* target)
{
#if defined(_DEBUG)
    assert(target != NULL);
    ClassID objId;
    ECode ec = IObject::Probe(target)->GetClassID(&objId);
    if (FAILED(ec)) {
        Logger::E(LOGTAG, "Target object [%s] is not a CAR object!", TO_CSTR(target));
        assert(0 && "Target object is not a CAR object!");
    }
    return ec;
#endif
}

AutoPtr<IObjectAnimator> ObjectAnimator::OfInt32(
    /* [in] */ IInterface* target,
    /* [in] */ const String& propertyName,
    /* [in] */ ArrayOf<Int32>* values)
{
    if (FAILED(CheckIsCarObject(target))) {
        return NULL;
    }

    AutoPtr<CObjectAnimator> anim;
    CObjectAnimator::NewByFriend((CObjectAnimator**)&anim);
    anim->constructor(target, propertyName);
    anim->SetInt32Values(values);
    return anim;
}

AutoPtr<IObjectAnimator> ObjectAnimator::OfInt32(
    /* [in] */ IInterface* target,
    /* [in] */ IProperty* property,
    /* [in] */ ArrayOf<Int32>* values)
{
    if (FAILED(CheckIsCarObject(target))) {
        return NULL;
    }

    AutoPtr<CObjectAnimator> anim;
    CObjectAnimator::NewByFriend((CObjectAnimator**)&anim);
    anim->constructor(target, property);
    anim->SetInt32Values(values);
    return anim;
}

AutoPtr<IObjectAnimator> ObjectAnimator::OfInt32(
    /* [in] */ IObject* target,
    /* [in] */ const String& xPropertyName,
    /* [in] */ const String& yPropertyName,
    /* [in] */ IPath* path)
{
    if (FAILED(CheckIsCarObject(target))) {
        return NULL;
    }

    AutoPtr<IPathKeyframes> keyframes = KeyframeSet::OfPath(path);
    AutoPtr<IPropertyValuesHolder> x = PropertyValuesHolder::OfKeyframes(xPropertyName,
            IKeyframes::Probe(((PathKeyframes*)keyframes.Get())->CreateXInt32Keyframes()));
    AutoPtr<IPropertyValuesHolder> y = PropertyValuesHolder::OfKeyframes(yPropertyName,
            IKeyframes::Probe(((PathKeyframes*)keyframes.Get())->CreateYInt32Keyframes()));

    AutoPtr<ArrayOf<IPropertyValuesHolder*> > a = ArrayOf<IPropertyValuesHolder*>::Alloc(2);
    a->Set(0, x);
    a->Set(1, y);
    return OfPropertyValuesHolder(target, a);
}

AutoPtr<IObjectAnimator> ObjectAnimator::OfInt32(
    /* [in] */ IInterface* target,
    /* [in] */ IProperty* xProperty,
    /* [in] */ IProperty* yProperty,
    /* [in] */ IPath* path)
{
    if (FAILED(CheckIsCarObject(target))) {
        return NULL;
    }

    AutoPtr<IPathKeyframes> keyframes = KeyframeSet::OfPath(path);
    AutoPtr<IPropertyValuesHolder> x = PropertyValuesHolder::OfKeyframes(xProperty,
            IKeyframes::Probe(((PathKeyframes*)keyframes.Get())->CreateXInt32Keyframes()));
    AutoPtr<IPropertyValuesHolder> y = PropertyValuesHolder::OfKeyframes(yProperty,
            IKeyframes::Probe(((PathKeyframes*)keyframes.Get())->CreateYInt32Keyframes()));
    AutoPtr<ArrayOf<IPropertyValuesHolder*> > a = ArrayOf<IPropertyValuesHolder*>::Alloc(2);
    a->Set(0, x);
    a->Set(1, y);
    return OfPropertyValuesHolder(target, a);
}

AutoPtr<IObjectAnimator> ObjectAnimator::OfMultiInt32(
    /* [in] */ IInterface* target,
    /* [in] */ const String& propertyName,
    /* [in] */ ArrayOf<ArrayOf<Int32>* >* values)
{
    if (FAILED(CheckIsCarObject(target))) {
        return NULL;
    }

    AutoPtr<IPropertyValuesHolder> pvh;
    PropertyValuesHolder::OfMultiInt32(propertyName, values, (IPropertyValuesHolder**)&pvh);

    AutoPtr<ArrayOf<IPropertyValuesHolder*> > a = ArrayOf<IPropertyValuesHolder*>::Alloc(1);
    a->Set(0, pvh);
    return OfPropertyValuesHolder(target, a);
}

AutoPtr<IObjectAnimator> ObjectAnimator::OfMultiInt32(
    /* [in] */ IInterface* target,
    /* [in] */ const String& propertyName,
    /* [in] */ IPath* path)
{
    if (FAILED(CheckIsCarObject(target))) {
        return NULL;
    }

    AutoPtr<IPropertyValuesHolder> pvh = PropertyValuesHolder::OfMultiInt32(propertyName, path);
    AutoPtr<ArrayOf<IPropertyValuesHolder*> > a = ArrayOf<IPropertyValuesHolder*>::Alloc(1);
    a->Set(0, pvh);
    return OfPropertyValuesHolder(target, a);
}

AutoPtr<IObjectAnimator> ObjectAnimator::OfMultiInt32(
    /* [in] */ IInterface* target,
    /* [in] */ const String& propertyName,
    /* [in] */ ITypeConverter* converter,
    /* [in] */ ITypeEvaluator* evaluator,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    if (FAILED(CheckIsCarObject(target))) {
        return NULL;
    }

    AutoPtr<IPropertyValuesHolder> pvh = PropertyValuesHolder::OfMultiInt32(propertyName, converter,
            evaluator, values);
    AutoPtr<ArrayOf<IPropertyValuesHolder*> > a = ArrayOf<IPropertyValuesHolder*>::Alloc(1);
    a->Set(0, pvh);
    return OfPropertyValuesHolder(target, a);
}

AutoPtr<IObjectAnimator> ObjectAnimator::OfArgb(
    /* [in] */ IInterface* target,
    /* [in] */ const String& propertyName,
    /* [in] */ ArrayOf<Int32>* values)
{
    if (FAILED(CheckIsCarObject(target))) {
        return NULL;
    }

    AutoPtr<IObjectAnimator> animator = OfInt32(target, propertyName, values);
    IValueAnimator::Probe(animator)->SetEvaluator(ITypeEvaluator::Probe(CArgbEvaluator::GetInstance()));
    return animator;
}

AutoPtr<IObjectAnimator> ObjectAnimator::OfArgb(
    /* [in] */ IInterface* target,
    /* [in] */ IProperty* property,
    /* [in] */ ArrayOf<Int32>* values)
{
    if (FAILED(CheckIsCarObject(target))) {
        return NULL;
    }

    AutoPtr<IObjectAnimator> animator = OfInt32(target, property, values);
    IValueAnimator::Probe(animator)->SetEvaluator(ITypeEvaluator::Probe(CArgbEvaluator::GetInstance()));
    return animator;
}

AutoPtr<IObjectAnimator> ObjectAnimator::OfFloat(
    /* [in] */ IInterface* target,
    /* [in] */ const String& xPropertyName,
    /* [in] */ const String& yPropertyName,
    /* [in] */ IPath* path)
{
    if (FAILED(CheckIsCarObject(target))) {
        return NULL;
    }

    AutoPtr<IPathKeyframes> keyframes = KeyframeSet::OfPath(path);
    AutoPtr<IPropertyValuesHolder> x = PropertyValuesHolder::OfKeyframes(xPropertyName,
            IKeyframes::Probe(((PathKeyframes*)keyframes.Get())->CreateXFloatKeyframes()));
    AutoPtr<IPropertyValuesHolder> y = PropertyValuesHolder::OfKeyframes(yPropertyName,
            IKeyframes::Probe(((PathKeyframes*)keyframes.Get())->CreateYFloatKeyframes()));
    AutoPtr<ArrayOf<IPropertyValuesHolder*> > a = ArrayOf<IPropertyValuesHolder*>::Alloc(2);
    a->Set(0, x);
    a->Set(1, y);
    return OfPropertyValuesHolder(target, a);
}

AutoPtr<IObjectAnimator> ObjectAnimator::OfFloat(
    /* [in] */ IInterface* target,
    /* [in] */ IProperty* xProperty,
    /* [in] */ IProperty* yProperty,
    /* [in] */ IPath* path)
{
    if (FAILED(CheckIsCarObject(target))) {
        return NULL;
    }

    AutoPtr<IPathKeyframes> keyframes = KeyframeSet::OfPath(path);
    AutoPtr<IPropertyValuesHolder> x = PropertyValuesHolder::OfKeyframes(xProperty,
            IKeyframes::Probe(((PathKeyframes*)keyframes.Get())->CreateXFloatKeyframes()));
    AutoPtr<IPropertyValuesHolder> y = PropertyValuesHolder::OfKeyframes(yProperty,
            IKeyframes::Probe(((PathKeyframes*)keyframes.Get())->CreateYFloatKeyframes()));
    AutoPtr<ArrayOf<IPropertyValuesHolder*> > a = ArrayOf<IPropertyValuesHolder*>::Alloc(2);
    a->Set(0, x);
    a->Set(1, y);
    return OfPropertyValuesHolder(target, a);
}

AutoPtr<IObjectAnimator> ObjectAnimator::OfMultiFloat(
    /* [in] */ IInterface* target,
    /* [in] */ const String& propertyName,
    /* [in] */ ArrayOf<ArrayOf<Float>*>* values)
{
    if (FAILED(CheckIsCarObject(target))) {
        return NULL;
    }

    AutoPtr<IPropertyValuesHolder> pvh;
    PropertyValuesHolder::OfMultiFloat(propertyName, values, (IPropertyValuesHolder**)&pvh);
    AutoPtr<ArrayOf<IPropertyValuesHolder*> > a = ArrayOf<IPropertyValuesHolder*>::Alloc(1);
    a->Set(0, pvh);
    return OfPropertyValuesHolder(target, a);
}

AutoPtr<IObjectAnimator> ObjectAnimator::OfMultiFloat(
    /* [in] */ IInterface* target,
    /* [in] */ const String& propertyName,
    /* [in] */ IPath* path)
{
    if (FAILED(CheckIsCarObject(target))) {
        return NULL;
    }

    AutoPtr<IPropertyValuesHolder> pvh = PropertyValuesHolder::OfMultiFloat(propertyName, path);
    AutoPtr<ArrayOf<IPropertyValuesHolder*> > a = ArrayOf<IPropertyValuesHolder*>::Alloc(1);
    a->Set(0, pvh);
    return OfPropertyValuesHolder(target, a);
}

AutoPtr<IObjectAnimator> ObjectAnimator::OfMultiFloat(
    /* [in] */ IInterface* target,
    /* [in] */ const String& propertyName,
    /* [in] */ ITypeConverter* converter,
    /* [in] */ ITypeEvaluator* evaluator,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    if (FAILED(CheckIsCarObject(target))) {
        return NULL;
    }

    AutoPtr<IPropertyValuesHolder> pvh = PropertyValuesHolder::OfMultiFloat(propertyName, converter,
            evaluator, values);
    AutoPtr<ArrayOf<IPropertyValuesHolder*> > a = ArrayOf<IPropertyValuesHolder*>::Alloc(1);
    a->Set(0, pvh);
    return OfPropertyValuesHolder(target, a);
}

AutoPtr<IObjectAnimator> ObjectAnimator::OfFloat(
    /* [in] */ IInterface* target,
    /* [in] */ const String& propertyName,
    /* [in] */ ArrayOf<Float>* values)
{
    if (FAILED(CheckIsCarObject(target))) {
        return NULL;
    }

    AutoPtr<CObjectAnimator> anim;
    CObjectAnimator::NewByFriend((CObjectAnimator**)&anim);
    anim->constructor(target, propertyName);
    anim->SetFloatValues(values);
    return anim;
}

AutoPtr<IObjectAnimator> ObjectAnimator::OfFloat(
    /* [in] */ IInterface* target,
    /* [in] */ IProperty* property,
    /* [in] */ ArrayOf<Float>* values)
{
    if (FAILED(CheckIsCarObject(target))) {
        return NULL;
    }

    AutoPtr<CObjectAnimator> anim;
    CObjectAnimator::NewByFriend((CObjectAnimator**)&anim);
    anim->constructor(target, property);
    anim->SetFloatValues(values);
    return anim;
}

AutoPtr<IObjectAnimator> ObjectAnimator::OfObject(
    /* [in] */ IInterface* target,
    /* [in] */ const String& propertyName,
    /* [in] */ ITypeEvaluator* evaluator,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    if (FAILED(CheckIsCarObject(target))) {
        return NULL;
    }

    AutoPtr<CObjectAnimator> anim;
    CObjectAnimator::NewByFriend((CObjectAnimator**)&anim);
    anim->constructor(target, propertyName);
    anim->SetObjectValues(values);
    anim->SetEvaluator(evaluator);
    return anim;
}

AutoPtr<IObjectAnimator> ObjectAnimator::OfObject(
    /* [in] */ IInterface* target,
    /* [in] */ IProperty* property,
    /* [in] */ ITypeEvaluator* evaluator,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    if (FAILED(CheckIsCarObject(target))) {
        return NULL;
    }

    AutoPtr<CObjectAnimator> anim;
    CObjectAnimator::NewByFriend((CObjectAnimator**)&anim);
    anim->constructor(target, property);
    anim->SetObjectValues(values);
    anim->SetEvaluator(evaluator);
    return anim;
}

AutoPtr<IObjectAnimator> ObjectAnimator::OfObject(
    /* [in] */ IInterface* target,
    /* [in] */ const String& propertyName,
    /* [in] */ /*@Nullable*/ ITypeConverter* converter,
    /* [in] */ IPath* path)
{
    if (FAILED(CheckIsCarObject(target))) {
        return NULL;
    }

    AutoPtr<IPropertyValuesHolder> pvh = PropertyValuesHolder::OfObject(propertyName, converter, path);
    AutoPtr<ArrayOf<IPropertyValuesHolder*> > a = ArrayOf<IPropertyValuesHolder*>::Alloc(1);
    a->Set(0, pvh);
    return OfPropertyValuesHolder(target, a);
}

AutoPtr<IObjectAnimator> ObjectAnimator::OfObject(
    /* [in] */ IInterface* target,
    /* [in] */ IProperty* property,
    /* [in] */ ITypeConverter* converter,
    /* [in] */ ITypeEvaluator* evaluator,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    if (FAILED(CheckIsCarObject(target))) {
        return NULL;
    }

    AutoPtr<IPropertyValuesHolder> pvh = PropertyValuesHolder::OfObject(property, converter, evaluator,
            values);
    AutoPtr<ArrayOf<IPropertyValuesHolder*> > a = ArrayOf<IPropertyValuesHolder*>::Alloc(1);
    a->Set(0, pvh);
    return OfPropertyValuesHolder(target, a);
}

AutoPtr<IObjectAnimator> ObjectAnimator::OfObject(
    /* [in] */ IInterface* target,
    /* [in] */ /*@NonNull*/ IProperty* property,
    /* [in] */ /*@Nullable*/ ITypeConverter* converter,
    /* [in] */ IPath* path)
{
    if (FAILED(CheckIsCarObject(target))) {
        return NULL;
    }

    AutoPtr<IPropertyValuesHolder> pvh = PropertyValuesHolder::OfObject(property, converter, path);
    AutoPtr<ArrayOf<IPropertyValuesHolder*> > a = ArrayOf<IPropertyValuesHolder*>::Alloc(1);
    a->Set(0, pvh);
    return OfPropertyValuesHolder(target, a);
}

AutoPtr<IObjectAnimator> ObjectAnimator::OfPropertyValuesHolder(
    /* [in] */ IInterface* target,
    /* [in] */ ArrayOf<IPropertyValuesHolder*>* values)
{
    if (FAILED(CheckIsCarObject(target))) {
        return NULL;
    }

    AutoPtr<IObjectAnimator> anim;
    CObjectAnimator::New((IObjectAnimator**)&anim);
    IAnimator::Probe(anim)->SetTarget(target);
    IValueAnimator::Probe(anim)->SetValues(values);
    return anim;
}

ECode ObjectAnimator::SetAutoCancel(
    /* [in] */ Boolean cancel)
{
    mAutoCancel = cancel;
    return NOERROR;
}

Boolean ObjectAnimator::HasSameTargetAndProperties(
    /* [in] */ /*@Nullable*/ IAnimator* anim)
{
    if (IObjectAnimator::Probe(anim)) {
        AutoPtr<ArrayOf<IPropertyValuesHolder*> > theirValues;
        IValueAnimator::Probe(anim)->GetValues((ArrayOf<IPropertyValuesHolder*>**)&theirValues);
        AutoPtr<IInterface> obj1;
        AutoPtr<IInterface> obj2;
        if ((IObjectAnimator::Probe(anim)->GetTarget((IInterface**)&obj1), obj1.Get())
                    == (GetTarget((IInterface**)&obj2), obj2.Get())
                && mValues->GetLength() == theirValues->GetLength()) {
            for (Int32 i = 0; i < mValues->GetLength(); ++i) {
                AutoPtr<IPropertyValuesHolder> pvhMine = (*mValues)[i];
                AutoPtr<IPropertyValuesHolder> pvhTheirs = (*theirValues)[i];

                String pn1, pn2;
                pvhMine->GetPropertyName(&pn1);
                pvhTheirs->GetPropertyName(&pn2);
                if (pn1 == NULL || !pn1.Equals(pn2)) {
                    return FALSE;
                }
            }
            return TRUE;
        }
    }
    return FALSE;
}

ECode ObjectAnimator::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    Int32 hashCode = 0;
    GetHashCode(&hashCode);
    AutoPtr<IInterface> t;
    GetTarget((IInterface**)&t);

    StringBuilder sb("ObjectAnimator@");
    sb += StringUtils::ToHexString(hashCode);
    sb += ", target:";
    sb += Object::ToString(t);

    if (mValues != NULL) {
        sb += ", values{";
        for (Int32 i = 0; i < mValues->GetLength(); ++i) {
            String v = Object::ToString((*mValues)[i]);
            sb += "\n    ";
            sb += v;
        }
        sb += "\n}"
    }
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos
