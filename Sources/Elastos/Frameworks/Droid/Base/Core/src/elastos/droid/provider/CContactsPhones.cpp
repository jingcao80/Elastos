
#include "Elastos.Droid.Content.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CContactsPhones.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::R;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsPhones)

CAR_INTERFACE_IMPL_4(CContactsPhones, Singleton, IContactsPhones, IBaseColumns, IContactsPhonesColumns, IContactsPeopleColumns)

ECode CContactsPhones::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::Parse(String("content://contacts/phones"), uri);
}

ECode CContactsPhones::GetCONTENT_FILTER_URL(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::Parse(String("content://contacts/phones/filter"), uri);
}

ECode CContactsPhones::GetDisplayLabel(
    /* [in] */ IContext* context,
    /* [in] */ Int32 type,
    /* [in] */ ICharSequence* sequence,
    /* [in] */ ArrayOf<ICharSequence*>* labelArray,
    /* [out] */ ICharSequence** label)
{
    VALIDATE_NOT_NULL(label);

    AutoPtr<ICharSequence> display;
    FAIL_RETURN(CString::New(String(""), (ICharSequence**)&display))

    if (type != IContactsPhonesColumns::TYPE_CUSTOM) {
        AutoPtr<ArrayOf<ICharSequence*> > labels;
        if (labelArray != NULL) {
            labels = labelArray;
        }
        else {
            AutoPtr<IResources> res;
            FAIL_RETURN(context->GetResources((IResources**)&res))
            FAIL_RETURN(res->GetTextArray(Elastos::Droid::R::array::phoneTypes, (ArrayOf<ICharSequence*>**)&labels))
        }

        Int32 size = labels->GetLength();
        if (type - 1 >= 0 && type - 1 < size) {
            display = (*labels)[type - 1];
        }
        else {
            display = (*labels)[IContactsPhonesColumns::TYPE_HOME - 1];
        }
    }
    else {
        Int32 length;
        FAIL_RETURN(sequence->GetLength(&length))
        if (length != 0) {
            display = sequence;
        }
    }
    *label = display;
    REFCOUNT_ADD(*label);
    return NOERROR;
}

ECode CContactsPhones::GetDisplayLabel(
    /* [in] */ IContext* context,
    /* [in] */ Int32 type,
    /* [in] */ ICharSequence* sequence,
    /* [out] */ ICharSequence** label)
{
    VALIDATE_NOT_NULL(label);

    return GetDisplayLabel(context, type, sequence, NULL, label);
}

} // Provider
} // Droid
} // Elastos