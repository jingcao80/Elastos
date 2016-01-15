
#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_PHONELAYOUTINFLATER_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_PHONELAYOUTINFLATER_H__

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/LayoutInflater.h"


using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::ILayoutInflaterFactory;
using Elastos::Droid::View::ILayoutInflaterFactory2;
using Elastos::Droid::View::ILayoutInflaterFilter;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

class PhoneLayoutInflater
    : public LayoutInflater
    , public IPhoneLayoutInflater
{
public:
    CAR_INTERFACE_DECL()

    PhoneLayoutInflater();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI CloneInContext(
        /* [in] */ IContext* newContext,
        /* [out] */ ILayoutInflater** newInflater) = 0;

protected:
    /** Override onCreateView to instantiate names that correspond to the
        widgets known to the Widget factory. If we don't find a match,
        call through to our super class.
    */
    CARAPI OnCreateView(
        /* [in] */ const String& name,
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IView** view);

private:
    static const AutoPtr< ArrayOf<String> > sClassPrefixList;
};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_POLICY_IMPL_PHONELAYOUTINFLATER_H__
