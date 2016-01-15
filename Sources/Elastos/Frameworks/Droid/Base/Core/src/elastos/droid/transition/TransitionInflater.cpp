
#include "Elastos.Droid.Content.h"
#include <Elastos.CoreLibrary.External.h>
#include "elastos/droid/transition/TransitionInflater.h"
#include "elastos/droid/transition/CFade.h"
#include "elastos/droid/transition/CChangeBounds.h"
#include "elastos/droid/transition/CSlide.h"
#include "elastos/droid/transition/CExplode.h"
#include "elastos/droid/transition/CChangeImageTransform.h"
#include "elastos/droid/transition/CChangeTransform.h"
#include "elastos/droid/transition/CChangeClipBounds.h"
#include "elastos/droid/transition/CAutoTransition.h"
#include "elastos/droid/transition/CRecolor.h"
#include "elastos/droid/transition/CChangeScroll.h"
#include "elastos/droid/transition/CTransitionSet.h"
#include "elastos/droid/transition/CTransitionManager.h"
#include "elastos/droid/transition/CArcMotion.h"
#include "elastos/droid/transition/CPatternPathMotion.h"
#include "elastos/droid/transition/CScene.h"
#include "elastos/droid/utility/CArrayMap.h"
#include "elastos/droid/utility/Xml.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::Xml;

namespace Elastos {
namespace Droid {
namespace Transition {

//===============================================================
// TransitionInflater::
//===============================================================

// private static final Class<?>[] sConstructorSignature = new Class[] {
//         Context.class, AttributeSet.class};

AutoPtr<IArrayMap> TransitionInflater::sConstructors;

CAR_INTERFACE_IMPL(TransitionInflater, Object, ITransitionInflater)

TransitionInflater::TransitionInflater()
{
    CArrayMap::New((IArrayMap**)&sConstructors);
}

ECode TransitionInflater::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    return NOERROR;
}

AutoPtr<ITransitionInflater> TransitionInflater::From(
    /* [in] */ IContext* context)
{
    AutoPtr<TransitionInflater> res = new TransitionInflater();
    res->constructor(context);
    return ITransitionInflater::Probe(res);
}

ECode TransitionInflater::InflateTransition(
    /* [in] */ Int32 resource,
    /* [out] */ ITransition** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    AutoPtr<IXmlResourceParser> parser;
    res->GetXml(resource, (IXmlResourceParser**)&parser);
    AutoPtr<ITransition> p = CreateTransitionFromXml(IXmlPullParser::Probe(parser), Xml::AsAttributeSet(IXmlPullParser::Probe(parser)), NULL);
    *result = p;
    REFCOUNT_ADD(*result)
    parser->Close();
    return NOERROR;
}

ECode TransitionInflater::InflateTransitionManager(
    /* [in] */ Int32 resource,
    /* [in] */ IViewGroup* sceneRoot,
    /* [out] */ ITransitionManager** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    AutoPtr<IXmlResourceParser> parser;
    res->GetXml(resource, (IXmlResourceParser**)&parser);
    AutoPtr<ITransitionManager> p = CreateTransitionManagerFromXml(IXmlPullParser::Probe(parser), Xml::AsAttributeSet(IXmlPullParser::Probe(parser)), sceneRoot);
    *result = p;
    REFCOUNT_ADD(*result)
    parser->Close();
    return NOERROR;
}

AutoPtr<ITransition> TransitionInflater::CreateTransitionFromXml(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ ITransition* parent)
{
    AutoPtr<ITransition> transition;

    // Make sure we are on a start tag.
    Int32 type = 0;
    Int32 depth = 0;
    parser->GetDepth(&depth);

    AutoPtr<ITransitionSet> transitionSet = (ITransitionSet::Probe(parent) != NULL)
            ? ITransitionSet::Probe(parent) : NULL;

    Int32 dpt = 0;
    while (((parser->Next(&type), type) != IXmlPullParser::END_TAG || (parser->GetDepth(&dpt), dpt) > depth)
            && type != IXmlPullParser::END_DOCUMENT) {

        if (type != IXmlPullParser::START_TAG) {
            continue;
        }

        String name;
        parser->GetName(&name);
        if (name.Equals(String("fade"))) {
            AutoPtr<IFade> p;
            CFade::New(mContext, attrs, (IFade**)&p);
            transition = ITransition::Probe(p);
        }
        else if (name.Equals(String("changeBounds"))) {
            AutoPtr<IChangeBounds> p;
            CChangeBounds::New(mContext, attrs, (IChangeBounds**)&p);
            transition = ITransition::Probe(p);
        }
        else if (name.Equals(String("slide"))) {
            AutoPtr<ISlide> p;
            CSlide::New(mContext, attrs, (ISlide**)&p);
            transition = ITransition::Probe(p);
        }
        else if (name.Equals(String("explode"))) {
            AutoPtr<IVisibility> p;
            CExplode::New(mContext, attrs, (IVisibility**)&p);
            transition = ITransition::Probe(p);
        }
        else if (name.Equals(String("changeImageTransform"))) {
            AutoPtr<ITransition> p;
            CChangeImageTransform::New(mContext, attrs, (ITransition**)&p);
            transition = p;
        }
        else if (name.Equals(String("changeTransform"))) {
            AutoPtr<IChangeTransform> p;
            CChangeTransform::New(mContext, attrs, (IChangeTransform**)&p);
            transition = ITransition::Probe(p);
        }
        else if (name.Equals(String("changeClipBounds"))) {
            AutoPtr<ITransition> p;
            CChangeClipBounds::New(mContext, attrs, (ITransition**)&p);
            transition = p;
        }
        else if (name.Equals(String("autoTransition"))) {
            AutoPtr<ITransitionSet> p;
            CAutoTransition::New(mContext, attrs, (ITransitionSet**)&p);
            transition = ITransition::Probe(p);
        }
        else if (name.Equals(String("recolor"))) {
            AutoPtr<ITransition> p;
            CRecolor::New(mContext, attrs, (ITransition**)&p);
            transition = p;
        }
        else if (name.Equals(String("changeScroll"))) {
            AutoPtr<ITransition> p;
            CChangeScroll::New(mContext, attrs, (ITransition**)&p);
            transition = p;
        }
        else if (name.Equals(String("transitionSet"))) {
            AutoPtr<ITransitionSet> p;
            CTransitionSet::New(mContext, attrs, (ITransitionSet**)&p);
            transition = ITransition::Probe(p);
        }
        else if (name.Equals(String("transition"))) {
            assert(0 && "TODO");
//            transition = (Transition) createCustom(attrs, Transition.class, "transition");
        }
        else if (name.Equals(String("targets"))) {
            GetTargetIds(parser, attrs, parent);
        }
        else if (name.Equals(String("arcMotion"))) {
            AutoPtr<IArcMotion> p;
            CArcMotion::New(mContext, attrs, (IArcMotion**)&p);
            parent->SetPathMotion(IPathMotion::Probe(p));
        }
        else if (name.Equals(String("pathMotion"))) {
            assert(0 && "TODO");
//            parent->SetPathMotion((PathMotion)createCustom(attrs, PathMotion.class, "pathMotion"));
        }
        else if (name.Equals(String("patternPathMotion"))) {
            AutoPtr<IPatternPathMotion> p;
            CPatternPathMotion::New(mContext, attrs, (IPatternPathMotion**)&p);
            parent->SetPathMotion(IPathMotion::Probe(p));
        }
        else {
//            return E_RUNTIME_EXCEPTION;
            return NULL;
        }
        if (transition != NULL) {
            Boolean bIsTag = FALSE;
            if (!(parser->IsEmptyElementTag(&bIsTag), bIsTag)) {
                CreateTransitionFromXml(parser, attrs, transition);
            }
            if (transitionSet != NULL) {
                transitionSet->AddTransition(transition);
                transition = NULL;
            }
            else if (parent != NULL) {
                return NULL;
//                return E_INFLATE_EXCEPTION;
//                throw new InflateException("Could not add transition to another transition.");
            }
        }
    }

    return transition;
}

// private Object createCustom(AttributeSet attrs, Class expectedType, String tag)
// {
//     String className = attrs.getAttributeValue(null, "class");

//     if (className == null) {
//         throw new InflateException(tag + " tag must have a 'class' attribute");
//     }

//     try {
//         synchronized(sConstructors) {
//             Constructor constructor = sConstructors.get(className);
//             if (constructor == null) {
//                 Class c = mContext.getClassLoader().loadClass(className)
//                         .asSubclass(expectedType);
//                 if (c != null) {
//                     constructor = c.getConstructor(sConstructorSignature);
//                     sConstructors.put(className, constructor);
//                 }
//             }

//             return constructor.newInstance(mContext, attrs);
//         }
//     } catch (InstantiationException e) {
//         throw new InflateException("Could not instantiate " + expectedType + " class " +
//                 className, e);
//     } catch (ClassNotFoundException e) {
//         throw new InflateException("Could not instantiate " + expectedType + " class " +
//                 className, e);
//     } catch (InvocationTargetException e) {
//         throw new InflateException("Could not instantiate " + expectedType + " class " +
//                 className, e);
//     } catch (NoSuchMethodException e) {
//         throw new InflateException("Could not instantiate " + expectedType + " class " +
//                 className, e);
//     } catch (IllegalAccessException e) {
//         throw new InflateException("Could not instantiate " + expectedType + " class " +
//                 className, e);
//     }
// }

void TransitionInflater::GetTargetIds(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ ITransition* transition)
{
    // Make sure we are on a start tag.
    Int32 type = 0, d = 0;
    Int32 depth = 0;
    parser->GetDepth(&depth);

    while (((parser->Next(&type), type) != IXmlPullParser::END_TAG || (parser->GetDepth(&d), d) > depth)
            && type != IXmlPullParser::END_DOCUMENT) {

        if (type != IXmlPullParser::START_TAG) {
            continue;
        }

        String name;
        parser->GetName(&name);
        if (name.Equals(String("target"))) {
            AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
                const_cast<Int32*>(R::styleable::TransitionTarget),
                ArraySize(R::styleable::TransitionTarget));
            AutoPtr<ITypedArray> a;
            mContext->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);
            Int32 id = 0;
            a->GetResourceId(R::styleable::TransitionTarget_targetId, 0, &id);
            String transitionName;
            if (id != 0) {
                transition->AddTarget(id);
            }
            else if ((a->GetResourceId(R::styleable::TransitionTarget_excludeId, 0, &id), id) != 0) {
                transition->ExcludeTarget(id, TRUE);
            }
            else if ((a->GetString(R::styleable::TransitionTarget_targetName, &transitionName), transitionName.IsNull())) {
                transition->AddTarget(transitionName);
            }
            else if ((a->GetString(R::styleable::TransitionTarget_excludeName, &transitionName), transitionName.IsNull())) {
                transition->ExcludeTarget(transitionName, TRUE);
            }
            else {
                String className;
                a->GetString(R::styleable::TransitionTarget_excludeClass, &className);
                // if (className != NULL) {
                //     Class clazz = Class.forName(className);
                //     transition->ExcludeTarget(clazz, TRUE);
                // }
                // else if ((className =
                //         a->GetString(R::styleable::TransitionTarget_targetClass)) != NULL) {
                //     Class clazz = Class.forName(className);
                //     transition->AddTarget(clazz);
                // }
            }
        }
        else {
            return;
//            return E_RUNTIME_EXCEPTION;
//            throw new RuntimeException("Unknown scene name: " + parser.getName());
        }
    }
}

