
#include "elastos/droid/internal/policy/impl/PhoneLayoutInflater.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

//=====================================================================
//                         PhoneLayoutInflater
//=====================================================================
CAR_INTERFACE_IMPL(PhoneLayoutInflater, LayoutInflater, IPhoneLayoutInflater)


static AutoPtr< ArrayOf<String> > sInit()
{
    AutoPtr< ArrayOf<String> > prefixs = ArrayOf<String>::Alloc(3);
    (*prefixs)[0] = "Elastos.Droid.Widget.";
    (*prefixs)[1] = "Elastos.Droid.Webkit.";
    (*prefixs)[2] = "Elastos.Droid.App.";
    return prefixs;
}

const AutoPtr< ArrayOf<String> > PhoneLayoutInflater::sClassPrefixList = sInit();

PhoneLayoutInflater::PhoneLayoutInflater()
{
}

ECode PhoneLayoutInflater::constructor(
    /* [in] */ IContext* context)
{
    return LayoutInflater::constructor(context);
}

ECode PhoneLayoutInflater::OnCreateView(
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

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

