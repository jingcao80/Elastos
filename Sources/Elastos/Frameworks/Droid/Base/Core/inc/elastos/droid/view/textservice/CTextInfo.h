
#ifndef __ELASTOS_DROID_VIEW_TEXTSERVICE_CTEXTINFO_H__
#define __ELASTOS_DROID_VIEW_TEXTSERVICE_CTEXTINFO_H__

#include "_Elastos_Droid_View_TextService_CTextInfo.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace View {
namespace TextService {

/**
 * This class contains a metadata of the input of TextService
 */
CarClass(CTextInfo)
    , public Object
    , public ITextInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /**
     * Constructor.
     * @param text the text which will be input to TextService
     */
    CARAPI constructor(
        /* [in] */ const String& text);

    /**
     * Constructor.
     * @param text the text which will be input to TextService
     * @param cookie the cookie for this TextInfo
     * @param sequence the sequence number for this TextInfo
     */
    CARAPI constructor(
        /* [in] */ const String& text,
        /* [in] */ Int32 cookie,
        /* [in] */ Int32 sequence);

    /**
     * Constructor.
     * @param charSequence the text which will be input to TextService. Attached spans that
     * implement {@link ParcelableSpan} will also be marshaled alongside with the text.
     * @param start the beginning of the range of text (inclusive).
     * @param end the end of the range of text (exclusive).
     * @param cookie the cookie for this TextInfo
     * @param sequenceNumber the sequence number for this TextInfo
     */
    CARAPI constructor(
        /* [in] */ ICharSequence* charSequence,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Int32 cookie,
        /* [in] */ Int32 sequenceNumber);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    /**
     * Used to package this object into a {@link Parcel}.
     *
     * @param dest The {@link Parcel} to be written.
     * @param flags The flags used for parceling.
     */
    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * @return the text which is an input of a text service
     */
    CARAPI GetText(
        /* [out] */ String* text);

    /**
     * @return the charSequence which is an input of a text service. This may have some parcelable
     * spans.
     */
    CARAPI GetCharSequence(
        /* [out] */ ICharSequence** seq);

    /**
     * @return the cookie of TextInfo
     */
    CARAPI GetCookie(
        /* [out] */ Int32* cookie);

    /**
     * @return the sequence of TextInfo
     */
    CARAPI GetSequence(
        /* [out] */ Int32* sequence);

private:
    static Int32 GetStringLengthOrZero(
        /* [in] */ const String& text);

private:
    AutoPtr<ICharSequence> mCharSequence;
    Int32 mCookie;
    Int32 mSequenceNumber;

    static Int32 DEFAULT_COOKIE;
    static Int32 DEFAULT_SEQUENCE_NUMBER;
};

}   //namespace TextService
}   //namespace View
}   //namespace Droid
}   //namespace Elastos

#endif //__ELASTOS_DROID_VIEW_TEXTSERVICE_CTEXTINFO_H__
