#include "elastos/droid/statusbar/CStatusBarIconList.h"

namespace Elastos {
namespace Droid {
namespace StatusBar {

ECode CStatusBarIconList::constructor()
{
    return NOERROR;
}

ECode CStatusBarIconList::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    assert(in != NULL);

    mSlots = NULL;
    in->ReadArrayOfString((ArrayOf<String>**)&mSlots);

    Int32 N = 0;
    in->ReadInt32(&N);
    if (N < 0) {
        mIcons = NULL;
    }
    else {
        IParcelable* parcelable;
        mIcons = ArrayOf<IStatusBarIcon*>::Alloc(N);
        Int32 value = 0;
        for (Int32 i = 0; i < N; i++) {
            in->ReadInt32(&value);
            if (value != 0) {
                AutoPtr<IStatusBarIcon> icon;
                CStatusBarIcon::New((IStatusBarIcon**)&icon);
                parcelable= IParcelable::Probe(icon);
                parcelable->ReadFromParcel(in);
                mIcons->Set(i, icon);
            }
        }
    }
    return NOERROR;
}

ECode CStatusBarIconList::WriteToParcel(
    /* [in] */ IParcel* out)
{
    assert(out != NULL);

    out->WriteArrayOfString(mSlots);
    if (mIcons == NULL) {
        out->WriteInt32(-1);
    }
    else {
        IParcelable* parcelable;
        Int32 N = mIcons->GetLength();
        out->WriteInt32(N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IStatusBarIcon> icon = (*mIcons)[i];
            if (icon == NULL) {
                out->WriteInt32(0);
            }
            else {
                out->WriteInt32(1);
                parcelable = IParcelable::Probe(icon);
                parcelable->WriteToParcel(out);
            }
        }
    }
    return NOERROR;
}

/**
 * Parcelable.Creator that instantiates StatusBarIconList objects
 */
/*
public static final Parcelable.Creator<StatusBarIconList> CREATOR
        = new Parcelable.Creator<StatusBarIconList>()
{
    public StatusBarIconList createFromParcel(Parcel parcel)
    {
        return new StatusBarIconList(parcel);
    }

    public StatusBarIconList[] newArray(int size)
    {
        return new StatusBarIconList[size];
    }
};
*/
ECode CStatusBarIconList::DefineSlots(
    /* [in] */ const ArrayOf<String>& slots)
{
    Int32 N = slots.GetLength();
    mSlots = ArrayOf<String>::Alloc(N);
    mSlots->Copy(&slots);
    mIcons = ArrayOf<IStatusBarIcon*>::Alloc(N);
    return NOERROR;
}

ECode CStatusBarIconList::GetSlotIndex(
    /* [in] */ const String& slot,
    /* [out] */ Int32* slotIndex)
{
    VALIDATE_NOT_NULL(slotIndex);
    Int32 N = mSlots->GetLength();
    for (Int32 i = 0; i < N; i++) {
        if (slot.Equals((*mSlots)[i])) {
            *slotIndex = i;
            return NOERROR;
        }
    }
    *slotIndex = -1;
    return NOERROR;
}

ECode CStatusBarIconList::Size(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    *size = mSlots->GetLength();
    return NOERROR;
}

ECode CStatusBarIconList::SetIcon(
    /* [in] */ Int32 index,
    /* [in] */ IStatusBarIcon* icon)
{
    mIcons->Set(index, icon);
    return NOERROR;
}

ECode CStatusBarIconList::RemoveIcon(
    /* [in] */ Int32 index)
{
    mIcons->Set(index, NULL);
    return NOERROR;
}

ECode CStatusBarIconList::GetSlot(
    /* [in] */ Int32 index,
    /* [out] */ String* slot)
{
    VALIDATE_NOT_NULL(slot);
    *slot = (*mSlots)[index];
    return NOERROR;
}

ECode CStatusBarIconList::GetIcon(
    /* [in] */ Int32 index,
    /* [out] */ IStatusBarIcon** icon)
{
    VALIDATE_NOT_NULL(icon);
    *icon = (*mIcons)[index];
    REFCOUNT_ADD(*icon);
    return NOERROR;
}

ECode CStatusBarIconList::GetViewIndex(
    /* [in] */ Int32 index,
    /* [out] */ Int32* viewIndex)
{
    VALIDATE_NOT_NULL(viewIndex);
    Int32 count = 0;
    for (Int32 i = 0; i < index; i++) {
        if ((*mIcons)[i] != NULL) {
            count++;
        }
    }
    *viewIndex = count;
    return NOERROR;
}

ECode CStatusBarIconList::CopyFrom(
    /* [in] */ IStatusBarIconList* that)
{
    mSlots = NULL;
    mIcons = NULL;
    that->GetSlots((ArrayOf<String>**)&mSlots);
    that->GetIcons((ArrayOf<IStatusBarIcon*>**)&mIcons);
    return NOERROR;
}

ECode CStatusBarIconList::Dump(
    /* [in] */ IPrintWriter* pw)
{
    Int32 N = mSlots->GetLength();
    pw->PrintStringln(String("Icon list:"));
    for (Int32 i = 0; i < N; i++) {
        StringBuilder sb("  ");
        sb += i;
        sb += ": (";
        sb += (*mSlots)[i];
        sb += ") ";
        //sb += (*mIcons)[i];
        pw->PrintStringln(sb.ToString());
    }
    return NOERROR;
}

ECode CStatusBarIconList::GetSlots(
    /* [out, callee] */ ArrayOf<String>** slots)
{
    VALIDATE_NOT_NULL(slots);
    *slots = mSlots;
    REFCOUNT_ADD(*slots);
    return NOERROR;
}

ECode CStatusBarIconList::GetIcons(
    /* [out, callee] */ ArrayOf<IStatusBarIcon*>** icons)
{
    VALIDATE_NOT_NULL(icons);
    *icons = mIcons;
    REFCOUNT_ADD(*icons);
    return NOERROR;
}

}//namespace StatusBar
}//namespace Droid
}//namespace Elastos
