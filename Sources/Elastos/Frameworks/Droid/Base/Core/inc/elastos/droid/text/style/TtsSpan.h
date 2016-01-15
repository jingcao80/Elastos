#ifndef __ELASTOS_DROID_TEXT_STYLE_TTSSPAN_H__
#define __ELASTOS_DROID_TEXT_STYLE_TTSSPAN_H__

#include "Elastos.Droid.Text.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IPersistableBundle;
using Elastos::Droid::Text::IParcelableSpan;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

/**
 * A span that supplies additional meta-data for the associated text intended
 * for text-to-speech engines. If the text is being processed by a
 * text-to-speech engine, the engine may use the data in this span in addition
 * to or instead of its associated text.
 *
 * Each instance of a TtsSpan has a type, for example {@link #TYPE_DATE}
 * or {@link #TYPE_MEASURE}. And a list of arguments, provided as
 * key-value pairs in a bundle.
 *
 * The inner classes are there for convenience and provide builders for each
 * TtsSpan type.
 */
class TtsSpan
    : public Object
    , public ITtsSpan
    , public IParcelableSpan
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    TtsSpan();

    virtual ~TtsSpan();

    CARAPI constructor();


    CARAPI constructor(
        /* [in] */ const String& type,
        /* [in] */ IPersistableBundle* args);

    /**
     * Returns the type.
     * @return The type of this instance.
     */
    CARAPI GetType(
        /* [out] */ String* type);

    /**
     * Returns a bundle of the arguments set.
     * @return The bundle of the arguments set.
     */
    CARAPI GetArgs(
        /* [out] */ IPersistableBundle** source);

    CARAPI GetSpanTypeId(
        /* [out] */ Int32* id);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    String mType;
    AutoPtr<IPersistableBundle> mArgs;
};

//assert(0 & "TODO");

// /**
//  * A simple builder for TtsSpans.
//  * This builder can be used directly, but the more specific subclasses of
//  * this builder like {@link TtsSpan.TextBuilder} and
//  * {@link TtsSpan.CardinalBuilder} are likely more useful.
//  *
//  * This class uses generics so methods from this class can return instances
//  * of its child classes, resulting in a fluent API (CRTP pattern).
//  */
// public static class Builder<C extends Builder<?>> {
//     // Holds the type of this class.
//     private final String mType;

//     // Holds the arguments of this class. It only stores objects of type
//     // String, Integer and Long.
//     private PersistableBundle mArgs = new PersistableBundle();

//     public Builder(String type) {
//         mType = type;
//     }

//     /**
//      * Returns a TtsSpan built from the parameters set by the setter
//      * methods.
//      * @return A TtsSpan built with parameters of this builder.
//      */
//     public TtsSpan build() {
//         return new TtsSpan(mType, mArgs);
//     }

//     /**
//      * Sets an argument to a string value.
//      * @param arg The argument name.
//      * @param value The value the argument should be set to.
//      * @return This instance.
//      */
//     @SuppressWarnings("unchecked")
//     public C setStringArgument(String arg, String value) {
//         mArgs.putString(arg, value);
//         return (C) this;
//     }

//     /**
//      * Sets an argument to an int value.
//      * @param arg The argument name.
//      * @param value The value the argument should be set to.
//      */
//     @SuppressWarnings("unchecked")
//     public C setIntArgument(String arg, int value) {
//         mArgs.putInt(arg, value);
//         return (C) this;
//     }

//     /**
//      * Sets an argument to a long value.
//      * @param arg The argument name.
//      * @param value The value the argument should be set to.
//      */
//     @SuppressWarnings("unchecked")
//     public C setLongArgument(String arg, long value) {
//         mArgs.putLong(arg, value);
//         return (C) this;
//     }
// }

// /**
//  * A builder for TtsSpans, has setters for morphosyntactic features.
//  * This builder can be used directly, but the more specific subclasses of
//  * this builder like {@link TtsSpan.TextBuilder} and
//  * {@link TtsSpan.CardinalBuilder} are likely more useful.
//  */
// public static class SemioticClassBuilder<C extends SemioticClassBuilder<?>>
//         extends Builder<C> {

//     public SemioticClassBuilder(String type) {
//         super(type);
//     }

