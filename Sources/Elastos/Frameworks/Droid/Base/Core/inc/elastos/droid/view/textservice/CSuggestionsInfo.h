//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_VIEW_TEXTSERVICE_CSUGGESTIONSINFO_H__
#define __ELASTOS_DROID_VIEW_TEXTSERVICE_CSUGGESTIONSINFO_H__

#include "_Elastos_Droid_View_TextService_CSuggestionsInfo.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace View {
namespace TextService {

/**
 * This class contains a metadata of suggestions from the text service
 */
CarClass(CSuggestionsInfo)
    , public Object
    , public ISuggestionsInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /**
     * Constructor.
     * @param suggestionsAttributes from the text service
     * @param suggestions from the text service
     */
    CARAPI constructor(
        /* [in] */ Int32 suggestionsAttributes,
        /* [in] */ ArrayOf<String>* suggestions);

    /**
     * Constructor.
     * @param suggestionsAttributes from the text service
     * @param suggestions from the text service
     * @param cookie the cookie of the input TextInfo
     * @param sequence the cookie of the input TextInfo
     */
    CARAPI constructor(
        /* [in] */ Int32 suggestionsAttributes,
        /* [in] */ ArrayOf<String>* suggestions,
        /* [in] */ Int32 cookie,
        /* [in] */ Int32 sequence);

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
     * Set the cookie and the sequence of SuggestionsInfo which are set to TextInfo from a client
     * application
     * @param cookie the cookie of an input TextInfo
     * @param sequence the cookie of an input TextInfo
     */
    CARAPI SetCookieAndSequence(
        /* [in] */ Int32 cookie,
        /* [in] */ Int32 sequence);

    /**
     * @return the cookie which may be set by a client application
     */
    CARAPI GetCookie(
        /* [out] */ Int32* cookie);

    /**
     * @return the sequence which may be set by a client application
     */
    CARAPI GetSequence(
        /* [out] */ Int32* sequence);

    /**
     * @return the attributes of suggestions. This includes whether the spell checker has the word
     * in its dictionary or not and whether the spell checker has confident suggestions for the
     * word or not.
     */
    CARAPI GetSuggestionsAttributes(
        /* [out] */ Int32* attr);

    /**
     * @return the count of the suggestions. If there's no suggestions at all, this method returns
     * -1. Even if this method returns 0, it doesn't necessarily mean that there are no suggestions
     * for the requested word. For instance, the caller could have been asked to limit the maximum
     * number of suggestions returned.
     */
    CARAPI GetSuggestionsCount(
        /* [out] */ Int32* count);

    /**
     * @param i the id of suggestions
     * @return the suggestion at the specified id
     */
    CARAPI GetSuggestionAt(
        /* [in] */ Int32 i,
        /* [out] */ String* str);

private:
    static AutoPtr<ArrayOf<String> > EMPTY;// = ArrayUtils.emptyArray(String.class);

    Int32 mSuggestionsAttributes;
    AutoPtr<ArrayOf<String> > mSuggestions;
    Boolean mSuggestionsAvailable;
    Int32 mCookie;
    Int32 mSequence;
};

}   //namespace TextService
}   //namespace View
}   //namespace Droid
}   //namespace Elastos

#endif //__ELASTOS_DROID_VIEW_TEXTSERVICE_CSUGGESTIONSINFO_H__
