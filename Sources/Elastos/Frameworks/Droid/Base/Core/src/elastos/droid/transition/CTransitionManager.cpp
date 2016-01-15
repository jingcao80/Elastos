
#include "elastos/droid/transition/CTransitionManager.h"
#include "elastos/droid/transition/CAutoTransition.h"
#include "elastos/droid/transition/CScene.h"
#include "elastos/droid/utility/CArrayMap.h"

using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::View::EIID_IOnPreDrawListener;
using Elastos::Droid::View::EIID_IViewOnAttachStateChangeListener;

using Elastos::Utility::CArrayList;
using Elastos::Utility::IMap;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace Transition {

//===============================================================
// CTransitionManager::
//===============================================================
const String CTransitionManager::TAG("TransitionManager");

AutoPtr<ITransition> CTransitionManager::sDefaultTransition;

AutoPtr<ArrayOf<String> > CTransitionManager::EMPTY_STRINGS;// = new String[0];

//AutoPtr<IThreadLocal> CTransitionManager::sRunningTransitions = new ThreadLocal();
AutoPtr<IArrayList> CTransitionManager::sPendingTransitions;

CAR_OBJECT_IMPL(CTransitionManager)

CAR_INTERFACE_IMPL(CTransitionManager, Object, ITransitionManager)

CTransitionManager::CTransitionManager()
{
    CAutoTransition::New((ITransition**)&sDefaultTransition);
    CArrayList::New((IArrayList**)&sPendingTransitions);
}

ECode CTransitionManager::constructor()
{
    return NOERROR;
}

ECode CTransitionManager::SetDefaultTransition(
    /* [in] */ ITransition* transition)
{
    sDefaultTransition = transition;
    return NOERROR;
}

AutoPtr<ITransition> CTransitionManager::GetDefaultTransition()
{
    return sDefaultTransition;
}

ECode CTransitionManager::SetTransition(
    /* [in] */ IScene* scene,
    /* [in] */ ITransition* transition)
{
    IMap::Probe(mSceneTransitions)->Put(scene, transition);
    return NOERROR;
}

ECode CTransitionManager::SetTransition(
    /* [in] */ IScene* fromScene,
    /* [in] */ IScene* toScene,
    /* [in] */ ITransition* transition)
{
    AutoPtr<IInterface> p;
    IMap::Probe(mScenePairTransitions)->Get(toScene, (IInterface**)&p);
    AutoPtr<IArrayMap> sceneTransitionMap = IArrayMap::Probe(p);
    if (sceneTransitionMap == NULL) {
        CArrayMap::New((IArrayMap**)&sceneTransitionMap);
        IMap::Probe(mScenePairTransitions)->Put(toScene, sceneTransitionMap);
    }
    IMap::Probe(sceneTransitionMap)->Put(fromScene, transition);
    return NOERROR;
}

AutoPtr<ITransition> CTransitionManager::GetTransition(
    /* [in] */ IScene* scene)
{
    AutoPtr<ITransition> transition = NULL;
    AutoPtr<IViewGroup> sceneRoot;
    scene->GetSceneRoot((IViewGroup**)&sceneRoot);
    if (sceneRoot != NULL) {
        // TODO: cached in Scene instead? long-term, cache in View itself
        AutoPtr<IScene> currScene = CScene::GetCurrentScene(IView::Probe(sceneRoot));
        if (currScene != NULL) {
            AutoPtr<IInterface> sc;
            IMap::Probe(mScenePairTransitions)->Get(scene, (IInterface**)&sc);
            AutoPtr<IArrayMap> sceneTransitionMap = IArrayMap::Probe(sc);
            if (sceneTransitionMap != NULL) {
                AutoPtr<IInterface> cS;
                IMap::Probe(sceneTransitionMap)->Get(currScene, (IInterface**)&cS);
                transition = ITransition::Probe(cS);
                if (transition != NULL) {
                    return transition;
                }
            }
        }
    }
    AutoPtr<IInterface> sce;
    IMap::Probe(mSceneTransitions)->Get(scene, (IInterface**)&sce);
    transition = ITransition::Probe(sce);
    return (transition != NULL) ? transition : sDefaultTransition;
}

void CTransitionManager::ChangeScene(
    /* [in] */ IScene* scene,
    /* [in] */ ITransition* transition)
{
    AutoPtr<IViewGroup> sceneRoot;
    scene->GetSceneRoot((IViewGroup**)&sceneRoot);

    AutoPtr<ITransition> transitionClone;
    if (transition != NULL) {
        AutoPtr<Transition> ct = (Transition*)transition;
        AutoPtr<IInterface> clone;
        ct->Clone((IInterface**)&clone);
        transitionClone = ITransition::Probe(clone);
        transitionClone->SetSceneRoot(sceneRoot);
    }

    AutoPtr<IScene> oldScene = CScene::GetCurrentScene(IView::Probe(sceneRoot));
    Boolean b = FALSE;
    if (oldScene != NULL && transitionClone != NULL &&
            (oldScene->IsCreatedFromLayoutResource(&b), b)) {
        transitionClone->SetCanRemoveViews(TRUE);
    }

    SceneChangeSetup(sceneRoot, transitionClone);

    scene->Enter();

    SceneChangeRunTransition(sceneRoot, transitionClone);
}

AutoPtr<IArrayMap> CTransitionManager::GetRunningTransitions()
{
    assert(0 && "TODO");
//     AutoPtr<IWeakReference> runningTransitions;// = sRunningTransitions->Get();
//     if (runningTransitions == NULL || runningTransitions->Get() == NULL) {
//         AutoPtr<IArrayMap> transitions;
//         CArrayMap::New((IArrayMap**)&transitions);
//         runningTransitions = new WeakReference(transitions);
// //        sRunningTransitions->Set(runningTransitions);
//     }
//     return runningTransitions->Get();
    return NULL;
}

void CTransitionManager::SceneChangeRunTransition(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ ITransition* transition)
{
    if (transition != NULL && sceneRoot != NULL) {
        AutoPtr<MultiListener> listener = new MultiListener(transition, sceneRoot);
        IView::Probe(sceneRoot)->AddOnAttachStateChangeListener(listener);
        AutoPtr<IViewTreeObserver> ob;
        IView::Probe(sceneRoot)->GetViewTreeObserver((IViewTreeObserver**)&ob);
        ob->AddOnPreDrawListener(listener);
    }
}

void CTransitionManager::SceneChangeSetup(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ ITransition* transition)
{
    // Capture current values
    AutoPtr<IArrayMap> p = GetRunningTransitions();
    AutoPtr<IInterface> rt;
    IMap::Probe(p)->Get(sceneRoot, (IInterface**)&rt);
    AutoPtr<IArrayList> runningTransitions = IArrayList::Probe(rt);

    Int32 size = 0;
    if (runningTransitions != NULL && (runningTransitions->GetSize(&size), size) > 0) {
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> t;
            runningTransitions->Get(i, (IInterface**)&t);
            AutoPtr<ITransition> runningTransition = ITransition::Probe(t);
            runningTransition->Pause(IView::Probe(sceneRoot));
        }
    }

    if (transition != NULL) {
        transition->CaptureValues(sceneRoot, TRUE);
    }

    // Notify previous scene that it is being exited
    AutoPtr<IScene> previousScene = CScene::GetCurrentScene(IView::Probe(sceneRoot));
    if (previousScene != NULL) {
        previousScene->Exit();
    }
}

ECode CTransitionManager::TransitionTo(
    /* [in] */ IScene* scene)
{
    // Auto transition if there is no transition declared for the Scene, but there is
    // a root or parent view
    ChangeScene(scene, GetTransition(scene));
    return NOERROR;
}

void CTransitionManager::Go(
    /* [in] */ IScene* scene)
{
    ChangeScene(scene, sDefaultTransition);
}

void CTransitionManager::Go(
    /* [in] */ IScene* scene,
    /* [in] */ ITransition* transition)
{
    ChangeScene(scene, transition);
}

void CTransitionManager::BeginDelayedTransition(
    /* [in] */ IViewGroup* sceneRoot)
{
    BeginDelayedTransition(sceneRoot, NULL);
}

void CTransitionManager::BeginDelayedTransition(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ ITransition* transition)
{
    Boolean bContain = FALSE, bLaid = FALSE;
    if (!(sPendingTransitions->Contains(sceneRoot, &bContain), bContain) &&
        (IView::Probe(sceneRoot)->IsLaidOut(&bLaid), bLaid)) {
        // if (Transition.DBG) {
        //     Log.d(TAG, "beginDelayedTransition: root, transition = " +
        //             sceneRoot + ", " + transition);
        // }
        sPendingTransitions->Add(sceneRoot);
        if (transition == NULL) {
            transition = sDefaultTransition;
        }
        AutoPtr<Transition> ct = (Transition*)transition;
        AutoPtr<IInterface> clone;
        ct->Clone((IInterface**)&clone);
        AutoPtr<ITransition> transitionClone = ITransition::Probe(clone);
        SceneChangeSetup(sceneRoot, transitionClone);
        CScene::SetCurrentScene(IView::Probe(sceneRoot), NULL);
        SceneChangeRunTransition(sceneRoot, transitionClone);
    }
}

//===============================================================
// CTransitionManager::MultiListener::
//===============================================================
CAR_INTERFACE_IMPL_2(CTransitionManager::MultiListener, Object, IOnPreDrawListener, IViewOnAttachStateChangeListener)

CTransitionManager::MultiListener::MultiListener(
    /* [in] */ ITransition* transition,
    /* [in] */ IViewGroup* sceneRoot)
{
    mTransition = transition;
    mSceneRoot = sceneRoot;
}

void CTransitionManager::MultiListener::RemoveListeners()
{
    AutoPtr<IViewTreeObserver> ob;
    IView::Probe(mSceneRoot)->GetViewTreeObserver((IViewTreeObserver**)&ob);
    ob->RemoveOnPreDrawListener(this);
    IView::Probe(mSceneRoot)->RemoveOnAttachStateChangeListener(this);
}

ECode CTransitionManager::MultiListener::OnViewAttachedToWindow(
    /* [in] */ IView* v)
{
    return NOERROR;
}

ECode CTransitionManager::MultiListener::OnViewDetachedFromWindow(
    /* [in] */ IView* v)
{
    RemoveListeners();

    sPendingTransitions->Remove(mSceneRoot);
    AutoPtr<IArrayMap> p = GetRunningTransitions();
    AutoPtr<IInterface> rt;
    IMap::Probe(p)->Get(mSceneRoot, (IInterface**)&rt);
    AutoPtr<IArrayList> runningTransitions = IArrayList::Probe(rt);
    Int32 size = 0;
    if (runningTransitions != NULL && (runningTransitions->GetSize(&size), size) > 0) {
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> t;
            runningTransitions->Get(i, (IInterface**)&t);
            AutoPtr<ITransition> runningTransition = ITransition::Probe(t);
            runningTransition->Resume(IView::Probe(mSceneRoot));
        }
    }
    mTransition->ClearValues(TRUE);
    return NOERROR;
}

ECode CTransitionManager::MultiListener::OnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    RemoveListeners();
    sPendingTransitions->Remove(mSceneRoot);
    // Add to running list, handle end to remove it
    AutoPtr<IArrayMap> runningTransitions = GetRunningTransitions();
    AutoPtr<IInterface> cT;
    IMap::Probe(runningTransitions)->Get(mSceneRoot, (IInterface**)&cT);
    AutoPtr<IArrayList> currentTransitions = IArrayList::Probe(cT);
    AutoPtr<IArrayList> previousRunningTransitions;
    Int32 size = 0;
    if (currentTransitions == NULL) {
        CArrayList::New((IArrayList**)&currentTransitions);
        IMap::Probe(runningTransitions)->Put(mSceneRoot, currentTransitions);
    }
    else if ((currentTransitions->GetSize(&size), size) > 0) {
        CArrayList::New(ICollection::Probe(currentTransitions), (IArrayList**)&previousRunningTransitions);
    }
    currentTransitions->Add(mTransition);
    AutoPtr<TransitionListenerAdapterOverride> p = new TransitionListenerAdapterOverride(mSceneRoot, runningTransitions);
    mTransition->AddListener(ITransitionListener::Probe(p));
    mTransition->CaptureValues(mSceneRoot, FALSE);
    if (previousRunningTransitions != NULL) {
        Int32 size = 0;
        previousRunningTransitions->GetSize(&size);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> t;
            previousRunningTransitions->Get(i, (IInterface**)&t);
            AutoPtr<ITransition> runningTransition = ITransition::Probe(t);
            runningTransition->Resume(IView::Probe(mSceneRoot));
        }
    }
    mTransition->PlayTransition(mSceneRoot);
    *result = TRUE;
    return NOERROR;
}

//===============================================================
// CTransitionManager::TransitionListenerAdapterOverride::
//===============================================================
CTransitionManager::TransitionListenerAdapterOverride::TransitionListenerAdapterOverride(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ IArrayMap* runningTransitions)
{
    mSceneRoot = sceneRoot;
    mRunningTransitions = runningTransitions;
}

ECode CTransitionManager::TransitionListenerAdapterOverride::OnTransitionEnd(
    /* [in] */ ITransition* transition)
{
    AutoPtr<IInterface> r;
    IMap::Probe(mRunningTransitions)->Get(mSceneRoot, (IInterface**)&r);
    AutoPtr<IArrayList> currentTransitions = IArrayList::Probe(r);
    currentTransitions->Remove(transition);
    return NOERROR;
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos
