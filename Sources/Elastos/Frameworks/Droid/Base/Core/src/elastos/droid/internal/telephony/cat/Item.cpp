
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
    constructor(-1, String(NULL));
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
    // ==================before translated======================
    // this.id = id;
    // this.text = text;
    // this.icon = null;
    return NOERROR;
}

ECode Item::constructor(
    /* [in] */ IParcel* in)
{
    // ==================before translated======================
    // id = in.readInt();
    // text = in.readString();
    // icon = in.readParcelable(null);
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
    // ==================before translated======================
    // dest.writeInt(id);
    // dest.writeString(text);
    // dest.writeParcelable(icon, flags);
    assert(0);
    return NOERROR;
}

ECode Item::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    constructor(source);
    return NOERROR;
}

ECode Item::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return text;
    assert(0);
    return NOERROR;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
