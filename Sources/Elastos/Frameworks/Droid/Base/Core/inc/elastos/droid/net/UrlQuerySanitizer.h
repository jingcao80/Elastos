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

#ifndef __ELASTOS_DROID_NET_URLQUERYSANITIZER_H__
#define __ELASTOS_DROID_NET_URLQUERYSANITIZER_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Utility::ISet;
using Elastos::Utility::IList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 *
 * Sanitizes the Query portion of a URL. Simple example:
 * <code>
 * UrlQuerySanitizer sanitizer = new UrlQuerySanitizer();
 * sanitizer.setAllowUnregisteredParamaters(true);
 * sanitizer.parseUrl("http://example.com/?name=Joe+User");
 * String name = sanitizer.getValue("name"));
 * // name now contains "Joe_User"
 * </code>
 *
 * Register ValueSanitizers to customize the way individual
 * parameters are sanitized:
 * <code>
 * UrlQuerySanitizer sanitizer = new UrlQuerySanitizer();
 * sanitizer.registerParamater("name", UrlQuerySanitizer.createSpaceLegal());
 * sanitizer.parseUrl("http://example.com/?name=Joe+User");
 * String name = sanitizer.getValue("name"));
 * // name now contains "Joe User". (The string is first decoded, which
 * // converts the '+' to a ' '. Then the string is sanitized, which
 * // converts the ' ' to an '_'. (The ' ' is converted because the default
 * unregistered parameter sanitizer does not allow any special characters,
 * and ' ' is a special character.)
 * </code>
 *
 * There are several ways to create ValueSanitizers. In order of increasing
 * sophistication:
 * <ol>
 * <li>Call one of the UrlQuerySanitizer.createXXX() methods.
 * <li>Construct your own instance of
 * UrlQuerySanitizer.IllegalCharacterValueSanitizer.
 * <li>Subclass UrlQuerySanitizer.ValueSanitizer to define your own value
 * sanitizer.
 * </ol>
 *
 */
