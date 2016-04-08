
#include <Elastos.CoreLibrary.External.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/animation/AnimatorInflater.h"
#include "elastos/droid/view/animation/AnimationUtils.h"
#include "elastos/droid/animation/CAnimatorSet.h"
#include "elastos/droid/animation/CValueAnimator.h"
#include "elastos/droid/animation/CArgbEvaluator.h"
#include "elastos/droid/animation/CObjectAnimator.h"
#include "elastos/droid/animation/StateListAnimator.h"
#include "elastos/droid/animation/KeyframeSet.h"
#include "elastos/droid/animation/PropertyValuesHolder.h"
#include "elastos/droid/animation/PathKeyframes.h"
#include "elastos/droid/utility/StateSet.h"
#include "elastos/droid/utility/Xml.h"
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::PathParser;
using Elastos::Droid::Utility::StateSet;
using Elastos::Droid::Utility::Xml;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Animation {

const String AnimatorInflater::TAG = String("AnimatorInflater");
const Int32 AnimatorInflater::TOGETHER = 0;
const Int32 AnimatorInflater::SEQUENTIALLY = 1;
const Int32 AnimatorInflater::VALUE_TYPE_FLOAT       = 0;
const Int32 AnimatorInflater::VALUE_TYPE_INT         = 1;
const Int32 AnimatorInflater::VALUE_TYPE_PATH        = 2;
const Int32 AnimatorInflater::VALUE_TYPE_COLOR       = 4;
const Int32 AnimatorInflater::VALUE_TYPE_CUSTOM      = 5;
const Boolean AnimatorInflater::DBG_ANIMATOR_INFLATER = FALSE;

CAR_INTERFACE_IMPL(AnimatorInflater::PathDataEvaluator, Object, ITypeEvaluator);
AnimatorInflater::PathDataEvaluator::PathDataEvaluator(
    /* [in] */ ArrayOf<PathDataNode*>* nodeArray)
    : mNodeArray(nodeArray)
{
}

ECode AnimatorInflater::PathDataEvaluator::Evaluate(
    /* [in] */ Float fraction,
    /* [in] */ IInterface* startPathData /*array*/,
    /* [in] */ IInterface* endPathData /*array*/,
    /* [out] */ IInterface** result)
{
    assert(IArrayList::Probe(startPathData) && IArrayList::Probe(endPathData));
    Int32 count = 0;
    IArrayList::Probe(startPathData)->GetSize(&count);
    AutoPtr<ArrayOf<PathDataNode*> > sa = ArrayOf<PathDataNode*>::Alloc(count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        IArrayList::Probe(startPathData)->Get(i, (IInterface**)&obj);
        sa->Set(i, (PathDataNode*)(IObject*)obj.Get());
    }

    IArrayList::Probe(endPathData)->GetSize(&count);
    AutoPtr<ArrayOf<PathDataNode*> > ea = ArrayOf<PathDataNode*>::Alloc(count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        IArrayList::Probe(endPathData)->Get(i, (IInterface**)&obj);
        ea->Set(i, (PathDataNode*)(IObject*)obj.Get());
    }

    if (!PathParser::CanMorph(sa, ea)) {
        // throw new IllegalArgumentException("Can't interpolate between"
        //         + " two incompatible pathData");
        *result = NULL;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mNodeArray == NULL || !PathParser::CanMorph(mNodeArray, sa)) {
        mNodeArray = PathParser::DeepCopyNodes(sa);
    }

    IArrayList::Probe(startPathData)->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> objs, obje;
        IArrayList::Probe(startPathData)->Get(i, (IInterface**)&objs);
        IArrayList::Probe(endPathData)->Get(i, (IInterface**)&obje);
        (*mNodeArray)[i]->InterpolatePathDataNode((PathDataNode*)(IObject*)objs.Get(),
                (PathDataNode*)(IObject*)obje.Get(), fraction);
    }

    AutoPtr<IArrayList> ra;
    CArrayList::New((IArrayList**)&ra);
    for (Int32 i = 0; i < mNodeArray->GetLength(); i++) {
        ra->Add((IObject*)((*mNodeArray)[i]->Probe(EIID_IObject)));
    }
    *result = ra;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode AnimatorInflater::LoadAnimator(
    /* [in] */ IContext* context,
    /* [in] */ Int32 id,
    /* [out] */ IAnimator** animator) /*throws NotFoundException*/
{
    VALIDATE_NOT_NULL(animator);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IResourcesTheme> rt;
    context->GetTheme((IResourcesTheme**)&rt);
    return LoadAnimator(res, rt, id, animator);
}

ECode AnimatorInflater::LoadAnimator(
    /* [in] */ IResources* resources,
    /* [in] */ IResourcesTheme* theme,
    /* [in] */ Int32 id,
    /* [out] */ IAnimator** animator) /*throws NotFoundException*/
{
    VALIDATE_NOT_NULL(animator);
    return LoadAnimator(resources, theme, id, 1, animator);
}

ECode AnimatorInflater::LoadAnimator(
    /* [in] */ IResources* resources,
    /* [in] */ IResourcesTheme* theme,
    /* [in] */ Int32 id,
    /* [in] */ Float pathErrorScale,
    /* [out] */ IAnimator** animator) /*throws NotFoundException*/
{
    VALIDATE_NOT_NULL(animator);
    AutoPtr<IXmlResourceParser> parser;
    ECode ec = NOERROR;
    // try {
    ec = resources->GetAnimation(id, (IXmlResourceParser**)&parser);
    FAIL_GOTO(ec, error);
    ec = CreateAnimatorFromXml(resources, theme, IXmlPullParser::Probe(parser), pathErrorScale, animator);
    // } catch (XmlPullParserException ex) {
    //     Resources.NotFoundException rnf =
    //             new Resources.NotFoundException("Can't load animation resource ID #0x" +
    //                     Integer.toHexString(id));
    //     rnf.initCause(ex);
    //     throw rnf;
    // } catch (IOException ex) {
    //     Resources.NotFoundException rnf =
    //             new Resources.NotFoundException("Can't load animation resource ID #0x" +
    //                     Integer.toHexString(id));
    //     rnf.initCause(ex);
    //     throw rnf;
    // } finally {
    //     if (parser != null) parser.close();
    // }

error:
    if (parser != NULL) {
        parser->Close();
    }

    if (ec != NOERROR) {
        *animator = NULL;
        return E_RESOURCES_NOT_FOUND_EXCEPTION;
    }

    return NOERROR;
}

ECode AnimatorInflater::LoadStateListAnimator(
    /* [in] */ IContext* context,
    /* [in] */ Int32 id,
    /* [out] */ IStateListAnimator** animator) /*throws NotFoundException*/
{
    VALIDATE_NOT_NULL(animator);
    AutoPtr<IXmlResourceParser> parser;
    ECode ec = NOERROR;
    // try {
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    ec = res->GetAnimation(id, (IXmlResourceParser**)&parser);
    FAIL_GOTO(ec, error);
    ec = CreateStateListAnimatorFromXml(context, IXmlPullParser::Probe(parser),
                Xml::AsAttributeSet(IXmlPullParser::Probe(parser)), animator);

error:
    if (parser != NULL) {
        parser->Close();
    }

    if (ec != NOERROR) {
        *animator = NULL;
        return E_RESOURCES_NOT_FOUND_EXCEPTION;
    }

    return NOERROR;
}

ECode AnimatorInflater::CreateStateListAnimatorFromXml(
    /* [in] */ IContext* context,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attributeSet,
    /* [out] */ IStateListAnimator** animator) /*throws IOException, XmlPullParserException*/
{
    VALIDATE_NOT_NULL(animator);
    Int32 type;
    AutoPtr<IStateListAnimator> stateListAnimator = new StateListAnimator();

    while (TRUE) {
        FAIL_RETURN(parser->Next(&type));
        switch (type) {
            case IXmlPullParser::END_DOCUMENT:
            case IXmlPullParser::END_TAG: {
                *animator = stateListAnimator;
                REFCOUNT_ADD(*animator);
                return NOERROR;
            }

            case IXmlPullParser::START_TAG:
                // parse item
                AutoPtr<IAnimator> animator;
                String name;
                FAIL_RETURN(parser->GetName(&name));
                if (name.Equals("item")) {
                    Int32 attributeCount = 0;
                    FAIL_RETURN(parser->GetAttributeCount(&attributeCount));
                    AutoPtr<ArrayOf<Int32> > states = ArrayOf<Int32>::Alloc(attributeCount);
                    Int32 stateIndex = 0;
                    for (Int32 i = 0; i < attributeCount; i++) {
                        Int32 attrName = 0;
                        FAIL_RETURN(attributeSet->GetAttributeNameResource(i, &attrName));
                        if (attrName == R::attr::animation) {
                            Int32 v = 0;
                            FAIL_RETURN(attributeSet->GetAttributeResourceValue(i, 0, &v));
                            FAIL_RETURN(LoadAnimator(context, v, (IAnimator**)&animator));
                        } else {
                            Boolean v = FALSE;
                            FAIL_RETURN(attributeSet->GetAttributeBooleanValue(i, FALSE, &v));
                            (*states)[stateIndex++] = v ? attrName : -attrName;
                        }
                    }

                    if (animator == NULL) {
                        AutoPtr<IResources> res;
                        context->GetResources((IResources**)&res);
                        AutoPtr<IResourcesTheme> theme;
                        context->GetTheme((IResourcesTheme**)&theme);
                        FAIL_RETURN(CreateAnimatorFromXml(res, theme, parser, 1, (IAnimator**)&animator));
                    }

                    if (animator == NULL) {
                        return E_RESOURCES_NOT_FOUND_EXCEPTION;
                        // throw new Resources.NotFoundException(
                        //         "animation state item must have a valid animation");
                    }
                    stateListAnimator->AddState(StateSet::TrimStateSet(states, stateIndex), animator);
                }
                break;
        }
    }

    return NOERROR;
}

void AnimatorInflater::ParseAnimatorFromTypeArray(
    /* [in] */ IValueAnimator* anim,
    /* [in] */ ITypedArray* arrayAnimator,
    /* [in] */ ITypedArray* arrayObjectAnimator,
    /* [in] */ Float pixelSize)
{
    Int32 value = 0;
    arrayAnimator->GetInt32(R::styleable::Animator_duration, 300, &value);
    Int64 duration = value;

    arrayAnimator->GetInt32(R::styleable::Animator_startOffset, 0, &value);
    Int64 startDelay = value;

    Int32 valueType = 0;
    arrayAnimator->GetInt32(R::styleable::Animator_valueType, VALUE_TYPE_FLOAT, &valueType);

    AutoPtr<ITypeEvaluator> evaluator;
    Boolean getFloats = (valueType == VALUE_TYPE_FLOAT);

    AutoPtr<ITypedValue> tvFrom;
    arrayAnimator->PeekValue(R::styleable::Animator_valueFrom, (ITypedValue**)&tvFrom);
    Boolean hasFrom = (tvFrom != NULL);
    Int32 fromType = 0;
    if (hasFrom) {
        tvFrom->GetType(&fromType);
    }

    AutoPtr<ITypedValue> tvTo;
    arrayAnimator->PeekValue(R::styleable::Animator_valueTo, (ITypedValue**)&tvTo);
    Boolean hasTo = (tvTo != NULL);
    Int32 toType = 0;
    if (hasTo) {
        tvTo->GetType(&toType);
    }

    // TODO: Further clean up this part of code into 4 types : path, color,
    // integer and float.
    if (valueType == VALUE_TYPE_PATH) {
        SetupAnimatorForPath(anim, arrayAnimator, (ITypeEvaluator**)&evaluator);
    } else {
        // Integer and float value types are handled here.
        if ((hasFrom && (fromType >= ITypedValue::TYPE_FIRST_COLOR_INT) &&
                (fromType <= ITypedValue::TYPE_LAST_COLOR_INT)) ||
                (hasTo && (toType >= ITypedValue::TYPE_FIRST_COLOR_INT) &&
                        (toType <= ITypedValue::TYPE_LAST_COLOR_INT))) {
            // special case for colors: ignore valueType and get ints
            getFloats = FALSE;
            evaluator = ITypeEvaluator::Probe(CArgbEvaluator::GetInstance());
        }
        SetupValues(anim, arrayAnimator, getFloats, hasFrom, fromType, hasTo, toType);
    }

    IAnimator::Probe(anim)->SetDuration(duration);
    IAnimator::Probe(anim)->SetStartDelay(startDelay);

    Boolean hasValue = FALSE;
    if (arrayAnimator->HasValue(R::styleable::Animator_repeatCount, &hasValue), hasValue) {
        Int32 v = 0;
        arrayAnimator->GetInt32(R::styleable::Animator_repeatCount, 0, &v);
        anim->SetRepeatCount(v);
    }
    if (arrayAnimator->HasValue(R::styleable::Animator_repeatMode, &hasValue), hasValue) {
        Int32 v = 0;
        arrayAnimator->GetInt32(R::styleable::Animator_repeatMode, IValueAnimator::ANIMATION_RESTART, &v);
        anim->SetRepeatMode(v);
    }
    if (evaluator != NULL) {
        anim->SetEvaluator(evaluator);
    }

    if (arrayObjectAnimator != NULL) {
        SetupObjectAnimator(anim, arrayObjectAnimator, getFloats, pixelSize);
    }
}

ECode AnimatorInflater::SetupAnimatorForPath(
    /* [in] */ IValueAnimator* anim,
    /* [in] */ ITypedArray* arrayAnimator,
    /* [out] */ ITypeEvaluator** te)
{
    VALIDATE_NOT_NULL(te);
    *te = NULL;
    AutoPtr<ITypeEvaluator> evaluator;
    String fromString;
    arrayAnimator->GetString(R::styleable::Animator_valueFrom, &fromString);
    String toString;
    arrayAnimator->GetString(R::styleable::Animator_valueTo, &toString);
    AutoPtr<ArrayOf<PathDataNode*> > nodesFrom = PathParser::CreateNodesFromPathData(fromString);
    AutoPtr<ArrayOf<PathDataNode*> > nodesTo = PathParser::CreateNodesFromPathData(toString);

    if (nodesFrom != NULL) {
        if (nodesTo != NULL) {
            AutoPtr<IArrayList> nfa, nta;
            CArrayList::New((IArrayList**)&nfa);
            for (Int32 i = 0; i < nodesFrom->GetLength(); i++) {
                nfa->Add((IObject*)((*nodesFrom)[i]->Probe(EIID_IObject)));
            }
            CArrayList::New((IArrayList**)&nta);
            for (Int32 i = 0; i < nodesTo->GetLength(); i++) {
                nta->Add((IObject*)((*nodesTo)[i]->Probe(EIID_IObject)));
            }

            AutoPtr<ArrayOf<IInterface*> > objs = ArrayOf<IInterface*>::Alloc(2);
            objs->Set(0, nfa);
            objs->Set(1, nta);
            anim->SetObjectValues(objs);
            if (!PathParser::CanMorph(nodesFrom, nodesTo)) {
                // throw new InflateException(arrayAnimator.getPositionDescription()
                //         + " Can't morph from " + fromString + " to " + toString);
                return E_INFLATE_EXCEPTION;
            }
        } else {
            AutoPtr<IArrayList> nfa;
            CArrayList::New((IArrayList**)&nfa);
            for (Int32 i = 0; i < nodesFrom->GetLength(); i++) {
                nfa->Add((IObject*)((*nodesFrom)[i]->Probe(EIID_IObject)));
            }
            AutoPtr<ArrayOf<IInterface*> > objs = ArrayOf<IInterface*>::Alloc(1);
            objs->Set(0, nfa);
            anim->SetObjectValues(objs);
        }
        *te = new PathDataEvaluator(PathParser::DeepCopyNodes(nodesFrom));
        REFCOUNT_ADD(*te);
    } else if (nodesTo != NULL) {
        AutoPtr<IArrayList> nta;
        CArrayList::New((IArrayList**)&nta);
        for (Int32 i = 0; i < nodesTo->GetLength(); i++) {
            nta->Add((IObject*)((*nodesTo)[i]->Probe(EIID_IObject)));
        }

        AutoPtr<ArrayOf<IInterface*> > objs = ArrayOf<IInterface*>::Alloc(2);
        objs->Set(0, nta);
        anim->SetObjectValues(objs);
        *te = new PathDataEvaluator(PathParser::DeepCopyNodes(nodesTo));
        REFCOUNT_ADD(*te);
    }

    if (DBG_ANIMATOR_INFLATER && evaluator != NULL) {
        Slogger::V(TAG, "create a new PathDataEvaluator here");
    }

    return NOERROR;
}

ECode AnimatorInflater::SetupObjectAnimator(
    /* [in] */ IValueAnimator* anim,
    /* [in] */ ITypedArray* arrayObjectAnimator,
    /* [in] */ Boolean getFloats,
    /* [in] */ Float pixelSize)
{
    AutoPtr<IObjectAnimator> oa = IObjectAnimator::Probe(anim);
    String pathData;
    arrayObjectAnimator->GetString(R::styleable::PropertyAnimator_pathData, &pathData);

    // Note that if there is a pathData defined in the Object Animator,
    // valueFrom / valueTo will be ignored.
    if (pathData != NULL) {
        String propertyXName;
        arrayObjectAnimator->GetString(R::styleable::PropertyAnimator_propertyXName, &propertyXName);
        String propertyYName;
        arrayObjectAnimator->GetString(R::styleable::PropertyAnimator_propertyYName, &propertyYName);

        if (propertyXName == NULL && propertyYName == NULL) {
            // throw new InflateException(arrayObjectAnimator.getPositionDescription()
            //         + " propertyXName or propertyYName is needed for PathData");
            return E_INFLATE_EXCEPTION;
        } else {
            AutoPtr<IPath> path = PathParser::CreatePathFromPathData(pathData);
            Float error = 0.5f * pixelSize; // max half a pixel error
            AutoPtr<IPathKeyframes> keyframeSet = KeyframeSet::OfPath(path, error);
            AutoPtr<IKeyframes> xKeyframes;
            AutoPtr<IKeyframes> yKeyframes;
            if (getFloats) {
                xKeyframes = IKeyframes::Probe(((PathKeyframes*)keyframeSet.Get())->CreateXFloatKeyframes());
                yKeyframes = IKeyframes::Probe(((PathKeyframes*)keyframeSet.Get())->CreateYFloatKeyframes());
            } else {
                xKeyframes = IKeyframes::Probe(((PathKeyframes*)keyframeSet.Get())->CreateXInt32Keyframes());
                yKeyframes = IKeyframes::Probe(((PathKeyframes*)keyframeSet.Get())->CreateYInt32Keyframes());
            }
            AutoPtr<IPropertyValuesHolder> x;
            AutoPtr<IPropertyValuesHolder> y;
            if (propertyXName != NULL) {
                x = PropertyValuesHolder::OfKeyframes(propertyXName, xKeyframes);
            }
            if (propertyYName != NULL) {
                y = PropertyValuesHolder::OfKeyframes(propertyYName, yKeyframes);
            }

            AutoPtr<ArrayOf<IPropertyValuesHolder*> > va;
            if (x == NULL) {
                va = ArrayOf<IPropertyValuesHolder*>::Alloc(1);
                va->Set(0, y);
                IValueAnimator::Probe(oa)->SetValues(va);
            } else if (y == NULL) {
                va = ArrayOf<IPropertyValuesHolder*>::Alloc(1);
                va->Set(0, x);
                IValueAnimator::Probe(oa)->SetValues(va);
            } else {
                va = ArrayOf<IPropertyValuesHolder*>::Alloc(2);
                va->Set(0, x);
                va->Set(1, y);
                IValueAnimator::Probe(oa)->SetValues(va);
            }
        }
    } else {
        String propertyName;
        arrayObjectAnimator->GetString(R::styleable::PropertyAnimator_propertyName, &propertyName);
        oa->SetPropertyName(propertyName);
    }

    return NOERROR;
}

void AnimatorInflater::SetupValues(
    /* [in] */ IValueAnimator* anim,
    /* [in] */ ITypedArray* arrayAnimator,
    /* [in] */ Boolean getFloats,
    /* [in] */ Boolean hasFrom,
    /* [in] */ Int32 fromType,
    /* [in] */ Boolean hasTo,
    /* [in] */ Int32 toType)
{
    Int32 valueFromIndex = R::styleable::Animator_valueFrom;
    Int32 valueToIndex = R::styleable::Animator_valueTo;
    if (getFloats) {
        Float valueFrom;
        Float valueTo;
        if (hasFrom) {
            if (fromType == ITypedValue::TYPE_DIMENSION) {
                arrayAnimator->GetDimension(valueFromIndex, 0, &valueFrom);
            } else {
                arrayAnimator->GetFloat(valueFromIndex, 0, &valueFrom);
            }
            if (hasTo) {
                if (toType == ITypedValue::TYPE_DIMENSION) {
                    arrayAnimator->GetDimension(valueToIndex, 0, &valueTo);
                } else {
                    arrayAnimator->GetFloat(valueToIndex, 0, &valueTo);
                }

                AutoPtr<ArrayOf<Float> > fa = ArrayOf<Float>::Alloc(2);
                fa->Set(0, valueFrom);
                fa->Set(1, valueTo);
                anim->SetFloatValues(fa);
            } else {
                AutoPtr<ArrayOf<Float> > fa = ArrayOf<Float>::Alloc(1);
                fa->Set(0, valueFrom);
                anim->SetFloatValues(fa);
            }
        } else {
            if (toType == ITypedValue::TYPE_DIMENSION) {
                arrayAnimator->GetDimension(valueToIndex, 0, &valueTo);
            } else {
                arrayAnimator->GetFloat(valueToIndex, 0, &valueTo);
            }

            AutoPtr<ArrayOf<Float> > fa = ArrayOf<Float>::Alloc(1);
            fa->Set(0, valueTo);
            anim->SetFloatValues(fa);
        }
    } else {
        Int32 valueFrom;
        Int32 valueTo;
        if (hasFrom) {
            Float tmp = 0;
            if (fromType == ITypedValue::TYPE_DIMENSION) {
                arrayAnimator->GetDimension(valueFromIndex, 0, &tmp);
                valueFrom = tmp;
            } else if ((fromType >= ITypedValue::TYPE_FIRST_COLOR_INT) &&
                    (fromType <= ITypedValue::TYPE_LAST_COLOR_INT)) {
                arrayAnimator->GetColor(valueFromIndex, 0, &valueFrom);
            } else {
                arrayAnimator->GetInt32(valueFromIndex, 0, &valueFrom);
            }
            if (hasTo) {
                if (toType == ITypedValue::TYPE_DIMENSION) {
                    arrayAnimator->GetDimension(valueToIndex, 0, &tmp);
                    valueTo = tmp;
                } else if ((toType >= ITypedValue::TYPE_FIRST_COLOR_INT) &&
                        (toType <= ITypedValue::TYPE_LAST_COLOR_INT)) {
                    arrayAnimator->GetColor(valueToIndex, 0, &valueTo);
                } else {
                    arrayAnimator->GetInt32(valueToIndex, 0, &valueTo);
                }

                AutoPtr<ArrayOf<Int32> > ia = ArrayOf<Int32>::Alloc(2);
                ia->Set(0, valueFrom);
                ia->Set(1, valueTo);
                anim->SetInt32Values(ia);
            } else {
                AutoPtr<ArrayOf<Int32> > ia = ArrayOf<Int32>::Alloc(1);
                ia->Set(0, valueFrom);
                anim->SetInt32Values(ia);
            }
        } else {
            if (hasTo) {
                if (toType == ITypedValue::TYPE_DIMENSION) {
                    Float tmp = 0;
                    arrayAnimator->GetDimension(valueToIndex, 0, &tmp);
                    valueTo = tmp;
                } else if ((toType >= ITypedValue::TYPE_FIRST_COLOR_INT) &&
                        (toType <= ITypedValue::TYPE_LAST_COLOR_INT)) {
                    arrayAnimator->GetColor(valueToIndex, 0, &valueTo);
                } else {
                    arrayAnimator->GetInt32(valueToIndex, 0, &valueTo);
                }

                AutoPtr<ArrayOf<Int32> > ia = ArrayOf<Int32>::Alloc(1);
                ia->Set(0, valueTo);
                anim->SetInt32Values(ia);
            }
        }
    }
}

ECode AnimatorInflater::CreateAnimatorFromXml(
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ Float pixelSize,
    /* [out] */ IAnimator** animator) /*throws XmlPullParserException, IOException */
{
    VALIDATE_NOT_NULL(animator);
    AutoPtr<IAttributeSet> set = Xml::AsAttributeSet(parser);
    return CreateAnimatorFromXml(res, theme, parser, set, NULL, 0,
            pixelSize, animator);
}

ECode AnimatorInflater::CreateAnimatorFromXml(
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IAnimatorSet* parent,
    /* [in] */ Int32 sequenceOrdering,
    /* [in] */ Float pixelSize,
    /* [out] */ IAnimator** animator) /*throws XmlPullParserException, IOException*/
{
    VALIDATE_NOT_NULL(animator);
    *animator = NULL;
    // ArrayList<Animator> childAnims = null;
    List<AutoPtr<IAnimator> > childAnims;

    // Make sure we are on a start tag.
    Int32 type;
    Int32 depth = 0, tmpD = 0;
    parser->GetDepth(&depth);
    ECode ec = NOERROR;

    AutoPtr<IAnimator> anim;
    while (((parser->Next(&type), type) != IXmlPullParser::END_TAG || (parser->GetDepth(&tmpD) ,tmpD) > depth)
            && type != IXmlPullParser::END_DOCUMENT) {

        if (type != IXmlPullParser::START_TAG) {
            continue;
        }

        String name;
        parser->GetName(&name);

        if (name.Equals(String("objectAnimator"))) {
            AutoPtr<IObjectAnimator> oa;
            FAIL_RETURN(LoadObjectAnimator(res, theme, attrs, pixelSize, (IObjectAnimator**)&oa));
            anim = IAnimator::Probe(oa);
        }
        else if (name.Equals(String("animator"))) {
            AutoPtr<IValueAnimator> va;
            FAIL_RETURN(LoadAnimator(res, theme, attrs, NULL, pixelSize, (IValueAnimator**)&va));
            anim = IAnimator::Probe(va);
        }
        else if (name.Equals(String("set"))) {
            anim = NULL;
            CAnimatorSet::New((IAnimator**)&anim);
            AutoPtr<ITypedArray> a;

            AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
                    const_cast<Int32 *>(R::styleable::AnimatorSet),
                    ArraySize(R::styleable::AnimatorSet));
            if (theme != NULL) {
                theme->ObtainStyledAttributes(attrs, attrIds, 0, 0, (ITypedArray**)&a);
            } else {
                res->ObtainAttributes(attrs, attrIds, (ITypedArray**)&a);
            }
            Int32 ordering = 0;
            a->GetInt32(R::styleable::AnimatorSet_ordering, TOGETHER, &ordering);
            AutoPtr<IAnimator> tmp;
            ec = CreateAnimatorFromXml(res, theme, parser, attrs, IAnimatorSet::Probe(anim), ordering,
                    pixelSize, (IAnimator**)&tmp);
            a->Recycle();
            if (FAILED(ec)) {
                return ec;
            }
        } else {
            // throw new RuntimeException("Unknown animator name: " + parser.getName());
            return E_RUNTIME_EXCEPTION;
        }

        if (parent != NULL) {
            childAnims.PushBack(anim);
        }
    }
    if (parent != NULL && childAnims.GetSize() > 0) {
        AutoPtr<ArrayOf<IAnimator*> > animsArray = ArrayOf<IAnimator*>::Alloc(childAnims.GetSize());
        Int32 index = 0;
        List<AutoPtr<IAnimator> >::Iterator ator = childAnims.Begin();
        for (; ator != childAnims.End(); ++ator) {
            animsArray->Set(index++, *ator);
        }
        if (sequenceOrdering == TOGETHER) {
            parent->PlayTogether(animsArray);
        } else {
            parent->PlaySequentially(animsArray);
        }
    }

    *animator = anim;
    REFCOUNT_ADD(*animator)
    return NOERROR;

}

ECode AnimatorInflater::LoadObjectAnimator(
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Float pathErrorScale,
    /* [out] */ IObjectAnimator** animator) /*throws NotFoundException*/
{
    VALIDATE_NOT_NULL(animator);
    CObjectAnimator::New(animator);

    AutoPtr<IValueAnimator> tmp;
    return LoadAnimator(res, theme, attrs, IValueAnimator::Probe(*animator), pathErrorScale, (IValueAnimator**)&tmp);
}

ECode AnimatorInflater::LoadAnimator(
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IValueAnimator* anim,
    /* [in] */ Float pathErrorScale,
    /* [out] */ IValueAnimator** animator) /*throws NotFoundException*/
{
    VALIDATE_NOT_NULL(animator);
    *animator = NULL;
    Int32 resID = 0;
    AutoPtr<ITypedArray> arrayAnimator;
    AutoPtr<ITypedArray> arrayObjectAnimator;
    ECode ec = NOERROR;

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::Animator),
            ArraySize(R::styleable::Animator));
    if (theme != NULL) {
        ec = theme->ObtainStyledAttributes(attrs,attrIds, 0, 0, (ITypedArray**)&arrayAnimator);
        FAIL_GOTO(ec, error);
    } else {
        ec = res->ObtainAttributes(attrs, attrIds, (ITypedArray**)&arrayAnimator);
        FAIL_GOTO(ec, error);
    }

    // If anim is not null, then it is an object animator.
    if (anim != NULL) {
        attrIds = ArrayOf<Int32>::Alloc(
                const_cast<Int32 *>(R::styleable::PropertyAnimator),
                ArraySize(R::styleable::PropertyAnimator));
        if (theme != NULL) {
            ec = theme->ObtainStyledAttributes(attrs,
                    attrIds, 0, 0, (ITypedArray**)&arrayObjectAnimator);
            FAIL_GOTO(ec, error);
        } else {
            ec = res->ObtainAttributes(attrs, attrIds, (ITypedArray**)&arrayObjectAnimator);
            FAIL_GOTO(ec, error);
        }
    }

    if (anim == NULL) {
        CValueAnimator::New((IAnimator**)&anim);
    }

    ParseAnimatorFromTypeArray(anim, arrayAnimator, arrayObjectAnimator, pathErrorScale);

    ec = arrayAnimator->GetResourceId(R::styleable::Animator_interpolator, 0, &resID);
    FAIL_GOTO(ec, error);
    if (resID > 0) {
        AutoPtr<IInterpolator> timeInterpolator;
        AnimationUtils::LoadInterpolator(res, theme, resID, (IInterpolator**)&timeInterpolator);
        ec = IAnimator::Probe(anim)->SetInterpolator(ITimeInterpolator::Probe(timeInterpolator));
        FAIL_GOTO(ec, error);
    }

error:
    arrayAnimator->Recycle();
    if (arrayObjectAnimator != NULL) {
        arrayObjectAnimator->Recycle();
    }

    *animator = anim;
    REFCOUNT_ADD(*animator);
    return ec;
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos
