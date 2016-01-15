
#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CPHONELAYOUTINFLATER_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CPHONELAYOUTINFLATER_H__

#include "_Elastos_Droid_Internal_Policy_Impl_CPhoneLayoutInflater.h"
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

CarClass(CPhoneLayoutInflater), public LayoutInflater
{
public:
    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI CloneInContext(
        /* [in] */ IContext* newContext,
        /* [out] */ ILayoutInflater** newInflater);

    CARAPI GetContext(
        /* [out] */ IContext** context);

    CARAPI GetFactory(
        /* [out] */ ILayoutInflaterFactory** factory);

    CARAPI GetFactory2(
        /* [out] */ ILayoutInflaterFactory2** factory);

    CARAPI SetFactory(
        /* [in] */ ILayoutInflaterFactory* factory);

    CARAPI SetFactory2(
        /* [in] */ ILayoutInflaterFactory2* factory);

    CARAPI SetPrivateFactory(
        /* [in] */ ILayoutInflaterFactory2* factory);

    CARAPI GetFilter(
        /* [out] */ ILayoutInflaterFilter** filter);

    CARAPI SetFilter(
        /* [in] */ ILayoutInflaterFilter* filter);

    CARAPI Inflate(
        /* [in] */ Int32 resource,
        /* [in] */ IViewGroup* root,
        /* [out] */ IView** view);

    CARAPI Inflate(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IViewGroup* root,
        /* [out] */ IView** view);

    CARAPI Inflate(
        /* [in] */ Int32 resource,
        /* [in] */ IViewGroup* root,
        /* [in] */ Boolean attachToRoot,
        /* [out] */ IView** view);

    CARAPI Inflate(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IViewGroup* root,
        /* [in] */ Boolean attachToRoot,
        /* [out] */ IView** view);

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
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_POLICY_IMPL_CPHONELAYOUTINFLATER_H__
