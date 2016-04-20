#ifndef __ELASTOS_DROID_TEXT_TEXTUTILS_H__
#define __ELASTOS_DROID_TEXT_TEXTUTILS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Text.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>

using Elastos::Core::Object;
using Elastos::Core::ICharSequence;
using Elastos::Utility::ILocale;
using Elastos::Utility::IIterable;
using Elastos::Utility::Regex::IPattern;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Text {

class MeasuredText;

class ECO_PUBLIC TextUtils
{
private:
    class ECO_LOCAL Reverser
        : public Object
        , public ICharSequence
        , public IGetChars
    {
    public:
        CAR_INTERFACE_DECL()

        Reverser(
            /* [in] */ ICharSequence* source,
            /* [in] */ Int32 start,
            /* [in] */ Int32 end);

        CARAPI GetLength(
            /* [out] */ Int32* len);

        CARAPI SubSequence(
            /* [in] */ Int32 start,
            /* [in] */ Int32 end,
            /* [out] */ ICharSequence** csq);

        CARAPI ToString(
            /* [out] */ String* info);

        CARAPI GetCharAt(
            /* [in] */ Int32 offset,
            /* [out] */ Char32* ch);

        CARAPI GetChars(
            /* [in] */ Int32 start,
            /* [in] */ Int32 end,
            /* [in] */ ArrayOf<Char32>* dest,
            /* [in] */ Int32 destoff);

    private:
        AutoPtr<ICharSequence> mSource;
        Int32 mStart;
        Int32 mEnd;
    };

public:
    class CHAR_SEQUENCE_CREATOR {
    public:
        static CARAPI CreateFromParcel(
            /* [in] */ IParcel* p,
            /* [out] */ ICharSequence** csq);

        static AutoPtr<ArrayOf<ICharSequence*> > NewArray(
            /* [in] */ Int32 size);
    };

public:
    static CARAPI GetChars(
        /* [in] */ ICharSequence* s,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ ArrayOf<Char32>* dest,
        /* [in] */ Int32 destoff);

    static CARAPI_(Int32) IndexOf(
        /* [in] */ ICharSequence* s,
        /* [in] */ Char32 ch);

    static CARAPI_(Int32) IndexOf(
        /* [in] */ ICharSequence* s,
        /* [in] */ Char32 ch,
        /* [in] */ Int32 start);

    static CARAPI_(Int32) IndexOf(
        /* [in] */ ICharSequence* s,
        /* [in] */ Char32 ch,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    static CARAPI_(Int32) LastIndexOf(
        /* [in] */ ICharSequence* s,
        /* [in] */ Char32 ch);

    static CARAPI_(Int32) LastIndexOf(
        /* [in] */ ICharSequence* s,
        /* [in] */ Char32 ch,
        /* [in] */ Int32 last);

    static CARAPI_(Int32) LastIndexOf(
        /* [in] */ ICharSequence* s,
        /* [in] */ Char32 ch,
        /* [in] */ Int32 start,
        /* [in] */ Int32 last);

    static CARAPI_(Int32) IndexOf(
        /* [in] */ ICharSequence* s,
        /* [in] */ ICharSequence* needle);

    static CARAPI_(Int32) IndexOf(
        /* [in] */ ICharSequence* s,
        /* [in] */ ICharSequence* needle,
        /* [in] */ Int32 start);

    static CARAPI_(Int32) IndexOf(
        /* [in] */ ICharSequence* s,
        /* [in] */ ICharSequence* needle,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    static CARAPI_(Boolean) RegionMatches(
        /* [in] */ ICharSequence* one,
        /* [in] */ Int32 toffset,
        /* [in] */ ICharSequence* two,
        /* [in] */ Int32 ooffset,
        /* [in] */ Int32 len);

    /**
     * Create a new String object containing the given range of characters
     * from the source string.  This is different than simply calling
     * {@link CharSequence#subSequence(int, int) CharSequence.subSequence}
     * in that it does not preserve any style runs in the source sequence,
     * allowing a more efficient implementation.
     */
    static CARAPI_(String) Substring(
        /* [in] */ ICharSequence* source,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    /**
     * Returns list of multiple {@link CharSequence} joined into a single
     * {@link CharSequence} separated by localized delimiter such as ", ".
     *
     * @hide
     */
    //public static CharSequence join(Iterable<CharSequence> list) {
    static AutoPtr<ICharSequence> Join(
        /* [in] */ IIterable* list);

    /**
     * Returns a string containing the tokens joined by delimiters.
     * @param tokens an array objects to be joined. Strings will be formed from
     *     the objects by calling object.toString().
     */
    static CARAPI_(String) Join(
        /* [in] */ ICharSequence* delimiter,
        /* [in] */ ArrayOf<IInterface*>* tokens);

    /**
     * Returns a string containing the tokens joined by delimiters.
     * @param tokens an array objects to be joined. Strings will be formed from
     *     the objects by calling object.toString().
     */
    static CARAPI_(String) Join(
        /* [in] */ const String& delimiter,
        /* [in] */ ArrayOf<String>* tokens);

    static CARAPI_(String) Join(
        /* [in] */ const String& delimiter,
        /* [in] */ List<String>* tokens);

    /**
     * Returns a string containing the tokens joined by delimiters.
     * @param tokens an array objects to be joined. Strings will be formed from
     *     the objects by calling object.toString().
     */
    static CARAPI_(String) Join(
        /* [in] */ ICharSequence* delimiter,
        /* [in] */ IIterable* tokens);

    /**
     * String.split() returns [''] when the string to be split is empty. This returns []. This does
     * not remove any empty strings from the result. For example split("a,", ","  ) returns {"a", ""}.
     *
     * @param text the string to split
     * @param expression the regular expression to match
     * @return an array of strings. The array will be empty if text is empty
     *
     * @throws NullPointerException if expression or text is null
     */
    static AutoPtr<ArrayOf<String> > Split(
        /* [in] */ const String& text,
        /* [in] */ const String& expression);

    /**
     * Splits a string on a pattern. String.split() returns [''] when the string to be
     * split is empty. This returns []. This does not remove any empty strings from the result.
     * @param text the string to split
     * @param pattern the regular expression to match
     * @return an array of strings. The array will be empty if text is empty
     *
     * @throws NullPointerException if expression or text is null
     */
//    public static String[] split(String text, Pattern pattern);

    static AutoPtr<ArrayOf<String> > Split(
        /* [in] */ const String& text,
        /* [in] */ IPattern* pattern);

    static CARAPI_(AutoPtr<ICharSequence>) StringOrSpannedString(
        /* [in] */ ICharSequence* source);

    /**
     * Returns true if the string is null or 0-length.
     * @param str the string to be examined
     * @return true if str is null or zero length
     */
    static CARAPI_(Boolean) IsEmpty(
        /* [in] */ ICharSequence* str);

    static CARAPI_(Boolean) IsEmpty(
        /* [in] */ const String& str);

    /**
     * Returns the length that the specified CharSequence would have if
     * spaces and control characters were trimmed from the start and end,
     * as by {@link String#trim}.
     */
    static CARAPI_(Int32) GetTrimmedLength(
        /* [in] */ ICharSequence* s);

    static CARAPI_(Int32) GetTrimmedLength(
        /* [in] */ const String& str);

    /**
     * Returns true if a and b are equal, including if they are both null.
     * <p><i>Note: In platform versions 1.1 and earlier, this method only worked well if
     * both the arguments were instances of String.</i></p>
     * @param a first CharSequence to check
     * @param b second CharSequence to check
     * @return true if a and b are equal
     */
    static CARAPI_(Boolean) Equals(
        /* [in] */ ICharSequence* a,
        /* [in] */ ICharSequence* b);

    static CARAPI_(Boolean) Equals(
        /* [in] */ const String& a,
        /* [in] */ const String& b);

    // XXX currently this only reverses chars, not spans
    static CARAPI_(AutoPtr<ICharSequence>) GetReverse(
        /* [in] */ ICharSequence* source,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    /**
     * Flatten a CharSequence and whatever styles can be copied across processes
     * into the parcel.
     */
    static CARAPI WriteToParcel(
        /* [in] */ ICharSequence* cs,
        /* [in] */ IParcel* p);

//    public static final Parcelable.Creator<CharSequence> CHAR_SEQUENCE_CREATOR;

    /**
     * Debugging tool to print the spans in a CharSequence.  The output will
     * be printed one span per line.  If the CharSequence is not a Spanned,
     * then the entire string will be printed on a single line.
     */
//    public static void dumpSpans(CharSequence cs, Printer printer, String prefix);

    /**
     * Return a new CharSequence in which each of the source strings is
     * replaced by the corresponding element of the destinations.
     */
    static AutoPtr<ICharSequence> Replace(
        /* [in] */ ICharSequence* temp,
        /* [in] */ ArrayOf<String>* sources,
        /* [in] */ ArrayOf<ICharSequence*>* destinations);

    /**
     * Replace instances of "^1", "^2", etc. in the
     * <code>template</code> CharSequence with the corresponding
     * <code>values</code>.  "^^" is used to produce a single caret in
     * the output.  Only up to 9 replacement values are supported,
     * "^10" will be produce the first replacement value followed by a
     * '0'.
     *
     * @param template the input text containing "^1"-style
     * placeholder values.  This object is not modified; a copy is
     * returned.
     *
     * @param values CharSequences substituted into the template.  The
     * first is substituted for "^1", the second for "^2", and so on.
     *
     * @return the new CharSequence produced by doing the replacement
     *
     * @throws IllegalArgumentException if the template requests a
     * value that was not provided, or if more than 9 values are
     * provided.
     */
    static AutoPtr<ICharSequence> ExpandTemplate(
        /* [in] */ ICharSequence* temp,
        /* [in] */ ArrayOf<ICharSequence*>* values);

    static CARAPI_(Int32) GetOffsetBefore(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 offset);

    static CARAPI_(Int32) GetOffsetAfter(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 offset);

    /**
     * Copies the spans from the region <code>start...end</code> in
     * <code>source</code> to the region
     * <code>destoff...destoff+end-start</code> in <code>dest</code>.
     * Spans in <code>source</code> that begin before <code>start</code>
     * or end after <code>end</code> but overlap this range are trimmed
     * as if they began at <code>start</code> or ended at <code>end</code>.
     *
     * @throws IndexOutOfBoundsException if any of the copied spans
     * are out of range in <code>dest</code>.
     */
    static CARAPI CopySpansFrom(
        /* [in] */ ISpanned* source,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ const InterfaceID& kind,
        /* [in] */ ISpannable* dest,
        /* [in] */ Int32 destoff);

    /**
     * Returns the original text if it fits in the specified width
     * given the properties of the specified Paint,
     * or, if it does not fit, a truncated
     * copy with ellipsis character added at the specified edge or center.
     */
    static CARAPI_(AutoPtr<ICharSequence>) Ellipsize(
        /* [in] */ ICharSequence* text,
        /* [in] */ ITextPaint* p,
        /* [in] */ Float avail,
        /* [in] */ TextUtilsTruncateAt where);

    /**
     * Returns the original text if it fits in the specified width
     * given the properties of the specified Paint,
     * or, if it does not fit, a copy with ellipsis character added
     * at the specified edge or center.
     * If <code>preserveLength</code> is specified, the returned copy
     * will be padded with zero-width spaces to preserve the original
     * length and offsets instead of truncating.
     * If <code>callback</code> is non-null, it will be called to
     * report the start and end of the ellipsized range.  TextDirection
     * is determined by the first strong directional character.
     */
    static CARAPI_(AutoPtr<ICharSequence>) Ellipsize(
        /* [in] */ ICharSequence* text,
        /* [in] */ ITextPaint* paint,
        /* [in] */ Float avail,
        /* [in] */ TextUtilsTruncateAt where,
        /* [in] */ Boolean preserveLength,
        /* [in] */ ITextUtilsEllipsizeCallback* callback);

    /**
     * Returns the original text if it fits in the specified width
     * given the properties of the specified Paint,
     * or, if it does not fit, a copy with ellipsis character added
     * at the specified edge or center.
     * If <code>preserveLength</code> is specified, the returned copy
     * will be padded with zero-width spaces to preserve the original
     * length and offsets instead of truncating.
     * If <code>callback</code> is non-null, it will be called to
     * report the start and end of the ellipsized range.
     *
     * @hide
     */
   static CARAPI_(AutoPtr<ICharSequence>) Ellipsize(
       /* [in] */ ICharSequence* text,
       /* [in] */ ITextPaint* paint,
       /* [in] */ Float avail,
       /* [in] */ TextUtilsTruncateAt where,
       /* [in] */ Boolean preserveLength,
       /* [in] */ ITextUtilsEllipsizeCallback* callback,
       /* [in] */ ITextDirectionHeuristic* textDir,
       /* [in] */ const String& ellipsis);

    /**
     * Converts a CharSequence of the comma-separated form "Andy, Bob,
     * Charles, David" that is too wide to fit into the specified width
     * into one like "Andy, Bob, 2 more".
     *
     * @param text the text to truncate
     * @param p the Paint with which to measure the text
     * @param avail the horizontal width available for the text
     * @param oneMore the string for "1 more" in the current locale
     * @param more the string for "%d more" in the current locale
     */
    static AutoPtr<ICharSequence> CommaEllipsize(
        /* [in] */ ICharSequence* text,
        /* [in] */ ITextPaint* p,
        /* [in] */ Float avail,
        /* [in] */ const String& oneMore,
        /* [in] */ const String& more);

    /**
     * @hide
     */
    static AutoPtr<ICharSequence> CommaEllipsize(
        /* [in] */ ICharSequence* text,
        /* [in] */ ITextPaint* p,
        /* [in] */ Float avail,
        /* [in] */ const String& oneMore,
        /* [in] */ const String& more,
        /* [in] */ ITextDirectionHeuristic* textDir);

    /* package */
    static CARAPI_(Boolean) DoesNotNeedBidi(
        /* [ini] */ ICharSequence* s,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    /* package */
    static CARAPI_(Boolean) DoesNotNeedBidi(
        /* [in] */ ArrayOf<Char32>* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 len);

    /* package */
    static CARAPI_(AutoPtr< ArrayOf<Char32> >) Obtain(
        /* [in] */ Int32 len);

    /* package */
    static CARAPI_(void) Recycle(
        /* [in] */ ArrayOf<Char32>* temp);

    /**
     * Html-encode the string.
     * @param s the string to be encoded
     * @return the encoded string
     */
    static String HtmlEncode(
        /* [in] */ const String& s);

    /**
     * Returns a CharSequence concatenating the specified CharSequences,
     * retaining their spans if any.
     */
    static CARAPI_(AutoPtr<ICharSequence>) Concat(
        /* [in] */ ArrayOf<ICharSequence*>* text);

    /**
     * Returns whether the given CharSequence contains any printable characters.
     */
    static Boolean IsGraphic(
        /* [in] */ ICharSequence* str);

    /**
     * Returns whether this character is a printable character.
     */
    static Boolean IsGraphic(
        /* [in] */ Char32 c);
    /**
     * Returns whether the given CharSequence contains only digits.
     */
    static Boolean IsDigitsOnly(
        /* [in] */ ICharSequence* str);

    /**
     * @hide
     */
    static Boolean IsPrintableAscii(
        /* [in] */ Char32 c);

    /**
     * @hide
     */
    static Boolean IsPrintableAsciiOnly(
        /* [in] */ ICharSequence* str);

    /**
     * Determine what caps mode should be in effect at the current offset in
     * the text.  Only the mode bits set in <var>reqModes</var> will be
     * checked.  Note that the caps mode flags here are explicitly defined
     * to match those in {@link InputType}.
     *
     * @param cs The text that should be checked for caps modes.
     * @param off Location in the text at which to check.
     * @param reqModes The modes to be checked: may be any combination of
     * {@link #CAP_MODE_CHARACTERS}, {@link #CAP_MODE_WORDS}, and
     * {@link #CAP_MODE_SENTENCES}.
     *
     * @return Returns the actual capitalization modes that can be in effect
     * at the current position, which is any combination of
     * {@link #CAP_MODE_CHARACTERS}, {@link #CAP_MODE_WORDS}, and
     * {@link #CAP_MODE_SENTENCES}.
     */
    static CARAPI_(Int32) GetCapsMode(
        /* [in] */ ICharSequence* cs,
        /* [in] */ Int32 off,
        /* [in] */ Int32 reqModes);

    /**
     * Does a comma-delimited list 'delimitedString' contain a certain item?
     * (without allocating memory)
     *
     * @hide
     */
    static CARAPI_(Boolean) DelimitedStringContains(
        /* [in] */ const String& delimitedString,
        /* [in] */ Char32 delimiter,
        /* [in] */ const String& item);

    /**
     * Removes empty spans from the <code>spans</code> array.
     *
     * When parsing a Spanned using {@link Spanned#nextSpanTransition(int, int, Class)}, empty spans
     * will (correctly) create span transitions, and calling getSpans on a slice of text bounded by
     * one of these transitions will (correctly) include the empty overlapping span.
     *
     * However, these empty spans should not be taken into account when layouting or rendering the
     * string and this method provides a way to filter getSpans' results accordingly.
     *
     * @param spans A list of spans retrieved using {@link Spanned#getSpans(int, int, Class)} from
     * the <code>spanned</code>
     * @param spanned The Spanned from which spans were extracted
     * @return A subset of spans where empty spans ({@link Spanned#getSpanStart(Object)}  ==
     * {@link Spanned#getSpanEnd(Object)} have been removed. The initial order is preserved
     * @hide
     */
    static CARAPI_(AutoPtr< ArrayOf<IInterface*> >) RemoveEmptySpans(
        /* [in] */ ArrayOf<IInterface*>* spans,
        /* [in] */ ISpanned* spanned,
        /* [in] */ const InterfaceID& klass);

    /**
     * Pack 2 int values into a long, useful as a return value for a range
     * @see #unpackRangeStartFromLong(long)
     * @see #unpackRangeEndFromLong(long)
     * @hide
     */
    static CARAPI_(Int64) PackRangeInInt64(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    /**
     * Get the start value from a range packed in a long by {@link #packRangeInLong(int, int)}
     * @see #unpackRangeEndFromLong(long)
     * @see #packRangeInLong(int, int)
     * @hide
     */
    static CARAPI_(Int32) UnpackRangeStartFromInt64(
        /* [in] */ Int64 range);

    /**
     * Get the end value from a range packed in a long by {@link #packRangeInLong(int, int)}
     * @see #unpackRangeStartFromLong(long)
     * @see #packRangeInLong(int, int)
     * @hide
     */
    static CARAPI_(Int32) UnpackRangeEndFromInt64(
        /* [in] */ Int64 range);

    /**
     * Return the layout direction for a given Locale
     *
     * @param locale the Locale for which we want the layout direction. Can be null.
     * @return the layout direction. This may be one of:
     * {@link android.view.View#LAYOUT_DIRECTION_LTR} or
     * {@link android.view.View#LAYOUT_DIRECTION_RTL}.
     *
     * Be careful: this code will need to be updated when vertical scripts will be supported
     */
    static CARAPI_(Int32) GetLayoutDirectionFromLocale(
        /* [in] */ ILocale* locale);

public:
//    /**
//     * Capitalization mode for {@link #getCapsMode}: capitalize all
//     * characters.  This value is explicitly defined to be the same as
//     * {@link InputType#TYPE_TEXT_FLAG_CAP_CHARACTERS}.
//     */
//    static const Int32 CAP_MODE_CHARACTERS
//            = InputType_TYPE_TEXT_FLAG_CAP_CHARACTERS;
//
//    /**
//     * Capitalization mode for {@link #getCapsMode}: capitalize the first
//     * character of all words.  This value is explicitly defined to be the same as
//     * {@link InputType#TYPE_TEXT_FLAG_CAP_WORDS}.
//     */
//    static const Int32 CAP_MODE_WORDS
//            = InputType_TYPE_TEXT_FLAG_CAP_WORDS;
//
//    /**
//     * Capitalization mode for {@link #getCapsMode}: capitalize the first
//     * character of each sentence.  This value is explicitly defined to be the same as
//     * {@link InputType#TYPE_TEXT_FLAG_CAP_SENTENCES}.
//     */
//    static const Int32 CAP_MODE_SENTENCES
//            = InputType_TYPE_TEXT_FLAG_CAP_SENTENCES;

private:
    ECO_LOCAL TextUtils();
    ECO_LOCAL TextUtils(const TextUtils& other);

    ECO_LOCAL static CARAPI WriteWhere(
        /* [in] */ IParcel* p,
        /* [in] */ ISpanned* sp,
        /* [in] */ IInterface* o);

    ECO_LOCAL static CARAPI ReadSpan(
        /* [in] */ IParcel* p,
        /* [in] */ ISpannable* sp,
        /* [in] */ IInterface* o);

    ECO_LOCAL static CARAPI_(Float) SetPara(
        /* [in] */ MeasuredText* mt,
        /* [in] */ ITextPaint* paint,
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ ITextDirectionHeuristic* textDir);

    /**
     * Fallback algorithm to detect the locale direction. Rely on the fist char of the
     * localized locale name. This will not work if the localized locale name is in English
     * (this is the case for ICU 4.4 and "Urdu" script)
     *
     * @param locale
     * @return the layout direction. This may be one of:
     * {@link View#LAYOUT_DIRECTION_LTR} or
     * {@link View#LAYOUT_DIRECTION_RTL}.
     *
     * Be careful: this code will need to be updated when vertical scripts will be supported
     *
     * @hide
     */
    ECO_LOCAL static CARAPI_(Int32) GetLayoutDirectionFromFirstChar(
        /* [in] */ ILocale* locale);

private:
    static const String TAG;// = "TextUtils";

    static const Char32 FIRST_RIGHT_TO_LEFT = 0x0590;//'\u0590';

    static Object sLock;

    static AutoPtr< ArrayOf<Char32> > sTemp;

    static AutoPtr< ArrayOf<String> > EMPTY_STRING_ARRAY;

    static const Char32 ZWNBS_CHAR = 0xFEFF;//'\uFEFF';

    static String ARAB_SCRIPT_SUBTAG;
    static String HEBR_SCRIPT_SUBTAG;
};

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TEXT_TEXTUTILS_H__
