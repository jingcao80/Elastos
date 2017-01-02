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

#ifndef __ELASTOS_DROID_WIDGET_SEEKBAR_H__
#define __ELASTOS_DROID_WIDGET_SEEKBAR_H__

#include "elastos/droid/widget/AbsSeekBar.h"

namespace Elastos {
namespace Droid {
namespace Widget {

class ECO_PUBLIC SeekBar
    : public AbsSeekBar
    , public ISeekBar
{
public:
    CAR_INTERFACE_DECL()

    SeekBar();

    ~SeekBar();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    /**
     * Sets a listener to receive notifications of changes to the SeekBar's progress level. Also
     * provides notifications of when the user starts and stops a touch gesture within the SeekBar.
     *
     * @param l The seek bar notification listener
     *
     * @see SeekBar.OnSeekBarChangeListener
     */
    CARAPI SetOnSeekBarChangeListener(
        /* [in] */ ISeekBarOnSeekBarChangeListener* l);

    // @Override
    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    // @Override
    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

protected:
    // @Override
    virtual CARAPI OnProgressRefresh(
        /* [in] */ Float scale,
        /* [in] */ Boolean fromUser);

    //java package access permission
    // @Override
    virtual CARAPI OnStartTrackingTouch();

    // @Override
    virtual CARAPI OnStopTrackingTouch();

private:
    AutoPtr<ISeekBarOnSeekBarChangeListener> mOnSeekBarChangeListener;
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_SEEKBAR_H__