//     /**
//      * Sets the gender information for this instance.
//      * @param gender Can any of {@link #GENDER_NEUTRAL},
//      *     {@link #GENDER_MALE} and {@link #GENDER_FEMALE}.
//      * @return This instance.
//      */
//     public C setGender(String gender) {
//         return setStringArgument(TtsSpan.ARG_GENDER, gender);
//     }

//     /**
//      * Sets the animacy information for this instance.
//      * @param animacy Can be any of {@link #ANIMACY_ANIMATE} and
//      *     {@link #ANIMACY_INANIMATE}.
//      * @return This instance.
//      */
//     public C setAnimacy(String animacy) {
//         return setStringArgument(TtsSpan.ARG_ANIMACY, animacy);
//     }

//     /**
//      * Sets the multiplicity information for this instance.
//      * @param multiplicity Can be any of
//      *     {@link #MULTIPLICITY_SINGLE}, {@link #MULTIPLICITY_DUAL} and
//      *     {@link #MULTIPLICITY_PLURAL}.
//      * @return This instance.
//      */
//     public C setMultiplicity(String multiplicity) {
//         return setStringArgument(TtsSpan.ARG_MULTIPLICITY, multiplicity);
//     }

//     /**
//      * Sets the grammatical case information for this instance.
//      * @param grammaticalCase Can be any of {@link #CASE_NOMINATIVE},
//      *     {@link #CASE_ACCUSATIVE}, {@link #CASE_DATIVE},
//      *     {@link #CASE_ABLATIVE}, {@link #CASE_GENITIVE},
//      *     {@link #CASE_VOCATIVE}, {@link #CASE_LOCATIVE} and
//      *     {@link #CASE_INSTRUMENTAL}.
//      * @return This instance.
//      */
//     public C setCase(String grammaticalCase) {
//         return setStringArgument(TtsSpan.ARG_CASE, grammaticalCase);
//     }
// }

// /**
//  * A builder for TtsSpans of type {@link #TYPE_TEXT}.
//  */
// public static class TextBuilder extends SemioticClassBuilder<TextBuilder> {

//     /**
//      * Creates a builder for a TtsSpan of type {@link #TYPE_TEXT}.
//      */
//     public TextBuilder() {
//         super(TtsSpan.TYPE_TEXT);
//     }

//     /**
//      * Creates a TtsSpan of type {@link #TYPE_TEXT} and sets the
//      * {@link #ARG_TEXT} argument.
//      * @param text The text to be synthesized.
//      * @see #setText(String)
//      */
//     public TextBuilder(String text) {
//         this();
//         setText(text);
//     }

//     /**
//      * Sets the {@link #ARG_TEXT} argument, the text to be synthesized.
//      * @param text The string that will be synthesized.
//      * @return This instance.
//      */
//     public TextBuilder setText(String text) {
//         return setStringArgument(TtsSpan.ARG_TEXT, text);
//     }
// }

// /**
//  * A builder for TtsSpans of type {@link #TYPE_CARDINAL}.
//  */
// public static class CardinalBuilder
//         extends SemioticClassBuilder<CardinalBuilder> {

//     /**
//      * Creates a builder for a TtsSpan of type {@link #TYPE_CARDINAL}.
//      */
//     public CardinalBuilder() {
//         super(TtsSpan.TYPE_CARDINAL);
//     }

//     /**
//      * Creates a TtsSpan of type {@link #TYPE_CARDINAL} and sets the
//      * {@link #ARG_NUMBER} argument.
//      * @param number The number to synthesize.
//      * @see #setNumber(long)
//      */
//     public CardinalBuilder(long number) {
//         this();
//         setNumber(number);
//     }

//     /**
//      * Creates a TtsSpan of type {@link #TYPE_CARDINAL} and sets the
//      * {@link #ARG_NUMBER} argument.
//      * @param number The number to synthesize.
//      * @see #setNumber(String)
//      */
//     public CardinalBuilder(String number) {
//         this();
//         setNumber(number);
//     }

//     /**
//      * Convenience method that converts the number to a String and set it to
//      * the value for {@link #ARG_NUMBER}.
//      * @param number The number that will be synthesized.
//      * @return This instance.
//      */
//     public CardinalBuilder setNumber(long number) {
//         return setNumber(String.valueOf(number));
//     }

//     /**
//      * Sets the {@link #ARG_NUMBER} argument.
//      * @param number A non-empty string of digits with an optional
//      *     leading + or -.
//      * @return This instance.
//      */
//     public CardinalBuilder setNumber(String number) {
//         return setStringArgument(TtsSpan.ARG_NUMBER, number);
//     }
// }

