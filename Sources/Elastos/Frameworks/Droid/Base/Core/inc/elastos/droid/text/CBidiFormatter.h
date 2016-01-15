
#ifndef __ELASTOS_DROID_TEXT_CBIDIFORMATTER_H__
#define __ELASTOS_DROID_TEXT_CBIDIFORMATTER_H__

#include "_Elastos_Droid_Text_CBidiFormatter.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Text {

CarClass(CBidiFormatter)
    , public Object
    , public IBidiFormatter
{
public:
    CBidiFormatter();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /**
     * @return Whether the context directionality is RTL
     */
    CARAPI IsRtlContext(
        /* [out] */ Boolean* ret);

    /**
     * @return Whether directionality "reset" should also be done before a string being
     * bidi-wrapped, not just after it.
     */
    CARAPI GetStereoReset(
        /* [out] */ Boolean* ret);

    /**
     * Returns a Unicode bidi mark matching the context directionality (LRM or RLM) if either the
     * overall or the exit directionality of a given string is opposite to the context directionality.
     * Putting this after the string (including its directionality declaration wrapping) prevents it
     * from "sticking" to other opposite-directionality text or a number appearing after it inline
     * with only neutral content in between. Otherwise returns the empty string. While the exit
     * directionality is determined by scanning the end of the string, the overall directionality is
     * given explicitly by a heuristic to estimate the {@code str}'s directionality.
     *
     * @param str String after which the mark may need to appear.
     * @param heuristic The text direction heuristic that will be used to estimate the {@code str}'s
     *                  directionality.
     * @return LRM for RTL text in LTR context; RLM for LTR text in RTL context;
     *     else, the empty string.
     *
     * @hide
     */
    CARAPI MarkAfter(
        /* [in] */ const String& str,
        /* [in] */ ITextDirectionHeuristic* heuristic,
        /* [out] */ String* ret);

    /**
     * Returns a Unicode bidi mark matching the context directionality (LRM or RLM) if either the
     * overall or the entry directionality of a given string is opposite to the context
     * directionality. Putting this before the string (including its directionality declaration
     * wrapping) prevents it from "sticking" to other opposite-directionality text appearing before
     * it inline with only neutral content in between. Otherwise returns the empty string. While the
     * entry directionality is determined by scanning the beginning of the string, the overall
     * directionality is given explicitly by a heuristic to estimate the {@code str}'s directionality.
     *
     * @param str String before which the mark may need to appear.
     * @param heuristic The text direction heuristic that will be used to estimate the {@code str}'s
     *                  directionality.
     * @return LRM for RTL text in LTR context; RLM for LTR text in RTL context;
     *     else, the empty string.
     *
     * @hide
     */
    CARAPI MarkBefore(
        /* [in] */ const String& str,
        /* [in] */ ITextDirectionHeuristic* heuristic,
        /* [out] */ String* ret);

    /**
     * Estimates the directionality of a string using the default text direction heuristic.
     *
     * @param str String whose directionality is to be estimated.
     * @return true if {@code str}'s estimated overall directionality is RTL. Otherwise returns
     *          false.
     */
    CARAPI IsRtl(
        /* [in] */ const String& str,
        /* [out] */ Boolean* ret);

    /**
     * Formats a string of given directionality for use in plain-text output of the context
     * directionality, so an opposite-directionality string is neither garbled nor garbles its
     * surroundings. This makes use of Unicode bidi formatting characters.
     * <p>
     * The algorithm: In case the given directionality doesn't match the context directionality, wraps
     * the string with Unicode bidi formatting characters: RLE+{@code str}+PDF for RTL text, or
     * LRE+{@code str}+PDF for LTR text.
     * <p>
     * If {@code isolate}, directionally isolates the string so that it does not garble its
     * surroundings. Currently, this is done by "resetting" the directionality after the string by
     * appending a trailing Unicode bidi mark matching the context directionality (LRM or RLM) when
     * either the overall directionality or the exit directionality of the string is opposite to that
     * of the context. If the formatter was built using {@link Builder#stereoReset(boolean)} and
     * passing "true" as an argument, also prepends a Unicode bidi mark matching the context
     * directionality when either the overall directionality or the entry directionality of the
     * string is opposite to that of the context. Note that as opposed to the overall
     * directionality, the entry and exit directionalities are determined from the string itself.
     * <p>
     * Does *not* do HTML-escaping.
     *
     * @param str The input string.
     * @param heuristic The algorithm to be used to estimate the string's overall direction.
     *        See {@link TextDirectionHeuristics} for pre-defined heuristics.
     * @param isolate Whether to directionally isolate the string to prevent it from garbling the
     *     content around it
     * @return Input string after applying the above processing.
     */
    CARAPI UnicodeWrap(
        /* [in] */ const String& str,
        /* [in] */ ITextDirectionHeuristic* heuristic,
        /* [in] */ Boolean isolate,
        /* [out] */ String* ret);

    /**
     * Operates like {@link #unicodeWrap(String, TextDirectionHeuristic, boolean)}, but assumes
     * {@code isolate} is true.
     *
     * @param str The input string.
     * @param heuristic The algorithm to be used to estimate the string's overall direction.
     *        See {@link TextDirectionHeuristics} for pre-defined heuristics.
     * @return Input string after applying the above processing.
     */
    CARAPI UnicodeWrap(
        /* [in] */ const String& str,
        /* [in] */ ITextDirectionHeuristic* heuristic,
        /* [out] */ String* ret);

    /**
     * Operates like {@link #unicodeWrap(String, TextDirectionHeuristic, boolean)}, but uses the
     * formatter's default direction estimation algorithm.
     *
     * @param str The input string.
     * @param isolate Whether to directionally isolate the string to prevent it from garbling the
     *     content around it
     * @return Input string after applying the above processing.
     */
    CARAPI UnicodeWrap(
        /* [in] */ const String& str,
        /* [in] */ Boolean isolate,
        /* [out] */ String* ret);

    /**
     * Operates like {@link #unicodeWrap(String, TextDirectionHeuristic, boolean)}, but uses the
     * formatter's default direction estimation algorithm and assumes {@code isolate} is true.
     *
     * @param str The input string.
     * @return Input string after applying the above processing.
     */
    CARAPI UnicodeWrap(
        /* [in] */ const String& str,
        /* [out] */ String* ret);

private:
    /**
     * The default text direction heuristic.
     */
    // private static TextDirectionHeuristic DEFAULT_TEXT_DIRECTION_HEURISTIC = FIRSTSTRONG_LTR;

    /**
     * Unicode "Left-To-Right Embedding" (LRE) character.
     */
    static const Char32 LRE;// = '\u202A';

    /**
     * Unicode "Right-To-Left Embedding" (RLE) character.
     */
    static const Char32 RLE;// = '\u202B';

    /**
     * Unicode "Pop Directional Formatting" (PDF) character.
     */
    static const Char32 PDF;// = '\u202C';

    /**
     *  Unicode "Left-To-Right Mark" (LRM) character.
     */
    static const Char32 LRM;// = '\u200E';

    /*
     * Unicode "Right-To-Left Mark" (RLM) character.
     */
    static const Char32 RLM;// = '\u200F';

    /*
     * String representation of LRM
     */
    static const String LRM_STRING;// = Character.toString(LRM);

    /*
     * String representation of RLM
     */
    static const String RLM_STRING;// = Character.toString(RLM);

    /**
     * Empty string constant.
     */
    static const String EMPTY_STRING;// = "";

    //
    static const Int32 FLAG_STEREO_RESET;// = 2;
    static const Int32 DEFAULT_FLAGS;// = FLAG_STEREO_RESET;


    static const AutoPtr<IBidiFormatter> DEFAULT_LTR_INSTANCE;// = new BidiFormatter(
            // false /* LTR context */,
            // DEFAULT_FLAGS,
            // DEFAULT_TEXT_DIRECTION_HEURISTIC);

    static const AutoPtr<IBidiFormatter> DEFAULT_RTL_INSTANCE;// = new BidiFormatter(
            // true /* RTL context */,
            // DEFAULT_FLAGS,
            // DEFAULT_TEXT_DIRECTION_HEURISTIC);

    Boolean mIsRtlContext;
    Int32 mFlags;
    //TextDirectionHeuristic mDefaultTextDirectionHeuristic;
};

} // namespace Text
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_CBIDIFORMATTER_H__
