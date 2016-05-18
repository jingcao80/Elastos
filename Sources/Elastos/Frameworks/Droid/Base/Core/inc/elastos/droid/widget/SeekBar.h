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
    virtual CARAPI_(void) OnProgressRefresh(
        /* [in] */ Float scale,
        /* [in] */ Boolean fromUser);

    //java package access permission
    // @Override
    virtual CARAPI_(void) OnStartTrackingTouch();

    // @Override
    virtual CARAPI_(void) OnStopTrackingTouch();

private:
    AutoPtr<ISeekBarOnSeekBarChangeListener> mOnSeekBarChangeListener;
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_SEEKBAR_H__
