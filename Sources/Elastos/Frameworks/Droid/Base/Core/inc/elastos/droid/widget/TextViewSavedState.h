
#ifndef __ELASTOS_DROID_WIDGET_TEXTVIEWSAVEDSTATE_H__
#define __ELASTOS_DROID_WIDGET_TEXTVIEWSAVEDSTATE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/ViewBaseSavedState.h"

using Elastos::Core::ICharSequence;
using Elastos::Droid::View::ViewBaseSavedState;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * User interface state that is stored by TextView for implementing
 * {@link View#onSaveInstanceState}.
 */
class TextViewSavedState : public ViewBaseSavedState
{
protected:
    TextViewSavedState();

    TextViewSavedState(
        /* [in] */ IParcelable* superState);

public:
    virtual CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    virtual CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

protected:
    CARAPI Init(
        /* [in] */ IParcelable* superState);

    CARAPI Init();

protected:
    Int32 mSelStart;
    Int32 mSelEnd;
    AutoPtr<ICharSequence> mText;
    Boolean mFrozenWithFocus;
    AutoPtr<ICharSequence> mError;
};


} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_TEXTVIEWSAVEDSTATE_H__
