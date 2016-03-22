
#ifndef __ELASTOS_DROID_NET_URI_H__
#define __ELASTOS_DROID_NET_URI_H__

#include "Elastos.Droid.Net.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/AbstractList.h>
#include <elastos/utility/etl/List.h>

using Elastos::Core::IComparable;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::IFile;
using Elastos::Utility::AbstractList;
using Elastos::Utility::Etl::List;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Immutable URI reference. A URI reference includes a URI and a fragment, the
 * component of the URI following a '#'. Builds and parses URI references
 * which conform to
 * <a href="http://www.faqs.org/rfcs/rfc2396.html">RFC 2396</a>.
 *
 * <p>In the interest of performance, this class performs little to no
 * validation. Behavior is undefined for invalid input. This class is very
 * forgiving--in the face of invalid input, it will return garbage
 * rather than throw an exception unless otherwise specified.
 */
class ECO_PUBLIC Uri
    : public Object
    , public IParcelable
    , public IComparable
    , public IUri
{
    /*

    This class aims to do as little up front work as possible. To accomplish
    that, we vary the implementation depending on what the user passes in.
    For example, we have one implementation if the user passes in a
    URI string (StringUri) and another if the user passes in the
    individual components (OpaqueUri).

    *Concurrency notes*: Like any truly immutable object, this class is safe
    for concurrent use. This class uses a caching pattern in some places where
    it doesn't use volatile or synchronized. This is safe to do with ints
    because getting or setting an int is atomic. It's safe to do with a String
    because the internal fields are final and the memory model guarantees other
    threads won't see a partially initialized instance. We are not guaranteed
    that some threads will immediately see changes from other threads on
    certain platforms, but we don't mind if those threads reconstruct the
    cached result. As a result, we get thread safe caching with no concurrency
    overhead, which means the most common case, access from a single thread,
    is as fast as possible.

    From the Java Language spec.:

    "17.5 Final Field Semantics

    ... when the object is seen by another thread, that thread will always
    see the correctly constructed version of that object's final fields.
    It will also see versions of any object or array referenced by
    those final fields that are at least as up-to-date as the final fields
    are."

    In that same vein, all non-transient fields within Uri
    implementations should be final and immutable so as to ensure true
    immutability for clients even when they don't use proper concurrency
    control.

    For reference, from RFC 2396:

    "4.3. Parsing a URI Reference

       A URI reference is typically parsed according to the four main
       components and fragment identifier in order to determine what
       components are present and whether the reference is relative or
       absolute.  The individual components are then parsed for their
       subparts and, if not opaque, to verify their validity.

       Although the BNF defines what is allowed in each component, it is
       ambiguous in terms of differentiating between an authority component
       and a path component that begins with two slash characters.  The
       greedy algorithm is used for disambiguation: the left-most matching
       rule soaks up as much of the URI reference string as it is capable of
       matching.  In other words, the authority component wins."

    The "four main components" of a hierarchical URI consist of
    <scheme>://<authority><path>?<query>

    */

public:
    /**
     * Wrapper for path segment array.
     */
    class PathSegments
        : public AbstractList
        , public IRandomAccess
    {
    public:
        CAR_INTERFACE_DECL()

        PathSegments(
            /* [in] */ ArrayOf<String>* segments,
            /* [in] */ Int32 size);

        CARAPI Get(
            /* [in] */ Int32 index,
            /* [out] */ IInterface** value);

        CARAPI GetSize(
            /* [out] */ Int32* result);

        CARAPI ToString(
            /* [out] */ String* str);

    public:
        /* const */ static AutoPtr<PathSegments> sEMPTY;

        /* const */ AutoPtr<ArrayOf<String> > mSegments;
        /* const */ Int32 mSize;
    };

    /**
     * Builds PathSegments.
     */
    class PathSegmentsBuilder
        : public Object
    {
    public:
        PathSegmentsBuilder();

        CARAPI Add(
            /* [in] */ const String& segment);

        CARAPI_(AutoPtr<PathSegments>) Build();
    private:
        List<String> mSegments;
        Int32 mSize;
    };

    /**
     * Support for part implementations.
     */
    class AbstractPart
        : public Object
        , public IAbstractPart
    {
    public:
        /**
         * Enum which indicates which representation of a given part we have.
         */
        class Representation
        {
        public:
            static const Int32 BOTH = 0;
            static const Int32 ENCODED = 1;
            static const Int32 DECODED = 2;
        };

    public:
        CAR_INTERFACE_DECL()

        AbstractPart(
            /* [in] */ const String& encoded,
            /* [in] */ const String& decoded);

        virtual CARAPI_(String) GetEncoded() = 0;

        CARAPI_(String) GetDecoded();

        CARAPI WriteTo(
            /* [in] */ IParcel* parcel);

    public:
        /* volatile */ String mEncoded;
        /* volatile */ String mDecoded;
    };

    /**
     * Immutable wrapper of encoded and decoded versions of a URI part. Lazily
     * creates the encoded or decoded version from the other.
     */
    class Part
        : public AbstractPart
    {
    public:
        virtual CARAPI_(Boolean) IsEmpty();

        virtual CARAPI_(String) GetEncoded();

        static CARAPI ReadFrom(
            /* [in] */ IParcel* parcel,
            /* [out] */ Part** part);

        /**
         * Returns given part or {@link #NULL} if the given part is null.
         */
        static CARAPI_(AutoPtr<Part>) NonNull(
            /* [in] */ Part* part);

        /**
         * Creates a part from the encoded string.
         *
         * @param encoded part string
         */
        static CARAPI_(AutoPtr<Part>) FromEncoded(
            /* [in] */ const String& encoded);

        /**
         * Creates a part from the decoded string.
         *
         * @param decoded part string
         */
        static CARAPI_(AutoPtr<Part>) FromDecoded(
            /* [in] */ const String& decoded);

        /**
         * Creates a part from the encoded and decoded strings.
         *
         * @param encoded part string
         * @param decoded part string
         */
        static CARAPI_(AutoPtr<Part>) From(
            /* [in] */ const String& encoded,
            /* [in] */ const String& decoded);

    protected:
        Part(
            /* [in] */ const String& encoded,
            /* [in] */ const String& decoded);

    public:
        /** A part with null values. */
        static const AutoPtr<Part> sNULL;

        /** A part with empty strings for values. */
        static const AutoPtr<Part> sEMPTY;
    };

    class EmptyPart
        : public Part
    {
    public:
        EmptyPart(
            /* [in] */ const String& value);

        // @Override
        CARAPI IsEmpty(
            /* [out] */ Boolean* result);
    };

    /**
     * Immutable wrapper of encoded and decoded versions of a path part. Lazily
     * creates the encoded or decoded version from the other.
     */
    class PathPart
        : public AbstractPart
    {
    public:
        virtual CARAPI_(String) GetEncoded();

        /**
         * Gets the individual path segments. Parses them if necessary.
         *
         * @return parsed path segments or null if this isn't a hierarchical
         *  URI
         */
        virtual CARAPI_(AutoPtr<PathSegments>) GetPathSegments();

        static CARAPI_(AutoPtr<PathPart>) AppendEncodedSegment(
            /* [in] */ PathPart* oldPart,
            /* [in] */ const String& newSegment);

        static CARAPI_(AutoPtr<PathPart>) AppendDecodedSegment(
            /* [in] */ PathPart* oldPart,
            /* [in] */ const String& decoded);

        static CARAPI ReadFrom(
            /* [in] */ IParcel* parcel,
            /* [out] */ PathPart** result);

        /**
         * Creates a path from the encoded string.
         *
         * @param encoded part string
         */
        static CARAPI_(AutoPtr<PathPart>) FromEncoded(
            /* [in] */ const String& encoded);

        /**
         * Creates a path from the decoded string.
         *
         * @param decoded part string
         */
        static CARAPI_(AutoPtr<PathPart>) FromDecoded(
            /* [in] */ const String& decoded);

        /**
         * Creates a path from the encoded and decoded strings.
         *
         * @param encoded part string
         * @param decoded part string
         */
        static CARAPI_(AutoPtr<PathPart>) From(
            /* [in] */ const String& encoded,
            /* [in] */ const String& decoded);

        /**
         * Prepends path values with "/" if they're present, not empty, and
         * they don't already start with "/".
         */
        static CARAPI_(AutoPtr<PathPart>) MakeAbsolute(
            /* [in] */ PathPart* oldPart);

    protected:
        PathPart(
            /* [in] */ const String& encoded,
            /* [in] */ const String& decoded);

    public:
        /** A part with null values. */
        static const AutoPtr<PathPart> sNULL;

        /** A part with empty strings for values. */
        static const AutoPtr<PathPart> sEMPTY;

    private:
        /**
         * Cached path segments. This doesn't need to be volatile--we don't
         * care if other threads see the result.
         */
        AutoPtr<PathSegments> mPathSegments;
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * Returns true if this URI is opaque like "mailto:nobody@google.com". The
     * scheme-specific part of an opaque URI cannot start with a '/'.
     */
    CARAPI IsOpaque(
        /* [out] */ Boolean* result);

    /**
     * Returns true if this URI is absolute, i.e.&nbsp;if it contains an
     * explicit scheme.
     *
     * @return true if this URI is absolute, false if it's relative
     */
    CARAPI IsAbsolute(
        /* [out] */ Boolean* result);

    /**
     * Compares this Uri to another object for equality. Returns true if the
     * encoded string representations of this Uri and the given Uri are
     * equal. Case counts. Paths are not normalized. If one Uri specifies a
     * default port explicitly and the other leaves it implicit, they will not
     * be considered equal.
     */
    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    /**
     * Hashes the encoded string represention of this Uri consistently with
     * {@link #equals(Object)}.
     */
    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    /**
     * Compares the string representation of this Uri with that of
     * another.
     */
    CARAPI CompareTo(
        /* [in] */ IInterface* other,
        /* [out] */ Int32* result);

    /**
     * Return a string representation of the URI that is safe to print
     * to logs and other places where PII should be avoided.
     * @hide
     */
    CARAPI ToSafeString(
        /* [out] */ String* result);

    /**
     * Creates a Uri which parses the given encoded URI string.
     *
     * @param uriString an RFC 2396-compliant, encoded URI
     * @throws NullPointerException if uriString is null
     * @return Uri for this given uri string
     */
    static CARAPI Parse(
        /* [in] */ const String& uriString,
        /* [out] */ IUri** result);

    /**
     * Creates a Uri from a file. The URI has the form
     * "file://<absolute path>". Encodes path characters with the exception of
     * '/'.
     *
     * <p>Example: "file:///tmp/android.txt"
     *
     * @throws NullPointerException if file is null
     * @return a Uri for the given file
     */
    static CARAPI FromFile(
        /* [in] */ IFile* file,
        /* [out] */ IUri** result);

    /**
     * Creates an opaque Uri from the given components. Encodes the ssp
     * which means this method cannot be used to create hierarchical URIs.
     *
     * @param scheme of the URI
     * @param ssp scheme-specific-part, everything between the
     *  scheme separator (':') and the fragment separator ('#'), which will
     *  get encoded
     * @param fragment fragment, everything after the '#', null if undefined,
     *  will get encoded
     *
     * @throws NullPointerException if scheme or ssp is null
     * @return Uri composed of the given scheme, ssp, and fragment
     *
     * @see Builder if you don't want the ssp and fragment to be encoded
     */
    static CARAPI FromParts(
        /* [in] */ const String& scheme,
        /* [in] */ const String& ssp,
        /* [in] */ const String& fragment,
        /* [out] */ IUri** result);

    /**
     * Returns a set of the unique names of all query parameters. Iterating
     * over the set will return the names in order of their first occurrence.
     *
     * @throws UnsupportedOperationException if this isn't a hierarchical URI
     *
     * @return a set of decoded names
     */
    CARAPI GetQueryParameterNames(
        /* [out] */ ISet** result);

    /**
     * Searches the query string for parameter values with the given key.
     *
     * @param key which will be encoded
     *
     * @throws UnsupportedOperationException if this isn't a hierarchical URI
     * @throws NullPointerException if key is null
     * @return a list of decoded values
     */
    CARAPI GetQueryParameters(
        /* [in] */ const String& key,
        /* [out] */ IList** result);

    /**
     * Searches the query string for the first value with the given key.
     *
     * <p><strong>Warning:</strong> Prior to Ice Cream Sandwich, this decoded
     * the '+' character as '+' rather than ' '.
     *
     * @param key which will be encoded
     * @throws UnsupportedOperationException if this isn't a hierarchical URI
     * @throws NullPointerException if key is null
     * @return the decoded value or null if no parameter is found
     */
    CARAPI GetQueryParameter(
        /* [in] */ const String& key,
        /* [out] */ String* result);

    /**
     * Searches the query string for the first value with the given key and interprets it
     * as a boolean value. "false" and "0" are interpreted as <code>false</code>, everything
     * else is interpreted as <code>true</code>.
     *
     * @param key which will be decoded
     * @param defaultValue the default value to return if there is no query parameter for key
     * @return the boolean interpretation of the query parameter key
     */
    CARAPI GetBooleanQueryParameter(
        /* [in] */ const String& key,
        /* [in] */ Boolean defaultValue,
        /* [out] */ Boolean* result);

    /**
     * Return an equivalent URI with a lowercase scheme component.
     * This aligns the Uri with Android best practices for
     * intent filtering.
     *
     * <p>For example, "HTTP://www.android.com" becomes
     * "http://www.android.com"
     *
     * <p>All URIs received from outside Android (such as user input,
     * or external sources like Bluetooth, NFC, or the Internet) should
     * be normalized before they are used to create an Intent.
     *
     * <p class="note">This method does <em>not</em> validate bad URI's,
     * or 'fix' poorly formatted URI's - so do not use it for input validation.
     * A Uri will always be returned, even if the Uri is badly formatted to
     * begin with and a scheme component cannot be found.
     *
     * @return normalized Uri (never null)
     * @see {@link android.content.Intent#setData}
     * @see {@link #setNormalizedData}
     */
    CARAPI NormalizeScheme(
        /* [out] */ IUri** result);

    /**
     * Reads Uris from Parcels.
     */
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    /**
     * Writes a Uri to a Parcel.
     *
     * @param out parcel to write to
     * @param uri to write, can be null
     */
    CARAPI WriteToParcel(
        /* [in] */ IParcel* out);

    static CARAPI WriteToParcel(
        /* [in] */ IParcel* parcel,
        /* [in] */ IUri* data);

    static CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel,
        /* [out] */ IUri** data);

    /**
     * Encodes characters in the given string as '%'-escaped octets
     * using the UTF-8 scheme. Leaves letters ("A-Z", "a-z"), numbers
     * ("0-9"), and unreserved characters ("_-!.~'()*") intact. Encodes
     * all other characters.
     *
     * @param s string to encode
     * @return an encoded version of s suitable for use as a URI component,
     *  or null if s is null
     */
    static CARAPI Encode(
        /* [in] */ const String& s,
        /* [out] */ String* result);

    /**
     * Encodes characters in the given string as '%'-escaped octets
     * using the UTF-8 scheme. Leaves letters ("A-Z", "a-z"), numbers
     * ("0-9"), and unreserved characters ("_-!.~'()*") intact. Encodes
     * all other characters with the exception of those specified in the
     * allow argument.
     *
     * @param s string to encode
     * @param allow set of additional characters to allow in the encoded form,
     *  null if no characters should be skipped
     * @return an encoded version of s suitable for use as a URI component,
     *  or null if s is null
     */
    static CARAPI Encode(
        /* [in] */ const String& s,
        /* [in] */ const String& allow,
        /* [out] */ String* result);

    /**
     * Decodes '%'-escaped octets in the given string using the UTF-8 scheme.
     * Replaces invalid octets with the unicode replacement character
     * ("\\uFFFD").
     *
     * @param s encoded string to decode
     * @return the given string with escaped octets decoded, or null if
     *  s is null
     */
    static CARAPI Decode(
        /* [in] */ const String& s,
        /* [out] */ String* result);

    /**
     * Creates a new Uri by appending an already-encoded path segment to a
     * base Uri.
     *
     * @param baseUri Uri to append path segment to
     * @param pathSegment encoded path segment to append
     * @return a new Uri based on baseUri with the given segment appended to
     *  the path
     * @throws NullPointerException if baseUri is null
     */
    static CARAPI WithAppendedPath(
        /* [in] */ IUri* baseUri,
        /* [in] */ const String& pathSegment,
        /* [out] */ IUri** result);

    /**
     * If this {@link Uri} is {@code file://}, then resolve and return its
     * canonical path. Also fixes legacy emulated storage paths so they are
     * usable across user boundaries. Should always be called from the app
     * process before sending elsewhere.
     *
     * @hide
     */
    CARAPI GetCanonicalUri(
        /* [out] */ IUri** result);

    /**
     * If this is a {@code file://} Uri, it will be reported to
     * {@link StrictMode}.
     *
     * @hide
     */
    CARAPI CheckFileUriExposed(
        /* [in] */ const String& location);

    /**
     * Test if this is a path prefix match against the given Uri. Verifies that
     * scheme, authority, and atomic path segments match.
     *
     * @hide
     */
    CARAPI IsPathPrefixMatch(
        /* [in] */ IUri* prefix,
        /* [out] */ Boolean* result);

    static CARAPI GetEMPTY(
            /* [out] */ IUri** result);

private:
    static CARAPI_(AutoPtr<IUri>) CreateEmpty();

    /**
     * Prevents external subclassing.
     */
    Uri(){}

private:
    /**
     * Returns true if the given character is allowed.
     *
     * @param c character to check
     * @param allow characters to allow
     * @return true if the character is allowed or false if it should be
     *  encoded
     */
    static CARAPI_(Boolean) IsAllowed(
        /* [in] */ Char32 c,
        /* [in] */ const String& allow);

private:
    /** Log tag. */
    static const String LOG;

    static const Char32 HEX_DIGITS[];

    /**
     * NOTE: EMPTY accesses this field during its own initialization, so this
     * field *must* be initialized first, or else EMPTY will see a null value!
     *
     * Placeholder for strings which haven't been cached. This enables us
     * to cache null. We intentionally create a new String instance so we can
     * compare its identity and there is no chance we will confuse it with
     * user data.
     */
    // @SuppressWarnings("RedundantStringConstructorCall")
    static const String NOT_CACHED;

    /**
     * The empty URI, equivalent to "".
     */
    static const AutoPtr<IUri> EMPTY;

    /** Index of a component which was not found. */
    static const Int32 NOT_FOUND;

    /** Placeholder value for an index which hasn't been calculated yet. */
    static const Int32 NOT_CALCULATED;

    /**
     * Error message presented when a user tries to treat an opaque URI as
     * hierarchical.
     */
    static const String NOT_HIERARCHICAL;

    /** Default encoding. */
    static const String DEFAULT_ENCODING;

    friend class StringUri;
    friend class AbstractHierarchicalUri;
    friend class OpaqueUri;
    friend class HierarchicalUri;
};

