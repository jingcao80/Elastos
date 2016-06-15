#ifndef __ELASTOS_DROID_TELEPHONY_CELLBROADCASTMESSAGE_H__
#define __ELASTOS_DROID_TELEPHONY_CELLBROADCASTMESSAGE_H__

#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// package android.telephony;
using Elastos::Droid::Content::IContentValues;
// import android.content.Context;
// import android.database.Cursor;
// import android.os.Parcel;
// import android.os.Parcelable;
// import android.provider.Telephony;
// import android.telephony.SubscriptionManager;
// import android.text.format.DateUtils;

//using Elastos::Droid::Os::IParcel;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Telephony {

/**
  * Application wrapper for {@link SmsCbMessage}. This is Parcelable so that
  * decoded broadcast message objects can be passed between running Services.
  * New broadcasts are received by the CellBroadcastReceiver app, which exports
  * the database of previously received broadcasts at "content://cellbroadcasts/".
  * The "android.permission.READ_CELL_BROADCASTS" permission is required to read
  * from the ContentProvider, and writes to the database are not allowed.<p>
  *
  * Use {@link #createFromCursor} to create CellBroadcastMessage objects from rows
  * in the database cursor returned by the ContentProvider.
  *
  * {@hide}
  */
class CellBroadcastMessage
    : public Object
    , public ICellBroadcastMessage
    , public IParcelable
{
//private:
//    class InnerParcelableCreator
//        : public Object
//        , public Parcelable::Creator<CellBroadcastMessage>
//    {
//    public:
//        InnerParcelableCreator(
//            /* [in] */ CellBroadcastMessage* owner);
//
//        // @Override
//        CARAPI_(AutoPtr<CellBroadcastMessage>) CreateFromParcel(
//            /* [in] */ IParcel* in);
//
//        // @Override
//        CARAPI_(AutoPtr< ArrayOf< AutoPtr<CellBroadcastMessage> > >) NewArray(
//            /* [in] */ Int32 size);
//
//    private:
//        CellBroadcastMessage* mOwner;
//    };

public:
    CAR_INTERFACE_DECL();

    CellBroadcastMessage();

    CARAPI constructor(
        /* [in] */ ISmsCbMessage* message);

    CARAPI constructor();
    /**
      * set Subscription information
      *
      * @hide
      */
    CARAPI SetSubId(
        /* [in] */ Int64 subId);

    /**
      * get Subscription information
      *
      * @hide
      */
    CARAPI GetSubId(
        /* [out] */ Int64* result);

    ///** Parcelable: no special flags. */
    //// @Override
    //CARAPI_(Int32) DescribeContents();

    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* out);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);
    /**
      * Create a CellBroadcastMessage from a row in the database.
      * @param cursor an open SQLite cursor pointing to the row to read
      * @return the new CellBroadcastMessage
      * @throws IllegalArgumentException if one of the required columns is missing
      */
    static CARAPI_(AutoPtr<CellBroadcastMessage>) CreateFromCursor(
        /* [in] */ ICursor* cursor);

    /**
      * Return a ContentValues object for insertion into the database.
      * @return a new ContentValues object containing this object's data
      */
    CARAPI GetContentValues(
        /* [out] */ IContentValues** result);

    /**
      * Set or clear the "read message" flag.
      * @param isRead true if the message has been read; false if not
      */
    CARAPI SetIsRead(
        /* [in] */ Boolean isRead);

    CARAPI GetLanguageCode(
        /* [out] */ String* result);

    CARAPI GetServiceCategory(
        /* [out] */ Int32* result);

    CARAPI GetDeliveryTime(
        /* [out] */ Int64* result);

    CARAPI GetMessageBody(
        /* [out] */ String* result);

    CARAPI IsRead(
        /* [out] */ Boolean* result);

    CARAPI GetSerialNumber(
        /* [out] */ Int32* result);

    CARAPI GetCmasWarningInfo(
        /* [out] */ ISmsCbCmasInfo** result);

    CARAPI GetEtwsWarningInfo(
        /* [out] */ ISmsCbEtwsInfo** result);

    /**
      * Return whether the broadcast is an emergency (PWS) message type.
      * This includes lower priority test messages and Amber alerts.
      *
      * All public alerts show the flashing warning icon in the dialog,
      * but only emergency alerts play the alert sound and speak the message.
      *
      * @return true if the message is PWS type; false otherwise
      */
    CARAPI IsPublicAlertMessage(
        /* [out] */ Boolean* result);

    /**
      * Returns whether the broadcast is an emergency (PWS) message type,
      * including test messages and AMBER alerts.
      *
      * @return true if the message is PWS type (ETWS or CMAS)
      */
    CARAPI IsEmergencyAlertMessage(
        /* [out] */ Boolean* result);

    /**
      * Return whether the broadcast is an ETWS emergency message type.
      * @return true if the message is ETWS emergency type; false otherwise
      */
    CARAPI IsEtwsMessage(
        /* [out] */ Boolean* result);

    /**
      * Return whether the broadcast is a CMAS emergency message type.
      * @return true if the message is CMAS emergency type; false otherwise
      */
    CARAPI IsCmasMessage(
        /* [out] */ Boolean* result);

    /**
      * Return the CMAS message class.
      * @return the CMAS message class, e.g. {@link SmsCbCmasInfo#CMAS_CLASS_SEVERE_THREAT}, or
      *  {@link SmsCbCmasInfo#CMAS_CLASS_UNKNOWN} if this is not a CMAS alert
      */
    CARAPI GetCmasMessageClass(
        /* [out] */ Int32* result);

    /**
      * Return whether the broadcast is an ETWS popup alert.
      * This method checks the message ID and the message code.
      * @return true if the message indicates an ETWS popup alert
      */
    CARAPI IsEtwsPopupAlert(
        /* [out] */ Boolean* result);

    /**
      * Return whether the broadcast is an ETWS emergency user alert.
      * This method checks the message ID and the message code.
      * @return true if the message indicates an ETWS emergency user alert
      */
    CARAPI IsEtwsEmergencyUserAlert(
        /* [out] */ Boolean* result);

    /**
      * Return whether the broadcast is an ETWS test message.
      * @return true if the message is an ETWS test message; false otherwise
      */
    CARAPI IsEtwsTestMessage(
        /* [out] */ Boolean* result);

    /**
      * Return the abbreviated date string for the message delivery time.
      * @param context the context object
      * @return a String to use in the broadcast list UI
      */
    CARAPI GetDateString(
        /* [in] */ IContext* context,
        /* [out] */ String* result);

    /**
      * Return the date string for the message delivery time, suitable for text-to-speech.
      * @param context the context object
      * @return a String for populating the list item AccessibilityEvent for TTS
      */
    CARAPI GetSpokenDateString(
        /* [in] */ IContext* context,
        /* [out] */ String* result);

    TO_STRING_IMPL("CellBroadcastMessage");

private:
    CellBroadcastMessage(
        /* [in] */ ISmsCbMessage* message,
        /* [in] */ Int64 deliveryTime,
        /* [in] */ Boolean isRead);

    CellBroadcastMessage(
        /* [in] */ IParcel* in);

public:
    /** Identifier for getExtra() when adding this object to an Intent. */
    static const String SMS_CB_MESSAGE_EXTRA;
    //static const AutoPtr<IParcelable> AutoPtr< ::Creator<CellBroadcastMessage> > CREATOR;

private:
    /** SmsCbMessage. */
    AutoPtr<ISmsCbMessage> mSmsCbMessage;
    Int64 mDeliveryTime;
    Boolean mIsRead;
    /**
      * Indicates the subId
      *
      * @hide
      */
    Int64 mSubId;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CELLBROADCASTMESSAGE_H__

