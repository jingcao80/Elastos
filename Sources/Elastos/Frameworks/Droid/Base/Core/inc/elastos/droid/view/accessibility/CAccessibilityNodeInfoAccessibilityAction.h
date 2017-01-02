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

#ifndef __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYNODEINFOACCESSIBILITYACTION_H__
#define __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYNODEINFOACCESSIBILITYACTION_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "_Elastos_Droid_View_Accessibility_CAccessibilityNodeInfoAccessibilityAction.h"
#include "elastos/droid/utility/Pools.h"

using Elastos::Droid::Utility::IArraySet;
using Elastos::Droid::Utility::Pools;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CarClass(CAccessibilityNodeInfoAccessibilityAction)
    , public Object
    , public IAccessibilityNodeInfoAccessibilityAction
{
    friend class CAccessibilityNodeInfo;
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAccessibilityNodeInfoAccessibilityAction();

    ~CAccessibilityNodeInfoAccessibilityAction();

    CARAPI constructor(
        /* [in] */ Int32 actionId,
        /* [in] */ ICharSequence* label);

    /**
     * Gets the id for this action.
     *
     * @return The action id.
     */
    CARAPI GetId(
        /* [out] */ Int32* actionId);

    /**
     * Gets the label for this action. Its purpose is to describe the
     * action to user.
     *
     * @return The label.
     */
    CARAPI GetLabel(
        /* [out] */ ICharSequence** label);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* info);

public:
    /**
     * Action that gives input focus to the node.
     */
    static const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> ACTION_FOCUS;

    /**
     * Action that clears input focus of the node.
     */
    static const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> ACTION_CLEAR_FOCUS;

    /**
     *  Action that selects the node.
     */
    static const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> ACTION_SELECT;

    /**
     * Action that deselects the node.
     */
    static const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> ACTION_CLEAR_SELECTION;

    /**
     * Action that clicks on the node info.
     */
    static const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> ACTION_CLICK;

    /**
     * Action that long clicks on the node.
     */
    static const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> ACTION_LONG_CLICK;

    /**
     * Action that gives accessibility focus to the node.
     */
    static const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> ACTION_ACCESSIBILITY_FOCUS;

    /**
     * Action that clears accessibility focus of the node.
     */
    static const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> ACTION_CLEAR_ACCESSIBILITY_FOCUS;

    /**
     * Action that requests to go to the next entity in this node's text
     * at a given movement granularity. For example, move to the next character,
     * word, etc.
     * <p>
     * <strong>Arguments:</strong>
     * {@link AccessibilityNodeInfo#ACTION_ARGUMENT_MOVEMENT_GRANULARITY_INT
     *  AccessibilityNodeInfo.ACTION_ARGUMENT_MOVEMENT_GRANULARITY_INT},
     * {@link AccessibilityNodeInfo#ACTION_ARGUMENT_EXTEND_SELECTION_BOOLEAN
     *  AccessibilityNodeInfo.ACTION_ARGUMENT_EXTEND_SELECTION_BOOLEAN}<br>
     * <strong>Example:</strong> Move to the previous character and do not extend selection.
     * <code><pre><p>
     *   Bundle arguments = new Bundle();
     *   arguments.putInt(AccessibilityNodeInfo.ACTION_ARGUMENT_MOVEMENT_GRANULARITY_INT,
     *           AccessibilityNodeInfo.MOVEMENT_GRANULARITY_CHARACTER);
     *   arguments.putBoolean(AccessibilityNodeInfo.ACTION_ARGUMENT_EXTEND_SELECTION_BOOLEAN,
     *           false);
     *   info.performAction(AccessibilityAction.ACTION_NEXT_AT_MOVEMENT_GRANULARITY.getId(),
     *           arguments);
     * </code></pre></p>
     * </p>
     *
     * @see AccessibilityNodeInfo#ACTION_ARGUMENT_MOVEMENT_GRANULARITY_INT
     *  AccessibilityNodeInfo.ACTION_ARGUMENT_MOVEMENT_GRANULARITY_INT
     * @see AccessibilityNodeInfo#ACTION_ARGUMENT_EXTEND_SELECTION_BOOLEAN
     *  AccessibilityNodeInfo.ACTION_ARGUMENT_EXTEND_SELECTION_BOOLEAN
     *
     * @see AccessibilityNodeInfo#setMovementGranularities(int)
     *  AccessibilityNodeInfo.ACTION_ARGUMENT_EXTEND_SELECTION_BOOLEAN
     * @see AccessibilityNodeInfo#getMovementGranularities()
     *  AccessibilityNodeInfo.getMovementGranularities()
     *
     * @see AccessibilityNodeInfo#MOVEMENT_GRANULARITY_CHARACTER
     *  AccessibilityNodeInfo.MOVEMENT_GRANULARITY_CHARACTER
     * @see AccessibilityNodeInfo#MOVEMENT_GRANULARITY_WORD
     *  AccessibilityNodeInfo.MOVEMENT_GRANULARITY_WORD
     * @see AccessibilityNodeInfo#MOVEMENT_GRANULARITY_LINE
     *  AccessibilityNodeInfo.MOVEMENT_GRANULARITY_LINE
     * @see AccessibilityNodeInfo#MOVEMENT_GRANULARITY_PARAGRAPH
     *  AccessibilityNodeInfo.MOVEMENT_GRANULARITY_PARAGRAPH
     * @see AccessibilityNodeInfo#MOVEMENT_GRANULARITY_PAGE
     *  AccessibilityNodeInfo.MOVEMENT_GRANULARITY_PAGE
     */
    static const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> ACTION_NEXT_AT_MOVEMENT_GRANULARITY;

    /**
     * Action that requests to go to the previous entity in this node's text
     * at a given movement granularity. For example, move to the next character,
     * word, etc.
     * <p>
     * <strong>Arguments:</strong>
     * {@link AccessibilityNodeInfo#ACTION_ARGUMENT_MOVEMENT_GRANULARITY_INT
     *  AccessibilityNodeInfo.ACTION_ARGUMENT_MOVEMENT_GRANULARITY_INT},
     * {@link AccessibilityNodeInfo#ACTION_ARGUMENT_EXTEND_SELECTION_BOOLEAN
     *  AccessibilityNodeInfo.ACTION_ARGUMENT_EXTEND_SELECTION_BOOLEAN}<br>
     * <strong>Example:</strong> Move to the next character and do not extend selection.
     * <code><pre><p>
     *   Bundle arguments = new Bundle();
     *   arguments.putInt(AccessibilityNodeInfo.ACTION_ARGUMENT_MOVEMENT_GRANULARITY_INT,
     *           AccessibilityNodeInfo.MOVEMENT_GRANULARITY_CHARACTER);
     *   arguments.putBoolean(AccessibilityNodeInfo.ACTION_ARGUMENT_EXTEND_SELECTION_BOOLEAN,
     *           false);
     *   info.performAction(AccessibilityAction.ACTION_PREVIOUS_AT_MOVEMENT_GRANULARITY.getId(),
     *           arguments);
     * </code></pre></p>
     * </p>
     *
     * @see AccessibilityNodeInfo#ACTION_ARGUMENT_MOVEMENT_GRANULARITY_INT
     *  AccessibilityNodeInfo.ACTION_ARGUMENT_MOVEMENT_GRANULARITY_INT
     * @see AccessibilityNodeInfo#ACTION_ARGUMENT_EXTEND_SELECTION_BOOLEAN
     *  AccessibilityNodeInfo.ACTION_ARGUMENT_EXTEND_SELECTION_BOOLEAN
     *
     * @see AccessibilityNodeInfo#setMovementGranularities(int)
     *   AccessibilityNodeInfo.setMovementGranularities(int)
     * @see AccessibilityNodeInfo#getMovementGranularities()
     *  AccessibilityNodeInfo.getMovementGranularities()
     *
     * @see AccessibilityNodeInfo#MOVEMENT_GRANULARITY_CHARACTER
     *  AccessibilityNodeInfo.MOVEMENT_GRANULARITY_CHARACTER
     * @see AccessibilityNodeInfo#MOVEMENT_GRANULARITY_WORD
     *  AccessibilityNodeInfo.MOVEMENT_GRANULARITY_WORD
     * @see AccessibilityNodeInfo#MOVEMENT_GRANULARITY_LINE
     *  AccessibilityNodeInfo.MOVEMENT_GRANULARITY_LINE
     * @see AccessibilityNodeInfo#MOVEMENT_GRANULARITY_PARAGRAPH
     *  AccessibilityNodeInfo.MOVEMENT_GRANULARITY_PARAGRAPH
     * @see AccessibilityNodeInfo#MOVEMENT_GRANULARITY_PAGE
     *  AccessibilityNodeInfo.MOVEMENT_GRANULARITY_PAGE
     */
    static const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> ACTION_PREVIOUS_AT_MOVEMENT_GRANULARITY;
    /**
     * Action to move to the next HTML element of a given type. For example, move
     * to the BUTTON, INPUT, TABLE, etc.
     * <p>
     * <strong>Arguments:</strong>
     * {@link AccessibilityNodeInfo#ACTION_ARGUMENT_HTML_ELEMENT_STRING
     *  AccessibilityNodeInfo.ACTION_ARGUMENT_HTML_ELEMENT_STRING}<br>
     * <strong>Example:</strong>
     * <code><pre><p>
     *   Bundle arguments = new Bundle();
     *   arguments.putString(AccessibilityNodeInfo.ACTION_ARGUMENT_HTML_ELEMENT_STRING, "BUTTON");
     *   info.performAction(AccessibilityAction.ACTION_NEXT_HTML_ELEMENT.getId(), arguments);
     * </code></pre></p>
     * </p>
     */
    static const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> ACTION_NEXT_HTML_ELEMENT;

    /**
     * Action to move to the previous HTML element of a given type. For example, move
     * to the BUTTON, INPUT, TABLE, etc.
     * <p>
     * <strong>Arguments:</strong>
     * {@link AccessibilityNodeInfo#ACTION_ARGUMENT_HTML_ELEMENT_STRING
     *  AccessibilityNodeInfo.ACTION_ARGUMENT_HTML_ELEMENT_STRING}<br>
     * <strong>Example:</strong>
     * <code><pre><p>
     *   Bundle arguments = new Bundle();
     *   arguments.putString(AccessibilityNodeInfo.ACTION_ARGUMENT_HTML_ELEMENT_STRING, "BUTTON");
     *   info.performAction(AccessibilityAction.ACTION_PREVIOUS_HTML_ELEMENT.getId(), arguments);
     * </code></pre></p>
     * </p>
     */
    static const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> ACTION_PREVIOUS_HTML_ELEMENT;

    /**
     * Action to scroll the node content forward.
     */
    static const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> ACTION_SCROLL_FORWARD;

    /**
     * Action to scroll the node content backward.
     */
    static const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> ACTION_SCROLL_BACKWARD;

    /**
     * Action to copy the current selection to the clipboard.
     */
    static const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> ACTION_COPY;

    /**
     * Action to paste the current clipboard content.
     */
    static const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> ACTION_PASTE;

    /**
     * Action to cut the current selection and place it to the clipboard.
     */
    static const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> ACTION_CUT;

    /**
     * Action to set the selection. Performing this action with no arguments
     * clears the selection.
     * <p>
     * <strong>Arguments:</strong>
     * {@link AccessibilityNodeInfo#ACTION_ARGUMENT_SELECTION_START_INT
     *  AccessibilityNodeInfo.ACTION_ARGUMENT_SELECTION_START_INT},
     * {@link AccessibilityNodeInfo#ACTION_ARGUMENT_SELECTION_END_INT
     *  AccessibilityNodeInfo.ACTION_ARGUMENT_SELECTION_END_INT}<br>
     * <strong>Example:</strong>
     * <code><pre><p>
     *   Bundle arguments = new Bundle();
     *   arguments.putInt(AccessibilityNodeInfo.ACTION_ARGUMENT_SELECTION_START_INT, 1);
     *   arguments.putInt(AccessibilityNodeInfo.ACTION_ARGUMENT_SELECTION_END_INT, 2);
     *   info.performAction(AccessibilityAction.ACTION_SET_SELECTION.getId(), arguments);
     * </code></pre></p>
     * </p>
     *
     * @see AccessibilityNodeInfo#ACTION_ARGUMENT_SELECTION_START_INT
     *  AccessibilityNodeInfo.ACTION_ARGUMENT_SELECTION_START_INT
     * @see AccessibilityNodeInfo#ACTION_ARGUMENT_SELECTION_END_INT
     *  AccessibilityNodeInfo.ACTION_ARGUMENT_SELECTION_END_INT
     */
    static const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> ACTION_SET_SELECTION;

    /**
     * Action to expand an expandable node.
     */
    static const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> ACTION_EXPAND;

    /**
     * Action to collapse an expandable node.
     */
    static const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> ACTION_COLLAPSE;

    /**
     * Action to dismiss a dismissable node.
     */
    static const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> ACTION_DISMISS;

    /**
     * Action that sets the text of the node. Performing the action without argument,
     * using <code> null</code> or empty {@link CharSequence} will clear the text. This
     * action will also put the cursor at the end of text.
     * <p>
     * <strong>Arguments:</strong>
     * {@link AccessibilityNodeInfo#ACTION_ARGUMENT_SET_TEXT_CHARSEQUENCE
     *  AccessibilityNodeInfo.ACTION_ARGUMENT_SET_TEXT_CHARSEQUENCE}<br>
     * <strong>Example:</strong>
     * <code><pre><p>
     *   Bundle arguments = new Bundle();
     *   arguments.putCharSequence(AccessibilityNodeInfo.ACTION_ARGUMENT_SET_TEXT_CHARSEQUENCE,
     *       "android");
     *   info.performAction(AccessibilityAction.ACTION_SET_TEXT.getId(), arguments);
     * </code></pre></p>
     */
    static const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> ACTION_SET_TEXT;

private:
    static const AutoPtr<IArraySet> sStandardActions;
    Int32 mActionId;
    AutoPtr<ICharSequence> mLabel;
};

} // Accessibility
} // View
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYNODEINFOACCESSIBILITYACTION_H__
