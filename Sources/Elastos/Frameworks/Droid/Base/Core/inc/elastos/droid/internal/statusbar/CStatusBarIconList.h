#ifndef __ELASTOS_DROID_INTERNAL_STATUSBAR_CSTATUSBARICONLIST_H__
#define __ELASTOS_DROID_INTERNAL_STATUSBAR_CSTATUSBARICONLIST_H__

#include "_Elastos_Droid_Internal_StatusBar_CStatusBarIconList.h"
#include <elastos/core/Object.h>

using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace StatusBar {

CarClass(CStatusBarIconList)
    , public Object
    , public IStatusBarIconList
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* out);

    CARAPI DefineSlots(
        /* [in] */ const ArrayOf<String>& slots);

    CARAPI GetSlotIndex(
        /* [in] */ const String& slot,
        /* [out] */ Int32* slotIndex);

    CARAPI Size(
        /* [out] */ Int32* size);

    CARAPI SetIcon(
        /* [in] */ Int32 index,
        /* [in] */ IStatusBarIcon* icon);

    CARAPI RemoveIcon(
        /* [in] */ Int32 index);

    CARAPI GetSlot(
        /* [in] */ Int32 index,
        /* [out] */ String* slot);

    CARAPI GetIcon(
        /* [in] */ Int32 index,
        /* [out] */ IStatusBarIcon** icon);

    CARAPI GetViewIndex(
        /* [in] */ Int32 index,
        /* [out] */ Int32* viewIndex);

    CARAPI CopyFrom(
        /* [in] */ IStatusBarIconList* that);

    CARAPI Dump(
        /* [in] */ IPrintWriter* pw);

    CARAPI GetSlots(
        /* [out, callee] */ ArrayOf<String>** slots);

    CARAPI GetIcons(
        /* [out, callee] */ ArrayOf<IStatusBarIcon*>** icons);

private:
    AutoPtr<ArrayOf<String> > mSlots;
    AutoPtr<ArrayOf<IStatusBarIcon*> > mIcons;

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
};

} //namespace StatusBar
} //namespace Internal
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_STATUSBAR_CSTATUSBARICONLIST_H__
