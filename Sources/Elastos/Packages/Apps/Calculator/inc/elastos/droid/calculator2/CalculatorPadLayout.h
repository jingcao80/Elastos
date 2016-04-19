#ifndef _ELASTOS_DROID_CALCULATOR2_CALCULATORPADLAYOUT_H__
#define _ELASTOS_DROID_CALCULATOR2_CALCULATORPADLAYOUT_H__

#include "_Elastos.Droid.Calculator2.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/view/ViewGroup.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::ViewGroup;

namespace Elastos {
namespace Droid {
namespace Calculator2 {

class CalculatorPadLayout
    : public ViewGroup
    , public ICalculatorPadLayout
{
public:
    CalculatorPadLayout();

    virtual ~CalculatorPadLayout();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI ShouldDelayChildPressedState(
        /* [out] */ Boolean* result);

    CARAPI GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** result);

    CARAPI GenerateDefaultLayoutParams(
        /* [out] */ IViewGroupLayoutParams** result);

protected:
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    CARAPI GenerateLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p,
        /* [out] */ IViewGroupLayoutParams** result);

    CARAPI CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p,
        /* [out] */ Boolean* result);

private:
    Int32 mRowCount;
    Int32 mColumnCount;
};

} // namespace Calculator2
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_CALCULATOR2_CALCULATORPADLAYOUT_H__