/**
 * Support for hierarchical URIs.
 */
class AbstractHierarchicalUri : public Uri
{
public:
    AbstractHierarchicalUri();

    virtual CARAPI GetLastPathSegment(
        /* [out] */ String* pathSegment);

    virtual CARAPI GetEncodedUserInfo(
        /* [out] */ String* userInfo);

    virtual CARAPI GetUserInfo(
        /* [out] */ String* userInfo);

    virtual CARAPI GetHost(
        /* [out] */ String* host);

    virtual CARAPI GetPort(
        /* [out] */ Int32* port);

private:
    CARAPI_(AutoPtr<Uri::Part>) GetUserInfoPart();

    CARAPI_(String) ParseUserInfo();

    CARAPI_(String) ParseHost();

    CARAPI_(Int32) ParsePort();

private:
    AutoPtr<Uri::Part> mUserInfo;
    String mHost;
    Int32 mPort;
};

/**
 * An implementation which wraps a String URI. This URI can be opaque or
 * hierarchical, but we extend AbstractHierarchicalUri in case we need
 * the hierarchical functionality.
 */
class StringUri
    : public AbstractHierarchicalUri
    , public IStringUri
{
public:
    StringUri();

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& uriString);

    static CARAPI ReadFrom(
        /* [in] */ IParcel* parcel,
        /* [out] */ IUri** uri);

    virtual CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    virtual CARAPI WriteToParcel(
        /* [in] */ IParcel* parcel);

    virtual CARAPI IsHierarchical(
        /* [out] */ Boolean* isHierarchical);

    virtual CARAPI IsRelative(
        /* [out] */ Boolean* isRelative);

    virtual CARAPI GetScheme(
        /* [out] */ String* scheme);

    virtual CARAPI GetEncodedSchemeSpecificPart(
        /* [out] */ String* essp);

    virtual CARAPI GetSchemeSpecificPart(
        /* [out] */ String* ssp);

    virtual CARAPI GetEncodedAuthority(
        /* [out] */ String* authority);

    virtual CARAPI GetAuthority(
        /* [out] */ String* authority);

    virtual CARAPI GetPath(
        /* [out] */ String* path);

    virtual CARAPI GetEncodedPath(
        /* [out] */ String* path);

    virtual CARAPI GetPathSegments(
        /* [out, callee] */ IList** pathSegments);

    virtual CARAPI GetEncodedQuery(
        /* [out] */ String* query);

    virtual CARAPI GetQuery(
        /* [out] */ String* query);

    virtual CARAPI GetEncodedFragment(
        /* [out] */ String* fragment);

    virtual CARAPI GetFragment(
        /* [out] */ String* fragment);

    virtual CARAPI ToString(
        /* [out] */ String* result);

    /**
     * Parses an authority out of the given URI string.
     *
     * @param uriString URI string
     * @param ssi scheme separator index, -1 for a relative URI
     *
     * @return the authority or null if none is found
     */
    static CARAPI_(String) ParseAuthority(
        /* [in] */ const String& uriString,
        /* [in] */ Int32 ssi);

    /**
     * Parses a path out of this given URI string.
     *
     * @param uriString URI string
     * @param ssi scheme separator index, -1 for a relative URI
     *
     * @return the path
     */
    static CARAPI_(String) ParsePath(
        /* [in] */ const String& uriString,
        /* [in] */ Int32 ssi);

    virtual CARAPI BuildUpon(
        /* [out] */ IUriBuilder** result);