AutoPtr<ITransitionManager> TransitionInflater::CreateTransitionManagerFromXml(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IViewGroup* sceneRoot)
{
    // Make sure we are on a start tag.
    Int32 type = 0;
    Int32 depth = 0;
    parser->GetDepth(&depth);
    AutoPtr<ITransitionManager> transitionManager;

    Int32 dpt = 0;
    while (((parser->Next(&type), type) != IXmlPullParser::END_TAG || (parser->GetDepth(&dpt), dpt) > depth)
            && type != IXmlPullParser::END_DOCUMENT) {

        if (type != IXmlPullParser::START_TAG) {
            continue;
        }

        String name;
        parser->GetName(&name);
        if (name.Equals(String("transitionManager"))) {
            CTransitionManager::New((ITransitionManager**)&transitionManager);
        }
        else if (name.Equals(String("transition")) && (transitionManager != NULL)) {
            LoadTransition(attrs, sceneRoot, transitionManager);
        }
        else {
        //    return E_RUNTIME_EXCEPTION;
            return NULL;
        }
    }
    return transitionManager;
}

void TransitionInflater::LoadTransition(
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ ITransitionManager* transitionManager)
{
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
                const_cast<Int32*>(R::styleable::TransitionManager),
                ArraySize(R::styleable::TransitionManager));
    AutoPtr<ITypedArray> a;
    mContext->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);
    Int32 transitionId = 0;
    a->GetResourceId(R::styleable::TransitionManager_transition, -1, &transitionId);
    Int32 fromId = 0;
    a->GetResourceId(R::styleable::TransitionManager_fromScene, -1, &fromId);
    AutoPtr<IScene> fromScene = (fromId < 0) ? NULL: CScene::GetSceneForLayout(sceneRoot, fromId, mContext);
    Int32 toId = 0;
    a->GetResourceId(R::styleable::TransitionManager_toScene, -1, &toId);
    AutoPtr<IScene> toScene = (toId < 0) ? NULL : CScene::GetSceneForLayout(sceneRoot, toId, mContext);

    if (transitionId >= 0) {
        AutoPtr<ITransition> transition;
        InflateTransition(transitionId, (ITransition**)&transition);
        if (transition != NULL) {
            if (toScene == NULL) {
                return;
            //    return E_RUNTIME_EXCEPTION;
            //     throw new RuntimeException("No toScene for transition ID " + transitionId);
            }
            if (fromScene == NULL) {
                transitionManager->SetTransition(toScene, transition);
            }
            else {
                transitionManager->SetTransition(fromScene, toScene, transition);
            }
        }
    }
    a->Recycle();
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos
