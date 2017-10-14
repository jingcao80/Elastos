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

#ifndef __ELASTOS_DROID_APP_FRAGMENT_TRANSACTION_H__
#define __ELASTOS_DROID_APP_FRAGMENT_TRANSACTION_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.App.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::IPair;
using Elastos::Droid::View::IView;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * API for performing a set of Fragment operations.
 *
 * <div class="special reference">
 * <h3>Developer Guides</h3>
 * <p>For more information about using fragments, read the
 * <a href="{@docRoot}guide/topics/fundamentals/fragments.html">Fragments</a> developer guide.</p>
 * </div>
 */
class FragmentTransaction
    : public Object
    , public IFragmentTransaction
{
public:
    CAR_INTERFACE_DECL()

    FragmentTransaction();

    virtual ~FragmentTransaction();

    /**
     * Calls {@link #add(int, Fragment, String)} with a 0 containerViewId.
     */
    virtual CARAPI Add(
        /* [in] */ IFragment* fragment,
        /* [in] */ const String& tag) = 0;

    /**
     * Calls {@link #add(int, Fragment, String)} with a null tag.
     */
    virtual CARAPI Add(
        /* [in] */ Int32 containerViewId,
        /* [in] */ IFragment* fragment) = 0;

    /**
     * Add a fragment to the activity state.  This fragment may optionally
     * also have its view (if {@link Fragment#onCreateView Fragment.onCreateView}
     * returns non-null) into a container view of the activity.
     *
     * @param containerViewId Optional identifier of the container this fragment is
     * to be placed in.  If 0, it will not be placed in a container.
     * @param fragment The fragment to be added.  This fragment must not already
     * be added to the activity.
     * @param tag Optional tag name for the fragment, to later retrieve the
     * fragment with {@link FragmentManager#findFragmentByTag(String)
     * FragmentManager.findFragmentByTag(String)}.
     *
     * @return Returns the same FragmentTransaction instance.
     */
    virtual CARAPI Add(
        /* [in] */ Int32 containerViewId,
        /* [in] */ IFragment* fragment,
        /* [in] */ const String& tag) = 0;

    /**
     * Calls {@link #replace(int, Fragment, String)} with a null tag.
     */
    virtual CARAPI Replace(
        /* [in] */ Int32 containerViewId,
        /* [in] */ IFragment* fragment) = 0;

    /**
     * Replace an existing fragment that was added to a container.  This is
     * essentially the same as calling {@link #remove(Fragment)} for all
     * currently added fragments that were added with the same containerViewId
     * and then {@link #add(int, Fragment, String)} with the same arguments
     * given here.
     *
     * @param containerViewId Identifier of the container whose fragment(s) are
     * to be replaced.
     * @param fragment The new fragment to place in the container.
     * @param tag Optional tag name for the fragment, to later retrieve the
     * fragment with {@link FragmentManager#findFragmentByTag(String)
     * FragmentManager.findFragmentByTag(String)}.
     *
     * @return Returns the same FragmentTransaction instance.
     */
    virtual CARAPI Replace(
        /* [in] */ Int32 containerViewId,
        /* [in] */ IFragment* fragment,
        /* [in] */ const String& tag) = 0;

    /**
     * Remove an existing fragment.  If it was added to a container, its view
     * is also removed from that container.
     *
     * @param fragment The fragment to be removed.
     *
     * @return Returns the same FragmentTransaction instance.
     */
    virtual CARAPI Remove(
        /* [in] */ IFragment* fragment) = 0;

    /**
     * Hides an existing fragment.  This is only relevant for fragments whose
     * views have been added to a container, as this will cause the view to
     * be hidden.
     *
     * @param fragment The fragment to be hidden.
     *
     * @return Returns the same FragmentTransaction instance.
     */
    virtual CARAPI Hide(
        /* [in] */ IFragment* fragment) = 0;

    /**
     * Shows a previously hidden fragment.  This is only relevant for fragments whose
     * views have been added to a container, as this will cause the view to
     * be shown.
     *
     * @param fragment The fragment to be shown.
     *
     * @return Returns the same FragmentTransaction instance.
     */
    virtual CARAPI Show(
        /* [in] */ IFragment* fragment) = 0;

    /**
     * Detach the given fragment from the UI.  This is the same state as
     * when it is put on the back stack: the fragment is removed from
     * the UI, however its state is still being actively managed by the
     * fragment manager.  When going into this state its view hierarchy
     * is destroyed.
     *
     * @param fragment The fragment to be detached.
     *
     * @return Returns the same FragmentTransaction instance.
     */
    virtual CARAPI Detach(
        /* [in] */ IFragment* fragment) = 0;

    /**
     * Re-attach a fragment after it had previously been detached from
     * the UI with {@link #detach(Fragment)}.  This
     * causes its view hierarchy to be re-created, attached to the UI,
     * and displayed.
     *
     * @param fragment The fragment to be attached.
     *
     * @return Returns the same FragmentTransaction instance.
     */
    virtual CARAPI Attach(
        /* [in] */ IFragment* fragment) = 0;

    /**
     * @return <code>true</code> if this transaction contains no operations,
     * <code>false</code> otherwise.
     */
    virtual CARAPI IsEmpty(
        /* [out] */ Boolean* isEmpty) = 0;

    /**
     * Set specific animation resources to run for the fragments that are
     * entering and exiting in this transaction. These animations will not be
     * played when popping the back stack.
     */
    virtual CARAPI SetCustomAnimations(
        /* [in] */ Int32 enter,
        /* [in] */ Int32 exit) = 0;

    /**
     * Set specific animation resources to run for the fragments that are
     * entering and exiting in this transaction. The <code>popEnter</code>
     * and <code>popExit</code> animations will be played for enter/exit
     * operations specifically when popping the back stack.
     */
    virtual CARAPI SetCustomAnimations(
        /* [in] */ Int32 enter,
        /* [in] */ Int32 exit,
        /* [in] */ Int32 popEnter,
        /* [in] */ Int32 popExit) = 0;

    /**
     * Select a standard transition animation for this transaction.  May be
     * one of {@link #TRANSIT_NONE}, {@link #TRANSIT_FRAGMENT_OPEN},
     * or {@link #TRANSIT_FRAGMENT_CLOSE}
     */
    virtual CARAPI SetTransition(
        /* [in] */ Int32 transit) = 0;

    /**
     * TODO: remove from API
     * @hide
     */
    CARAPI SetCustomTransition(
        /* [in] */ Int32 sceneRootId,
        /* [in] */ Int32 transitionId);

    /**
     * Used with to map a View from a removed or hidden Fragment to a View from a shown
     * or added Fragment.
     * @param sharedElement A View in a disappearing Fragment to match with a View in an
     *                      appearing Fragment.
     * @param name The transitionName for a View in an appearing Fragment to match to the shared
     *             element.
     */
    virtual CARAPI AddSharedElement(
        /* [in] */ IView* sharedElement,
        /* [in] */ const String& name) = 0;

    /**
     * TODO: remove from API
     * @hide
     */
    virtual CARAPI SetSharedElement(
        /* [in] */ IView* sharedElement,
        /* [in] */ const String& name) = 0;

    /**
     * TODO: remove from API
     * @hide
     */
    virtual CARAPI SetSharedElements(
        /* [in] */ ArrayOf<IPair*>* sharedElements) = 0;//Pair<View, String>

    /**
     * Set a custom style resource that will be used for resolving transit
     * animations.
     */
    virtual CARAPI SetTransitionStyle(
        /* [in] */ Int32 styleRes) = 0;

    /**
     * Add this transaction to the back stack.  This means that the transaction
     * will be remembered after it is committed, and will reverse its operation
     * when later popped off the stack.
     *
     * @param name An optional name for this back stack state, or null.
     */
    virtual CARAPI AddToBackStack(
        /* [in] */ const String& name) = 0;

    /**
     * Returns true if this FragmentTransaction is allowed to be added to the back
     * stack. If this method would return false, {@link #addToBackStack(String)}
     * will throw {@link IllegalStateException}.
     *
     * @return True if {@link #addToBackStack(String)} is permitted on this transaction.
     */
    virtual CARAPI IsAddToBackStackAllowed(
        /* [out] */ Boolean* bval) = 0;

    /**
     * Disallow calls to {@link #addToBackStack(String)}. Any future calls to
     * addToBackStack will throw {@link IllegalStateException}. If addToBackStack
     * has already been called, this method will throw IllegalStateException.
     */
    virtual CARAPI DisallowAddToBackStack() = 0;

    /**
     * Set the full title to show as a bread crumb when this transaction
     * is on the back stack, as used by {@link FragmentBreadCrumbs}.
     *
     * @param res A string resource containing the title.
     */
    virtual CARAPI SetBreadCrumbTitle(
        /* [in] */ Int32 res) = 0;

    /**
     * Like {@link #setBreadCrumbTitle(int)} but taking a raw string; this
     * method is <em>not</em> recommended, as the string can not be changed
     * later if the locale changes.
     */
    virtual CARAPI SetBreadCrumbTitle(
        /* [in] */ ICharSequence*  text) = 0;

    /**
     * Set the short title to show as a bread crumb when this transaction
     * is on the back stack, as used by {@link FragmentBreadCrumbs}.
     *
     * @param res A string resource containing the title.
     */
    virtual CARAPI SetBreadCrumbShortTitle(
        /* [in] */ Int32 res) = 0;

    /**
     * Like {@link #setBreadCrumbShortTitle(int)} but taking a raw string; this
     * method is <em>not</em> recommended, as the string can not be changed
     * later if the locale changes.
     */
    virtual CARAPI SetBreadCrumbShortTitle(
        /* [in] */ ICharSequence*  text) = 0;

    /**
     * Schedules a commit of this transaction.  The commit does
     * not happen immediately; it will be scheduled as work on the main thread
     * to be done the next time that thread is ready.
     *
     * <p class="note">A transaction can only be committed with this method
     * prior to its containing activity saving its state.  If the commit is
     * attempted after that point, an exception will be thrown.  This is
     * because the state after the commit can be lost if the activity needs to
     * be restored from its state.  See {@link #commitAllowingStateLoss()} for
     * situations where it may be okay to lose the commit.</p>
     *
     * @return Returns the identifier of this transaction's back stack entry,
     * if {@link #addToBackStack(String)} had been called.  Otherwise, returns
     * a negative number.
     */
    virtual CARAPI Commit(
        /* [out] */ Int32* result) = 0;

    /**
     * Like {@link #commit} but allows the commit to be executed after an
     * activity's state is saved.  This is dangerous because the commit can
     * be lost if the activity needs to later be restored from its state, so
     * this should only be used for cases where it is okay for the UI state
     * to change unexpectedly on the user.
     */
    virtual CARAPI CommitAllowingStateLoss(
        /* [out] */ Int32* result) = 0;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_FRAGMENT_TRANSACTION_H__