private:
    /** Finds the first ':'. Returns -1 if none found. */
    CARAPI_(Int32) FindSchemeSeparator();

    /** Finds the first '#'. Returns -1 if none found. */
    CARAPI_(Int32) FindFragmentSeparator();

    CARAPI_(String) ParseScheme();

    CARAPI_(AutoPtr<Uri::Part>) GetSsp();

    CARAPI_(String) ParseSsp();

    CARAPI_(AutoPtr<Uri::Part>) GetAuthorityPart();

    CARAPI_(AutoPtr<Uri::PathPart>) GetPathPart();

    CARAPI_(String) ParsePath();

    CARAPI_(AutoPtr<Uri::Part>) GetQueryPart();

    CARAPI_(String) ParseQuery();

    CARAPI_(AutoPtr<Uri::Part>) GetFragmentPart();

    CARAPI_(String) ParseFragment();

protected:
    /** URI string representation. */
    String mUriString;

    /** Cached scheme separator index. */
    /* volatile */ Int32 mCachedSsi;// = NOT_CALCULATED;

    /** Cached fragment separator index. */
    /* volatile */ Int32 mCachedFsi;// = NOT_CALCULATED;

    /* volatile */ String mScheme;
    AutoPtr<Uri::Part> mSsp;
    AutoPtr<Uri::Part> mAuthority;
    AutoPtr<Uri::PathPart> mPath;
    AutoPtr<Uri::Part> mQuery;
    AutoPtr<Uri::Part> mFragment;

    friend class Uri;
};

class OpaqueUri
    : public Uri
    , public IOpaqueUri
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& scheme,
        /* [in] */ Handle32 ssp,
        /* [in] */ Handle32 fragment);

    static CARAPI ReadFrom(
        /* [in] */ IParcel* parcel,
        /* [out] */ IUri** uri);

    virtual CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    virtual CARAPI WriteToParcel(
        /* [in] */ IParcel* parcel);

    virtual CARAPI IsHierarchical(
        /* [out] */ Boolean* isHierarchical);

    virtual CARAPI IsRelative(
        /* [out] */ Boolean* isRelative);

    virtual CARAPI GetScheme(
        /* [out] */ String* scheme);

    virtual CARAPI GetEncodedSchemeSpecificPart(
        /* [out] */ String* essp);

    virtual CARAPI GetSchemeSpecificPart(
        /* [out] */ String* ssp);

    virtual CARAPI GetEncodedAuthority(
        /* [out] */ String* authority);

    virtual CARAPI GetAuthority(
        /* [out] */ String* authority);

    virtual CARAPI GetPath(
        /* [out] */ String* path);

    virtual CARAPI GetEncodedPath(
        /* [out] */ String* path);

    virtual CARAPI GetQuery(
        /* [out] */ String* query);

    virtual CARAPI GetEncodedQuery(
        /* [out] */ String* query);

    virtual CARAPI GetFragment(
        /* [out] */ String* fragment);

    virtual CARAPI GetEncodedFragment(
        /* [out] */ String* fragment);

    virtual CARAPI GetPathSegments(
        /* [out] */ IList** pathSegments);

    virtual CARAPI GetLastPathSegment(
        /* [out] */ String* fragment);

    virtual CARAPI GetUserInfo(
        /* [out] */ String* fragment);

    virtual CARAPI GetEncodedUserInfo(
        /* [out] */ String* fragment);

    virtual CARAPI GetHost(
        /* [out] */ String* fragment);

    virtual CARAPI GetPort(
        /* [out] */ Int32* fragment);

    virtual CARAPI ToString(
        /* [out] */ String* result);

    virtual CARAPI BuildUpon(
        /* [out] */ IUriBuilder** result);

private:
    String mScheme;
    AutoPtr<Uri::Part> mSsp;
    AutoPtr<Uri::Part> mFragment;
    String mCachedString;

    friend class UriBuilder;
    friend class Uri;
};

class HierarchicalUri
    : public AbstractHierarchicalUri
    , public IHierarchicalUri
{
    friend class UriBuilder;
    friend class Uri;

public:
    HierarchicalUri();

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& scheme,
        /* [in] */ Handle32 authority,
        /* [in] */ Handle32 path,
        /* [in] */ Handle32 query,
        /* [in] */ Handle32 fragment);

    static CARAPI ReadFrom(
        /* [in] */ IParcel* parcel,
        /* [out] */ IUri** result);

    virtual CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    virtual CARAPI WriteToParcel(
        /* [in] */ IParcel* parcel);

    virtual CARAPI IsHierarchical(
        /* [out] */ Boolean* isHierarchical);

    virtual CARAPI IsRelative(
        /* [out] */ Boolean* isRelative);

    virtual CARAPI GetScheme(
        /* [out] */ String* scheme);

    virtual CARAPI GetEncodedSchemeSpecificPart(
        /* [out] */ String* part);

    virtual CARAPI GetSchemeSpecificPart(
        /* [out] */ String* part);

    virtual CARAPI GetAuthority(
        /* [out] */ String* authority);

    virtual CARAPI GetEncodedAuthority(
        /* [out] */ String* authority);

    virtual CARAPI GetEncodedPath(
        /* [out] */ String* path);

    virtual CARAPI GetPath(
        /* [out] */ String* path);

    virtual CARAPI GetQuery(
        /* [out] */ String* query);

    virtual CARAPI GetEncodedQuery(
        /* [out] */ String* query);

    virtual CARAPI GetFragment(
        /* [out] */ String* fragment);

    virtual CARAPI GetEncodedFragment(
        /* [out] */ String* fragment);

    // return a list of string
    virtual CARAPI GetPathSegments(
        /* [out] */ IList** segments);

    virtual CARAPI ToString(
        /* [out] */ String* info);

    virtual CARAPI BuildUpon(
        /* [out] */ IUriBuilder** builder);

private:
    CARAPI_(AutoPtr<Part>) GetSsp();

    /**
     * Creates the encoded scheme-specific part from its sub parts.
     */
    CARAPI_(String) MakeSchemeSpecificPart();

    CARAPI AppendSspTo(StringBuilder& builder);

    CARAPI_(String) MakeUriString();

private:
    String mScheme; // can be null
    AutoPtr<Uri::Part> mAuthority;
    AutoPtr<Uri::PathPart> mPath;
    AutoPtr<Uri::Part> mQuery;
    AutoPtr<Uri::Part> mFragment;

    AutoPtr<Uri::Part> mSsp;
    String mUriString;// = NOT_CACHED;;
};

/**
 * Helper class for building or manipulating URI references. Not safe for
 * concurrent use.
 *
 * <p>An absolute hierarchical URI reference follows the pattern:
 * {@code <scheme>://<authority><absolute path>?<query>#<fragment>}
 *
 * <p>Relative URI references (which are always hierarchical) follow one
 * of two patterns: {@code <relative or absolute path>?<query>#<fragment>}
 * or {@code //<authority><absolute path>?<query>#<fragment>}
 *
 * <p>An opaque URI follows this pattern:
 * {@code <scheme>:<opaque part>#<fragment>}
 *
 * <p>Use {@link Uri#buildUpon()} to obtain a builder representing an existing URI.
 */
class UriBuilder
    : public Object
    , public IUriBuilder
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Constructs a new Builder.
     */
    CARAPI constructor();

    /**
     * Sets the scheme.
     *
     * @param scheme name or {@code null} if this is a relative Uri
     */
    CARAPI Scheme(
        /* [in] */ const String& scheme);

    CARAPI OpaquePart(
        /* [in] */ Uri::Part* opaquePart);

    /**
     * Encodes and sets the given opaque scheme-specific-part.
     *
     * @param opaquePart decoded opaque part
     */
    CARAPI OpaquePart(
        /* [in] */ const String& opaquePart);

    /**
     * Sets the previously encoded opaque scheme-specific-part.
     *
     * @param opaquePart encoded opaque part
     */
    CARAPI EncodedOpaquePart(
        /* [in] */ const String& opaquePart);

    CARAPI Authority(
        /* [in] */ Uri::Part* authority);

    /**
     * Encodes and sets the authority.
     */
    CARAPI Authority(
        /* [in] */ const String& authority);

    /**
     * Sets the previously encoded authority.
     */
    CARAPI EncodedAuthority(
        /* [in] */ const String& authority);

    CARAPI Path(
        /* [in] */ Uri::PathPart* path);

    /**
     * Sets the path. Leaves '/' characters intact but encodes others as
     * necessary.
     *
     * <p>If the path is not null and doesn't start with a '/', and if
     * you specify a scheme and/or authority, the builder will prepend the
     * given path with a '/'.
     */
    CARAPI Path(
        /* [in] */ const String& path);

    /**
     * Sets the previously encoded path.
     *
     * <p>If the path is not null and doesn't start with a '/', and if
     * you specify a scheme and/or authority, the builder will prepend the
     * given path with a '/'.
     */
    CARAPI EncodedPath(
        /* [in] */ const String& path);

    /**
     * Encodes the given segment and appends it to the path.
     */
    CARAPI AppendPath(
        /* [in] */ const String& newSegment);

    /**
     * Appends the given segment to the path.
     */
    CARAPI AppendEncodedPath(
        /* [in] */ const String& newSegment);

    CARAPI Query(
        /* [in] */ Uri::Part* query);

    /**
     * Encodes and sets the query.
     */
    CARAPI Query(
        /* [in] */ const String& query);

    /**
     * Sets the previously encoded query.
     */
    CARAPI EncodedQuery(
        /* [in] */ const String& query);

    CARAPI Fragment(
        /* [in] */ Uri::Part* fragment);

    /**
     * Encodes and sets the fragment.
     */
    CARAPI Fragment(
        /* [in] */ const String& fragment);

    /**
     * Sets the previously encoded fragment.
     */
    CARAPI EncodedFragment(
        /* [in] */ const String& fragment);

    /**
     * Encodes the key and value and then appends the parameter to the
     * query string.
     *
     * @param key which will be encoded
     * @param value which will be encoded
     */
    CARAPI AppendQueryParameter(
        /* [in] */ const String& key,
        /* [in] */ const String& value);

    /**
     * Clears the the previously set query.
     */
    CARAPI ClearQuery();

    /**
     * Constructs a Uri with the current attributes.
     *
     * @throws UnsupportedOperationException if the URI is opaque and the
     *  scheme is null
     */
    CARAPI Build(
        /* [out] */ IUri** result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

private:
    CARAPI HasSchemeOrAuthority(
        /* [out] */ Boolean* result);

private:
    String mScheme;
    AutoPtr<Uri::Part> mOpaquePart;
    AutoPtr<Uri::Part> mAuthority;
    AutoPtr<Uri::PathPart> mPath;
    AutoPtr<Uri::Part> mQuery;
    AutoPtr<Uri::Part> mFragment;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_URI_H__
