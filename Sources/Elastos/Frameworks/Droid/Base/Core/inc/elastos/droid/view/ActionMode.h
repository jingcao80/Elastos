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

#ifndef __ELASTOS_DROID_VIEW_ACTIONMODE_H__
#define __ELASTOS_DROID_VIEW_ACTIONMODE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace View {

class ActionMode
    : public Object
    , public IActionMode
{
public:
    CAR_INTERFACE_DECL()

    ActionMode();

    /**
     * Set a tag object associated with this ActionMode.
     *
     * <p>Like the tag available to views, this allows applications to associate arbitrary
     * data with an ActionMode for later reference.
     *
     * @param tag Tag to associate with this ActionMode
     *
     * @see #getTag()
     */
    virtual CARAPI SetTag(
        /* [in] */ IInterface* tag);

    /**
     * Retrieve the tag object associated with this ActionMode.
     *
     * <p>Like the tag available to views, this allows applications to associate arbitrary
     * data with an ActionMode for later reference.
     *
     * @return Tag associated with this ActionMode
     *
     * @see #setTag(Object)
     */
    virtual CARAPI GetTag(
        /* [out] */ IInterface** tag);

    /**
     * Set the title of the action mode. This method will have no visible effect if
     * a custom view has been set.
     *
     * @param title Title string to set
     *
     * @see #setTitle(Int32)
     * @see #setCustomView(View)
     */
    virtual CARAPI SetTitle(
        /* [in] */ ICharSequence* title) = 0;

    /**
     * Set the title of the action mode. This method will have no visible effect if
     * a custom view has been set.
     *
     * @param resId Resource ID of a string to set as the title
     *
     * @see #setTitle(CharSequence)
     * @see #setCustomView(View)
     */
    virtual CARAPI SetTitle(
        /* [in] */ Int32 resId) = 0;

    /**
     * Set the subtitle of the action mode. This method will have no visible effect if
     * a custom view has been set.
     *
     * @param subtitle Subtitle string to set
     *
     * @see #setSubtitle(Int32)
     * @see #setCustomView(View)
     */
    virtual CARAPI SetSubtitle(
        /* [in] */ ICharSequence* subtitle) = 0;

    /**
     * Set the subtitle of the action mode. This method will have no visible effect if
     * a custom view has been set.
     *
     * @param resId Resource ID of a string to set as the subtitle
     *
     * @see #setSubtitle(CharSequence)
     * @see #setCustomView(View)
     */
    virtual CARAPI SetSubtitle(
        /* [in] */ Int32 resId) = 0;

    /**
     * Set whether or not the title/subtitle display for this action mode
     * is optional.
     *
     * <p>In many cases the supplied title for an action mode is merely
     * meant to add context and is not strictly required for the action
     * mode to be useful. If the title is optional, the system may choose
     * to hide the title entirely rather than truncate it due to a lack
     * of available space.</p>
     *
     * <p>Note that this is merely a hint; the underlying implementation
     * may choose to ignore this setting under some circumstances.</p>
     *
     * @param titleOptional true if the title only presents optional information.
     */
    virtual CARAPI SetTitleOptionalHint(
        /* [in] */ Boolean titleOptional);

    /**
     * @return true if this action mode has been given a hint to consider the
     *         title/subtitle display to be optional.
     *
     * @see #setTitleOptionalHint(Boolean)
     * @see #isTitleOptional()
     */
    virtual CARAPI GetTitleOptionalHint(
        /* [out] */ Boolean* titleOptionalHint);

    /**
     * @return true if this action mode considers the title and subtitle fields
     *         as optional. Optional titles may not be displayed to the user.
     */
    virtual CARAPI IsTitleOptional(
        /* [out] */ Boolean* isTitleOptional);

    /**
     * Set a custom view for this action mode. The custom view will take the place of
     * the title and subtitle. Useful for things like search boxes.
     *
     * @param view Custom view to use in place of the title/subtitle.
     *
     * @see #setTitle(CharSequence)
     * @see #setSubtitle(CharSequence)
     */
    virtual CARAPI SetCustomView(
        /* [in] */ IView* view) = 0;

    /**
     * Invalidate the action mode and refresh menu content. The mode's
     * {@link ActionMode.Callback} will have its
     * {@link Callback#onPrepareActionMode(ActionMode, Menu)} method called.
     * If it returns true the menu will be scanned for updated content and any relevant changes
     * will be reflected to the user.
     */
    virtual CARAPI Invalidate() = 0;

    /**
     * Finish and close this action mode. The action mode's {@link ActionMode.Callback} will
     * have its {@link Callback#onDestroyActionMode(ActionMode)} method called.
     */
    virtual CARAPI Finish() = 0;

    /**
     * Returns the menu of actions that this action mode presents.
     * @return The action mode's menu.
     */
    virtual CARAPI GetMenu(
        /* [out] */ IMenu** menu) = 0;

    /**
     * Returns the current title of this action mode.
     * @return Title text
     */
    virtual CARAPI GetTitle(
        /* [out] */ ICharSequence** title) = 0;

    /**
     * Returns the current subtitle of this action mode.
     * @return Subtitle text
     */
    virtual CARAPI GetSubtitle(
        /* [out] */ ICharSequence** subtitle) = 0;

    /**
     * Returns the current custom view for this action mode.
     * @return The current custom view
     */
    virtual CARAPI GetCustomView(
        /* [out] */ IView** view) = 0;

    /**
     * Returns a {@link MenuInflater} with the ActionMode's context.
     */
    virtual CARAPI GetMenuInflater(
        /* [out] */ IMenuInflater** menuInflater) = 0;

    /**
     * Returns whether the UI presenting this action mode can take focus or not.
     * This is used by internal components within the framework that would otherwise
     * present an action mode UI that requires focus, such as an EditText as a custom view.
     *
     * @return true if the UI used to show this action mode can take focus
     * @hide Internal use only
     */
    virtual CARAPI IsUiFocusable(
        /* [out] */ Boolean* isUiFocusable);

private:
    AutoPtr<IInterface> mTag;
    Boolean mTitleOptionalHint;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_ACTIONMODE_H__
