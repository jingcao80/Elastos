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

#ifndef __ELASTOS_DROID_WIDGET_RADIOGROUP_H__
#define __ELASTOS_DROID_WIDGET_RADIOGROUP_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/LinearLayout.h"

using Elastos::Droid::View::IViewGroupOnHierarchyChangeListener;

namespace Elastos {
namespace Droid {
namespace Widget {

class ECO_PUBLIC RadioGroup
    : public LinearLayout
    , public IRadioGroup
{
public:
    class RadioGroupLayoutParams
        : public LinearLayout::LinearLayoutLayoutParams
        , public IRadioGroupLayoutParams
    {
    public:
        CAR_INTERFACE_DECL()

        RadioGroupLayoutParams();

        ~RadioGroupLayoutParams();

        /**
         * {@inheritDoc}
         */
        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ IAttributeSet* attrs);

        /**
         * {@inheritDoc}
         */
        CARAPI constructor(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        /**
         * {@inheritDoc}
         */
        CARAPI constructor(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height,
            /* [in] */ Float weight);

        /**
         * {@inheritDoc}
         */
        CARAPI constructor(
            /* [in] */ IViewGroupLayoutParams* source);

        /**
         * {@inheritDoc}
         */
        CARAPI constructor(
            /* [in] */ IViewGroupMarginLayoutParams* source);

    protected:
        /**
         * <p>Fixes the child's width to
         * {@link android.view.ViewGroup.LayoutParams#WRAP_CONTENT} and the child's
         * height to  {@link android.view.ViewGroup.LayoutParams#WRAP_CONTENT}
         * when not specified in the XML file.</p>
         *
         * @param a the styled attributes set
         * @param widthAttr the width attribute to fetch
         * @param heightAttr the height attribute to fetch
         */
        // @Override
        virtual CARAPI SetBaseAttributes(
            /* [in] */ ITypedArray* a,
            /* [in] */ Int32 widthAttr,
            /* [in] */ Int32 heightAttr);
    };

private:
    class ECO_LOCAL CheckedStateTracker
        : public Object
        , public ICompoundButtonOnCheckedChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        CheckedStateTracker(
            /* [in] */ RadioGroup* host);

        CARAPI OnCheckedChanged(
            /* [in] */ ICompoundButton* buttonView,
            /* [in] */ Boolean isChecked);

    private:
        RadioGroup* mHost;
    };

    /**
     * <p>A pass-through listener acts upon the events and dispatches them
     * to another listener. This allows the table layout to set its own internal
     * hierarchy change listener without preventing the user to setup his.</p>
     */
    class ECO_LOCAL PassThroughHierarchyChangeListener
        : public Object
        , public IViewGroupOnHierarchyChangeListener
    {
        friend class RadioGroup;
    public:
        CAR_INTERFACE_DECL()

        PassThroughHierarchyChangeListener(
            /* [in] */ RadioGroup* host);

        /**
         * {@inheritDoc}
         */
        CARAPI OnChildViewAdded(
            /* [in] */ IView* parent,
            /* [in] */ IView* child);

        /**
         * {@inheritDoc}
         */
        CARAPI OnChildViewRemoved(
            /* [in] */ IView* parent,
            /* [in] */ IView* child);

    private:
        AutoPtr<IViewGroupOnHierarchyChangeListener> mOnHierarchyChangeListener;
        RadioGroup* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    RadioGroup();

    ~RadioGroup();

    /**
     * {@inheritDoc}
     */
    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * {@inheritDoc}
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI SetOnHierarchyChangeListener(
        /* [in] */ IViewGroupOnHierarchyChangeListener* listener);

    //@Override
    CARAPI AddView(
        /* [in] */ IView* child,
        /* [in] */ Int32 index,
        /* [in] */ IViewGroupLayoutParams* params);

    /**
     * <p>Sets the selection to the radio button whose identifier is passed in
     * parameter. Using -1 as the selection identifier clears the selection;
     * such an operation is equivalent to invoking {@link #clearCheck()}.</p>
     *
     * @param id the unique id of the radio button to select in this group
     *
     * @see #getCheckedRadioButtonId()
     * @see #clearCheck()
     */
    CARAPI Check(
        /* [in] */ Int32 id);

    /**
     * <p>Returns the identifier of the selected radio button in this group.
     * Upon empty selection, the returned value is -1.</p>
     *
     * @return the unique id of the selected radio button in this group
     *
     * @see #check(Int32)
     * @see #clearCheck()
     */
    CARAPI GetCheckedRadioButtonId(
        /* [out] */ Int32* id);

    /**
     * <p>Clears the selection. When the selection is cleared, no radio button
     * in this group is selected and {@link #getCheckedRadioButtonId()} returns
     * null.</p>
     *
     * @see #check(Int32)
     * @see #getCheckedRadioButtonId()
     */
    CARAPI ClearCheck();

    /**
     * <p>Register a callback to be invoked when the checked radio button
     * changes in this group.</p>
     *
     * @param listener the callback to call on checked state change
     */
    CARAPI SetOnCheckedChangeListener(
        /* [in] */ IRadioGroupOnCheckedChangeListener* listener);

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** params);

    //@Override
    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    //@Override
    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

protected:
    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI OnFinishInflate();

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI_(Boolean) CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    //@Override
    CARAPI GenerateDefaultLayoutParams(
        /* [out] */ IViewGroupLayoutParams** params);

private:
    ECO_LOCAL CARAPI_(void) Init();

    ECO_LOCAL CARAPI_(void) SetCheckedId(
        /* [in] */ Int32 id);

    ECO_LOCAL CARAPI_(void) SetCheckedStateForView(
        /* [in] */ Int32 viewId,
        /* [in] */ Boolean checked);

private:
    // holds the checked id; the selection is empty by default
    Int32 mCheckedId;
    // tracks children radio buttons checked state
    AutoPtr<ICompoundButtonOnCheckedChangeListener> mChildOnCheckedChangeListener;
    // when TRUE, mOnCheckedChangeListener discards events
    Boolean mProtectFromCheckedChange;
    AutoPtr<IRadioGroupOnCheckedChangeListener> mOnCheckedChangeListener;
    AutoPtr<PassThroughHierarchyChangeListener> mPassThroughListener;
};


} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_RADIOGROUP_H__