// /**
//  * A builder for TtsSpans of type {@link #TYPE_ORDINAL}.
//  */
// public static class OrdinalBuilder
//         extends SemioticClassBuilder<OrdinalBuilder> {

//     /**
//      * Creates a builder for a TtsSpan of type {@link #TYPE_ORDINAL}.
//      */
//     public OrdinalBuilder() {
//         super(TtsSpan.TYPE_ORDINAL);
//     }

//     /**
//      * Creates a TtsSpan of type {@link #TYPE_ORDINAL} and sets the
//      * {@link #ARG_NUMBER} argument.
//      * @param number The ordinal number to synthesize.
//      * @see #setNumber(long)
//      */
//     public OrdinalBuilder(long number) {
//         this();
//         setNumber(number);
//     }

//     /**
//      * Creates a TtsSpan of type {@link #TYPE_ORDINAL} and sets the
//      * {@link #ARG_NUMBER} argument.
//      * @param number The number to synthesize.
//      * @see #setNumber(String)
//      */
//     public OrdinalBuilder(String number) {
//         this();
//         setNumber(number);
//     }

//     /**
//      * Convenience method that converts the number to a String and sets it
//      * to the value for {@link #ARG_NUMBER}.
//      * @param number The ordinal number that will be synthesized.
//      * @return This instance.
//      */
//     public OrdinalBuilder setNumber(long number) {
//         return setNumber(String.valueOf(number));
//     }

//     /**
//      * Sets the {@link #ARG_NUMBER} argument.
//      * @param number A non-empty string of digits with an optional
//      *     leading + or -.
//      * @return This instance.
//      */
//     public OrdinalBuilder setNumber(String number) {
//         return setStringArgument(TtsSpan.ARG_NUMBER, number);
//     }
// }

// /**
//  * A builder for TtsSpans of type {@link #TYPE_DECIMAL}.
//  */
// public static class DecimalBuilder
//         extends SemioticClassBuilder<DecimalBuilder> {

//     /**
//      * Creates a builder for a TtsSpan of type {@link #TYPE_DECIMAL}.
//      */
//     public DecimalBuilder() {
//         super(TtsSpan.TYPE_DECIMAL);
//     }

//     /**
//      * Creates a TtsSpan of type {@link #TYPE_DECIMAL} and sets the
//      * {@link #ARG_INTEGER_PART} and {@link #ARG_FRACTIONAL_PART} arguments.
//      * @see {@link #setArgumentsFromDouble(double, int, int)
//      */
//     public DecimalBuilder(double number,
//                           int minimumFractionDigits,
//                           int maximumFractionDigits) {
//         this();
//         setArgumentsFromDouble(number,
//                                minimumFractionDigits,
//                                maximumFractionDigits);
//     }

//     /**
//      * Creates a TtsSpan of type {@link #TYPE_DECIMAL} and sets the
//      * {@link #ARG_INTEGER_PART} and {@link #ARG_FRACTIONAL_PART} arguments.
//      */
//     public DecimalBuilder(String integerPart, String fractionalPart) {
//         this();
//         setIntegerPart(integerPart);
//         setFractionalPart(fractionalPart);
//     }

//     /**
//      * Convenience method takes a double and a maximum number of fractional
//      * digits, it sets the {@link #ARG_INTEGER_PART} and
//      * {@link #ARG_FRACTIONAL_PART} arguments.
//      * @param number The number to be synthesized.
//      * @param minimumFractionDigits The minimum number of fraction digits
//      *     that are pronounced.
//      * @param maximumFractionDigits The maximum number of fraction digits
//      *     that are pronounced. If maximumFractionDigits <
//      *     minimumFractionDigits then minimumFractionDigits will be assumed
//      *     to be equal to maximumFractionDigits.
//      * @return This instance.
//      */
//     public DecimalBuilder setArgumentsFromDouble(
//             double number,
//             int minimumFractionDigits,
//             int maximumFractionDigits) {
//         // Format double.
//         NumberFormat formatter = NumberFormat.getInstance(Locale.US);
//         formatter.setMinimumFractionDigits(maximumFractionDigits);
//         formatter.setMaximumFractionDigits(maximumFractionDigits);
//         formatter.setGroupingUsed(false);
//         String str = formatter.format(number);

//         // Split at decimal point.
//         int i = str.indexOf('.');
//         if (i >= 0) {
//             setIntegerPart(str.substring(0, i));
//             setFractionalPart(str.substring(i + 1));
//         } else {
//             setIntegerPart(str);
//         }
//         return this;
//     }

//     /**
//      * Convenience method that converts the number to a String and sets it
//      * to the value for {@link #ARG_INTEGER_PART}.
//      * @param integerPart The integer part of the decimal.
//      * @return This instance.
//      */
//     public DecimalBuilder setIntegerPart(long integerPart) {
//         return setIntegerPart(String.valueOf(integerPart));
//     }

//     /**
//      * Sets the {@link #ARG_INTEGER_PART} argument.
//      * @param integerPart A non-empty string of digits with an optional
//      *     leading + or -.
//      * @return This instance.
//      */
//     public DecimalBuilder setIntegerPart(String integerPart) {
//         return setStringArgument(TtsSpan.ARG_INTEGER_PART, integerPart);
//     }

//     /**
//      * Sets the {@link #ARG_FRACTIONAL_PART} argument.
//      * @param fractionalPart A non-empty string of digits.
//      * @return This instance.
//      */
//     public DecimalBuilder setFractionalPart(String fractionalPart) {
//         return setStringArgument(TtsSpan.ARG_FRACTIONAL_PART,
//                                  fractionalPart);
//     }
// }

// /**
//  * A builder for TtsSpans of type {@link #TYPE_FRACTION}.
//  */
// public static class FractionBuilder
//         extends SemioticClassBuilder<FractionBuilder> {

//     /**
//      * Creates a builder for a TtsSpan of type {@link #TYPE_FRACTION}.
//      */
//     public FractionBuilder() {
//         super(TtsSpan.TYPE_FRACTION);
//     }

//     /**
//      * Creates a TtsSpan of type {@link #TYPE_FRACTION} and sets the
//      * {@link #ARG_INTEGER_PART}, {@link #ARG_NUMERATOR}, and
//      * {@link #ARG_DENOMINATOR} arguments.
//      */
//     public FractionBuilder(long integerPart,
//                            long numerator,
//                            long denominator) {
//         this();
//         setIntegerPart(integerPart);
//         setNumerator(numerator);
//         setDenominator(denominator);
//     }

//     /**
//      * Convenience method that converts the integer to a String and sets the
//      * argument {@link #ARG_NUMBER}.
//      * @param integerPart The integer part.
//      * @return This instance.
//      */
//     public FractionBuilder setIntegerPart(long integerPart) {
//         return setIntegerPart(String.valueOf(integerPart));
//     }

//     /**
//      * Sets the {@link #ARG_INTEGER_PART} argument.
//      * @param integerPart A non-empty string of digits with an optional
//      *     leading + or -.
//      * @return This instance.
//      */
//     public FractionBuilder setIntegerPart(String integerPart) {
//         return setStringArgument(TtsSpan.ARG_INTEGER_PART, integerPart);
//     }

//     /**
//      * Convenience method that converts the numerator to a String and sets
//      * the argument {@link #ARG_NUMERATOR}.
//      * @param numerator The numerator.
//      * @return This instance.
//      */
//     public FractionBuilder setNumerator(long numerator) {
//         return setNumerator(String.valueOf(numerator));
//     }

//     /**
//      * Sets the {@link #ARG_NUMERATOR} argument.
//      * @param numerator A non-empty string of digits with an optional
//      *     leading + or -.
//      * @return This instance.
//      */
//     public FractionBuilder setNumerator(String numerator) {
//         return setStringArgument(TtsSpan.ARG_NUMERATOR, numerator);
//     }

//     /**
//      * Convenience method that converts the denominator to a String and sets
//      * the argument {@link #ARG_DENOMINATOR}.
//      * @param denominator The denominator.
//      * @return This instance.
//      */
//     public FractionBuilder setDenominator(long denominator) {
//         return setDenominator(String.valueOf(denominator));
//     }

//     /**
//      * Sets the {@link #ARG_DENOMINATOR} argument.
//      * @param denominator A non-empty string of digits with an optional
//      *     leading + or -.
//      * @return This instance.
//      */
//     public FractionBuilder setDenominator(String denominator) {
//         return setStringArgument(TtsSpan.ARG_DENOMINATOR, denominator);
//     }
// }

// /**
//  * A builder for TtsSpans of type {@link #TYPE_MEASURE}.
//  */
// public static class MeasureBuilder
//         extends SemioticClassBuilder<MeasureBuilder> {

//     /**
//      * Creates a builder for a TtsSpan of type {@link #TYPE_MEASURE}.
//      */
//     public MeasureBuilder() {
//         super(TtsSpan.TYPE_MEASURE);
//     }

//     *
//      * Convenience method that converts the number to a String and set it to
//      * the value for {@link #ARG_NUMBER}.
//      * @param number The amount of the measure.
//      * @return This instance.

//     public MeasureBuilder setNumber(long number) {
//         return setNumber(String.valueOf(number));
//     }

//     /**
//      * Sets the {@link #ARG_NUMBER} argument.
//      * @param number A non-empty string of digits with an optional
//      *     leading + or -.
//      * @return This instance.
//      */
//     public MeasureBuilder setNumber(String number) {
//         return setStringArgument(TtsSpan.ARG_NUMBER, number);
//     }

//     /**
//      * Convenience method that converts the integer part to a String and set
//      * it to the value for {@link #ARG_INTEGER_PART}.
//      * @param integerPart The integer part of a decimal or fraction.
//      * @return This instance.
//      */
//     public MeasureBuilder setIntegerPart(long integerPart) {
//         return setNumber(String.valueOf(integerPart));
//     }

//     /**
//      * Sets the {@link #ARG_INTEGER_PART} argument.
//      * @param integerPart The integer part of a decimal or fraction; a
//      * non-empty string of digits with an optional
//      *     leading + or -.
//      * @return This instance.
//      */
//     public MeasureBuilder setIntegerPart(String integerPart) {
//         return setStringArgument(TtsSpan.ARG_INTEGER_PART, integerPart);
//     }

//     /**
//      * Sets the {@link #ARG_FRACTIONAL_PART} argument.
//      * @param fractionalPart The fractional part of a decimal; a non-empty
//      *     string of digits with an optional leading + or -.
//      * @return This instance.
//      */
//     public MeasureBuilder setFractionalPart(String fractionalPart) {
//         return setStringArgument(TtsSpan.ARG_FRACTIONAL_PART,
//                                  fractionalPart);
//     }

//     /**
//      * Convenience method that converts the numerator to a String and set it
//      * to the value for {@link #ARG_NUMERATOR}.
//      * @param numerator The numerator of a fraction.
//      * @return This instance.
//      */
//     public MeasureBuilder setNumerator(long numerator) {
//         return setNumerator(String.valueOf(numerator));
//     }

//     /**
//      * Sets the {@link #ARG_NUMERATOR} argument.
//      * @param numerator The numerator of a fraction; a non-empty string of
//      *     digits with an optional leading + or -.
//      * @return This instance.
//      */
//     public MeasureBuilder setNumerator(String numerator) {
//         return setStringArgument(TtsSpan.ARG_NUMERATOR, numerator);
//     }

//     /**
//      * Convenience method that converts the denominator to a String and set
//      * it to the value for {@link #ARG_DENOMINATOR}.
//      * @param denominator The denominator of a fraction.
//      * @return This instance.
//      */
//     public MeasureBuilder setDenominator(long denominator) {
//         return setDenominator(String.valueOf(denominator));
//     }

//     /**
//      * Sets the {@link #ARG_DENOMINATOR} argument.
//      * @param denominator The denominator of a fraction; a non-empty string
//      *     of digits with an optional leading + or -.
//      * @return This instance.
//      */
//     public MeasureBuilder setDenominator(String denominator) {
//         return setStringArgument(TtsSpan.ARG_DENOMINATOR, denominator);
//     }

//     /**
//      * Sets the {@link #ARG_UNIT} argument.
//      * @param unit The unit of the measure.
//      * @return This instance.
//      * @see {@link TtsSpan.ARG_UNIT}
//      */
//     public MeasureBuilder setUnit(String unit) {
//         return setStringArgument(TtsSpan.ARG_UNIT, unit);
//     }
// }

// /**
//  * A builder for TtsSpans of type {@link #TYPE_TIME}.
//  */
// public static class TimeBuilder
//         extends SemioticClassBuilder<TimeBuilder> {

//     /**
//      * Creates a builder for a TtsSpan of type {@link #TYPE_TIME}.
//      */
//     public TimeBuilder() {
//         super(TtsSpan.TYPE_TIME);
//     }

//     /**
//      * Creates a builder for a TtsSpan of type {@link #TYPE_TIME} and
//      * sets the {@link #ARG_HOURS} and {@link #ARG_MINUTES} arguments.
//      */
//     public TimeBuilder(int hours, int minutes) {
//         this();
//         setHours(hours);
//         setMinutes(minutes);
//     }

//     /**
//      * Sets the {@link #ARG_HOURS} argument.
//      * @param hours The value to be set for hours. See {@link #ARG_HOURS}.
//      * @return This instance.
//      * @see {@link #ARG_HOURS}
//      */
//     public TimeBuilder setHours(int hours) {
//         return setIntArgument(TtsSpan.ARG_HOURS, hours);
//     }

//     /**
//      * Sets the {@link #ARG_MINUTES} argument.
//      * @param minutes The value to be set for minutes. See
//      *     {@link #ARG_MINUTES}.
//      * @return This instance.
//      * @see {@link #ARG_MINUTES}
//      */
//     public TimeBuilder setMinutes(int minutes) {
//         return setIntArgument(TtsSpan.ARG_MINUTES, minutes);
//     }
// }

// /**
//  * A builder for TtsSpans of type {@link #TYPE_DATE}.
//  */
// public static class DateBuilder
//         extends SemioticClassBuilder<DateBuilder> {

//     /**
//      * Creates a builder for a TtsSpan of type {@link #TYPE_DATE}.
//      */
//     public DateBuilder() {
//         super(TtsSpan.TYPE_DATE);
//     }

//     /**
//      * Creates a builder for a TtsSpan of type {@link #TYPE_TIME} and
//      * possibly sets the {@link #ARG_WEEKDAY}, {@link #ARG_DAY},
//      * {@link #ARG_MONTH} and {@link #ARG_YEAR} arguments. Pass null to any
//      * argument to leave it unset.
//      */
//     public DateBuilder(Integer weekday,
//                        Integer day,
//                        Integer month,
//                        Integer year) {
//         this();
//         if (weekday != null) {
//             setWeekday(weekday);
//         }
//         if (day != null) {
//             setDay(day);
//         }
//         if (month != null) {
//             setMonth(month);
//         }
//         if (year != null) {
//             setYear(year);
//         }
//     }

//     /**
//      * Sets the {@link #ARG_WEEKDAY} argument.
//      * @param weekday The value to be set for weekday. See
//      *     {@link #ARG_WEEKDAY}.
//      * @return This instance.
//      * @see {@link #ARG_WEEKDAY}
//      */
//     public DateBuilder setWeekday(int weekday) {
//         return setIntArgument(TtsSpan.ARG_WEEKDAY, weekday);
//     }

//     /**
//      * Sets the {@link #ARG_DAY} argument.
//      * @param day The value to be set for day. See {@link #ARG_DAY}.
//      * @return This instance.
//      * @see {@link #ARG_DAY}
//      */
//     public DateBuilder setDay(int day) {
//         return setIntArgument(TtsSpan.ARG_DAY, day);
//     }

//     /**
//      * Sets the {@link #ARG_MONTH} argument.
//      * @param month The value to be set for month. See {@link #ARG_MONTH}.
//      * @return This instance.
//      * @see {@link #ARG_MONTH}
//      */
//     public DateBuilder setMonth(int month) {
//         return setIntArgument(TtsSpan.ARG_MONTH, month);
//     }

//     /**
//      * Sets the {@link #ARG_YEAR} argument.
//      * @param year The value to be set for year. See {@link #ARG_YEAR}.
//      * @return This instance.
//      * @see {@link #ARG_YEAR}
//      */
//     public DateBuilder setYear(int year) {
//         return setIntArgument(TtsSpan.ARG_YEAR, year);
//     }
// }

// /**
//  * A builder for TtsSpans of type {@link #TYPE_MONEY}.
//  */
// public static class MoneyBuilder
//         extends SemioticClassBuilder<MoneyBuilder> {

//     /**
//      * Creates a TtsSpan of type {@link #TYPE_MONEY}.
//      */
//     public MoneyBuilder() {
//         super(TtsSpan.TYPE_MONEY);
//     }

