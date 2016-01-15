
#include "elastos/droid/impl/CPhoneLayoutInflater.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

AutoPtr< ArrayOf<String> > sInit()
{
    AutoPtr< ArrayOf<String> > prefixs = ArrayOf<String>::Alloc(2);
    (*prefixs)[0] = "Elastos.Droid.Widget.";
    (*prefixs)[1] = "Elastos.Droid.Webkit.";
    return prefixs;
}

const AutoPtr< ArrayOf<String> > CPhoneLayoutInflater::sClassPrefixList = sInit();

ECode CPhoneLayoutInflater::constructor(
    /* [in] */ IContext* context)
{
    return LayoutInflater::Init(context);
}

PInterface CPhoneLayoutInflater::Probe(
    /* [in] */ REIID riid)
{
    return _CPhoneLayoutInflater::Probe(riid);
}

ECode CPhoneLayoutInflater::CloneInContext(
    /* [in] */ IContext* newContext,
    /* [out] */ ILayoutInflater** newInflater)
{
    return CPhoneLayoutInflater::New(newContext, newInflater);
}

ECode CPhoneLayoutInflater::OnCreateView(
    /* [in] */ const String& name,
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    for (Int32 i = 0; i < sClassPrefixList->GetLength(); ++i) {
        String prefix = (*sClassPrefixList)[i];
        ECode ec = CreateView(name, prefix, attrs, view);
        if (SUCCEEDED(ec) && (*view != NULL)) return ec;
    }

    return LayoutInflater::OnCreateView(name, attrs, view);
}

ECode CPhoneLayoutInflater::GetContext(
    /* [out] */ IContext** context)
{
    VALIDATE_NOT_NULL(context);

    *context = LayoutInflater::GetContext();
    REFCOUNT_ADD(*context);
    return NOERROR;
}

ECode CPhoneLayoutInflater::GetFactory(
    /* [out] */ ILayoutInflaterFactory** factory)
{
//    VALIDATE_NOT_NULL(factory);
//
//    return LayoutInflater::GetFactory(factory);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CPhoneLayoutInflater::GetFactory2(
    /* [out] */ ILayoutInflaterFactory2** factory)
{
//    VALIDATE_NOT_NULL(factory);
//
//    return LayoutInflater::GetFactory2(factory);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CPhoneLayoutInflater::SetFactory(
    /* [in] */ ILayoutInflaterFactory* factory)
{
//    return LayoutInflater::SetFactory(factory);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CPhoneLayoutInflater::SetFactory2(
    /* [in] */ ILayoutInflaterFactory2* factory)
{
//    return LayoutInflater::SetFactory2(factory);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CPhoneLayoutInflater::SetPrivateFactory(
    /* [in] */ ILayoutInflaterFactory2* factory)
{
    return LayoutInflater::SetPrivateFactory(factory);
}

ECode CPhoneLayoutInflater::GetFilter(
    /* [out] */ ILayoutInflaterFilter** filter)
{
    VALIDATE_NOT_NULL(filter);
    return LayoutInflater::GetFilter(filter);
}

ECode CPhoneLayoutInflater::SetFilter(
    /* [in] */ ILayoutInflaterFilter* filter)
{
    return LayoutInflater::SetFilter(filter);
}

ECode CPhoneLayoutInflater::Inflate(
    /* [in] */ Int32 resource,
    /* [in] */ IViewGroup* root,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    return LayoutInflater::Inflate(resource, root, view);
}

ECode CPhoneLayoutInflater::Inflate(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IViewGroup* root,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    return LayoutInflater::Inflate(parser, root, view);
}

ECode CPhoneLayoutInflater::Inflate(
    /* [in] */ Int32 resource,
    /* [in] */ IViewGroup* root,
    /* [in] */ Boolean attachToRoot,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    return LayoutInflater::Inflate(resource, root, attachToRoot, view);
}

ECode CPhoneLayoutInflater::Inflate(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IViewGroup* root,
    /* [in] */ Boolean attachToRoot,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    return LayoutInflater::Inflate(parser, root, attachToRoot, view);
}

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namepsace Droid
} // namespace Elastos