class UrlQuerySanitizer
    : public Object
    , public IUrlQuerySanitizer
{
public:
    CAR_INTERFACE_DECL()

    UrlQuerySanitizer();

    /**
     * Get the current value sanitizer used when processing
     * unregistered parameter values.
     * <p>
     * <b>Note:</b> The default unregistered parameter value sanitizer is
     * one that doesn't allow any special characters, similar to what
     * is returned by calling createAllIllegal.
     *
     * @return the current ValueSanitizer used to sanitize unregistered
     * parameter values.
     */
    CARAPI GetUnregisteredParameterValueSanitizer(
        /* [out] */ IUrlQuerySanitizerValueSanitizer** result);

    /**
     * Set the value sanitizer used when processing unregistered
     * parameter values.
     * @param sanitizer set the ValueSanitizer used to sanitize unregistered
     * parameter values.
     */
    CARAPI SetUnregisteredParameterValueSanitizer(
        /* [in] */ IUrlQuerySanitizerValueSanitizer* sanitizer);

    /**
     * Return a value sanitizer that does not allow any special characters,
     * and also does not allow script URLs.
     * @return a value sanitizer
     */
    static CARAPI GetAllIllegal(
        /* [out] */ IUrlQuerySanitizerValueSanitizer** result);

    /**
     * Return a value sanitizer that allows everything except Nul ('\0')
     * characters. Script URLs are allowed.
     * @return a value sanitizer
     */
    static CARAPI GetAllButNulLegal(
        /* [out] */ IUrlQuerySanitizerValueSanitizer** result);

    /**
     * Return a value sanitizer that allows everything except Nul ('\0')
     * characters, space (' '), and other whitespace characters.
     * Script URLs are allowed.
     * @return a value sanitizer
     */
    static CARAPI GetAllButWhitespaceLegal(
        /* [out] */ IUrlQuerySanitizerValueSanitizer** result);

    /**
     * Return a value sanitizer that allows all the characters used by
     * encoded URLs. Does not allow script URLs.
     * @return a value sanitizer
     */
    static CARAPI GetUrlLegal(
        /* [out] */ IUrlQuerySanitizerValueSanitizer** result);

    /**
     * Return a value sanitizer that allows all the characters used by
     * encoded URLs and allows spaces, which are not technically legal
     * in encoded URLs, but commonly appear anyway.
     * Does not allow script URLs.
     * @return a value sanitizer
     */
    static CARAPI GetUrlAndSpaceLegal(
        /* [out] */ IUrlQuerySanitizerValueSanitizer** result);

    /**
     * Return a value sanitizer that does not allow any special characters
     * except ampersand ('&'). Does not allow script URLs.
     * @return a value sanitizer
     */
    static CARAPI GetAmpLegal(
        /* [out] */ IUrlQuerySanitizerValueSanitizer** result);

    /**
     * Return a value sanitizer that does not allow any special characters
     * except ampersand ('&') and space (' '). Does not allow script URLs.
     * @return a value sanitizer
     */
    static CARAPI GetAmpAndSpaceLegal(
        /* [out] */ IUrlQuerySanitizerValueSanitizer** result);

    /**
     * Return a value sanitizer that does not allow any special characters
     * except space (' '). Does not allow script URLs.
     * @return a value sanitizer
     */
    static CARAPI GetSpaceLegal(
        /* [out] */ IUrlQuerySanitizerValueSanitizer** result);

    /**
     * Return a value sanitizer that allows any special characters
     * except angle brackets ('<' and '>') and Nul ('\0').
     * Allows script URLs.
     * @return a value sanitizer
     */
    static CARAPI GetAllButNulAndAngleBracketsLegal(
        /* [out] */ IUrlQuerySanitizerValueSanitizer** result);

    /**
     * Constructs a UrlQuerySanitizer.
     * <p>
     * Defaults:
     * <ul>
     * <li>unregistered parameters are not allowed.
     * <li>the last instance of a repeated parameter is preferred.
     * <li>The default value sanitizer is an AllIllegal value sanitizer.
     * <ul>
     */
    CARAPI constructor();

    /**
     * Constructs a UrlQuerySanitizer and parse a URL.
     * This constructor is provided for convenience when the
     * default parsing behavior is acceptable.
     * <p>
     * Because the URL is parsed before the constructor returns, there isn't
     * a chance to configure the sanitizer to change the parsing behavior.
     * <p>
     * <code>
     * UrlQuerySanitizer sanitizer = new UrlQuerySanitizer(myUrl);
     * String name = sanitizer.getValue("name");
     * </code>
     * <p>
     * Defaults:
     * <ul>
     * <li>unregistered parameters <em>are</em> allowed.
     * <li>the last instance of a repeated parameter is preferred.
     * <li>The default value sanitizer is an AllIllegal value sanitizer.
     * <ul>
     */
    CARAPI constructor(
        /* [in] */ const String& url);

    /**
     * Parse the query parameters out of an encoded URL.
     * Works by extracting the query portion from the URL and then
     * calling parseQuery(). If there is no query portion it is
     * treated as if the query portion is an empty string.
     * @param url the encoded URL to parse.
     */
    CARAPI ParseUrl(
        /* [in] */ const String& url);

    /**
     * Parse a query. A query string is any number of parameter-value clauses
     * separated by any non-zero number of ampersands. A parameter-value clause
     * is a parameter followed by an equal sign, followed by a value. If the
     * equal sign is missing, the value is assumed to be the empty string.
     * @param query the query to parse.
     */
    CARAPI ParseQuery(
        /* [in] */ const String& query);

    /**
     * Get a set of all of the parameters found in the sanitized query.
     * <p>
     * Note: Do not modify this set. Treat it as a read-only set.
     * @return all the parameters found in the current query.
     */
    CARAPI GetParameterSet(
        /* [out] */ ISet** result);

    /**
     * An array list of all of the parameter value pairs in the sanitized
     * query, in the order they appeared in the query. May contain duplicate
     * parameters.
     * <p class="note"><b>Note:</b> Do not modify this list. Treat it as a read-only list.</p>
     */
    CARAPI GetParameterList(
        /* [out] */ IList** result);

    /**
     * Check if a parameter exists in the current sanitized query.
     * @param parameter the unencoded name of a parameter.
     * @return true if the paramater exists in the current sanitized queary.
     */
    CARAPI HasParameter(
        /* [in] */ const String& parameter,
        /* [out] */ Boolean* result);

    /**
     * Get the value for a parameter in the current sanitized query.
     * Returns null if the parameter does not
     * exit.
     * @param parameter the unencoded name of a parameter.
     * @return the sanitized unencoded value of the parameter,
     * or null if the parameter does not exist.
     */
    CARAPI GetValue(
        /* [in] */ const String& parameter,
        /* [out] */ String* result);

    /**
     * Register a value sanitizer for a particular parameter. Can also be used
     * to replace or remove an already-set value sanitizer.
     * <p>
     * Registering a non-null value sanitizer for a particular parameter
     * makes that parameter a registered parameter.
     * @param parameter an unencoded parameter name
     * @param valueSanitizer the value sanitizer to use for a particular
     * parameter. May be null in order to unregister that parameter.
     * @see #getAllowUnregisteredParamaters()
     */
    CARAPI RegisterParameter(
        /* [in] */ const String& parameter,
        /* [in] */ IUrlQuerySanitizerValueSanitizer* valueSanitizer);

    /**
     * Register a value sanitizer for an array of parameters.
     * @param parameters An array of unencoded parameter names.
     * @param valueSanitizer
     * @see #registerParameter
     */
    CARAPI RegisterParameters(
        /* [in] */ ArrayOf<String>* parameters,
        /* [in] */ IUrlQuerySanitizerValueSanitizer* valueSanitizer);

    /**
     * Set whether or not unregistered parameters are allowed. If they
     * are not allowed, then they will be dropped when a query is sanitized.
     * <p>
     * Defaults to false.
     * @param allowUnregisteredParamaters true to allow unregistered parameters.
     * @see #getAllowUnregisteredParamaters()
     */
    CARAPI SetAllowUnregisteredParamaters(
        /* [in] */ Boolean allowUnregisteredParamaters);

    /**
     * Get whether or not unregistered parameters are allowed. If not
     * allowed, they will be dropped when a query is parsed.
     * @return true if unregistered parameters are allowed.
     * @see #setAllowUnregisteredParamaters(boolean)
     */
    CARAPI GetAllowUnregisteredParamaters(
        /* [out] */ Boolean* result);

    /**
     * Set whether or not the first occurrence of a repeated parameter is
     * preferred. True means the first repeated parameter is preferred.
     * False means that the last repeated parameter is preferred.
     * <p>
     * The preferred parameter is the one that is returned when getParameter
     * is called.
     * <p>
     * defaults to false.
     * @param preferFirstRepeatedParameter True if the first repeated
     * parameter is preferred.
     * @see #getPreferFirstRepeatedParameter()
     */
    CARAPI SetPreferFirstRepeatedParameter(
        /* [in] */ Boolean preferFirstRepeatedParameter);

    /**
     * Get whether or not the first occurrence of a repeated parameter is
     * preferred.
     * @return true if the first occurrence of a repeated parameter is
     * preferred.
     * @see #setPreferFirstRepeatedParameter(boolean)
     */
    CARAPI GetPreferFirstRepeatedParameter(
        /* [out] */ Boolean* result);

    /**
     * Parse an escaped parameter-value pair. The default implementation
     * unescapes both the parameter and the value, then looks up the
     * effective value sanitizer for the parameter and uses it to sanitize
     * the value. If all goes well then addSanitizedValue is called with
     * the unescaped parameter and the sanitized unescaped value.
     * @param parameter an escaped parameter
     * @param value an unsanitzied escaped value
     */
    CARAPI ParseEntry(
        /* [in] */ const String& parameter,
        /* [in] */ const String& value);

    /**
     * Record a sanitized parameter-value pair. Override if you want to
     * do additional filtering or validation.
     * @param parameter an unescaped parameter
     * @param value a sanitized unescaped value
     */
    CARAPI AddSanitizedEntry(
        /* [in] */ const String& parameter,
        /* [in] */ const String& value);

    /**
     * Get the value sanitizer for a parameter. Returns null if there
     * is no value sanitizer registered for the parameter.
     * @param parameter the unescaped parameter
     * @return the currently registered value sanitizer for this parameter.
     * @see #registerParameter(String, android.net.UrlQuerySanitizer.ValueSanitizer)
     */
    CARAPI GetValueSanitizer(
        /* [in] */ const String& parameter,
        /* [out] */ IUrlQuerySanitizerValueSanitizer** result);

    /**
     * Get the effective value sanitizer for a parameter. Like getValueSanitizer,
     * except if there is no value sanitizer registered for a parameter, and
     * unregistered paramaters are allowed, then the default value sanitizer is
     * returned.
     * @param parameter an unescaped parameter
     * @return the effective value sanitizer for a parameter.
     */
    CARAPI GetEffectiveValueSanitizer(
        /* [in] */ const String& parameter,
        /* [out] */ IUrlQuerySanitizerValueSanitizer** result);

    /**
     * Unescape an escaped string.
     * <ul>
     * <li>'+' characters are replaced by
     * ' ' characters.
     * <li>Valid "%xx" escape sequences are replaced by the
     * corresponding unescaped character.
     * <li>Invalid escape sequences such as %1z", are passed through unchanged.
     * <ol>
     * @param string the escaped string
     * @return the unescaped string.
     */
    CARAPI Unescape(
        /* [in] */ const String& string,
        /* [out] */ String* result);

    /**
     * Test if a character is a hexidecimal digit. Both upper case and lower
     * case hex digits are allowed.
     * @param c the character to test
     * @return true if c is a hex digit.
     */
    CARAPI_(Boolean) IsHexDigit(
        /* [in] */ Char32 c);

    /**
     * Convert a character that represents a hexidecimal digit into an integer.
     * If the character is not a hexidecimal digit, then -1 is returned.
     * Both upper case and lower case hex digits are allowed.
     * @param c the hexidecimal digit.
     * @return the integer value of the hexidecimal digit.
     */
    CARAPI_(Int32) DecodeHexDigit(
        /* [in] */ Char32 c);

    /**
     * Clear the existing entries. Called to get ready to parse a new
     * query string.
     */
    CARAPI Clear();

private:
    CARAPI_(AutoPtr<IHashMap>) CreateHashMap();

    CARAPI_(AutoPtr<IArrayList>) CreateArrayList();

    static CARAPI_(AutoPtr<IUrlQuerySanitizerValueSanitizer>) CreateValueSanitizer(
        /* [in] */ Int32 value);

private:
    // Private fields for singleton sanitizers:
    static const AutoPtr<IUrlQuerySanitizerValueSanitizer> sAllIllegal;
    static const AutoPtr<IUrlQuerySanitizerValueSanitizer> sAllButNulLegal;
    static const AutoPtr<IUrlQuerySanitizerValueSanitizer> sAllButWhitespaceLegal;
    static const AutoPtr<IUrlQuerySanitizerValueSanitizer> sURLLegal;
    static const AutoPtr<IUrlQuerySanitizerValueSanitizer> sUrlAndSpaceLegal;
    static const AutoPtr<IUrlQuerySanitizerValueSanitizer> sAmpLegal;
    static const AutoPtr<IUrlQuerySanitizerValueSanitizer> sAmpAndSpaceLegal;
    static const AutoPtr<IUrlQuerySanitizerValueSanitizer> sSpaceLegal;
    static const AutoPtr<IUrlQuerySanitizerValueSanitizer> sAllButNulAndAngleBracketsLegal;

    /* const */ AutoPtr<IHashMap> mSanitizers;
    /* const */ AutoPtr<IHashMap> mEntries;
    /* const */ AutoPtr<IArrayList> mEntriesList;
    Boolean mAllowUnregisteredParamaters;
    Boolean mPreferFirstRepeatedParameter;
    AutoPtr<IUrlQuerySanitizerValueSanitizer> mUnregisteredParameterValueSanitizer;
};

/**
 * A simple tuple that holds parameter-value pairs.
 *
 */
class UrlQuerySanitizerParameterValuePair
    : public Object
    , public IUrlQuerySanitizerParameterValuePair
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Construct a parameter-value tuple.
     * @param parameter an unencoded parameter
     * @param value an unencoded value
     */
    CARAPI constructor(
        /* [in] */ const String& parameter,
        /* [in] */ const String& value);

    /**
     * The unencoded parameter
     */
    CARAPI GetParameter(
        /* [out] */ String* result);

    /**
     * The unencoded parameter
     */
    CARAPI SetParameter(
        /* [in] */ const String& mParameter);

    /**
     * The unencoded value
     */
    CARAPI GetValue(
        /* [out] */ String* result);

    /**
     * The unencoded value
     */
    CARAPI SetValue(
        /* [in] */ const String& mValue);

private:
    /**
     * The unencoded parameter
     */
    String mParameter;

    /**
     * The unencoded value
     */
    String mValue;
};

/**
 * Sanitize values based on which characters they contain. Illegal
 * characters are replaced with either space or '_', depending upon
 * whether space is a legal character or not.
 */
class UrlQuerySanitizerIllegalCharacterValueSanitizer
    : public Object
    , public IUrlQuerySanitizerValueSanitizer
    , public IUrlQuerySanitizerIllegalCharacterValueSanitizer
{
public:
    CAR_INTERFACE_DECL()

    UrlQuerySanitizerIllegalCharacterValueSanitizer();
    /**
     * Construct a sanitizer. The parameters set the behavior of the
     * sanitizer.
     * @param flags some combination of the XXX_OK flags.
     */
    CARAPI constructor(
        /* [in] */ Int32 flags);

    /**
     * Sanitize a value.
     * <ol>
     * <li>If script URLs are not OK, the will be removed.
     * <li>If neither spaces nor other white space is OK, then
     * white space will be trimmed from the beginning and end of
     * the URL. (Just the actual white space characters are trimmed, not
     * other control codes.)
     * <li> Illegal characters will be replaced with
     * either ' ' or '_', depending on whether a space is itself a
     * legal character.
     * </ol>
     * @param value
     * @return the sanitized value
     */
    CARAPI Sanitize(
        /* [in] */ const String& value,
        /* [out] */ String* result);

private:
    /**
     * Trim whitespace from the beginning and end of a string.
     * <p>
     * Note: can't use {@link String#trim} because {@link String#trim} has a
     * different definition of whitespace than we want.
     * @param value the string to trim
     * @return the trimmed string
     */
    CARAPI_(String) TrimWhitespace(
        /* [in] */ const String& value);

    /**
     * Check if c is whitespace.
     * @param c character to test
     * @return true if c is a whitespace character
     */
    CARAPI_(Boolean) IsWhitespace(
        /* [in] */ Char32 c);

    /**
     * Check whether an individual character is legal. Uses the
     * flag bit-set passed into the constructor.
     * @param c
     * @return true if c is a legal character
     */
    CARAPI_(Boolean) CharacterIsLegal(
        /* [in] */ Char32 c);

private:
    Int32 mFlags;

    /**
     *  Script URL definitions
     */
    static const String JAVASCRIPT_PREFIX;

    static const String VBSCRIPT_PREFIX;

    static const Int32 MIN_SCRIPT_PREFIX_LENGTH;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_URLQUERYSANITIZER_H__
