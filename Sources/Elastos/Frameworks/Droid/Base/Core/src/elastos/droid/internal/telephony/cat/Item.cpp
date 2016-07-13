
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cat/Item.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

////=====================================================================
////                     Item::InnerParcelableCreator
////=====================================================================
//Item::InnerParcelableCreator::InnerParcelableCreator(
//    /* [in] */ Item* owner)
//    : mOwner(owner)
//{
//    // ==================before translated======================
//    // mOwner = owner;
//}
//
//ECode Item::InnerParcelableCreator::CreateFromParcel(
//    /* [in] */ IParcel* in,
//    /* [out] */ Item** result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return new Item(in);
//    assert(0);
//    return NOERROR;
//}
//
//ECode Item::InnerParcelableCreator::NewArray(
//    /* [in] */ Int32 size,
//    /* [out] */ Item[]** result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return new Item[size];
//    assert(0);
//    return NOERROR;
//}
//
////=====================================================================
////                                 Item
////=====================================================================
//const AutoPtr<IParcelable> AutoPtr< ::Creator<Item> > Item::CREATOR = new InnerParcelableCreator(this);
CAR_INTERFACE_IMPL_2(Item, Object, IItem, IParcelable);

Item::Item()
{
}

ECode Item::constructor()
{
    constructor(-1, String(NULL));
    return NOERROR;
}

ECode Item::constructor(
    /* [in] */ Int32 id,
    /* [in] */ const String& text)
{
    mId = id;
    mText = text;
    mIcon = NULL;
    return NOERROR;
}

//ECode Item::DescribeContents(
//    /* [out] */ Int32* result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return 0;
//    assert(0);
//    return NOERROR;
//}

ECode Item::WriteToParcel(
    /* [in] */ IParcel* dest)
    ///* [in] */ Int32 flags)
{
    dest->WriteInt32(mId);
    dest->WriteString(mText);
    IParcelable::Probe(mIcon)->WriteToParcel(dest);
    return NOERROR;
}

ECode Item::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    in->ReadInt32(&mId);
    in->ReadString(&mText);
    IParcelable::Probe(mIcon)->ReadFromParcel(in);
    return NOERROR;
}

ECode Item::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mText;
    return NOERROR;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
