
#ifndef __ELASTOS_DROID_TEXT_CBIDIFORMATTER_H__
#define __ELASTOS_DROID_TEXT_CBIDIFORMATTER_H__

#include "_Elastos_Droid_Text_CBidiFormatter.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Text {

CarClass(CBidiFormatter)
    , public Object
    , public IBidiFormatter
{
    friend class CBidiFormatterBuilder;
private:
    class DirectionalityEstimator
        : public Object
    {
    public:
        /**
         * Constructor.
         *
         * @param text The string to scan.
         * @param isHtml Whether the text to be scanned is to be treated as HTML, i.e. skipping over
         *     tags and entities.
         */
        DirectionalityEstimator(
            /* [in] */ const String& text,
            /* [in] */ Boolean isHtml);

        ~DirectionalityEstimator();

        /**
         * Returns the directionality of the first character with strong directionality in the
         * string, or DIR_UNKNOWN if none was encountered. Treats a non-BN character between an
         * LRE/RLE/LRO/RLO and its matching PDF as a strong character, LTR after LRE/LRO, and RTL
         * after RLE/RLO. The results are undefined for a string containing unbalanced
         * LRE/RLE/LRO/RLO/PDF characters.
         */
        CARAPI_(Int32) GetEntryDir();

        /**
         * Returns the directionality of the last character with strong directionality in the
         * string, or DIR_UNKNOWN if none was encountered. For efficiency, actually scans backwards
         * from the end of the string. Treats a non-BN character between an LRE/RLE/LRO/RLO and its
         * matching PDF as a strong character, LTR after LRE/LRO, and RTL after RLE/RLO. The results
         * are undefined for a string containing unbalanced LRE/RLE/LRO/RLO/PDF characters.
         */
        CARAPI_(Int32) GetExitDir();

        /**
         * Returns the Character.DIRECTIONALITY_... value of the next codepoint and advances
         * charIndex. If isHtml, and the codepoint is '<' or '&', advances through the tag/entity,
         * and returns Character.DIRECTIONALITY_WHITESPACE. For an entity, it would be best to
         * figure out the actual character, and return its dirtype, but treating it as whitespace is
         * good enough for our purposes.
         *
         * @throws java.lang.IndexOutOfBoundsException if called when charIndex >= length or < 0.
         */
        CARAPI_(Byte) DirTypeForward();

        /**
         * Returns the Character.DIRECTIONALITY_... value of the preceding codepoint and advances
         * charIndex backwards. If isHtml, and the codepoint is the end of a complete HTML tag or
         * entity, advances over the whole tag/entity and returns
         * Character.DIRECTIONALITY_WHITESPACE. For an entity, it would be best to figure out the
         * actual character, and return its dirtype, but treating it as whitespace is good enough
         * for our purposes.
         *
         * @throws java.lang.IndexOutOfBoundsException if called when charIndex > length or <= 0.
         */
        CARAPI_(Byte) DirTypeBackward();

    private:
        // Internal methods

        /**
         * Gets the bidi character class, i.e. Character.getDirectionality(), of a given char, using
         * a cache for speed. Not designed for supplementary codepoints, whose results we do not
         * cache.
         */
        static CARAPI_(Byte) GetCachedDirectionality(
            /* [in] */ Char32 c);

        /**
         * Advances charIndex forward through an HTML tag (after the opening &lt; has already been
         * read) and returns Character.DIRECTIONALITY_WHITESPACE. If there is no matching &gt;,
         * does not change charIndex and returns Character.DIRECTIONALITY_OTHER_NEUTRALS (for the
         * &lt; that hadn't been part of a tag after all).
         */
        CARAPI_(Byte) SkipTagForward();

        /**
         * Advances charIndex backward through an HTML tag (after the closing &gt; has already been
         * read) and returns Character.DIRECTIONALITY_WHITESPACE. If there is no matching &lt;, does
         * not change charIndex and returns Character.DIRECTIONALITY_OTHER_NEUTRALS (for the &gt;
         * that hadn't been part of a tag after all). Nevertheless, the running time for calling
         * skipTagBackward() in a loop remains linear in the size of the text, even for a text like
         * "&gt;&gt;&gt;&gt;", because skipTagBackward() also stops looking for a matching &lt;
         * when it encounters another &gt;.
         */
        CARAPI_(Byte) SkipTagBackward();

        /**
         * Advances charIndex forward through an HTML character entity tag (after the opening
         * &amp; has already been read) and returns Character.DIRECTIONALITY_WHITESPACE. It would be
         * best to figure out the actual character and return its dirtype, but this is good enough.
         */
        CARAPI_(Byte) SkipEntityForward();

        /**
         * Advances charIndex backward through an HTML character entity tag (after the closing ;
         * has already been read) and returns Character.DIRECTIONALITY_WHITESPACE. It would be best
         * to figure out the actual character and return its dirtype, but this is good enough.
         * If there is no matching &amp;, does not change charIndex and returns
         * Character.DIRECTIONALITY_OTHER_NEUTRALS (for the ';' that did not start an entity after
         * all). Nevertheless, the running time for calling skipEntityBackward() in a loop remains
         * linear in the size of the text, even for a text like ";;;;;;;", because skipTagBackward()
         * also stops looking for a matching &amp; when it encounters another ;.
         */
        CARAPI_(Byte) SkipEntityBackward();

        static CARAPI_(Boolean) Init();

    private:
        // Internal static variables and constants.

        /**
         * Size of the bidi character class cache. The results of the Character.getDirectionality()
         * calls on the lowest DIR_TYPE_CACHE_SIZE codepoints are kept in an array for speed.
         * The 0x700 value is designed to leave all the European and Near Eastern languages in the
         * cache. It can be reduced to 0x180, restricting the cache to the Western European
         * languages.
         */
        static const Int32 DIR_TYPE_CACHE_SIZE;

        /**
         * The bidi character class cache.
         */
        static AutoPtr< ArrayOf<Byte> > DIR_TYPE_CACHE;

        static Boolean InitStatic;

        // Internal instance variables.

        /**
         * The text to be scanned.
         */
        String mText;

        /**
         * Whether the text to be scanned is to be treated as HTML, i.e. skipping over tags and
         * entities when looking for the next / preceding dir type.
         */
        Boolean mIsHtml;

        /**
         * The length of the text in chars.
         */
        Int32 mLength;

        /**
         * The current position in the text.
         */
        Int32 mCharIndex;

        /**
         * The char encountered by the last dirTypeForward or dirTypeBackward call. If it
         * encountered a supplementary codepoint, this contains a char that is not a valid
         * codepoint. This is ok, because this member is only used to detect some well-known ASCII
         * syntax, e.g. "http://" and the beginning of an HTML tag or entity.
         */
        Char32 mLastChar;

    };
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBidiFormatter();

    ~CBidiFormatter();

    CARAPI constructor(
        /* [in] */ Boolean isRtlContext,
        /* [in] */ Int32 flags,
        /* [in] */ ITextDirectionHeuristic* heuristic);

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
     * Helper method to return true if the Locale directionality is RTL.
     *
     * @param locale The Locale whose directionality will be checked to be RTL or LTR
     * @return true if the {@code locale} directionality is RTL. False otherwise.
     */
    static CARAPI_(Boolean) IsRtlLocale(
        /* [in] */ ILocale* locale);

    /**
     * Returns the directionality of the last character with strong directionality in the string, or
     * DIR_UNKNOWN if none was encountered. For efficiency, actually scans backwards from the end of
     * the string. Treats a non-BN character between an LRE/RLE/LRO/RLO and its matching PDF as a
     * strong character, LTR after LRE/LRO, and RTL after RLE/RLO. The results are undefined for a
     * string containing unbalanced LRE/RLE/LRO/RLO/PDF characters. The intended use is to check
     * whether a logically separate item that starts with a number or a character of the string's
     * exit directionality and follows this string inline (not counting any neutral characters in
     * between) would "stick" to it in an opposite-directionality context, thus being displayed in
     * an incorrect position. An LRM or RLM character (the one of the context's directionality)
     * between the two will prevent such sticking.
     *
     * @param str the string to check.
     */
    static CARAPI_(Int32) GetExitDir(
        /* [in] */ const String& str);

    /**
     * Returns the directionality of the first character with strong directionality in the string,
     * or DIR_UNKNOWN if none was encountered. Treats a non-BN character between an
     * LRE/RLE/LRO/RLO and its matching PDF as a strong character, LTR after LRE/LRO, and RTL after
     * RLE/RLO. The results are undefined for a string containing unbalanced LRE/RLE/LRO/RLO/PDF
     * characters. The intended use is to check whether a logically separate item that ends with a
     * character of the string's entry directionality and precedes the string inline (not counting
     * any neutral characters in between) would "stick" to it in an opposite-directionality context,
     * thus being displayed in an incorrect position. An LRM or RLM character (the one of the
     * context's directionality) between the two will prevent such sticking.
     *
     * @param str the string to check.
     */
    static CARAPI_(Int32) GetEntryDir(
        /* [in] */ const String& str);


private:
    /**
     * The default text direction heuristic.
     */
    static AutoPtr<ITextDirectionHeuristic> DEFAULT_TEXT_DIRECTION_HEURISTIC;

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
    AutoPtr<ITextDirectionHeuristic> mDefaultTextDirectionHeuristic;

    /**
     * Enum for directionality type.
     */
    static const Int32 DIR_LTR;
    static const Int32 DIR_UNKNOWN;
    static const Int32 DIR_RTL;
};

} // namespace Text
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_CBIDIFORMATTER_H__