//     /**
//      * Convenience method that converts the number to a String and set it to
//      * the value for {@link #ARG_INTEGER_PART}.
//      * @param integerPart The integer part of the amount.
//      * @return This instance.
//      */
//     public MoneyBuilder setIntegerPart(long integerPart) {
//         return setIntegerPart(String.valueOf(integerPart));
//     }

//     /**
//      * Sets the {@link #ARG_INTEGER_PART} argument.
//      * @param integerPart A non-empty string of digits with an optional
//      *     leading + or -.
//      * @return This instance.
//      */
//     public MoneyBuilder setIntegerPart(String integerPart) {
//         return setStringArgument(TtsSpan.ARG_INTEGER_PART, integerPart);
//     }

//     /**
//      * Sets the {@link #ARG_FRACTIONAL_PART} argument.
//      * @param fractionalPart Can be a string of digits of any size.
//      * @return This instance.
//      */
//     public MoneyBuilder setFractionalPart(String fractionalPart) {
//         return setStringArgument(TtsSpan.ARG_FRACTIONAL_PART, fractionalPart);
//     }

//     /**
//      * Sets the {@link #ARG_CURRENCY} argument.
//      * @param currency Should be a ISO4217 currency code, e.g. "USD".
//      * @return This instance.
//      */
//     public MoneyBuilder setCurrency(String currency) {
//         return setStringArgument(TtsSpan.ARG_CURRENCY, currency);
//     }

//     /**
//      * Sets the {@link #ARG_QUANTITY} argument.
//      * @param quantity
//      * @return This instance.
//      */
//     public MoneyBuilder setQuantity(String quantity) {
//         return setStringArgument(TtsSpan.ARG_QUANTITY, quantity);
//     }
// }

// /**
//  * A builder for TtsSpans of type {@link #TYPE_TELEPHONE}.
//  */
// public static class TelephoneBuilder
//         extends SemioticClassBuilder<TelephoneBuilder> {

//     /**
//      * Creates a TtsSpan of type {@link #TYPE_TELEPHONE}.
//      */
//     public TelephoneBuilder() {
//         super(TtsSpan.TYPE_TELEPHONE);
//     }

//     /**
//      * Creates a TtsSpan of type {@link #TYPE_TELEPHONE} and sets the
//      * {@link #ARG_NUMBER_PARTS} argument.
//      */
//     public TelephoneBuilder(String numberParts) {
//         this();
//         setNumberParts(numberParts);
//     }

//     /**
//      * Sets the {@link #ARG_COUNTRY_CODE} argument.
//      * @param countryCode The country code can be a series of digits
//      * optionally prefixed with a "+".
//      * @return This instance.
//      */
//     public TelephoneBuilder setCountryCode(String countryCode) {
//         return setStringArgument(TtsSpan.ARG_COUNTRY_CODE, countryCode);
//     }

//     /**
//      * Sets the {@link #ARG_NUMBER_PARTS} argument.
//      * @param numberParts The main telephone number. Can be a series of
//      *     digits and letters separated by spaces, "/", "-" or ".".
//      * @return This instance.
//      */
//     public TelephoneBuilder setNumberParts(String numberParts) {
//         return setStringArgument(TtsSpan.ARG_NUMBER_PARTS, numberParts);
//     }

//     /**
//      * Sets the {@link #ARG_EXTENSION} argument.
//      * @param extension The extension can be a series of digits.
//      * @return This instance.
//      */
//     public TelephoneBuilder setExtension(String extension) {
//         return setStringArgument(TtsSpan.ARG_EXTENSION, extension);
//     }
// }

// /**
//  * A builder for TtsSpans of type {@link #TYPE_ELECTRONIC}.
//  */
// public static class ElectronicBuilder
//         extends SemioticClassBuilder<ElectronicBuilder> {

//     /**
//      * Creates a TtsSpan of type {@link #TYPE_ELECTRONIC}.
//      */
//     public ElectronicBuilder() {
//         super(TtsSpan.TYPE_ELECTRONIC);
//     }

//     /**
//      * Sets the {@link #ARG_USERNAME} and {@link #ARG_DOMAIN}
//      *     arguments, representing an email address.
//      * @param username The part before the @ in the email address.
//      * @param domain The part after the @ in the email address.
//      * @return This instance.
//      */
//     public ElectronicBuilder setEmailArguments(String username,
//                                                String domain) {
//         return setDomain(domain).setUsername(username);
//     }

