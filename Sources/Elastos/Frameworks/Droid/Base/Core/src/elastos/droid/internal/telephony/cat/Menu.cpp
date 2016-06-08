
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/internal/telephony/cat/Menu.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

////=====================================================================
////                     Menu::InnerParcelableCreator
////=====================================================================
//Menu::InnerParcelableCreator::InnerParcelableCreator(
//    /* [in] */ Menu* owner)
//    : mOwner(owner)
//{
//    // ==================before translated======================
//    // mOwner = owner;
//}
//
//ECode Menu::InnerParcelableCreator::CreateFromParcel(
//    /* [in] */ IParcel* in,
//    /* [out] */ Menu** result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return new Menu(in);
//    assert(0);
//    return NOERROR;
//}
//
//ECode Menu::InnerParcelableCreator::NewArray(
//    /* [in] */ Int32 size,
//    /* [out] */ Menu[]** result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return new Menu[size];
//    assert(0);
//    return NOERROR;
//}
//
////=====================================================================
////                                 Menu
////=====================================================================
//const AutoPtr<IParcelable> AutoPtr< ::Creator<Menu> > Menu::CREATOR = new InnerParcelableCreator(this);
CAR_INTERFACE_IMPL_2(Menu, Object, IMenu, IParcelable);

Menu::Menu()
{
    // ==================before translated======================
    // // Create an empty list.
    // items = new ArrayList<Item>();
    // title = null;
    // titleAttrs = null;
    // defaultItem = 0;
    // softKeyPreferred = false;
    // helpAvailable = false;
    // titleIconSelfExplanatory = false;
    // itemsIconSelfExplanatory = false;
    // titleIcon = null;
    // // set default style to be navigation menu.
    // presentationType = PresentationType.NAVIGATION_OPTIONS;
}

//ECode Menu::DescribeContents(
//    /* [out] */ Int32* result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return 0;
//    assert(0);
//    return NOERROR;
//}

ECode Menu::WriteToParcel(
    /* [in] */ IParcel* dest)
    ///* [in] */ Int32 flags)
{
    // ==================before translated======================
    // dest.writeString(title);
    // dest.writeParcelable(titleIcon, flags);
    // // write items list to the parcel.
    // int size = items.size();
    // dest.writeInt(size);
    // for (int i=0; i<size; i++) {
    //     dest.writeParcelable(items.get(i), flags);
    // }
    // dest.writeInt(defaultItem);
    // dest.writeInt(softKeyPreferred ? 1 : 0);
    // dest.writeInt(helpAvailable ? 1 : 0);
    // dest.writeInt(titleIconSelfExplanatory ? 1 : 0);
    // dest.writeInt(itemsIconSelfExplanatory ? 1 : 0);
    // dest.writeInt(presentationType.ordinal());
    assert(0);
    return NOERROR;
}

ECode Menu::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    constructor(source);
    return NOERROR;
}

ECode Menu::constructor(
    /* [in] */ IParcel* in)
{
    // ==================before translated======================
    // title = in.readString();
    // titleIcon = in.readParcelable(null);
    // // rebuild items list.
    // items = new ArrayList<Item>();
    // int size = in.readInt();
    // for (int i=0; i<size; i++) {
    //     Item item = in.readParcelable(null);
    //     items.add(item);
    // }
    // defaultItem = in.readInt();
    // softKeyPreferred = in.readInt() == 1 ? true : false;
    // helpAvailable = in.readInt() == 1 ? true : false;
    // titleIconSelfExplanatory = in.readInt() == 1 ? true : false;
    // itemsIconSelfExplanatory = in.readInt() == 1 ? true : false;
    // presentationType = PresentationType.values()[in.readInt()];
    return NOERROR;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
