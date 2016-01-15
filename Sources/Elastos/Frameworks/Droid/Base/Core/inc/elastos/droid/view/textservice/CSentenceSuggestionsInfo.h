
#ifndef __ELASTOS_DROID_VIEW_TEXTSERVICE_CSENTENCESUGGESTIONSINFO_H__
#define __ELASTOS_DROID_VIEW_TEXTSERVICE_CSENTENCESUGGESTIONSINFO_H__

#include "_Elastos_Droid_View_TextService_CSentenceSuggestionsInfo.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace View {
namespace TextService {

CarClass(CSentenceSuggestionsInfo)
    , public Object
    , public ISentenceSuggestionsInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /**
     * Constructor.
     * @param suggestionsInfos from the text service
     * @param offsets the array of offsets of suggestions
     * @param lengths the array of lengths of suggestions
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<ISuggestionsInfo*>* suggestionsInfos,
        /* [in] */ ArrayOf<Int32>* offsets,
        /* [in] */ ArrayOf<Int32>* lengths);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

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
     * @return the count of {@link SuggestionsInfo}s this instance holds.
     */
    CARAPI GetSuggestionsCount(
        /* [out] */ Int32* count);

    /**
     * @param i the id of {@link SuggestionsInfo}s this instance holds.
     * @return a {@link SuggestionsInfo} at the specified id
     */
    CARAPI GetSuggestionsInfoAt(
        /* [in] */ Int32 i,
        /* [out] */ ISuggestionsInfo** info);

    /**
     * @param i the id of {@link SuggestionsInfo}s this instance holds
     * @return the offset of the specified {@link SuggestionsInfo}
     */
    CARAPI GetOffsetAt(
        /* [in] */ Int32 i,
        /* [out] */ Int32* offset);

    /**
     * @param i the id of {@link SuggestionsInfo}s this instance holds
     * @return the length of the specified {@link SuggestionsInfo}
     */
    CARAPI GetLengthAt(
        /* [in] */ Int32 i,
        /* [out] */ Int32* length);

private:
    AutoPtr<ArrayOf<ISuggestionsInfo*> > mSuggestionsInfos;
    AutoPtr<ArrayOf<Int32> > mOffsets;
    AutoPtr<ArrayOf<Int32> > mLengths;
};

}   //namespace TextService
}   //namespace View
}   //namespace Droid
}   //namespace Elastos

#endif //__ELASTOS_DROID_VIEW_TEXTSERVICE_CSENTENCESUGGESTIONSINFO_H__
