#ifndef __ELASTOS_DROID_CONTENT_INTENTFILTER_H__
#define __ELASTOS_DROID_CONTENT_INTENTFILTER_H__

#include "Elastos.Droid.Content.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IPatternMatcher;
using Elastos::Droid::Utility::IPrinter;
using Elastos::Utility::Etl::List;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlSerializer;

namespace Elastos {
namespace Droid {
namespace Content {

class ECO_PUBLIC IntentFilter
    : public Object
    , public IIntentFilter
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& action);

    CARAPI constructor(
        /* [in] */ const String& action,
        /* [in] */ const String& dataType);

    CARAPI constructor(
        /* [in] */ IIntentFilter* o);

    /**
     * Create a new IntentFilter instance with a specified action and MIME
     * type, where you know the MIME type is correctly formatted.  This catches
     * the {@link MalformedMimeTypeException} exception that the constructor
     * can call and turns it into a runtime exception.
     *
     * @param action The action to match, i.e. Intent.ACTION_VIEW.
     * @param dataType The type to match, i.e. "vnd.android.cursor.dir/person".
     *
     * @return A new IntentFilter for the given action and type.
     *
     * @see #IntentFilter(String, String)
     */
    static CARAPI Create(
        /* [in] */ const String& action,
        /* [in] */ const String& dataType,
        /* [out] */ IIntentFilter** filter);

    /**
     * Modify priority of this filter.  The default priority is 0. Positive
     * values will be before the default, lower values will be after it.
     * Applications must use a value that is larger than
     * {@link #SYSTEM_LOW_PRIORITY} and smaller than
     * {@link #SYSTEM_HIGH_PRIORITY} .
     *
     * @param priority The new priority value.
     *
     * @see #getPriority
     * @see #SYSTEM_LOW_PRIORITY
     * @see #SYSTEM_HIGH_PRIORITY
     */
    CARAPI SetPriority(
        /* [in] */ Int32 priority);

    /**
     * Return the priority of this filter.
     *
     * @return The priority of the filter.
     *
     * @see #setPriority
     */
    CARAPI_(Int32) GetPriority();

    CARAPI GetPriority(
        /* [out] */ Int32* result);

    /**
     * Add a new Intent action to match against.  If any actions are included
     * in the filter, then an Intent's action must be one of those values for
     * it to match.  If no actions are included, the Intent action is ignored.
     *
     * @param action Name of the action to match, i.e. Intent.ACTION_VIEW.
     */
    CARAPI AddAction(
        /* [in] */ const String& action);

    /**
     * Return the number of actions in the filter.
     */
    CARAPI_(Int32) CountActions();

    CARAPI CountActions(
        /* [out] */ Int32* result);

    /**
     * Return an action in the filter.
     */
    CARAPI_(String) GetAction(
        /* [in] */ Int32 index);

    CARAPI GetAction(
        /* [in] */ Int32 index,
        /* [out] */ String* action);

    /**
     * Is the given action included in the filter?  Note that if the filter
     * does not include any actions, false will <em>always</em> be returned.
     *
     * @param action The action to look for.
     *
     * @return True if the action is explicitly mentioned in the filter.
     */
    CARAPI_(Boolean) HasAction(
        /* [in] */ const String& action);

    CARAPI HasAction(
        /* [in] */ const String& action,
        /* [out] */ Boolean* has);

    /**
     * Match this filter against an Intent's action.  If the filter does not
     * specify any actions, the match will always fail.
     *
     * @param action The desired action to look for.
     *
     * @return True if the action is listed in the filter or the filter does
     *         not specify any actions.
     */
    CARAPI_(Boolean) MatchAction(
        /* [in] */ const String& action);

    CARAPI MatchAction(
        /* [in] */ const String& action,
        /* [out] */ Boolean* has);

    CARAPI_(AutoPtr< ArrayOf<String> >) GetActions();

    CARAPI GetActions(
        /* [out, callee] */ ArrayOf<String>** actions);

    /**
     * Add a new Intent data type to match against.  If any types are
     * included in the filter, then an Intent's data must be <em>either</em>
     * one of these types <em>or</em> a matching scheme.  If no data types
     * are included, then an Intent will only match if it specifies no data.
     *
     * <p><em>Note: MIME type matching in the Android framework is
     * case-sensitive, unlike formal RFC MIME types.  As a result,
     * you should always write your MIME types with lower case letters,
     * and any MIME types you receive from outside of Android should be
     * converted to lower case before supplying them here.</em></p>
     *
     * <p>Throws {@link MalformedMimeTypeException} if the given MIME type is
     * not syntactically correct.
     *
     * @param type Name of the data type to match, i.e. "vnd.android.cursor.dir/person".
     *
     * @see #matchData
     */
    CARAPI AddDataType(
        /* [in] */ const String& type);

    /**
     * Is the given data type included in the filter?  Note that if the filter
     * does not include any type, false will <em>always</em> be returned.
     *
     * @param type The data type to look for.
     *
     * @return True if the type is explicitly mentioned in the filter.
     */
    CARAPI_(Boolean) HasDataType(
        /* [in] */ const String& type);

    CARAPI HasDataType(
        /* [in] */ const String& type,
        /* [out] */ Boolean* has);

    CARAPI HasExactDataType(
        /* [in] */ const String& type,
        /* [out] */ Boolean* has);

    CARAPI HasPartialTypes(
        /* [out] */ Boolean* has);

    /**
     * Return the number of data types in the filter.
     */
    CARAPI_(Int32) CountDataTypes();

    CARAPI CountDataTypes(
        /* [out] */ Int32* result);

    /**
     * Return a data type in the filter.
     */
    CARAPI_(String) GetDataType(
        /* [in] */ Int32 index);

    CARAPI GetDataType(
        /* [in] */ Int32 index,
        /* [out] */ String* type);

    CARAPI_(AutoPtr< ArrayOf<String> >) GetTypes();

    CARAPI GetTypes(
        /* [out, callee] */ ArrayOf<String>** types);

    /**
     * Add a new Intent data scheme to match against.  If any schemes are
     * included in the filter, then an Intent's data must be <em>either</em>
     * one of these schemes <em>or</em> a matching data type.  If no schemes
     * are included, then an Intent will match only if it includes no data.
     *
     * <p><em>Note: scheme matching in the Android framework is
     * case-sensitive, unlike formal RFC schemes.  As a result,
     * you should always write your schemes with lower case letters,
     * and any schemes you receive from outside of Android should be
     * converted to lower case before supplying them here.</em></p>
     *
     * @param scheme Name of the scheme to match, i.e. "http".
     *
     * @see #matchData
     */
    CARAPI AddDataScheme(
        /* [in] */ const String& scheme);

    /**
     * Return the number of data schemes in the filter.
     */
    CARAPI_(Int32) CountDataSchemes();

    CARAPI CountDataSchemes(
        /* [out] */ Int32* result);

     /**
     * Return a data scheme in the filter.
     */
    CARAPI_(String) GetDataScheme(
        /* [in] */ Int32 index);

    CARAPI GetDataScheme(
        /* [in] */ Int32 index,
        /* [out] */ String* scheme);

    /**
     * Is the given data scheme included in the filter?  Note that if the
     * filter does not include any scheme, false will <em>always</em> be
     * returned.
     *
     * @param scheme The data scheme to look for.
     *
     * @return True if the scheme is explicitly mentioned in the filter.
     */
    CARAPI_(Boolean) HasDataScheme(
        /* [in] */ const String& scheme);

    CARAPI HasDataScheme(
        /* [in] */ const String& scheme,
        /* [out] */ Boolean* has);

    CARAPI_(AutoPtr< ArrayOf<String> >) GetSchemes();

    CARAPI GetSchemes(
        /* [out, callee] */ ArrayOf<String>** schemes);

    /**
     * Add a new Intent data "scheme specific part" to match against.  The filter must
     * include one or more schemes (via {@link #addDataScheme}) for the
     * scheme specific part to be considered.  If any scheme specific parts are
     * included in the filter, then an Intent's data must match one of
     * them.  If no scheme specific parts are included, then only the scheme must match.
     *
     * <p>The "scheme specific part" that this matches against is the string returned
     * by {@link android.net.Uri#getSchemeSpecificPart() Uri.getSchemeSpecificPart}.
     * For Uris that contain a path, this kind of matching is not generally of interest,
     * since {@link #addDataAuthority(String, String)} and
     * {@link #addDataPath(String, int)} can provide a better mechanism for matching
     * them.  However, for Uris that do not contain a path, the authority and path
     * are empty, so this is the only way to match against the non-scheme part.</p>
     *
     * @param ssp Either a raw string that must exactly match the scheme specific part
     * path, or a simple pattern, depending on <var>type</var>.
     * @param type Determines how <var>ssp</var> will be compared to
     * determine a match: either {@link PatternMatcher#PATTERN_LITERAL},
     * {@link PatternMatcher#PATTERN_PREFIX}, or
     * {@link PatternMatcher#PATTERN_SIMPLE_GLOB}.
     *
     * @see #matchData
     * @see #addDataScheme
     */
    CARAPI AddDataSchemeSpecificPart(
        /* [in] */ const String& ssp,
        /* [in] */ Int32 type);

