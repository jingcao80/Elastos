
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/internal/telephony/cat/CatMenu.h"

using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;

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
CAR_INTERFACE_IMPL_2(CatMenu, Object, IMenu, IParcelable);

CatMenu::CatMenu()
{
    // Create an empty list.
    CArrayList::New((IList**)&mItems);
    mTitle = NULL;
    mTitleAttrs = NULL;
    mDefaultItem = 0;
    mSoftKeyPreferred = FALSE;
    mHelpAvailable = FALSE;
    mTitleIconSelfExplanatory = FALSE;
    mItemsIconSelfExplanatory = FALSE;
    mTitleIcon = NULL;
    // set default style to be navigation menu.
    mPresentationType = PresentationType_NAVIGATION_OPTIONS;
}

ECode CatMenu::constructor()
{
    return NOERROR;
}

ECode CatMenu::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mTitle);
    IParcelable::Probe(mTitleIcon)->WriteToParcel(dest);
    // write items list to the parcel.
    Int32 size = 0;
    mItems->GetSize(&size);
    dest->WriteInt32(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> p;
        mItems->Get(i, (IInterface**)&p);
        IParcelable::Probe(p)->WriteToParcel(dest);
    }
    dest->WriteInt32(mDefaultItem);
    dest->WriteInt32(mSoftKeyPreferred ? 1 : 0);
    dest->WriteInt32(mHelpAvailable ? 1 : 0);
    dest->WriteInt32(mTitleIconSelfExplanatory ? 1 : 0);
    dest->WriteInt32(mItemsIconSelfExplanatory ? 1 : 0);
    dest->WriteInt32(mPresentationType);
    return NOERROR;
}

ECode CatMenu::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    in->ReadString(&mTitle);
    IParcelable::Probe(mTitleIcon)->ReadFromParcel(in);
    // rebuild items list.
    CArrayList::New((IList**)&mItems);
    Int32 size = 0;
    in->ReadInt32(&size);
    for (Int32 i = 0; i < size; i++) {
        assert(0 && "TODO");
        AutoPtr<IItem> item;// = in->ReadParcelable(NULL);
        mItems->Add(item);
    }
    in->ReadInt32(&mDefaultItem);
    Int32 softKeyPreferred = 0;
    in->ReadInt32(&softKeyPreferred);
    mSoftKeyPreferred = softKeyPreferred == 1 ? TRUE : FALSE;
    Int32 helpAvailable = 0;
    in->ReadInt32(&helpAvailable);
    mHelpAvailable = helpAvailable == 1 ? TRUE : FALSE;
    Int32 titleIconSelfExplanatory = 0;
    in->ReadInt32(&titleIconSelfExplanatory);
    mTitleIconSelfExplanatory = titleIconSelfExplanatory == 1 ? TRUE : FALSE;
    Int32 itemsIconSelfExplanatory = 0;
    in->ReadInt32(&itemsIconSelfExplanatory);
    mItemsIconSelfExplanatory = itemsIconSelfExplanatory == 1 ? TRUE : FALSE;
    Int32 presentationType = 0;
    in->ReadInt32(&presentationType);
    mPresentationType = presentationType;
    return NOERROR;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
