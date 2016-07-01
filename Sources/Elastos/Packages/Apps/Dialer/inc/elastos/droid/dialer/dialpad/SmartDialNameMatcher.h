#ifndef __ELASTOS_DROID_DIALER_DIALPAD_SMARTDIALNAMEMATCHER_H__
#define __ELASTOS_DROID_DIALER_DIALPAD_SMARTDIALNAMEMATCHER_H__

#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>
#include <elastos/core/StringBuilder.h>
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Core::StringBuilder;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Dialpad {

/**
 * {@link #SmartDialNameMatcher} contains utility functions to remove accents from accented
 * characters and normalize a phone number. It also contains the matching logic that determines if
 * a contact's display name matches a numeric query. The boolean variable
 * {@link #ALLOW_INITIAL_MATCH} controls the behavior of the matching logic and determines
 * whether we allow matches like 57 - (J)ohn (S)mith.
 */
class SmartDialNameMatcher
    : public Object
    , public ISmartDialNameMatcher
{
public:
    CAR_INTERFACE_DECL();

    // @VisibleForTesting
    CARAPI constructor(
        /* [in] */ const String& query);

    CARAPI constructor(
        /* [in] */ const String& query,
        /* [in] */ ISmartDialMap* map);

    /**
     * Strips a phone number of unnecessary characters (spaces, dashes, etc.)
     *
     * @param number Phone number we want to normalize
     * @return Phone number consisting of digits from 0-9
     */
    static CARAPI_(String) NormalizeNumber(
        /* [in] */ const String& number,
        /* [in] */ ISmartDialMap* map);

    /**
     * Strips a phone number of unnecessary characters (spaces, dashes, etc.)
     *
     * @param number Phone number we want to normalize
     * @param offset Offset to start from
     * @return Phone number consisting of digits from 0-9
     */
    static CARAPI_(String) NormalizeNumber(
        /* [in] */ const String& number,
        /* [in] */ Int32 offset,
        /* [in] */ ISmartDialMap* map);

    /**
     * Matches a phone number against a query. Let the test application overwrite the NANP setting.
     *
     * @param phoneNumber - Raw phone number
     * @param query - Normalized query (only contains numbers from 0-9)
     * @param useNanp - Overwriting nanp setting boolean, used for testing.
     * @return {@literal null} if the number and the query don't match, a valid
     *         SmartDialMatchPosition with the matching positions otherwise
     */
     // @VisibleForTesting
    CARAPI MatchesNumber(
        /* [in] */ String phoneNumber,
        /* [in] */ String query,
        /* [in] */ Boolean useNanp,
        /* [out] */ ISmartDialMatchPosition** position);

    /**
     * Matches a phone number against the saved query, taking care of formatting characters and also
     * taking into account country code prefixes and special NANP number treatment.
     *
     * @param phoneNumber - Raw phone number
     * @return {@literal null} if the number and the query don't match, a valid
     *         SmartDialMatchPosition with the matching positions otherwise
     */
    CARAPI MatchesNumber(
        /* [in] */ String phoneNumber,
        /* [out] */ ISmartDialMatchPosition** position);

    /**
     * Matches a phone number against a query, taking care of formatting characters and also
     * taking into account country code prefixes and special NANP number treatment.
     *
     * @param phoneNumber - Raw phone number
     * @param query - Normalized query (only contains numbers from 0-9)
     * @return {@literal null} if the number and the query don't match, a valid
     *         SmartDialMatchPosition with the matching positions otherwise
     */
    CARAPI MatchesNumber(
        /* [in] */ String phoneNumber,
        /* [in] */ String query,
        /* [out] */ ISmartDialMatchPosition** position);

    /**
     * This function iterates through each token in the display name, trying to match the query
     * to the numeric equivalent of the token.
     *
     * A token is defined as a range in the display name delimited by characters that have no
     * latin alphabet equivalents (e.g. spaces - ' ', periods - ',', underscores - '_' or chinese
     * characters - '王'). Transliteration from non-latin characters to latin character will be
     * done on a best effort basis - e.g. 'Ü' - 'u'.
     *
     * For example,
     * the display name "Phillips Thomas Jr" contains three tokens: "phillips", "thomas", and "jr".
     *
     * A match must begin at the start of a token.
     * For example, typing 846(Tho) would match "Phillips Thomas", but 466(hom) would not.
     *
     * Also, a match can extend across tokens.
     * For example, typing 37337(FredS) would match (Fred S)mith.
     *
     * @param displayName The normalized(no accented characters) display name we intend to match
     * against.
     * @param query The string of digits that we want to match the display name to.
     * @param matchList An array list of {@link SmartDialMatchPosition}s that we add matched
     * positions to.
     * @return Returns true if a combination of the tokens in displayName match the query
     * string contained in query. If the function returns true, matchList will contain an
     * ArrayList of match positions (multiple matches correspond to initial matches).
     */
    // @VisibleForTesting
    CARAPI_(Boolean) MatchesCombination(
        /* [in] */ const String& displayName,
        /* [in] */ const String& query,
        /* [in] */ IArrayList* matchList);

    CARAPI Matches(
        /* [in] */ const String& displayName,
        /* [out] */ Boolean* result);

    CARAPI GetMatchPositions(
        /* [out] */ IArrayList** positions);

    CARAPI SetQuery(
        /* [in] */ const String& query);

    CARAPI GetNameMatchPositionsInString(
        /* [out] */ String* result);

    CARAPI GetNumberMatchPositionsInString(
        /* [out] */ String* result);

    CARAPI GetQuery(
        /* [out] */ String* result);
private:
    /**
     * Constructs empty highlight mask. Bit 0 at a position means there is no match, Bit 1 means
     * there is a match and should be highlighted in the TextView.
     * @param builder StringBuilder object
     * @param length Length of the desired mask.
     */
    CARAPI_(void) ConstructEmptyMask(
        /* [in] */ StringBuilder* builder,
        /* [in] */ Int32 length);

    /**
     * Replaces the 0-bit at a position with 1-bit, indicating that there is a match.
     * @param builder StringBuilder object.
     * @param matchPos Match Positions to mask as 1.
     */
    CARAPI_(void) ReplaceBitInMask(
        /* [in] */ StringBuilder* builder,
        /* [in] */ ISmartDialMatchPosition* matchPos);

    /**
     * Matches a phone number against a query, taking care of formatting characters
     *
     * @param phoneNumber - Raw phone number
     * @param query - Normalized query (only contains numbers from 0-9)
     * @param offset - The position in the number to start the match against (used to ignore
     * leading prefixes/country codes)
     * @return {@literal null} if the number and the query don't match, a valid
     *         SmartDialMatchPosition with the matching positions otherwise
     */
    CARAPI_(AutoPtr<ISmartDialMatchPosition>) MatchesNumberWithOffset(
        /* [in] */ const String& phoneNumber,
        /* [in] */ const String& query,
        /* [in] */ Int32 offset);

public:
    static const AutoPtr<ISmartDialMap> LATIN_SMART_DIAL_MAP; // = new LatinSmartDialMap();

private:
    String mQuery;

    // Whether or not we allow matches like 57 - (J)ohn (S)mith
    static const Boolean ALLOW_INITIAL_MATCH; // = true;

    // The maximum length of the initial we will match - typically set to 1 to minimize false
    // positives
    static const Int32 INITIAL_LENGTH_LIMIT; // = 1;

    AutoPtr<IArrayList> mMatchPositions;

    AutoPtr<ISmartDialMap> mMap;

    String mNameMatchMask;
    String mPhoneNumberMatchMask;
};

} // Dialpad
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_DIALPAD_SMARTDIALNAMEMATCHER_H__
