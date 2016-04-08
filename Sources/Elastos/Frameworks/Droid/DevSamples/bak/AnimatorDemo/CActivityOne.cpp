#include "elastos/droid/ext/frameworkdef.h"
#include "CActivityOne.h"
#include <stdio.h>
#include "R.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::CStringWrapper;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Animation::IArgbEvaluator;
using Elastos::Droid::Animation::CArgbEvaluator;
using Elastos::Droid::Animation::IFloatEvaluator;
using Elastos::Droid::Animation::CFloatEvaluator;
using Elastos::Droid::Animation::CPropertyValuesHolderHelper;
using Elastos::Droid::Animation::IPropertyValuesHolderHelper;
using Elastos::Droid::Animation::IPropertyValuesHolder;
using Elastos::Droid::Utility::CPropertyHelper;
using Elastos::Droid::Utility::IPropertyHelper;
using Elastos::Droid::Utility::IProperty;
using Elastos::Droid::Animation::EIID_ITypeEvaluator;
using Elastos::Droid::View::Animation::IAccelerateInterpolator;
using Elastos::Droid::View::Animation::CAccelerateInterpolator;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AnimatorDemo {

CAR_INTERFACE_IMPL(CActivityOne::MyEvaluator, ITypeEvaluator)

CActivityOne::MyEvaluator::MyEvaluator()
{}

ECode CActivityOne::MyEvaluator::Evaluate(
    /* [in] */ Float fraction,
    /* [in] */ IInterface* startValue,
    /* [in] */ IInterface* endValue,
    /* [out] */ IInterface** result)
{
    String start, end;
    ICharSequence::Probe(startValue)->ToString(&start);
    ICharSequence::Probe(endValue)->ToString(&end);
    Int32 startLength = start.GetLength(), endLength = start.GetLength();
    AutoPtr<ArrayOf<Char32> > sArray = start.GetChars();
    AutoPtr<ArrayOf<Char32> > eArray = end.GetChars();
    if(startLength >= endLength)
    {
        AutoPtr<ArrayOf<Char32> > rArray = ArrayOf<Char32>::Alloc(startLength);
        for(Int32 i = 0; i < startLength; i++)
        {
            if(i < endLength)
            {
                Int32 val = (*sArray)[i] - (*eArray)[i];
                (*rArray)[i] = (*sArray)[i] - val * fraction;
            } else {
                Char32 zero = ' ';
                Int32 val = (*sArray)[i] - zero;
                (*rArray)[i] = (*sArray)[i] - val * fraction;
            }
        }
        String rstStr(*rArray);
        AutoPtr<ICharSequence> csq;
        CStringWrapper::New(rstStr, (ICharSequence**)&csq);
        *result = csq.Get();
        REFCOUNT_ADD(*result)
        return NOERROR;
    } else {
        AutoPtr<ArrayOf<Char32> > rArray = ArrayOf<Char32>::Alloc(endLength);
        for(Int32 i = 0; i < endLength; i++)
        {
            if(i < startLength)
            {
                Int32 val = (*sArray)[i] - (*eArray)[i];
                (*rArray)[i] = (*sArray)[i] - val * fraction;
            } else {
                Char32 zero = ' ';
                Int32 val = zero - (*eArray)[i];
                (*rArray)[i] = zero - val * fraction;
            }
        }
        String rstStr(*rArray);
        AutoPtr<ICharSequence> csq;
        CStringWrapper::New(rstStr, (ICharSequence**)&csq);
        *result = csq.Get();
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
}

CAR_INTERFACE_IMPL(CActivityOne::MyListener, IViewOnClickListener)

CActivityOne::MyListener::MyListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{}

ECode CActivityOne::MyListener::OnClick(
    /* [in] */ IView* v)
{
    AutoPtr<IObjectAnimator> objectAnimator;
    AutoPtr<IObjectAnimatorHelper> animatorHelper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&animatorHelper);
    Int32 id;
    v->GetId(&id);
    switch(id)
    {
        case R::id::bt_inter:
        {
            String start("123456789");
            String end("987654321");
            AutoPtr<ICharSequence> sSeq;
            AutoPtr<ICharSequence> eSeq;
            CStringWrapper::New(start, (ICharSequence**)&sSeq);
            CStringWrapper::New(end, (ICharSequence**)&eSeq);
            AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(2);
            array->Set(0, sSeq);
            array->Set(1, eSeq);
            AutoPtr<ITypeEvaluator> evaluator= new MyEvaluator();
            animatorHelper->OfObject(mHost->mButtonInter, String("text"), evaluator, array, (IObjectAnimator**)&objectAnimator);
            break;
        }
        case R::id::bt_color:
        {
            AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(4);
            (*array)[0] = 0xff8080FF;
            (*array)[1] = 0xffFF8080;
            (*array)[2] = 0xff80ffff;
            (*array)[3] = 0xff80ff80;
            AutoPtr<IArgbEvaluator> argb;
            CArgbEvaluator::New((IArgbEvaluator**)&argb);
            AutoPtr<IPropertyHelper> propertyHelper;
            CPropertyHelper::AcquireSingleton((IPropertyHelper**)&propertyHelper);
            AutoPtr<IObject> objTemp = IObject::Probe(mHost->mButtonColor);
            ClassID clsId;
            objTemp->GetClassID(&clsId);
            String funcName("backgroundColor");
            AutoPtr<IProperty> property;
            propertyHelper->Of(clsId, CarDataType_Int32, funcName, (IProperty**)&property);
            animatorHelper->OfInt32(mHost->mButtonColor, property, array, (IObjectAnimator**)&objectAnimator);
            AutoPtr<IAccelerateInterpolator> ipt;
            CAccelerateInterpolator::New((IAccelerateInterpolator**)&ipt);
            objectAnimator->SetInterpolator(ipt);
            objectAnimator->SetEvaluator(argb);
            break;
        }
        case R::id::bt_alpha:
        {
            AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(4);
            (*array)[0] = 0.0f;
            (*array)[1] = 1.0f;
            (*array)[2] = 0.0f;
            (*array)[3] = 1.0f;
            // AutoPtr<IFloatEvaluator> argb;
            // CFloatEvaluator::New((IFloatEvaluator**)&argb);
            AutoPtr<IPropertyHelper> propertyHelper;
            CPropertyHelper::AcquireSingleton((IPropertyHelper**)&propertyHelper);
            AutoPtr<IObject> objTemp = IObject::Probe(mHost->mButtonAlpha);
            ClassID clsId;
            objTemp->GetClassID(&clsId);
            String funcName("alpha");
            AutoPtr<IProperty> property;
            propertyHelper->Of(clsId, CarDataType_Float, funcName, (IProperty**)&property);
            animatorHelper->OfFloat(mHost->mButtonAlpha, property, array, (IObjectAnimator**)&objectAnimator);
            // objectAnimator->SetEvaluator(argb);
            break;
        }
        case R::id::bt_property:
        {
            String start("123456789");
            String end("987654321");
            AutoPtr<ICharSequence> sSeq;
            AutoPtr<ICharSequence> eSeq;
            CStringWrapper::New(start, (ICharSequence**)&sSeq);
            CStringWrapper::New(end, (ICharSequence**)&eSeq);
            AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(2);
            array->Set(0, sSeq);
            array->Set(1, eSeq);
            AutoPtr<IProperty> property;
            AutoPtr<IObject> obj = IObject::Probe(mHost->mButtonProperty);
            assert(obj != NULL);
            ClassID clsId;
            obj->GetClassID(&clsId);
            AutoPtr<IPropertyHelper> propertyHelper;
            CPropertyHelper::AcquireSingleton((IPropertyHelper**)&propertyHelper);
            ASSERT_SUCCEEDED(propertyHelper->Of(clsId, CarDataType_Interface, String("text"), (IProperty**)&property));
            AutoPtr<ITypeEvaluator> evaluator= new MyEvaluator();
            animatorHelper->OfObject(mHost->mButtonProperty, property, evaluator, array, (IObjectAnimator**)&objectAnimator);
            break;
        }
        case R::id::bt_valueHolder:
        {
            String start("123456789");
            String end("987654321");
            AutoPtr<ICharSequence> sSeq;
            AutoPtr<ICharSequence> eSeq;
            CStringWrapper::New(start, (ICharSequence**)&sSeq);
            CStringWrapper::New(end, (ICharSequence**)&eSeq);
            AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(2);
            array->Set(0, sSeq);
            array->Set(1, eSeq);
            AutoPtr<IProperty> property;
            AutoPtr<IObject> obj = IObject::Probe(mHost->mButtonProperty);
            assert(obj != NULL);
            ClassID clsId;
            obj->GetClassID(&clsId);
            AutoPtr<IPropertyHelper> propertyHelper;
            CPropertyHelper::AcquireSingleton((IPropertyHelper**)&propertyHelper);
            ASSERT_SUCCEEDED(propertyHelper->Of(clsId, CarDataType_Interface, String("text"), (IProperty**)&property));
            AutoPtr<ITypeEvaluator> evaluator= new MyEvaluator();
            AutoPtr<IPropertyValuesHolderHelper> holderHelper;
            CPropertyValuesHolderHelper::AcquireSingleton((IPropertyValuesHolderHelper**)&holderHelper);
            AutoPtr<IPropertyValuesHolder> holder;
            holderHelper->OfObject(property, evaluator, array, (IPropertyValuesHolder**)&holder);
            AutoPtr<ArrayOf<IPropertyValuesHolder*> > holderArray = ArrayOf<IPropertyValuesHolder*>::Alloc(1);
            holderArray->Set(0, holder);
            animatorHelper->OfPropertyValuesHolder(mHost->mButtonValueHolder, holderArray, (IObjectAnimator**)&objectAnimator);
            break;
        }
        default:
            break;
    }

    objectAnimator->SetRepeatMode(IObjectAnimator::ANIMATION_REVERSE);
    objectAnimator->SetDuration(1300);

    objectAnimator->Start();
    return NOERROR;
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);
    mButtonInter = IButton::Probe(FindViewById(R::id::bt_inter));
    mButtonColor = IButton::Probe(FindViewById(R::id::bt_color));
    mButtonAlpha = IButton::Probe(FindViewById(R::id::bt_alpha));
    mButtonProperty = IButton::Probe(FindViewById(R::id::bt_property));
    mButtonValueHolder = IButton::Probe(FindViewById(R::id::bt_valueHolder));
    AutoPtr<IViewOnClickListener> l = new MyListener(this);
    mButtonInter->SetOnClickListener(l);
    mButtonColor->SetOnClickListener(l);
    mButtonAlpha->SetOnClickListener(l);
    mButtonProperty->SetOnClickListener(l);
    mButtonValueHolder->SetOnClickListener(l);
    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    PRINT_ENTER_LEAVE("CActivityOne::OnStart");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnResume()
{
    PRINT_ENTER_LEAVE("CActivityOne::OnResume");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnPause()
{
    PRINT_ENTER_LEAVE("CActivityOne::OnPause");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnStop()
{
    PRINT_ENTER_LEAVE("OnStop");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnDestroy()
{
    PRINT_ENTER_LEAVE("OnDestroy");
    // TODO: Add your code here
    return NOERROR;
}

} // namespace AnimatorDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