//     /**
//      * Sets the {@link #ARG_PROTOCOL} argument.
//      * @param protocol The protocol of the URI. Examples are "http" and
//      *     "ftp".
//      * @return This instance.
//      */
//     public ElectronicBuilder setProtocol(String protocol) {
//         return setStringArgument(TtsSpan.ARG_PROTOCOL, protocol);
//     }

//     /**
//      * Sets the {@link #ARG_USERNAME} argument.
//      * @return This instance.
//      */
//     public ElectronicBuilder setUsername(String username) {
//         return setStringArgument(TtsSpan.ARG_USERNAME, username);
//     }

//     /**
//      * Sets the {@link #ARG_PASSWORD} argument.
//      * @return This instance.
//      */
//     public ElectronicBuilder setPassword(String password) {
//         return setStringArgument(TtsSpan.ARG_PASSWORD, password);
//     }

//     /**
//      * Sets the {@link #ARG_DOMAIN} argument.
//      * @param domain The domain, for example "source.android.com".
//      * @return This instance.
//      */
//     public ElectronicBuilder setDomain(String domain) {
//         return setStringArgument(TtsSpan.ARG_DOMAIN, domain);
//     }

//     /**
//      * Sets the {@link #ARG_PORT} argument.
//      * @return This instance.
//      */
//     public ElectronicBuilder setPort(int port) {
//         return setIntArgument(TtsSpan.ARG_PORT, port);
//     }

//     /**
//      * Sets the {@link #ARG_PATH} argument.
//      * @param path For example "source/index.html".
//      * @return This instance.
//      */
//     public ElectronicBuilder setPath(String path) {
//         return setStringArgument(TtsSpan.ARG_PATH, path);
//     }

//     /**
//      * Sets the {@link #ARG_QUERY_STRING} argument.
//      * @param queryString For example "arg=value&argtwo=value".
//      * @return This instance.
//      */
//     public ElectronicBuilder setQueryString(String queryString) {
//         return setStringArgument(TtsSpan.ARG_QUERY_STRING, queryString);
//     }

//     /**
//      * Sets the {@link #ARG_FRAGMENT_ID} argument.
//      * @return This instance.
//      */
//     public ElectronicBuilder setFragmentId(String fragmentId) {
//         return setStringArgument(TtsSpan.ARG_FRAGMENT_ID, fragmentId);
//     }
// }

// /**
//  * A builder for TtsSpans of type {@link #TYPE_DIGITS}.
//  */
// public static class DigitsBuilder
//         extends SemioticClassBuilder<DigitsBuilder> {

//     /**
//      * Creates a builder for a TtsSpan of type {@link #TYPE_DIGITS}.
//      */
//     public DigitsBuilder() {
//         super(TtsSpan.TYPE_DIGITS);
//     }

//     /**
//      * Creates a builder for a TtsSpan of type {@link #TYPE_DIGITS}
//      * and sets the {@link #ARG_DIGITS} argument.
//      */
//     public DigitsBuilder(String digits) {
//         this();
//         setDigits(digits);
//     }

//     /**
//      * Sets the {@link #ARG_DIGITS} argument.
//      * @param digits A string of digits.
//      * @return This instance.
//      */
//     public DigitsBuilder setDigits(String digits) {
//         return setStringArgument(TtsSpan.ARG_DIGITS, digits);
//     }
// }

// /**
//  * A builder for TtsSpans of type {@link #TYPE_VERBATIM}.
//  */
// public static class VerbatimBuilder
//         extends SemioticClassBuilder<VerbatimBuilder> {

//     /**
//      * Creates a builder for a TtsSpan of type {@link #TYPE_VERBATIM}.
//      */
//     public VerbatimBuilder() {
//         super(TtsSpan.TYPE_VERBATIM);
//     }

//     /**
//      * Creates a builder for a TtsSpan of type {@link #TYPE_VERBATIM}
//      * and sets the {@link #ARG_VERBATIM} argument.
//      */
//     public VerbatimBuilder(String verbatim) {
//         this();
//         setVerbatim(verbatim);
//     }

//     /**
//      * Sets the {@link #ARG_VERBATIM} argument.
//      * @param verbatim A string of characters that will be read verbatim,
//      *     except whitespace.
//      * @return This instance.
//      */
//     public VerbatimBuilder setVerbatim(String verbatim) {
//         return setStringArgument(TtsSpan.ARG_VERBATIM, verbatim);
//     }
// }

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_TTSSPAN_H__