    /** @hide */
    CARAPI AddDataSchemeSpecificPart(
        /* [in] */ IPatternMatcher* ssp);

    /**
     * Return the number of data scheme specific parts in the filter.
     */
    CARAPI CountDataSchemeSpecificParts(
        /* [out] */ Int32* result);

    /**
     * Return a data scheme specific part in the filter.
     */
    CARAPI GetDataSchemeSpecificPart(
        /* [in] */ Int32 index,
        /* [out] */ IPatternMatcher** ssp);

    /**
     * Is the given data scheme specific part included in the filter?  Note that if the
     * filter does not include any scheme specific parts, false will <em>always</em> be
     * returned.
     *
     * @param data The scheme specific part that is being looked for.
     *
     * @return Returns true if the data string matches a scheme specific part listed in the
     *         filter.
     */
    CARAPI HasDataSchemeSpecificPart(
        /* [in] */ const String& data,
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI HasDataSchemeSpecificPart(
        /* [in] */ IPatternMatcher* data,
        /* [out] */ Boolean* result);

    /**
     * Return an iterator over the filter's data scheme specific parts.
     */
    CARAPI_(AutoPtr< ArrayOf<IPatternMatcher*> >) GetSchemeSpecificParts();

    CARAPI GetSchemeSpecificParts(
        /* [out, callee] */ ArrayOf<IPatternMatcher*>** array);

    /**
     * Add a new Intent data authority to match against.  The filter must
     * include one or more schemes (via {@link #addDataScheme}) for the
     * authority to be considered.  If any authorities are
     * included in the filter, then an Intent's data must match one of
     * them.  If no authorities are included, then only the scheme must match.
     *
     * <p><em>Note: host name in the Android framework is
     * case-sensitive, unlike formal RFC host names.  As a result,
     * you should always write your host names with lower case letters,
     * and any host names you receive from outside of Android should be
     * converted to lower case before supplying them here.</em></p>
     *
     * @param host The host part of the authority to match.  May start with a
     *             single '*' to wildcard the front of the host name.
     * @param port Optional port part of the authority to match.  If null, any
     *             port is allowed.
     *
     * @see #matchData
     * @see #addDataScheme
     */
    CARAPI AddDataAuthority(
        /* [in] */ const String& host,
        /* [in] */ const String& port);

    CARAPI AddDataAuthority(
        /* [in] */ IIntentFilterAuthorityEntry* authority);

    /**
     * Return the number of data authorities in the filter.
     */
    CARAPI_(Int32) CountDataAuthorities();

    CARAPI CountDataAuthorities(
        /* [out] */ Int32* result);

    /**
     * Return a data authority in the filter.
     */
    CARAPI_(AutoPtr<IIntentFilterAuthorityEntry>) GetDataAuthority(
        /* [in] */ Int32 index);

    CARAPI GetDataAuthority(
        /* [in] */ Int32 index,
        /* [out] */ IIntentFilterAuthorityEntry** authority);

    /**
     * Is the given data authority included in the filter?  Note that if the
     * filter does not include any authorities, false will <em>always</em> be
     * returned.
     *
     * @param data The data whose authority is being looked for.
     *
     * @return Returns true if the data string matches an authority listed in the
     *         filter.
     */
    CARAPI HasDataAuthority(
        /* [in] */ IUri* data,
        /* [out] */ Boolean* result);

    CARAPI HasDataAuthority(
        /* [in] */ IIntentFilterAuthorityEntry* data,
        /* [out] */ Boolean* result);

    /**
     * Return an iterator over the filter's data authorities.
     */
    CARAPI_(AutoPtr< ArrayOf<IIntentFilterAuthorityEntry*> >) GetAuthorities();

    CARAPI GetAuthorities(
        /* [out, callee] */ ArrayOf<IIntentFilterAuthorityEntry*>** authorities);

    /**
     * Add a new Intent data oath to match against.  The filter must
     * include one or more schemes (via {@link #addDataScheme}) <em>and</em>
     * one or more authorities (via {@link #addDataAuthority}) for the
     * path to be considered.  If any paths are
     * included in the filter, then an Intent's data must match one of
     * them.  If no paths are included, then only the scheme/authority must
     * match.
     *
     * <p>The path given here can either be a literal that must directly
     * match or match against a prefix, or it can be a simple globbing pattern.
     * If the latter, you can use '*' anywhere in the pattern to match zero
     * or more instances of the previous character, '.' as a wildcard to match
     * any character, and '\' to escape the next character.
     *
     * @param path Either a raw string that must exactly match the file
     * path, or a simple pattern, depending on <var>type</var>.
     * @param type Determines how <var>path</var> will be compared to
     * determine a match: either {@link PatternMatcher#PATTERN_LITERAL},
     * {@link PatternMatcher#PATTERN_PREFIX}, or
     * {@link PatternMatcher#PATTERN_SIMPLE_GLOB}.
     *
     * @see #matchData
     * @see #addDataScheme
     * @see #addDataAuthority
     */
    CARAPI AddDataPath(
        /* [in] */ const String& path,
        /* [in] */ Int32 type);

    CARAPI AddDataPath(
        /* [in] */ IPatternMatcher* pm);

    /**
     * Return the number of data paths in the filter.
     */
    CARAPI_(Int32) CountDataPaths();

    CARAPI CountDataPaths(
        /* [out] */ Int32* result);

    /**
     * Return a data path in the filter.
     */
    CARAPI_(AutoPtr<IPatternMatcher>) GetDataPath(
        /* [in] */ Int32 index);

    CARAPI GetDataPath(
        /* [in] */ Int32 index,
        /* [out] */ IPatternMatcher** pm);

    /**
     * Is the given data path included in the filter?  Note that if the
     * filter does not include any paths, false will <em>always</em> be
     * returned.
     *
     * @param data The data path to look for.  This is without the scheme
     *             prefix.
     *
     * @return True if the data string matches a path listed in the
     *         filter.
     */
    CARAPI HasDataPath(
        /* [in] */ const String& data,
        /* [out] */ Boolean* result);

    CARAPI HasDataPath(
        /* [in] */ IPatternMatcher* pm,
        /* [out] */ Boolean* result);

    CARAPI_(AutoPtr< ArrayOf<IPatternMatcher*> >) GetPaths();

    CARAPI GetPaths(
        /* [out, callee] */ ArrayOf<IPatternMatcher*>** paths);

    /**
     * Match this intent filter against the given Intent data.  This ignores
     * the data scheme -- unlike {@link #matchData}, the authority will match
     * regardless of whether there is a matching scheme.
     *
     * @param data The data whose authority is being looked for.
     *
     * @return Returns either {@link #MATCH_CATEGORY_HOST},
     * {@link #MATCH_CATEGORY_PORT}, {@link #NO_MATCH_DATA}.
     */
    CARAPI_(Int32) MatchDataAuthority(
        /* [in] */ IUri* data);

    CARAPI MatchDataAuthority(
        /* [in] */ IUri* data,
        /* [out] */ Int32* match);

    /**
     * Match this filter against an Intent's data (type, scheme and path). If
     * the filter does not specify any types and does not specify any
     * schemes/paths, the match will only succeed if the intent does not
     * also specify a type or data.
     *
     * <p>Be aware that to match against an authority, you must also specify a base
     * scheme the authority is in.  To match against a data path, both a scheme
     * and authority must be specified.  If the filter does not specify any
     * types or schemes that it matches against, it is considered to be empty
     * (any authority or data path given is ignored, as if it were empty as
     * well).
     *
     * <p><em>Note: MIME type, Uri scheme, and host name matching in the
     * Android framework is case-sensitive, unlike the formal RFC definitions.
     * As a result, you should always write these elements with lower case letters,
     * and normalize any MIME types or Uris you receive from
     * outside of Android to ensure these elements are lower case before
     * supplying them here.</em></p>
     *
     * @param type The desired data type to look for, as returned by
     *             Intent.resolveType().
     * @param scheme The desired data scheme to look for, as returned by
     *               Intent.getScheme().
     * @param data The full data string to match against, as supplied in
     *             Intent.data.
     *
     * @return Returns either a valid match constant (a combination of
     * {@link #MATCH_CATEGORY_MASK} and {@link #MATCH_ADJUSTMENT_MASK}),
     * or one of the error codes {@link #NO_MATCH_TYPE} if the type didn't match
     * or {@link #NO_MATCH_DATA} if the scheme/path didn't match.
     *
     * @see #match
     */
    CARAPI_(Int32) MatchData(
        /* [in] */ const String& type,
        /* [in] */ const String& scheme,
        /* [in] */ IUri* data);

    CARAPI MatchData(
        /* [in] */ const String& type,
        /* [in] */ const String& scheme,
        /* [in] */ IUri* data,
        /* [out] */  Int32* match);

    /**
     * Add a new Intent category to match against.  The semantics of
     * categories is the opposite of actions -- an Intent includes the
     * categories that it requires, all of which must be included in the
     * filter in order to match.  In other words, adding a category to the
     * filter has no impact on matching unless that category is specified in
     * the intent.
     *
     * @param category Name of category to match, i.e. Intent.CATEGORY_EMBED.
     */
    CARAPI AddCategory(
        /* [in] */ const String& category);

    /**
     * Return the number of categories in the filter.
     */
    CARAPI_(Int32) CountCategories();

    CARAPI CountCategories(
        /* [out] */ Int32* count);

    /**
     * Return a category in the filter.
     */
    CARAPI_(String) GetCategory(
        /* [in] */ Int32 index);

    CARAPI GetCategory(
        /* [in] */ Int32 index,
        /* [out] */ String* category);

    /**
     * Is the given category included in the filter?
     *
     * @param category The category that the filter supports.
     *
     * @return True if the category is explicitly mentioned in the filter.
     */
    CARAPI_(Boolean) HasCategory(
        /* [in] */ const String& category);

    CARAPI HasCategory(
        /* [in] */ const String& category,
        /* [out] */ Boolean* has);

    CARAPI_(AutoPtr< ArrayOf<String> >) GetCategories();

    CARAPI GetCategories(
        /* [out] */ ArrayOf<String>** categories);

    /**
     * Match this filter against an Intent's categories.  Each category in
     * the Intent must be specified by the filter; if any are not in the
     * filter, the match fails.
     *
     * @param categories The categories included in the intent, as returned by
     *                   Intent.getCategories().
     *
     * @return If all categories match (success), null; else the name of the
     *         first category that didn't match.
     */
    CARAPI_(String) MatchCategories(
        /* [in] */ ArrayOf<String>* categories);

    CARAPI MatchCategories(
        /* [in] */ ArrayOf<String>* categories,
        /* [out] */ String* match);

    /**
     * Test whether this filter matches the given <var>intent</var>.
     *
     * @param intent The Intent to compare against.
     * @param resolve If true, the intent's type will be resolved by calling
     *                Intent.resolveType(); otherwise a simple match against
     *                Intent.type will be performed.
     * @param logTag Tag to use in debugging messages.
     *
     * @return Returns either a valid match constant (a combination of
     * {@link #MATCH_CATEGORY_MASK} and {@link #MATCH_ADJUSTMENT_MASK}),
     * or one of the error codes {@link #NO_MATCH_TYPE} if the type didn't match,
     * {@link #NO_MATCH_DATA} if the scheme/path didn't match,
     * {@link #NO_MATCH_ACTION if the action didn't match, or
     * {@link #NO_MATCH_CATEGORY} if one or more categories didn't match.
     *
     * @return How well the filter matches.  Negative if it doesn't match,
     *         zero or positive positive value if it does with a higher
     *         value representing a better match.
     *
     * @see #match(String, String, String, android.net.Uri , Set, String)
     */
    CARAPI_(Int32) Match(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IIntent* intent,
        /* [in] */ Boolean resolve,
        /* [in] */ const String& logTag);

    CARAPI Match(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IIntent* intent,
        /* [in] */ Boolean resolve,
        /* [in] */ const String& logTag,
        /* [out] */ Int32* match);

    /**
     * Test whether this filter matches the given intent data.  A match is
     * only successful if the actions and categories in the Intent match
     * against the filter, as described in {@link IntentFilter}; in that case,
     * the match result returned will be as per {@link #matchData}.
     *
     * @param action The intent action to match against (Intent.getAction).
     * @param type The intent type to match against (Intent.resolveType()).
     * @param scheme The data scheme to match against (Intent.getScheme()).
     * @param data The data URI to match against (Intent.getData()).
     * @param categories The categories to match against
     *                   (Intent.getCategories()).
     * @param logTag Tag to use in debugging messages.
     *
     * @return Returns either a valid match constant (a combination of
     * {@link #MATCH_CATEGORY_MASK} and {@link #MATCH_ADJUSTMENT_MASK}),
     * or one of the error codes {@link #NO_MATCH_TYPE} if the type didn't match,
     * {@link #NO_MATCH_DATA} if the scheme/path didn't match,
     * {@link #NO_MATCH_ACTION if the action didn't match, or
     * {@link #NO_MATCH_CATEGORY} if one or more categories didn't match.
     *
     * @see #matchData
     * @see Intent#getAction
     * @see Intent#resolveType
     * @see Intent#getScheme
     * @see Intent#getData
     * @see Intent#getCategories
     */
    CARAPI_(Int32) Match(
        /* [in] */ const String& action,
        /* [in] */ const String& type,
        /* [in] */ const String& scheme,
        /* [in] */ IUri* data,
        /* [in] */ ArrayOf<String>* categories,
        /* [in] */ const String& logTag);

    CARAPI Match(
        /* [in] */ const String& action,
        /* [in] */ const String& type,
        /* [in] */ const String& scheme,
        /* [in] */ IUri* data,
        /* [in] */ ArrayOf<String>* categories,
        /* [in] */ const String& logTag,
        /* [out] */ Int32* match);

    /**
     * Write the contents of the IntentFilter as an XML stream.
     */
    virtual CARAPI WriteToXml(
        /* [in] */ IXmlSerializer* serializer);

    virtual CARAPI ReadFromXml(
        /* [in] */ IXmlPullParser* parser);

    virtual CARAPI OnCompareTie(
        /* [in] */ IIntentFilter* other,
        /* [out] */ Int32* result);

    virtual CARAPI Dump(
        /* [in] */ IPrinter* du,
        /* [in] */ const String& prefix);

    virtual CARAPI ReadFromParcel(
        /* [in] */ IParcel *source);

    virtual CARAPI WriteToParcel(
        /* [in] */ IParcel *dest);

    /**
     * For debugging -- perform a check on the filter, return true if it passed
     * or false if it failed.
     *
     * {@hide}
     */
    virtual CARAPI_(Boolean) DebugCheck();

protected:
    /**
     * New empty IntentFilter.
     */
    IntentFilter();

    virtual ~IntentFilter();

private:
    ECO_LOCAL static CARAPI_(Int32) FindStringInSet(
        /* [in] */ ArrayOf<String>* set,
        /* [in] */ const String& string,
        /* [in] */ const ArrayOf<Int32>& lengths,
        /* [in] */ Int32 lenPos);

    ECO_LOCAL static CARAPI_(AutoPtr< ArrayOf<String> >) AddStringToSet(
        /* [in] */ ArrayOf<String>* set,
        /* [in] */ const String& string,
        /* [in] */ ArrayOf<Int32>& lengths,
        /* [in] */ Int32 lenPos);

    ECO_LOCAL static CARAPI_(AutoPtr< ArrayOf<String> >) RemoveStringFromSet(
        /* [in] */ ArrayOf<String>* set,
        /* [in] */ const String& string,
        /* [in] */ ArrayOf<Int32>& lengths,
        /* [in] */ Int32 lenPos);

    ECO_LOCAL CARAPI_(Boolean) FindMimeType(
        /* [in] */ const String& type);

protected:
    static const String SGLOB_STR;
    static const String PREFIX_STR;
    static const String LITERAL_STR;
    static const String PATH_STR;
    static const String PORT_STR;
    static const String HOST_STR;
    static const String AUTH_STR;
    static const String SSP_STR;
    static const String SCHEME_STR;
    static const String TYPE_STR;
    static const String CAT_STR;
    static const String NAME_STR;
    static const String ACTION_STR;

    Int32 mPriority;
    List<String> mActions;
    AutoPtr<List<String> > mCategories;
    AutoPtr<List<String> > mDataSchemes;
    AutoPtr< List< AutoPtr<IPatternMatcher> > > mDataSchemeSpecificParts;
    AutoPtr< List< AutoPtr<IIntentFilterAuthorityEntry> > > mDataAuthorities;
    AutoPtr< List< AutoPtr<IPatternMatcher> > > mDataPaths;
    AutoPtr<List<String> > mDataTypes;
    Boolean mHasPartialTypes;
};

} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_INTENTFILTER_H__
