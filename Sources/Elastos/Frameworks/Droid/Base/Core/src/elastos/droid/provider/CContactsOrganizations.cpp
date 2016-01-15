
#include "Elastos.Droid.Content.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CContactsOrganizations.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::R;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsOrganizations)

CAR_INTERFACE_IMPL_3(CContactsOrganizations, Singleton, IContactsOrganizations, IBaseColumns, IContactsOrganizationColumns)

ECode CContactsOrganizations::constructor()
{
    return NOERROR;
}

ECode CContactsOrganizations::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::Parse(String("content://contacts/organizations"), uri);
}

ECode CContactsOrganizations::GetDisplayLabel(
    /* [in] */ IContext* context,
    /* [in] */ Int32 type,
    /* [in] */ ICharSequence* label,
    /* [out] */ ICharSequence** lb)
{
    AutoPtr<ICharSequence> display;
    FAIL_RETURN(CString::New(String(""), (ICharSequence**)&display))

    if (type != IContactsOrganizationColumns::TYPE_CUSTOM) {
        AutoPtr<ArrayOf<ICharSequence*> > labels;
        AutoPtr<IResources> res;
        FAIL_RETURN(context->GetResources((IResources**)&res))
        FAIL_RETURN(res->GetTextArray(Elastos::Droid::R::array::organizationTypes, (ArrayOf<ICharSequence*>**)&labels))

        Int32 size = labels->GetLength();
        if (type - 1 >= 0 && type - 1 < size) {
            display = (*labels)[type - 1];
        }
        else {
            display = (*labels)[IContactsOrganizationColumns::TYPE_WORK - 1];
        }
    }
    else {
        if (!TextUtils::IsEmpty(label)) {
            display = label;
        }
    }
    *lb = display;
    REFCOUNT_ADD(*lb);
    return NOERROR;
}

} // Provider
} // Droid
} // Elastos