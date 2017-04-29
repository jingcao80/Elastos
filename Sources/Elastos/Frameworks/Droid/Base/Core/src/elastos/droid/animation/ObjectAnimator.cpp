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

#include "elastos/droid/animation/ObjectAnimator.h"
#include "elastos/droid/animation/CObjectAnimator.h"
#include "elastos/droid/animation/PropertyValuesHolder.h"
#include "elastos/droid/animation/PathKeyframes.h"
#include "elastos/droid/animation/CArgbEvaluator.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Animation {

const String ObjectAnimator::LOGTAG("ObjectAnimator");
const Boolean ObjectAnimator::DBG = FALSE;

CAR_INTERFACE_IMPL(ObjectAnimator, ValueAnimator, IObjectAnimator);

ObjectAnimator::ObjectAnimator()
    : mAutoCancel(FALSE)
{}

ECode ObjectAnimator::constructor()
{
    return NOERROR;
}

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
    if (!mPropertyName.IsNull()) {
        propertyName = mPropertyName;
    }
    else if (mProperty != NULL) {
        mProperty->GetName(&propertyName);
    }
    else if (mValues != NULL && mValues->GetLength() > 0) {
        for (Int32 i = 0; i < mValues->GetLength(); ++i) {
            if (i == 0) {
                propertyName = "";
            }
            else {
                propertyName += ",";
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
    return String("animator:") + name;
}

ECode ObjectAnimator::constructor(
    /* [in] */ IInterface* target,
    /* [in] */ const String& propertyName)
{
    SetTarget(target);
    SetPropertyName(propertyName);
    return NOERROR;
}

ECode ObjectAnimator::constructor(
    /* [in] */ IInterface* target,
    /* [in] */ IProperty* property)
{
    SetTarget(target);
    SetProperty(property);
    return NOERROR;
}

static ECode CheckIsCarObject(
    /* [in] */ IInterface* target)
{
    ECode ec = NOERROR;
#if defined(_DEBUG)
    if (target != NULL) {
        ClassID objId;
        if (IObject::Probe(target) == NULL ||
                FAILED(IObject::Probe(target)->GetClassID(&objId))) {
            Logger::E("ObjectAnimator", "Target object [%s] is not a CAR object!", TO_CSTR(target));
            assert(0 && "Target object is not a CAR object!");
        }
    }
#endif
    return ec;
}

AutoPtr<IObjectAnimator> ObjectAnimator::OfInt32(
    /* [in] */ IInterface* target,
    /* [in] */ const String& propertyName,
    /* [in] */ ArrayOf<Int32>* values)
{
    if (FAILED(CheckIsCarObject(target))) {
        return NULL;
    }
    AutoPtr<IObjectAnimator> anim;
    CObjectAnimator::New((IObjectAnimator**)&anim);
    CObjectAnimator* oa = (CObjectAnimator*)anim.Get();
    oa->constructor(target, propertyName);
    IValueAnimator::Probe(anim)->SetInt32Values(values);
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
    PathKeyframes* pkf = (PathKeyframes*)keyframes.Get();
    AutoPtr<IInt32Keyframes> xfkf = pkf->CreateXInt32Keyframes();
    AutoPtr<IInt32Keyframes> yfkf = pkf->CreateYInt32Keyframes();
    AutoPtr<IPropertyValuesHolder> x = PropertyValuesHolder::OfKeyframes(
            xPropertyName, IKeyframes::Probe(xfkf));
    AutoPtr<IPropertyValuesHolder> y = PropertyValuesHolder::OfKeyframes(
            yPropertyName, IKeyframes::Probe(yfkf));
    AutoPtr<ArrayOf<IPropertyValuesHolder*> > a = ArrayOf<IPropertyValuesHolder*>::Alloc(2);
    a->Set(0, x);
    a->Set(1, y);
    return OfPropertyValuesHolder(target, a);
}

AutoPtr<IObjectAnimator> ObjectAnimator::OfInt32(
    /* [in] */ IInterface* target,
    /* [in] */ IProperty* property,
    /* [in] */ ArrayOf<Int32>* values)
{
    if (FAILED(CheckIsCarObject(target))) {
        return NULL;
    }
    AutoPtr<IObjectAnimator> anim;
    CObjectAnimator::New((IObjectAnimator**)&anim);
    CObjectAnimator* oa = (CObjectAnimator*)anim.Get();
    oa->constructor(target, property);
    IValueAnimator::Probe(anim)->SetInt32Values(values);
    return anim;
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
    PathKeyframes* pkf = (PathKeyframes*)keyframes.Get();
    AutoPtr<IInt32Keyframes> xfkf = pkf->CreateXInt32Keyframes();
    AutoPtr<IInt32Keyframes> yfkf = pkf->CreateYInt32Keyframes();
    AutoPtr<IPropertyValuesHolder> x = PropertyValuesHolder::OfKeyframes(
            xProperty, IKeyframes::Probe(xfkf));
    AutoPtr<IPropertyValuesHolder> y = PropertyValuesHolder::OfKeyframes(
            yProperty, IKeyframes::Probe(yfkf));
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
    /* [in] */ const String& propertyName,
    /* [in] */ ArrayOf<Float>* values)
{
    if (FAILED(CheckIsCarObject(target))) {
        return NULL;
    }
    AutoPtr<IObjectAnimator> anim;
    CObjectAnimator::New((IObjectAnimator**)&anim);
    CObjectAnimator* oa = (CObjectAnimator*)anim.Get();
    oa->constructor(target, propertyName);
    IValueAnimator::Probe(anim)->SetFloatValues(values);
    return anim;
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
    PathKeyframes* pkf = (PathKeyframes*)keyframes.Get();
    AutoPtr<IFloatKeyframes> xfkf = pkf->CreateXFloatKeyframes();
    AutoPtr<IFloatKeyframes> yfkf = pkf->CreateYFloatKeyframes();
    AutoPtr<IPropertyValuesHolder> x = PropertyValuesHolder::OfKeyframes(
            xPropertyName, IKeyframes::Probe(xfkf));
    AutoPtr<IPropertyValuesHolder> y = PropertyValuesHolder::OfKeyframes(
            yPropertyName, IKeyframes::Probe(yfkf));
    AutoPtr<ArrayOf<IPropertyValuesHolder*> > a = ArrayOf<IPropertyValuesHolder*>::Alloc(2);
    a->Set(0, x);
    a->Set(1, y);
    return OfPropertyValuesHolder(target, a);
}

AutoPtr<IObjectAnimator> ObjectAnimator::OfFloat(
    /* [in] */ IInterface* target,
    /* [in] */ IProperty* property,
    /* [in] */ ArrayOf<Float>* values)
{
    if (FAILED(CheckIsCarObject(target))) {
        return NULL;
    }
    AutoPtr<IObjectAnimator> anim;
    CObjectAnimator::New((IObjectAnimator**)&anim);
    CObjectAnimator* oa = (CObjectAnimator*)anim.Get();
    oa->constructor(target, property);
    IValueAnimator::Probe(anim)->SetFloatValues(values);
    return anim;
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
    PathKeyframes* pkf = (PathKeyframes*)keyframes.Get();
    AutoPtr<IFloatKeyframes> xfkf = pkf->CreateXFloatKeyframes();
    AutoPtr<IFloatKeyframes> yfkf = pkf->CreateYFloatKeyframes();
    AutoPtr<IPropertyValuesHolder> x = PropertyValuesHolder::OfKeyframes(
            xProperty, IKeyframes::Probe(xfkf));
    AutoPtr<IPropertyValuesHolder> y = PropertyValuesHolder::OfKeyframes(
            yProperty, IKeyframes::Probe(yfkf));
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

AutoPtr<IObjectAnimator> ObjectAnimator::OfObject(
    /* [in] */ IInterface* target,
    /* [in] */ const String& propertyName,
    /* [in] */ ITypeEvaluator* evaluator,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    if (FAILED(CheckIsCarObject(target))) {
        return NULL;
    }
    AutoPtr<IObjectAnimator> anim;
    CObjectAnimator::New((IObjectAnimator**)&anim);
    CObjectAnimator* oa = (CObjectAnimator*)anim.Get();
    oa->constructor(target, propertyName);
    IValueAnimator* va = IValueAnimator::Probe(anim);
    va->SetObjectValues(values);
    va->SetEvaluator(evaluator);
    return anim;
}

AutoPtr<IObjectAnimator> ObjectAnimator::OfObject(
    /* [in] */ IInterface* target,
    /* [in] */ const String& propertyName,
    /* [in] */ ITypeConverter* converter,
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
    /* [in] */ ITypeEvaluator* evaluator,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    if (FAILED(CheckIsCarObject(target))) {
        return NULL;
    }
    AutoPtr<IObjectAnimator> anim;
    CObjectAnimator::New((IObjectAnimator**)&anim);
    CObjectAnimator* oa = (CObjectAnimator*)anim.Get();
    oa->constructor(target, property);
    IValueAnimator* va = IValueAnimator::Probe(anim);
    va->SetObjectValues(values);
    va->SetEvaluator(evaluator);
    return anim;
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

ECode ObjectAnimator::SetInt32Values(
    /* [in] */ ArrayOf<Int32>* values)
{
    if (mValues == NULL || mValues->GetLength() == 0) {
        // No values yet - this animator is being constructed piecemeal. Init the values with
        // whatever the current propertyName is
        AutoPtr<ArrayOf<IPropertyValuesHolder*> > params = ArrayOf<IPropertyValuesHolder*>::Alloc(1);
        if (mProperty != NULL) {
            AutoPtr<IPropertyValuesHolder> holder = PropertyValuesHolder::OfInt32(mProperty, values);
            params->Set(0, holder);
            SetValues(params);
        }
        else {
            AutoPtr<IPropertyValuesHolder> holder = PropertyValuesHolder::OfInt32(mPropertyName, values);
            params->Set(0, holder);
            SetValues(params);
        }
    }
    else {
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
        AutoPtr<ArrayOf<IPropertyValuesHolder*> > params = ArrayOf<IPropertyValuesHolder*>::Alloc(1);
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
        AutoPtr<ArrayOf<IPropertyValuesHolder*> > params = ArrayOf<IPropertyValuesHolder*>::Alloc(1);
        if (mProperty != NULL) {
            AutoPtr<IPropertyValuesHolder> holder = PropertyValuesHolder::OfObject(mProperty, NULL, values);
            params->Set(0, holder);
            SetValues(params);
        }
        else {
            AutoPtr<IPropertyValuesHolder> holder = PropertyValuesHolder::OfObject(mPropertyName, NULL, values);
            params->Set(0, holder);
            SetValues(params);
        }
    }
    else {
        ValueAnimator::SetObjectValues(values);
    }
    return NOERROR;
}

ECode ObjectAnimator::SetAutoCancel(
    /* [in] */ Boolean cancel)
{
    mAutoCancel = cancel;
    return NOERROR;
}

Boolean ObjectAnimator::HasSameTargetAndProperties(
    /* [in] */ IAnimator* anim)
{
    if (IObjectAnimator::Probe(anim)) {
        AutoPtr<ArrayOf<IPropertyValuesHolder*> > theirValues;
        IValueAnimator::Probe(anim)->GetValues((ArrayOf<IPropertyValuesHolder*>**)&theirValues);
        AutoPtr<IInterface> obj1;
        IObjectAnimator::Probe(anim)->GetTarget((IInterface**)&obj1);
        AutoPtr<IInterface> obj2;
        GetTarget((IInterface**)&obj2);
        if (IInterface::Probe(obj1) == IInterface::Probe(obj2)
                && mValues->GetLength() == theirValues->GetLength()) {
            for (Int32 i = 0; i < mValues->GetLength(); ++i) {
                AutoPtr<IPropertyValuesHolder> pvhMine = (*mValues)[i];
                AutoPtr<IPropertyValuesHolder> pvhTheirs = (*theirValues)[i];
                String pn1, pn2;
                pvhMine->GetPropertyName(&pn1);
                pvhTheirs->GetPropertyName(&pn2);
                if (pn1.IsNull() || !pn1.Equals(pn2)) {
                    return FALSE;
                }
            }
            return TRUE;
        }
    }
    return FALSE;
}

ECode ObjectAnimator::Start()
{
    // See if any of the current active/pending animators need to be canceled
    AutoPtr<AnimationHandler> handler = (AnimationHandler*)pthread_getspecific(sAnimationHandler);
    if (handler != NULL) {
        List<AutoPtr<IValueAnimator> >::ReverseIterator it;
        for (it = handler->mAnimations.RBegin(); it != handler->mAnimations.REnd(); ++it) {
            AutoPtr<IObjectAnimator> anim = IObjectAnimator::Probe(*it);
            if (anim) {
                IAnimator* animator = IAnimator::Probe(anim);
                if (((ObjectAnimator*)anim.Get())->mAutoCancel && HasSameTargetAndProperties(animator)) {
                    animator->Cancel();
                }
            }
        }
        for (it = handler->mPendingAnimations.RBegin(); it != handler->mPendingAnimations.REnd(); ++it) {
            AutoPtr<IObjectAnimator> anim = IObjectAnimator::Probe(*it);
            if (anim) {
                IAnimator* animator = IAnimator::Probe(anim);
                if (((ObjectAnimator*)anim.Get())->mAutoCancel && HasSameTargetAndProperties(animator)) {
                    animator->Cancel();
                }
            }
        }
        for (it = handler->mDelayedAnims.RBegin(); it != handler->mDelayedAnims.REnd(); ++it) {
            AutoPtr<IObjectAnimator> anim = IObjectAnimator::Probe(*it);
            if (anim) {
                IAnimator* animator = IAnimator::Probe(anim);
                if (((ObjectAnimator*)anim.Get())->mAutoCancel && HasSameTargetAndProperties(animator)) {
                    animator->Cancel();
                }
            }
        }
    }
    if (DBG) {
        Int64 duration;
        GetDuration(&duration);
        Logger::D(LOGTAG, " >>> %s :\n Anim target: %s, duration: %lld", TO_CSTR(this), TO_CSTR(mTarget), duration);
        for (Int32 i = 0; i < mValues->GetLength(); ++i) {
            Logger::D(LOGTAG, "         Values[%d]: %s", i, TO_CSTR((*mValues)[i]));
        }
    }
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
                PropertyValuesHolder* pvh = (PropertyValuesHolder*)(*mValues)[i];
                pvh->SetupSetterAndGetter(target);
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
    if (IInterface::Probe(oldTarget) != IInterface::Probe(target)) {
        mTarget = NULL;
        if (target != NULL) {
            CheckIsCarObject(target);
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
            PropertyValuesHolder* pvh = (PropertyValuesHolder*)(*mValues)[i];
            pvh->SetupStartValue(target);
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
            PropertyValuesHolder* pvh = (PropertyValuesHolder*)(*mValues)[i];
            pvh->SetupEndValue(target);
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
    else if (mTarget == NULL && target == NULL) {
        return NOERROR;
    }

    ValueAnimator::AnimateValue(fraction);
    Int32 numValues = mValues->GetLength();
    for (Int32 i = 0; i < numValues; ++i) {
        PropertyValuesHolder* pvh = (PropertyValuesHolder*)(*mValues)[i];
        pvh->SetAnimatedValue(target);
    }
    return NOERROR;
}

ECode ObjectAnimator::Clone(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    AutoPtr<IObjectAnimator> newObject = new CObjectAnimator();
    CloneImpl(newObject);
    *object = newObject;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode ObjectAnimator::CloneImpl(
    /* [in] */ IObjectAnimator* object)
{
    ValueAnimator::CloneImpl(IValueAnimator::Probe(object));

    ObjectAnimator* anim = (ObjectAnimator*)object;
    anim->mTarget = mTarget;
    anim->mPropertyName = mPropertyName;
    anim->mProperty = mProperty;
    return NOERROR;
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
            if (i != 0) sb += ",";
            sb += Object::ToString((*mValues)[i]);
        }
    }
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos
