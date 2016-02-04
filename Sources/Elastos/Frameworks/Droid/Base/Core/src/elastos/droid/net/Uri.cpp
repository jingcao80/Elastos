
#include "_Elastos.Droid.Os.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Libcore.h>
#include <Elastos.CoreLibrary.Net.h>
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/net/CUriBuilder.h"
#include "elastos/droid/net/ReturnOutValue.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/os/Build.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Objects.h>
#include <elastos/utility/etl/HashSet.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::Build;
// using Elastos::Droid::Os::CEnvironment;
// using Elastos::Droid::Os::CStrictMode;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::IStrictMode;
using Elastos::Droid::Utility::ILog;

using Elastos::Core::CString;
using Elastos::Core::EIID_IComparable;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::Charset::CStandardCharsets;
using Elastos::IO::Charset::ICharset;
using Elastos::IO::Charset::IStandardCharsets;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::Net::CURLEncoder;
using Elastos::Net::IURLEncoder;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::CLinkedHashSet;
using Elastos::Utility::EIID_IRandomAccess;
using Elastos::Utility::Etl::HashSet;
using Elastos::Utility::Etl::List;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::ICollections;
using Elastos::Utility::ILinkedHashSet;
using Elastos::Utility::IList;
using Elastos::Utility::ILocale;
using Elastos::Utility::IRandom;
using Elastos::Utility::IRandomAccess;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Objects;

using Libcore::Net::CUriCodecHelper;
using Libcore::Net::IUriCodec;
using Libcore::Net::IUriCodecHelper;

namespace Elastos {
namespace Droid {
namespace Net {

//====================================================================
//                    StringUri
//====================================================================
StringUri::StringUri()
    : mCachedSsi(Uri::NOT_CALCULATED)
    , mCachedFsi(Uri::NOT_CALCULATED)
    , mScheme(Uri::NOT_CACHED)
{}

ECode StringUri::constructor(
    /* [in] */ const String& uriString)
{
    if (uriString.IsNull()) {
        Logger::E("Uri", "uriString");
        return E_NULL_POINTER_EXCEPTION;
    }

    mUriString = uriString;
    return NOERROR;
}

ECode StringUri::ReadFrom(
    /* [in] */ IParcel* parcel,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = NULL;

    String str;
    parcel->ReadString(&str);

    AutoPtr<StringUri> curi = new StringUri();
    curi->constructor(str);
    *uri = IUri::Probe(curi);
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode StringUri::WriteToParcel(
    /* [in] */ IParcel* parcel)
{
    parcel->WriteInt32(TYPE_ID);
    parcel->WriteString(mUriString);
    return NOERROR;
}

ECode StringUri::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    Int32 id;
    parcel->ReadInt32(&id);
    parcel->ReadString(&mUriString);
    return NOERROR;
}

Int32 StringUri::FindSchemeSeparator()
{
    return mCachedSsi == Uri::NOT_CALCULATED
            ? mCachedSsi = mUriString.IndexOf(':')
            : mCachedSsi;
}

Int32 StringUri::FindFragmentSeparator()
{
    return mCachedFsi == Uri::NOT_CALCULATED
            ? mCachedFsi = mUriString.IndexOf('#', FindSchemeSeparator())
            : mCachedFsi;
}

ECode StringUri::IsHierarchical(
    /* [out] */ Boolean* isHierarchical)
{
    VALIDATE_NOT_NULL(isHierarchical);

    Int32 ssi = FindSchemeSeparator();
    if (ssi == Uri::NOT_FOUND) {
        // All relative URIs are hierarchical.
        *isHierarchical = TRUE;
        return NOERROR;
    }

    if (mUriString.GetLength() == (UInt32)(ssi + 1)) {
        // No ssp.
        *isHierarchical = FALSE;
        return NOERROR;
    }

    // If the ssp starts with a '/', this is hierarchical.
    *isHierarchical = mUriString.GetChar(ssi + 1) == '/';
    return NOERROR;
}

ECode StringUri::IsRelative(
    /* [out] */ Boolean* isRelative)
{
    VALIDATE_NOT_NULL(isRelative);

    // Note: We return true if the index is 0
    *isRelative = FindSchemeSeparator() == NOT_FOUND;
    return NOERROR;
}

ECode StringUri::GetScheme(
    /* [out] */ String* scheme)
{
    VALIDATE_NOT_NULL(scheme);

    Boolean cached = (!mScheme.Equals(NOT_CACHED));
    *scheme = cached ? mScheme : (mScheme = ParseScheme());
    return NOERROR;
}

String StringUri::ParseScheme()
{
    Int32 ssi = FindSchemeSeparator();
    return ssi == NOT_FOUND ? String(NULL) : mUriString.Substring(0, ssi);
}

AutoPtr<Uri::Part> StringUri::GetSsp()
{
    if (mSsp == NULL) {
        mSsp = Part::FromEncoded(ParseSsp());
    }
    return mSsp;
}

ECode StringUri::GetEncodedSchemeSpecificPart(
    /* [out] */ String* essp)
{
    VALIDATE_NOT_NULL(essp);
    *essp = NULL;

    AutoPtr<Uri::Part> part = GetSsp();
    if (part) {
        *essp = part->GetEncoded();
    }
    return NOERROR;
}

ECode StringUri::GetSchemeSpecificPart(
    /* [out] */ String* ssp)
{
    VALIDATE_NOT_NULL(ssp);
    *ssp = NULL;

    AutoPtr<Uri::Part> part = GetSsp();
    if (part) {
        *ssp = part->GetDecoded();
    }
    return NOERROR;
}

String StringUri::ParseSsp()
{
    Int32 ssi = FindSchemeSeparator();
    Int32 fsi = FindFragmentSeparator();

    // Return everything between ssi and fsi.
    return fsi == NOT_FOUND
            ? mUriString.Substring(ssi + 1)
            : mUriString.Substring(ssi + 1, fsi);
}

AutoPtr<Uri::Part> StringUri::GetAuthorityPart()
{
    if (mAuthority == NULL) {
        String encodedAuthority = ParseAuthority(mUriString, FindSchemeSeparator());
        return mAuthority = Part::FromEncoded(encodedAuthority);
    }

    return mAuthority;
}

ECode StringUri::GetEncodedAuthority(
    /* [out] */ String* authority)
{
    VALIDATE_NOT_NULL(authority);
    *authority = NULL;

    AutoPtr<Uri::Part> part = GetAuthorityPart();
    if (part) {
        *authority = part->GetEncoded();
    }
    return NOERROR;
}

ECode StringUri::GetAuthority(
    /* [out] */ String* authority)
{
    VALIDATE_NOT_NULL(authority);

    AutoPtr<Uri::Part> part = GetAuthorityPart();
    assert(part != NULL);
    *authority = part->GetDecoded();
    return NOERROR;
}

AutoPtr<Uri::PathPart> StringUri::GetPathPart()
{
    if (mPath == NULL) {
        mPath = PathPart::FromEncoded(ParsePath());
    }
    return mPath;
}

ECode StringUri::GetPath(
    /* [out] */ String* path)
{
    VALIDATE_NOT_NULL(path);
    *path = NULL;

    AutoPtr<Uri::PathPart> part = GetPathPart();
    if (part) {
        *path = part->GetDecoded();
    }
    return NOERROR;
}

ECode StringUri::GetEncodedPath(
    /* [out] */ String* path)
{
    VALIDATE_NOT_NULL(path);
    *path = NULL;

    AutoPtr<Uri::PathPart> part = GetPathPart();
    if (part) {
        *path = part->GetEncoded();
    }
    return NOERROR;
}

ECode StringUri::GetPathSegments(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<Uri::PathPart> part = GetPathPart();
    AutoPtr<PathSegments> segments = part->GetPathSegments();

    FUNC_RETURN(IList::Probe(segments))
}

String StringUri::ParsePath()
{
    String uriString = mUriString;
    Int32 ssi = FindSchemeSeparator();

    // If the URI is absolute.
    if (ssi > -1) {
        AutoPtr<ArrayOf<Char32> > charArray = uriString.GetChars();
        // Is there anything after the ':'?
        Boolean schemeOnly = (UInt32)(ssi + 1) == charArray->GetLength();
        if (schemeOnly) {
            // Opaque URI.
            return String(NULL);
        }

        // A '/' after the ':' means this is hierarchical.
        if ((*charArray)[ssi + 1] != '/') {
            // Opaque URI.
            return String(NULL);
        }
    }
    else {
        // All relative URIs are hierarchical.
    }

    return ParsePath(uriString, ssi);
}

AutoPtr<Uri::Part> StringUri::GetQueryPart()
{
    if (mQuery == NULL) {
        mQuery = Part::FromEncoded(ParseQuery());
    }
    return mQuery;
}

ECode StringUri::GetEncodedQuery(
    /* [out] */ String* query)
{
    VALIDATE_NOT_NULL(query);
    *query = NULL;

    AutoPtr<Uri::Part> part = GetQueryPart();
    if (part) {
        *query = part->GetEncoded();
    }
    return NOERROR;
}

String StringUri::ParseQuery()
{
    // It doesn't make sense to cache this index. We only ever
    // calculate it once.
    Int32 qsi = mUriString.IndexOf('?', FindSchemeSeparator());
    if (qsi == NOT_FOUND) {
        return String(NULL);
    }

    Int32 fsi = FindFragmentSeparator();
    if (fsi == NOT_FOUND) {
        return mUriString.Substring(qsi + 1);
    }

    if (fsi < qsi) {
        // Invalid.
        return String(NULL);
    }

    return mUriString.Substring(qsi + 1, fsi);
}

ECode StringUri::GetQuery(
    /* [out] */ String* query)
{
    VALIDATE_NOT_NULL(query);
    *query = NULL;

    AutoPtr<Uri::Part> part = GetQueryPart();
    if (part) {
        *query = part->GetDecoded();
    }
    return NOERROR;
}

AutoPtr<Uri::Part> StringUri::GetFragmentPart()
{
    if (mFragment == NULL) {
        mFragment = Part::FromEncoded(ParseFragment());
    }
    return mFragment;
}

ECode StringUri::GetEncodedFragment(
    /* [out] */ String* fragment)
{
    VALIDATE_NOT_NULL(fragment);
    *fragment = NULL;

    AutoPtr<Uri::Part> part = GetFragmentPart();
    if (part) {
        *fragment = part->GetEncoded();
    }
    return NOERROR;
}

String StringUri::ParseFragment()
{
    Int32 fsi = FindFragmentSeparator();
    return fsi == Uri::NOT_FOUND ?
        String(NULL) : mUriString.Substring(fsi + 1);
}

ECode StringUri::GetFragment(
    /* [out] */ String* fragment)
{
    VALIDATE_NOT_NULL(fragment);
    *fragment = NULL;

    AutoPtr<Uri::Part> part = GetFragmentPart();
    if (part) {
        *fragment = part->GetDecoded();
    }
    return NOERROR;
}

ECode StringUri::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mUriString;
    return NOERROR;
}

String StringUri::ParseAuthority(
    /* [in] */ const String& uriString,
    /* [in] */ Int32 ssi)
{
    AutoPtr<ArrayOf<Char32> > charArray = uriString.GetChars();
    Int32 length = charArray->GetLength();

    // If "//" follows the scheme separator, we have an authority.
    if (length > ssi + 2
            && (*charArray)[ssi + 1] == '/'
            && (*charArray)[ssi + 2] == '/') {
        // We have an authority.

        // Look for the start of the path, query, or fragment, or the
        // end of the string.
        Int32 end = ssi + 3;
        while (end < length) {
            switch ((*charArray)[end]) {
                case '/': // Start of path
                case '?': // Start of query
                case '#': // Start of fragment
                    goto LOOP;
            }
            end++;
        }
LOOP:
        return uriString.Substring(ssi + 3, end);
    }
    else {
        return String(NULL);
    }
}

String StringUri::ParsePath(
    /* [in] */ const String& uriString,
    /* [in] */ Int32 ssi)
{
    AutoPtr<ArrayOf<Char32> > charArray = uriString.GetChars();
    Int32 length = charArray->GetLength();

    // Find start of path.
    Int32 pathStart, pathEnd;
    if (length > ssi + 2
            && (*charArray)[ssi + 1] == '/'
            && (*charArray)[ssi + 2] == '/') {
        // Skip over authority to path.
        pathStart = ssi + 3;
        while (pathStart < length) {
            switch ((*charArray)[pathStart]) {
                case '?': // Start of query
                case '#': // Start of fragment
                    return String(""); // Empty path.
                case '/': // Start of path!
                    goto LOOP1;
            }
            pathStart++;
        }
    }
    else {
        // Path starts immediately after scheme separator.
        pathStart = ssi + 1;
    }
LOOP1:

    // Find end of path.
    pathEnd = pathStart;
    while (pathEnd < length) {
        switch ((*charArray)[pathEnd]) {
            case '?': // Start of query
            case '#': // Start of fragment
                goto LOOP2;
        }
        pathEnd++;
    }
LOOP2:

    return uriString.Substring(pathStart, pathEnd);
}

ECode StringUri::BuildUpon(
    /* [out] */ IUriBuilder** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    String str;
    GetScheme(&str);

    AutoPtr<UriBuilder> builder = new UriBuilder();
    builder->constructor();
    builder->Scheme(str);

    Boolean isHierarchical;
    IsHierarchical(&isHierarchical);

    if (isHierarchical) {
        AutoPtr<Uri::Part> p = GetAuthorityPart();
        builder->Authority(p.Get());
        AutoPtr<Uri::PathPart> pp = GetPathPart();
        builder->Path(pp.Get());
        p = GetQueryPart();
        builder->Query(p.Get());
        p = GetFragmentPart();
        builder->Fragment(p.Get());
    }
    else {
        AutoPtr<Uri::Part> p = GetSsp();
        builder->OpaquePart(p.Get());
        p = GetFragmentPart();
        builder->Fragment(p.Get());
    }

    *result = IUriBuilder::Probe(builder);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//====================================================================
//                    OpaqueUri
//====================================================================
ECode OpaqueUri::constructor(
    /* [in] */ const String& scheme,
    /* [in] */ Uri::Part* ssp,
    /* [in] */ Uri::Part* fragment)
{
    mScheme = scheme;
    mSsp = (Part*)ssp;
    mFragment = (Part*)fragment;

    if (mFragment == NULL) {
        mFragment = Part::sNULL;
    }

    return NOERROR;
}

ECode OpaqueUri::ReadFrom(
    /* [in] */ IParcel* parcel,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    String str;
    parcel->ReadString(&str);

    AutoPtr<Uri::Part> p1, p2;
    Uri::Part::ReadFrom(parcel, (Uri::Part**)&p1);
    Uri::Part::ReadFrom(parcel, (Uri::Part**)&p2);

    AutoPtr<OpaqueUri> curi = new OpaqueUri();
    curi->constructor(str, p1, p2);
    *result = IUri::Probe(curi);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode OpaqueUri::WriteToParcel(
    /* [in] */ IParcel* parcel)
{
    VALIDATE_NOT_NULL(parcel);
    parcel->WriteInt32(OpaqueUri::TYPE_ID);
    parcel->WriteString(mScheme);

    assert(mSsp != NULL);
    mSsp->WriteTo(parcel);
    assert(mFragment != NULL);
    mFragment->WriteTo(parcel);
    return NOERROR;
}

ECode OpaqueUri::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    VALIDATE_NOT_NULL(parcel);
    Int32 id;
    parcel->ReadInt32(&id);
    parcel->ReadString(&mScheme);

    mSsp = NULL;
    Uri::Part::ReadFrom(parcel, (Uri::Part**)&mSsp);
    mFragment = NULL;
    Uri::Part::ReadFrom(parcel, (Uri::Part**)&mFragment);
    return NOERROR;
}

ECode OpaqueUri::IsHierarchical(
    /* [out] */ Boolean* isHierarchical)
{
    VALIDATE_NOT_NULL(isHierarchical);

    *isHierarchical = FALSE;
    return NOERROR;
}

ECode OpaqueUri::IsRelative(
    /* [out] */ Boolean* isRelative)
{
    VALIDATE_NOT_NULL(isRelative);

    *isRelative = mScheme.IsNull();
    return NOERROR;
}

ECode OpaqueUri::GetScheme(
    /* [out] */ String* scheme)
{
    VALIDATE_NOT_NULL(scheme);

    *scheme = mScheme;
    return NOERROR;
}

ECode OpaqueUri::GetEncodedSchemeSpecificPart(
    /* [out] */ String* essp)
{
    VALIDATE_NOT_NULL(essp);

    *essp = mSsp->GetEncoded();
    return NOERROR;
}

ECode OpaqueUri::GetSchemeSpecificPart(
    /* [out] */ String* ssp)
{
    VALIDATE_NOT_NULL(ssp);

    *ssp = mSsp->GetDecoded();
    return NOERROR;
}

ECode OpaqueUri::GetAuthority(
    /* [out] */ String* authority)
{
    VALIDATE_NOT_NULL(authority);

    *authority = NULL;
    return NOERROR;
}

ECode OpaqueUri::GetEncodedAuthority(
    /* [out] */ String* authority)
{
    VALIDATE_NOT_NULL(authority);

    *authority = NULL;
    return NOERROR;
}

ECode OpaqueUri::GetPath(
    /* [out] */ String* path)
{
    VALIDATE_NOT_NULL(path);

    *path = NULL;
    return NOERROR;
}

ECode OpaqueUri::GetEncodedPath(
    /* [out] */ String* path)
{
    VALIDATE_NOT_NULL(path);

    *path = NULL;
    return NOERROR;
}

ECode OpaqueUri::GetQuery(
    /* [out] */ String* query)
{
    VALIDATE_NOT_NULL(query);

    *query = NULL;
    return NOERROR;
}

ECode OpaqueUri::GetEncodedQuery(
    /* [out] */ String* query)
{
    VALIDATE_NOT_NULL(query);

    *query = NULL;
    return NOERROR;
}

ECode OpaqueUri::GetFragment(
    /* [out] */ String* fragment)
{
    VALIDATE_NOT_NULL(fragment);

    *fragment = mFragment->GetDecoded();
    return NOERROR;
}

ECode OpaqueUri::GetEncodedFragment(
    /* [out] */ String* fragment)
{
    VALIDATE_NOT_NULL(fragment);

    *fragment = mFragment->GetEncoded();
    return NOERROR;
}

ECode OpaqueUri::GetPathSegments(
    /* [out, callee] */ IList** pathSegments)
{
    VALIDATE_NOT_NULL(pathSegments);

    *pathSegments = NULL;
    return NOERROR;
}

ECode OpaqueUri::GetLastPathSegment(
    /* [out] */ String* pathSegment)
{
    VALIDATE_NOT_NULL(pathSegment);

    *pathSegment = NULL;
    return NOERROR;
}

ECode OpaqueUri::GetUserInfo(
    /* [out] */ String* userInfo)
{
    VALIDATE_NOT_NULL(userInfo);

    *userInfo = NULL;
    return NOERROR;
}

ECode OpaqueUri::GetEncodedUserInfo(
    /* [out] */ String* userInfo)
{
    VALIDATE_NOT_NULL(userInfo);

    *userInfo = NULL;
    return NOERROR;
}

ECode OpaqueUri::GetHost(
    /* [out] */ String* host)
{
    VALIDATE_NOT_NULL(host);

    *host = NULL;
    return NOERROR;
}

ECode OpaqueUri::GetPort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port);

    *port = -1;
    return NOERROR;
}

ECode OpaqueUri::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    // @SuppressWarnings("StringEquality")
    Boolean cached = (!mCachedString.IsNull() && !mCachedString.Equals(Uri::NOT_CACHED));
    if (!cached) {
        StringBuilder sb(mScheme);
        sb += (":");

        String str;
        GetEncodedSchemeSpecificPart(&str);
        sb += str;

        Boolean isEmpty = mFragment->IsEmpty();
        if (!isEmpty) {
            sb += ("#");
            str = mFragment->GetEncoded();
            sb += str;
        }

        mCachedString = sb.ToString();
    }

    *result = mCachedString;
    return NOERROR;
}

ECode OpaqueUri::BuildUpon(
    /* [out] */ IUriBuilder** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<IUriBuilder> builder;
    FAIL_RETURN(CUriBuilder::New((IUriBuilder**)&builder));

    FAIL_RETURN(builder->Scheme(mScheme));
    FAIL_RETURN(((UriBuilder*)builder.Get())->OpaquePart(mSsp));
    FAIL_RETURN(((UriBuilder*)builder.Get())->Fragment(mFragment));
    *result = builder;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//====================================================================================
//              Uri::PathSegments
//====================================================================================
AutoPtr<Uri::PathSegments> Uri::PathSegments::sEMPTY = new Uri::PathSegments(ArrayOf<String>::Alloc(0), 0);

CAR_INTERFACE_IMPL(Uri::PathSegments, AbstractList, IRandomAccess)

Uri::PathSegments::PathSegments(
    /* [in] */ ArrayOf<String>* segments,
    /* [in] */ Int32 size)
    : mSegments(segments)
    , mSize(size)
{}

ECode Uri::PathSegments::Get(
    /* [in] */ Int32 index,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (index >= mSize) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    AutoPtr<ICharSequence> csq;
    CString::New((*mSegments)[index], (ICharSequence**)&csq);
    *result = csq;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Uri::PathSegments::Size(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mSize;
    return NOERROR;
}

//====================================================================================
//              Uri::PathSegmentsBuilder
//====================================================================================

Uri::PathSegmentsBuilder::PathSegmentsBuilder()
    : mSize(0)
{
}

ECode Uri::PathSegmentsBuilder::Add(
    /* [in] */ const String& segment)
{
    mSegments.PushBack(segment);
    mSize++;

    return NOERROR;
}

AutoPtr<Uri::PathSegments> Uri::PathSegmentsBuilder::Build()
{
    if (mSegments.IsEmpty()) {
        return Uri::PathSegments::sEMPTY;
    }

    // try {
    AutoPtr<ArrayOf<String> > segments = ArrayOf<String>::Alloc(mSegments.GetSize());
    if (!segments) {
        return Uri::PathSegments::sEMPTY;
    }

    List<String>::Iterator itor;
    Int32 i = 0;
    for (itor = mSegments.Begin(); itor != mSegments.End(); ++itor, ++i) {
        (*segments)[i] = *itor;
    }

    AutoPtr<Uri::PathSegments> pathSegments = new Uri::PathSegments(segments, mSize);

    // } finally {
    // Makes sure this doesn't get reused.
    mSegments.Clear();
    mSize = 0;
    // }

    return pathSegments;
}

//====================================================================
//                    AbstractHierarchicalUri
//====================================================================
AbstractHierarchicalUri::AbstractHierarchicalUri()
    : mHost(NOT_CACHED)
    , mPort(NOT_CALCULATED)
{
}

ECode AbstractHierarchicalUri::GetLastPathSegment(
    /* [out] */ String* pathSegment)
{
    VALIDATE_NOT_NULL(pathSegment);
    *pathSegment = NULL;

    AutoPtr<IList> segments;
    GetPathSegments((IList**)&segments);
    if (segments == NULL) {
        return NOERROR;
    }
    else if (Ptr(segments)->Func(segments->GetSize) == 0) {
        return NOERROR;
    }

    Int32 size;
    segments->GetSize(&size);
    AutoPtr<IInterface> obj;
    segments->Get(size - 1, (IInterface**)&obj);
    ICharSequence::Probe(obj)->ToString(pathSegment);
    return NOERROR;
}

AutoPtr<Uri::Part> AbstractHierarchicalUri::GetUserInfoPart()
{
    if (mUserInfo == NULL) {
        String result = ParseUserInfo();
        mUserInfo = Uri::Part::FromEncoded(result);
    }

    return mUserInfo;
}

ECode AbstractHierarchicalUri::GetEncodedUserInfo(
    /* [out] */ String* userInfo)
{
    VALIDATE_NOT_NULL(userInfo);

    *userInfo = GetUserInfoPart()->GetEncoded();
    return NOERROR;
}

String AbstractHierarchicalUri::ParseUserInfo()
{
    String authority;
    GetEncodedAuthority(&authority);
    if (authority.IsNull()) {
        return String(NULL);
    }

    Int32 end = authority.IndexOf('@');
    if (end == Uri::NOT_FOUND) {
        return String(NULL);
    }
    else {
        return authority.Substring(0, end);
    }
}

ECode AbstractHierarchicalUri::GetUserInfo(
    /* [out] */ String* userInfo)
{
    VALIDATE_NOT_NULL(userInfo);

    *userInfo = GetUserInfoPart()->GetDecoded();
    return NOERROR;
}

ECode AbstractHierarchicalUri::GetHost(
    /* [out] */ String* host)
{
    VALIDATE_NOT_NULL(host);

    Boolean cached = !mHost.Equals(Uri::NOT_CACHED);

    if (!cached) {
        mHost = ParseHost();
    }

    *host = mHost;
    return NOERROR;
}

String AbstractHierarchicalUri::ParseHost()
{
    String authority;
    GetEncodedAuthority(&authority);
    if (authority.IsNull()) {
        return String(NULL);

    }

    // Parse out user info and then port.
    Int32 userInfoSeparator = authority.IndexOf('@');
    Int32 portSeparator = authority.IndexOf(':', userInfoSeparator);

    String encodedHost = portSeparator == Uri::NOT_FOUND
        ? authority.Substring(userInfoSeparator + 1)
        : authority.Substring(userInfoSeparator + 1, portSeparator);

    String result;
    Decode(encodedHost, &result);
    return result;
}

ECode AbstractHierarchicalUri::GetPort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port);

    if (mPort == Uri::NOT_CALCULATED) {
        mPort = ParsePort();
    }
    *port = mPort;
    return NOERROR;
}

Int32 AbstractHierarchicalUri::ParsePort()
{
    String authority;
    GetEncodedAuthority(&authority);
    if (authority.IsNull()) {
        return -1;
    }

    // Make sure we look for the port separtor *after* the user info
    // separator. We have URLs with a ':' in the user info.
    Int32 userInfoSeparator = authority.IndexOf('@');
    Int32 portSeparator = authority.IndexOf(':', userInfoSeparator);

    if (portSeparator == Uri::NOT_FOUND) {
        return -1;
    }

    String portString;
    Decode(authority.Substring(portSeparator + 1), &portString);

    Int32 result;
    result = StringUtils::ParseInt32(portString);
    return result;
}

//====================================================================
//                      HierarchicalUri
//====================================================================
HierarchicalUri::HierarchicalUri()
{
    mUriString = Uri::NOT_CACHED;
}

ECode HierarchicalUri::constructor(
    /* [in] */ const String& scheme,
    /* [in] */ Uri::Part* authority,
    /* [in] */ Uri::PathPart* path,
    /* [in] */ Uri::Part* query,
    /* [in] */ Uri::Part* fragment)
{
    mScheme = scheme;
    mAuthority = authority;
    mPath = path;
    mQuery = query;
    mFragment = fragment;
    return NOERROR;
}

ECode HierarchicalUri::ReadFrom(
    /* [in] */ IParcel* parcel,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    String str;
    parcel->ReadString(&str);

    AutoPtr<Uri::Part> p1, p3, p4;
    AutoPtr<Uri::PathPart> p2;
    Uri::Part::ReadFrom(parcel, (Uri::Part**)&p1);
    Uri::PathPart::ReadFrom(parcel, (Uri::PathPart**)&p2);
    Uri::Part::ReadFrom(parcel, (Uri::Part**)&p3);
    Uri::Part::ReadFrom(parcel, (Uri::Part**)&p4);

    AutoPtr<HierarchicalUri> curi = new HierarchicalUri();
    curi->constructor(str, p1, p2, p3, p4);

    *result = IUri::Probe(curi);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode HierarchicalUri::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    Int32 id;
    parcel->ReadInt32(&id);
    parcel->ReadString(&mScheme);

    mAuthority = NULL;
    Uri::Part::ReadFrom(parcel, (Uri::Part**)&mAuthority);

    mPath = NULL;
    Uri::PathPart::ReadFrom(parcel, (Uri::PathPart**)&mPath);

    mQuery = NULL;
    Uri::Part::ReadFrom(parcel, (Uri::Part**)&mQuery);

    mFragment = NULL;
    Uri::Part::ReadFrom(parcel, (Uri::Part**)&mFragment);
    return NOERROR;
}

ECode HierarchicalUri::WriteToParcel(
    /* [in] */ IParcel* parcel)
{
    parcel->WriteInt32(TYPE_ID);
    parcel->WriteString(mScheme);
    mAuthority->WriteTo(parcel);
    mPath->WriteTo(parcel);
    mQuery->WriteTo(parcel);
    mFragment->WriteTo(parcel);
    return NOERROR;
}

ECode HierarchicalUri::IsHierarchical(
    /* [out] */ Boolean* isHierarchical)
{
    VALIDATE_NOT_NULL(isHierarchical);

    *isHierarchical = TRUE;
    return NOERROR;
}

ECode HierarchicalUri::IsRelative(
    /* [out] */ Boolean* isRelative)
{
    VALIDATE_NOT_NULL(isRelative);

    *isRelative = !mScheme.IsNull();
    return NOERROR;
}

ECode HierarchicalUri::GetScheme(
    /* [out] */ String* scheme)
{
    VALIDATE_NOT_NULL(scheme);

    *scheme = mScheme;
    return NOERROR;
}

AutoPtr<Uri::Part> HierarchicalUri::GetSsp()
{
    if (mSsp == NULL) {
        mSsp = Uri::Part::FromEncoded(MakeSchemeSpecificPart());
    }
    return mSsp;
}

ECode HierarchicalUri::GetEncodedSchemeSpecificPart(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<Part> part = GetSsp();
    *result = part->GetEncoded();
    return NOERROR;
}

ECode HierarchicalUri::GetSchemeSpecificPart(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<Part> part = GetSsp();
    *result = part->GetDecoded();
    return NOERROR;
}

String HierarchicalUri::MakeSchemeSpecificPart()
{
    StringBuilder builder;
    AppendSspTo(builder);
    return builder.ToString();
}

ECode HierarchicalUri::AppendSspTo(
    /* [in] */ StringBuilder& builder)
{
    String encodedAuthority = mAuthority->GetEncoded();
    if (!encodedAuthority.IsNull()) {
        // Even if the authority is "", we still want to append "//".
        builder += ("//");
        builder += encodedAuthority;
    }

    String encodedPath = mPath->GetEncoded();
    if (!encodedPath.IsNull()) {
        builder += encodedPath;
    }

    if (!mQuery->IsEmpty()) {
        builder += ('?');
        builder += mQuery->GetEncoded();
    }
    return NOERROR;
}


ECode HierarchicalUri::GetAuthority(
    /* [out] */ String* authority)
{
    VALIDATE_NOT_NULL(authority);

    *authority = mAuthority->GetDecoded();
    return NOERROR;
}

ECode HierarchicalUri::GetEncodedAuthority(
    /* [out] */ String* authority)
{
    VALIDATE_NOT_NULL(authority);

    *authority = mAuthority->GetEncoded();
    return NOERROR;
}

ECode HierarchicalUri::GetEncodedPath(
    /* [out] */ String* path)
{
    VALIDATE_NOT_NULL(path);

    *path = mPath->GetEncoded();
    return NOERROR;
}

ECode HierarchicalUri::GetPath(
    /* [out] */ String* path)
{
    VALIDATE_NOT_NULL(path);

    *path = mPath->GetDecoded();
    return NOERROR;
}

ECode HierarchicalUri::GetQuery(
    /* [out] */ String* query)
{
    VALIDATE_NOT_NULL(query);

    *query = mPath->GetDecoded();
    return NOERROR;
}

ECode HierarchicalUri::GetEncodedQuery(
    /* [out] */ String* query)
{
    VALIDATE_NOT_NULL(query);

    *query = mPath->GetEncoded();
    return NOERROR;
}

ECode HierarchicalUri::GetFragment(
    /* [out] */ String* fragment)
{
    VALIDATE_NOT_NULL(fragment);

    *fragment = mPath->GetEncoded();
    return NOERROR;
}

ECode HierarchicalUri::GetEncodedFragment(
    /* [out] */ String* fragment)
{
    VALIDATE_NOT_NULL(fragment);

    *fragment = mPath->GetEncoded();
    return NOERROR;
}

// return a list of string
ECode HierarchicalUri::GetPathSegments(
    /* [out, callee] */ IList** segments)
{
    VALIDATE_NOT_NULL(segments);
    *segments = NULL;

    AutoPtr<Uri::PathSegments> pathSegments = mPath->GetPathSegments();
    if (pathSegments != NULL) {
        *segments = IList::Probe(pathSegments);
        REFCOUNT_ADD(*segments);
    }
    return NOERROR;
}

ECode HierarchicalUri::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);

    Boolean cached = !mUriString.Equals(Uri::NOT_CACHED);
    *info = cached ? mUriString
            : (mUriString = MakeUriString());
    return NOERROR;
}

String HierarchicalUri::MakeUriString()
{
    StringBuilder builder;

    if (!mScheme.IsNull()) {
        builder += mScheme;
        builder.AppendChar(':');
    }

    AppendSspTo(builder);

    if (!mFragment->IsEmpty()) {
        builder.AppendChar('#');
        builder += mFragment->GetEncoded();
    }

    return builder.ToString();
}

ECode HierarchicalUri::BuildUpon(
    /* [out] */ IUriBuilder** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<UriBuilder> builder = new UriBuilder();
    builder->constructor();

    builder->Scheme(mScheme);
    builder->Authority(mAuthority);
    builder->Path(mPath);
    builder->Query(mQuery);
    builder->Fragment(mFragment);
    *result = IUriBuilder::Probe(builder);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//====================================================================
//                      UriBuilder
//====================================================================
CAR_INTERFACE_IMPL(UriBuilder, Object, IUriBuilder)

ECode UriBuilder::constructor()
{
    return NOERROR;
}

ECode UriBuilder::Scheme(
    /* [in] */ const String& scheme)
{
    mScheme = scheme;
    return NOERROR;
}

ECode UriBuilder::OpaquePart(
    /* [in] */ Uri::Part* opaquePart)
{
    mOpaquePart = (Uri::Part*)opaquePart;
    return NOERROR;
}

ECode UriBuilder::OpaquePart(
    /* [in] */ const String& opaquePart)
{
    AutoPtr<Uri::Part> part = Uri::Part::FromDecoded(opaquePart);
    mOpaquePart = part;
    return NOERROR;
}

ECode UriBuilder::EncodedOpaquePart(
    /* [in] */ const String& opaquePart)
{
    AutoPtr<Uri::Part> part = Uri::Part::FromDecoded(opaquePart);
    mOpaquePart = part;
    return NOERROR;
}

ECode UriBuilder::Authority(
    /* [in] */ Uri::Part* authority)
{
    // This URI will be hierarchical.
    mOpaquePart = NULL;
    mAuthority = authority;
    return NOERROR;
}

ECode UriBuilder::Authority(
    /* [in] */ const String& authority)
{
    // This URI will be hierarchical.
    mOpaquePart = NULL;
    AutoPtr<Uri::Part> part = Uri::Part::FromDecoded(authority);
    mAuthority = part;
    return NOERROR;
}

ECode UriBuilder::EncodedAuthority(
    /* [in] */ const String& authority)
{
    // This URI will be hierarchical.
    mOpaquePart = NULL;
    AutoPtr<Uri::Part> part = Uri::Part::FromDecoded(authority);
    mAuthority = part;
    return NOERROR;
}

ECode UriBuilder::Path(
    /* [in] */ Uri::PathPart* path)
{
    // This URI will be hierarchical.
    mOpaquePart = NULL;
    mPath = path;
    return NOERROR;
}

ECode UriBuilder::Path(
    /* [in] */ const String& path)
{
    // This URI will be hierarchical.
    mOpaquePart = NULL;
    AutoPtr<Uri::PathPart> part = Uri::PathPart::FromDecoded(path);
    mPath = part;
    return NOERROR;
}

ECode UriBuilder::EncodedPath(
    /* [in] */ const String& path)
{
    // This URI will be hierarchical.
    mOpaquePart = NULL;
    AutoPtr<Uri::PathPart> part = Uri::PathPart::FromDecoded(path);
    mPath = part;
    return NOERROR;
}

ECode UriBuilder::AppendPath(
    /* [in] */ const String& newSegment)
{
    // This URI will be hierarchical.
    mOpaquePart = NULL;
    AutoPtr<Uri::PathPart> part = Uri::PathPart::AppendDecodedSegment(mPath, newSegment);
    mPath = part;
    return NOERROR;
}

ECode UriBuilder::AppendEncodedPath(
    /* [in] */ const String& newSegment)
{
    // This URI will be hierarchical.
    mOpaquePart = NULL;
    AutoPtr<Uri::PathPart> part = Uri::PathPart::AppendEncodedSegment(mPath, newSegment);
    mPath = part;
    return NOERROR;
}

ECode UriBuilder::Query(
    /* [in] */ Uri::Part* query)
{
    // This URI will be hierarchical.
    mOpaquePart = NULL;
    mQuery = query;
    return NOERROR;
}

ECode UriBuilder::Query(
    /* [in] */ const String& query)
{
    // This URI will be hierarchical.
    mOpaquePart = NULL;
    AutoPtr<Uri::Part> part = Uri::Part::FromDecoded(query);
    mQuery = part;
    return NOERROR;
}

ECode UriBuilder::EncodedQuery(
    /* [in] */ const String& query)
{
    // This URI will be hierarchical.
    mOpaquePart = NULL;
    AutoPtr<Uri::Part> part = Uri::Part::FromDecoded(query);
    mQuery = part;
    return NOERROR;
}

ECode UriBuilder::Fragment(
    /* [in] */ Uri::Part* fragment)
{
    mFragment = fragment;
    return NOERROR;
}

ECode UriBuilder::Fragment(
    /* [in] */ const String& fragment)
{
    AutoPtr<Uri::Part> part = Uri::Part::FromDecoded(fragment);
    mFragment = part;
    return NOERROR;
}

ECode UriBuilder::EncodedFragment(
    /* [in] */ const String& fragment)
{
    AutoPtr<Uri::Part> part = Uri::Part::FromDecoded(fragment);
    mFragment = part;
    return NOERROR;
}

ECode UriBuilder::AppendQueryParameter(
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    // This URI will be hierarchical.
    mOpaquePart = NULL;

    String nullStr;
    String keyParam, valueParam;
    Uri::Encode(key, nullStr, &keyParam);
    Uri::Encode(value, nullStr, &valueParam);

    StringBuilder sb(keyParam);
    sb += "=";
    sb += valueParam;
    String encodedParameter = sb.ToString();

    if (mQuery == NULL) {
        mQuery = Uri::Part::FromEncoded(encodedParameter);
        return NOERROR;
    }

    String oldQuery = mQuery->GetEncoded();
    if (oldQuery.IsNullOrEmpty()) {
        mQuery = Uri::Part::FromEncoded(encodedParameter);
    } else {
        sb.Reset();
        sb += oldQuery;
        sb += "&";
        sb += encodedParameter;
        mQuery = Uri::Part::FromEncoded(sb.ToString());
    }

    return NOERROR;
}

ECode UriBuilder::ClearQuery()
{
    return Query((Handle32)NULL);
}

ECode UriBuilder::Build(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<IUri> uri;
    if (mOpaquePart != NULL) {
        if (mScheme.IsNull()) {
            Logger::E("UriBuilder", "An opaque URI must have a scheme.");
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }

        uri = new OpaqueUri();
        FAIL_RETURN(((OpaqueUri*)uri.Get())->constructor(mScheme, mOpaquePart, mFragment));
    }
    else {
        // Hierarchical URIs should not return null for getPath().
        AutoPtr<Uri::PathPart> path = mPath;
        if (path == NULL || path == Uri::PathPart::sNULL) {
            path = Uri::PathPart::sEMPTY;
        }
        else {
            // If we have a scheme and/or authority, the path must
            // be absolute. Prepend it with a '/' if necessary.
            if (Ptr(this)->Func(this->HasSchemeOrAuthority)) {
                path = Uri::PathPart::MakeAbsolute(path);
            }
        }

        uri = new HierarchicalUri();
        FAIL_RETURN(((HierarchicalUri*)uri.Get())->constructor(mScheme, mAuthority, mPath, mQuery, mFragment));
    }

    *result = uri;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode UriBuilder::HasSchemeOrAuthority(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = !mScheme.IsNull()
            || (mAuthority != NULL && mAuthority != Uri::Part::sNULL);
    return NOERROR;
}

ECode UriBuilder::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<IUri> uri;
    FAIL_RETURN(Build((IUri**)&uri));
    if (uri != NULL) {
        return IObject::Probe(uri)->ToString(result);
    }
    return NOERROR;
}

//====================================================================================
//              Uri::AbstractPart
//====================================================================================
Uri::AbstractPart::AbstractPart(
    /* [in] */ const String& encoded,
    /* [in] */ const String& decoded)
    : mEncoded(encoded)
    , mDecoded(decoded)
{}

String Uri::AbstractPart::GetDecoded()
{
    // @SuppressWarnings("StringEquality")
    if (Uri::NOT_CACHED.Equals(mDecoded)) {
        Uri::Decode(mEncoded, &mDecoded);
    }
    return mDecoded;
}

ECode Uri::AbstractPart::WriteTo(
    /* [in] */ IParcel* parcel)
{
    // @SuppressWarnings("StringEquality")
    Boolean hasEncoded = !Uri::NOT_CACHED.Equals(mEncoded);

    // @SuppressWarnings("StringEquality")
    Boolean hasDecoded = !Uri::NOT_CACHED.Equals(mDecoded);

    if (hasEncoded && hasDecoded) {
        parcel->WriteInt32(Representation::BOTH);
        parcel->WriteString(mEncoded);
        parcel->WriteString(mDecoded);
    }
    else if (hasEncoded) {
        parcel->WriteInt32(Representation::ENCODED);
        parcel->WriteString(mEncoded);
    }
    else if (hasDecoded) {
        parcel->WriteInt32(Representation::DECODED);
        parcel->WriteString(mDecoded);
    }
    else {
        Logger::E("Uri::AbstractPart", "Neither encoded nor decoded");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

//====================================================================================
//              Uri::EmptyPart
//====================================================================================
Uri::EmptyPart::EmptyPart(
    /* [in] */ const String& value)
    : Part(value, value)
{}

ECode Uri::EmptyPart::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = TRUE;
    return NOERROR;
}

//====================================================================================
//              Uri::Part
//====================================================================================
const AutoPtr<Uri::Part> Uri::Part::sNULL = new EmptyPart(String(NULL));
const AutoPtr<Uri::Part> Uri::Part::sEMPTY = new EmptyPart(String(""));

Uri::Part::Part(
    /* [in] */ const String& encoded,
    /* [in] */ const String& decoded)
    : AbstractPart(encoded, decoded)
{}

Boolean Uri::Part::IsEmpty()
{
    return FALSE;
}

String Uri::Part::GetEncoded()
{
    // @SuppressWarnings("StringEquality")
    if (Uri::NOT_CACHED.Equals(mEncoded)) {
        Uri::Encode(mDecoded, &mEncoded);
    }
    return mEncoded;
}

ECode Uri::Part::ReadFrom(
    /* [in] */ IParcel* parcel,
    /* [out] */ Part** part)
{
    VALIDATE_NOT_NULL(part);
    *part = NULL;

    AutoPtr<Part> result;
    Int32 representation;
    parcel->ReadInt32(&representation);
    String str1, str2;
    switch (representation) {
        case Representation::BOTH:
            parcel->ReadString(&str1);
            parcel->ReadString(&str2);
            result = From(str1, str2);
            break;

        case Representation::ENCODED:
            parcel->ReadString(&str1);
            result = FromEncoded(str1);
            break;

        case Representation::DECODED:
            parcel->ReadString(&str1);
            result = FromDecoded(str1);
            break;
        default:
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
            // throw new IllegalArgumentException("Unknown representation: "
            //         + representation);
    }

    *part = result;
    REFCOUNT_ADD(*part);
    return NOERROR;
}

AutoPtr<Uri::Part> Uri::Part::NonNull(
    /* [in] */ Part* part)
{
    return part == NULL ? sNULL.Get() : part;
}

/**
 * Creates a part from the encoded string.
 *
 * @param encoded part string
 */
AutoPtr<Uri::Part> Uri::Part::FromEncoded(
    /* [in] */ const String& encoded)
{
    return From(encoded, NOT_CACHED);
}

/**
 * Creates a part from the decoded string.
 *
 * @param decoded part string
 */
AutoPtr<Uri::Part> Uri::Part::FromDecoded(
    /* [in] */ const String& decoded)
{
    return From(NOT_CACHED, decoded);
}

AutoPtr<Uri::Part> Uri::Part::From(
    /* [in] */ const String& encoded,
    /* [in] */ const String& decoded)
{
    // We have to check both encoded and decoded in case one is
    // NOT_CACHED.

    if (encoded.IsNull()) {
        return sNULL;
    }
    if (encoded.IsEmpty()) {
        return sEMPTY;
    }

    if (decoded.IsNull()) {
        return sNULL;
    }
    if (decoded.IsEmpty()) {
        return sEMPTY;
    }

    AutoPtr<Part> p = new Part(encoded, decoded);
    return p;
}

//====================================================================================
//              Uri::PathPart
//====================================================================================
const AutoPtr<Uri::PathPart> Uri::PathPart::sNULL = new PathPart(String(NULL), String(NULL));
const AutoPtr<Uri::PathPart> Uri::PathPart::sEMPTY = new PathPart(String(""), String(""));

Uri::PathPart::PathPart(
    /* [in] */ const String& encoded,
    /* [in] */ const String& decoded)
    : AbstractPart(encoded, decoded)
{}

String Uri::PathPart::GetEncoded()
{
    // @SuppressWarnings("StringEquality")
    if (Uri::NOT_CACHED.Equals(mEncoded)) {
        // Uri::Encode(mDecoded, &mEncoded);
        Encode(mDecoded, String("/"), &mEncoded);
    }

    return mEncoded;
}

AutoPtr<Uri::PathSegments> Uri::PathPart::GetPathSegments()
{
    if (mPathSegments != NULL) {
        return mPathSegments;
    }

    String path = GetEncoded();
    if (path.IsNull()) {
        mPathSegments = PathSegments::sEMPTY;
        return mPathSegments;
    }

    AutoPtr<Uri::PathSegmentsBuilder> segmentBuilder = new Uri::PathSegmentsBuilder();

    Int32 previous = 0;
    Int32 current;
    while ((current = path.IndexOf('/', previous)) > -1) {
        // This check keeps us from adding a segment if the path starts
        // '/' and an empty segment for "//".

        if (previous < current) {
            String decodedSegment;
            Uri::Decode(path.Substring(previous, current), &decodedSegment);
            segmentBuilder->Add(decodedSegment);
        }
        previous = current + 1;
    }

    // Add in the final path segment.
    if (previous < (Int32)path.GetLength()) {
        String decoded;
        Uri::Decode(path.Substring(previous), &decoded);
        segmentBuilder->Add(decoded);
    }

    mPathSegments = segmentBuilder->Build();
    return mPathSegments;
}

AutoPtr<Uri::PathPart> Uri::PathPart::AppendEncodedSegment(
    /* [in] */ Uri::PathPart* oldPart,
    /* [in] */ const String& newSegment)
{
    if (oldPart == NULL) {
        // No old path.
        return FromEncoded(String("/") + newSegment);
    }

    String oldPath = oldPart->GetEncoded();
    if (oldPath.IsNull()) {
        oldPath = String("");
    }

    UInt32 oldPathLength = oldPath.GetLength();
    String newPath;
    if (oldPathLength == 0) {
        // No old path.
        newPath = String("/") + newSegment;
    } else if (oldPath[(oldPathLength - 1)] == '/') {
        newPath = oldPath + newSegment;
    } else {
        newPath = oldPath + String("/") + newSegment;
    }

    return FromEncoded(newPath);
}

AutoPtr<Uri::PathPart> Uri::PathPart::AppendDecodedSegment(
    /* [in] */ Uri::PathPart* oldPart,
    /* [in] */ const String& decoded)
{
    String encoded;
    Uri::Encode(decoded, &encoded);

    // TODO: Should we reuse old PathSegments? Probably not.
    return AppendEncodedSegment(oldPart, encoded);
}

ECode Uri::PathPart::ReadFrom(
    /* [in] */ IParcel* parcel,
    /* [out] */ Uri::PathPart** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    Int32 representation;
    parcel->ReadInt32(&representation);

    AutoPtr<PathPart> part;
    String str1, str2;
    switch (representation) {
        case AbstractPart::Representation::BOTH: {
            parcel->ReadString(&str1);
            parcel->ReadString(&str2);
            part = From(str1, str2);
            break;
        }
        case AbstractPart::Representation::ENCODED: {
            parcel->ReadString(&str1);
            part = FromEncoded(str1);
            break;
        }
        case AbstractPart::Representation::DECODED: {
            parcel->ReadString(&str1);
            part = FromDecoded(str1);
            break;
        }
        default: {
            return E_ASSERTION_ERROR;
        }
    }

    *result = part;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

AutoPtr<Uri::PathPart> Uri::PathPart::FromEncoded(
    /* [in] */ const String& encoded)
{
    return From(encoded, NOT_CACHED);
}

AutoPtr<Uri::PathPart> Uri::PathPart::FromDecoded(
    /* [in] */ const String& decoded)
{
    return From(NOT_CACHED, decoded);
}

AutoPtr<Uri::PathPart> Uri::PathPart::From(
    /* [in] */ const String& encoded,
    /* [in] */ const String& decoded)
{
    if (encoded.IsNull()) {
        return sNULL;
    }

    if (encoded.IsEmpty()) {
        return sEMPTY;
    }

    return new PathPart(encoded, decoded);
}

/**
 * Prepends path values with "/" if they're present, not empty, and
 * they don't already start with "/".
 */
AutoPtr<Uri::PathPart> Uri::PathPart::MakeAbsolute(
    /* [in] */ Uri::PathPart* oldPart)
{
    // @SuppressWarnings("StringEquality")
    Boolean encodedCached = !oldPart->mEncoded.Equals(Uri::NOT_CACHED);

    // We don't care which version we use, and we don't want to force
    // unneccessary encoding/decoding.
    String oldPath = encodedCached ? oldPart->mEncoded : oldPart->mDecoded;
    if (oldPath.IsNullOrEmpty() || oldPath.StartWith("/")) {
        return oldPart;
    }

    // Prepend encoded string if present.
    String newEncoded = encodedCached ? String("/") + oldPart->mEncoded : Uri::NOT_CACHED;

    // Prepend decoded string if present.
    // @SuppressWarnings("StringEquality")
    Boolean decodedCached = !oldPart->mDecoded.Equals(
        Uri::NOT_CACHED);
    String newDecoded = decodedCached ? String("/") + oldPart->mDecoded : Uri::NOT_CACHED;

    return new PathPart(newEncoded, newDecoded);
}

//====================================================================================
// Uri
//====================================================================================
CAR_INTERFACE_IMPL_3(Uri, Object, IParcelable, IComparable, IUri)

const String Uri::LOG("Uri");
const String Uri::NOT_CACHED("NOT CACHED");
const Char32 Uri::HEX_DIGITS[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
const Int32 Uri::NOT_FOUND = -1;
const Int32 Uri::NOT_CALCULATED = -2;
const String Uri::NOT_HIERARCHICAL("This isn't a hierarchical URI.");
const String Uri::DEFAULT_ENCODING("UTF-8");
const Int32 Uri::NULL_TYPE_ID = 0;

AutoPtr<IUri> Uri::CreateEmpty()
{
    AutoPtr<IUri> rev;
    rev = new HierarchicalUri();
    ((HierarchicalUri*)rev.Get())->constructor(String(NULL), Part::sNULL, PathPart::sEMPTY, Part::sNULL, Part::sNULL);
    return rev;
}
const AutoPtr<IUri> Uri::EMPTY = CreateEmpty();

ECode Uri::GetEMPTY(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)

    *result = EMPTY;
    REFCOUNT_ADD(*result)
    return NOERROR;
}


ECode Uri::IsOpaque(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    Boolean isHierachical;
    IsHierarchical(&isHierachical);
    *result = !isHierachical;
    return NOERROR;
}

ECode Uri::IsAbsolute(
    /* [out] */ Boolean* isAbsolute)
{
    VALIDATE_NOT_NULL(isAbsolute);

    Boolean isRelative;
    IsRelative(&isRelative);
    *isAbsolute = !isRelative;
    return NOERROR;
}

ECode Uri::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (TO_IINTERFACE(this) == IInterface::Probe(o)) FUNC_RETURN(TRUE)
    if (IUri::Probe(o) == NULL) {
        FUNC_RETURN(FALSE)
    }
    String sThis, sO;
    ToString(&sThis);
    IObject::Probe(o)->ToString(&sO);
    *result = sO.Equals(sThis);
    return NOERROR;
}

ECode Uri::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    String s;
    ToString(&s);
    *result = s.GetHashCode();
    return NOERROR;
}

ECode Uri::CompareTo(
    /* [in] */ IInterface* other,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    String s, sOther;
    ToString(&s);
    IObject::Probe(other)->ToString(&sOther);
    *result = s.Compare(sOther);
    return NOERROR;
}

ECode Uri::ToSafeString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    String scheme;
    GetScheme(&scheme);
    String ssp;
    GetSchemeSpecificPart(&ssp);
    if (scheme != NULL) {
        if (scheme.EqualsIgnoreCase("tel") || scheme.EqualsIgnoreCase("sip")
                || scheme.EqualsIgnoreCase("sms") || scheme.EqualsIgnoreCase("smsto")
                || scheme.EqualsIgnoreCase("mailto")) {
            StringBuilder builder(64);
            builder.Append(scheme);
            builder.Append(':');
            if (ssp != NULL) {
                for (Int32 i=0; i<ssp.GetLength(); i++) {
                    Char32 c = ssp.GetChar(i);
                    if (c == '-' || c == '@' || c == '.') {
                        builder.AppendChar(c);
                    } else {
                        builder.Append('x');
                    }
                }
            }
            return builder.ToString(result);
        }
    }
    // Not a sensitive scheme, but let's still be conservative about
    // the data we include -- only the ssp, not the query params or
    // fragment, because those can often have sensitive info.
    StringBuilder builder(64);
    if (scheme != NULL) {
        builder.Append(scheme);
        builder.Append(':');
    }
    if (ssp != NULL) {
        builder.Append(ssp);
    }
    return builder.ToString(result);
}

ECode Uri::Parse(
    /* [in] */ const String& uriString,
    /* [out] */ IUri** result)
{
    AutoPtr<StringUri> rev = new StringUri();
    rev->constructor(uriString);
    FUNC_RETURN(rev)
}

ECode Uri::FromFile(
    /* [in] */ IFile* file,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);

    if (file == NULL) {
        Logger::E("Uri", "file");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<PathPart> path = PathPart::FromDecoded(Ptr(file)->Func(file->GetAbsolutePath));
    AutoPtr<HierarchicalUri> rev = new HierarchicalUri();
    rev->constructor(String("file"), Part::sEMPTY, path, Part::sNULL, Part::sNULL);
    FUNC_RETURN(rev)
}

ECode Uri::FromParts(
    /* [in] */ const String& scheme,
    /* [in] */ const String& ssp,
    /* [in] */ const String& fragment,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);

    if (scheme == NULL) {
        Logger::E("Uri", "scheme");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (ssp == NULL) {
        Logger::E("Uri", "ssp");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<OpaqueUri> rev = new OpaqueUri();
    rev->constructor(scheme, Part::FromDecoded(ssp), Part::FromDecoded(fragment));
    FUNC_RETURN(rev)
}

ECode Uri::GetQueryParameterNames(
    /* [out] */ ISet** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (Ptr(this)->Func(this->IsOpaque)) {
        Logger::E("Uri", NOT_HIERARCHICAL);
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    String query;
    GetEncodedQuery(&query);
    if (query == NULL) {
        AutoPtr<ICollections> helper;
        CCollections::AcquireSingleton((ICollections**)&helper);
        return helper->GetEmptySet(result);
    }

    AutoPtr<ISet> names;
    CLinkedHashSet::New((ISet**)&names);
    Int32 start = 0;
    do {
        Int32 next = query.IndexOf('&', start);
        Int32 end = (next == -1) ? query.GetLength() : next;
        Int32 separator = query.IndexOf('=', start);
        if (separator > end || separator == -1) {
            separator = end;
        }
        String name = query.Substring(start, separator);
        String decode;
        Decode(name, &decode);
        AutoPtr<ICharSequence> csq;
        CString::New(decode, (ICharSequence**)&csq);
        names->Add(csq);
        // Move start to end of name.
        start = end + 1;
    } while (start < query.GetLength());
    AutoPtr<ICollections> helper;
    CCollections::AcquireSingleton((ICollections**)&helper);
    helper->UnmodifiableSet(names, result);
    return NOERROR;
}

ECode Uri::GetQueryParameters(
    /* [in] */ const String& key,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (Ptr(this)->Func(this->IsOpaque)) {
        Logger::E("Uri", NOT_HIERARCHICAL);
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    if (key == NULL) {
      Logger::E("Uri", "key");
      return E_NULL_POINTER_EXCEPTION;
    }
    String query;
    GetEncodedQuery(&query);
    if (query == NULL) {
        AutoPtr<ICollections> helper;
        CCollections::AcquireSingleton((ICollections**)&helper);
        return helper->GetEmptyList(result);
    }
    String encodedKey;
    // try {
    AutoPtr<IURLEncoder> helper;
    CURLEncoder::AcquireSingleton((IURLEncoder**)&helper);
    ECode ec = helper->Encode(key, DEFAULT_ENCODING, &encodedKey);
    // } catch (UnsupportedEncodingException e) {
    if (FAILED(ec)) {
        if (ec == (ECode)E_UNSUPPORTED_ENCODING_EXCEPTION) {
            Logger::E("Uri", "%d", ec);
            return E_ASSERTION_ERROR;
        }
        return ec;
    }
    // }
    AutoPtr<IArrayList> values;
    CArrayList::New((IArrayList**)&values);
    Int32 start = 0;
    do {
        Int32 nextAmpersand = query.IndexOf('&', start);
        Int32 end = nextAmpersand != -1 ? nextAmpersand : query.GetLength();
        Int32 separator = query.IndexOf('=', start);
        if (separator > end || separator == -1) {
            separator = end;
        }
        if (separator - start == encodedKey.GetLength()
                    && query.RegionMatches(start, encodedKey, 0, encodedKey.GetLength())) {
            if (separator == end) {
                AutoPtr<ICharSequence> csq;
                CString::New(String(""), (ICharSequence**)&csq);
                values->Add(csq);
            } else {
                String decode;
                Decode(query.Substring(separator + 1, end), &decode);
                AutoPtr<ICharSequence> csq;
                CString::New(decode, (ICharSequence**)&csq);
                values->Add(csq);
            }
        }
        // Move start to end of name.
        if (nextAmpersand != -1) {
            start = nextAmpersand + 1;
        } else {
            break;
        }
    } while (TRUE);
    AutoPtr<ICollections> collectionsHelper;
    CCollections::AcquireSingleton((ICollections**)&collectionsHelper);
    return collectionsHelper->UnmodifiableList(IList::Probe(values), result);
}

ECode Uri::GetQueryParameter(
    /* [in] */ const String& key,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    Boolean isOpaque;
    IsOpaque(&isOpaque);

    if (key.IsNull()) {
        Slogger::E("Uri", "key");
        return E_RUNTIME_EXCEPTION;
    }

    String query;
    GetEncodedQuery(&query);
    if (query.IsNull()) {
        return NOERROR;
    }

    String encodedKey;
    Encode(key, String(), &encodedKey);
    Int32 encodedKeyLength = encodedKey.GetLength();

    Int32 length = query.GetLength();
    Int32 start = 0, end, nextAmpersand, separator;
    String encodedValue, decodedValue;

    AutoPtr<IUriCodecHelper> urlCodechelper;
    CUriCodecHelper::AcquireSingleton((IUriCodecHelper**)&urlCodechelper);
    AutoPtr<ICharset> charset;
    AutoPtr<IStandardCharsets> charsetHelper;
    CStandardCharsets::AcquireSingleton((IStandardCharsets**)&charsetHelper );
    charsetHelper->GetUTF_8((ICharset**)&charset);

    do {
        nextAmpersand = query.IndexOf('&', start);
        end = nextAmpersand != -1 ? nextAmpersand : length;

        separator = query.IndexOf('=', start);
        if (separator > end || separator == -1) {
            separator = end;
        }

        if (separator - start == encodedKeyLength
                && query.RegionMatches(start, encodedKey, 0, encodedKeyLength)) {
            if (separator == end) {
                *result = String("");
                return E_NULL_POINTER_EXCEPTION;
            }
            else {
                encodedValue = query.Substring(separator + 1, end);
                return urlCodechelper->Decode(encodedValue, TRUE, charset, FALSE, result);
            }
        }

        // Move start to end of name.
        if (nextAmpersand != -1) {
            start = nextAmpersand + 1;
        }
        else {
            break;
        }
    } while (true);

    return NOERROR;
}

ECode Uri::GetBooleanQueryParameter(
    /* [in] */ const String& key,
    /* [in] */ Boolean defaultValue,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = defaultValue;

    String flag;
    GetQueryParameter(key, &flag);
    if (flag.IsNull()) {
        return NOERROR;
    }

    flag = flag.ToLowerCase(); // flag = flag.toLowerCase(Locale.ROOT);
    *result = !flag.Equals("false") && !flag.Equals("0");
    return NOERROR;
}

ECode Uri::NormalizeScheme(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    String scheme;
    GetScheme(&scheme);
    if (scheme.IsNull()) {
        *result = THIS_PROBE(IUri);
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    String lowerScheme = scheme.ToLowerCase(); // String lowerScheme = scheme.toLowerCase(Locale.ROOT);

    if (scheme.Equals(lowerScheme)) {
        *result = THIS_PROBE(IUri);
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    AutoPtr<IUriBuilder> builder;
    BuildUpon((IUriBuilder**)&builder);
    FAIL_RETURN(builder->Scheme(lowerScheme));
    FAIL_RETURN(builder->Build(result));
    return NOERROR;
}

ECode Uri::WriteToParcel(
    /* [in] */ IParcel* out)
{
    return NOERROR;
}

ECode Uri::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    return NOERROR;
}

ECode Uri::Encode(
    /* [in] */ const String& s,
    /* [out] */ String* result)
{
    return Encode(s, String(NULL), result);
}

ECode Uri::Encode(
    /* [in] */ const String& s,
    /* [in] */ const String& allow,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (s.IsNull()) {
        return NOERROR;
    }

    // Lazily-initialized buffers.
    StringBuilder encoded;
    Int32 oldLength = s.GetLength();
    String subStr, toEncode;

    // This loop alternates between copying over allowed characters and
    // encoding in chunks. This results in fewer method calls and
    // allocations than encoding one character at a time.
    Int32 current = 0, nextToEncode, end, nextAllowed;
    while (current < oldLength) {
        // Start in "copying" mode where we copy over allowed chars.

        // Find the next character which needs to be encoded.
        nextToEncode = current;
        while (nextToEncode < oldLength && IsAllowed(s.GetChar(nextToEncode), allow)) {
            nextToEncode++;
        }

        // If there's nothing more to encode...
        if (nextToEncode == oldLength) {
            if (current == 0) {
                // We didn't need to encode anything!
                *result = s;
                return NOERROR;
            } else {
                // Presumably, we've already done some encoding.
                end = (current + oldLength);
                if (end > oldLength) {
                    end = oldLength;
                }
                subStr = s.Substring(current, end);
                encoded += subStr;
                *result = encoded.ToString();
                return NOERROR;
            }
        }

        if (nextToEncode > current) {
            // Append allowed characters leading up to this point.
            end = (current + nextToEncode);
            if (end > oldLength) {
                end = oldLength;
            }
            subStr = s.Substring(current, end);
            encoded += subStr;
        } else {
            // assert nextToEncode == current
        }

        // Switch to "encoding" mode.

        // Find the next allowed character.
        current = nextToEncode;
        nextAllowed = current + 1;
        while (nextAllowed < oldLength && !IsAllowed(s.GetChar(nextAllowed), allow)) {
            nextAllowed++;
        }

        // Convert the substring to bytes and encode the bytes as
        // '%'-escaped octets.
        toEncode = s.Substring(current, nextAllowed);
        // try {
            // TODO: ALEX fix getBytes
            // byte[] bytes = toEncode.getBytes(DEFAULT_ENCODING);
            // int bytesLength = bytes.length;
            Int32 bytesLength = toEncode.GetLength();
            for (Int32 i = 0; i < bytesLength; i++) {
                encoded += "%";
                encoded.AppendChar(HEX_DIGITS[(toEncode[i] & 0xf0) >> 4]);
                encoded.AppendChar(HEX_DIGITS[toEncode[i] & 0xf]);
            }
        // } catch (UnsupportedEncodingException e) {
        //     throw new AssertionError(e);
//             }

        current = nextAllowed;
    }

    // Encoded could still be null at this point if s is empty.
    String encodedStr = encoded.ToString();
    *result = encodedStr.IsNull() ? s : encodedStr;
    return NOERROR;
}

Boolean Uri::IsAllowed(
    /* [in] */ Char32 c,
    /* [in] */ const String& allow)
{
    String pattern("_-!.~'()*");
    return (c >= 'A' && c <= 'Z')
            || (c >= 'a' && c <= 'z')
            || (c >= '0' && c <= '9')
            || pattern.IndexOf(c) != NOT_FOUND
            || (!allow.IsNull() && allow.IndexOf(c) != NOT_FOUND);
}

ECode Uri::Decode(
    /* [in] */ const String& s,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (s.IsNull()) {
        return NOERROR;
    }

    AutoPtr<IStandardCharsets> charsetsHelper;
    CStandardCharsets::AcquireSingleton((IStandardCharsets**)&charsetsHelper);
    AutoPtr<ICharset> charset;
    charsetsHelper->GetUTF_8((ICharset**)&charset);

    AutoPtr<IUriCodecHelper> uriCodecHelper;
    CUriCodecHelper::AcquireSingleton((IUriCodecHelper**)&uriCodecHelper);
    return uriCodecHelper->Decode(s, FALSE, charset, FALSE, result);
}

ECode Uri::WithAppendedPath(
    /* [in] */ IUri* baseUri,
    /* [in] */ const String& pathSegment,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    assert(baseUri);

    AutoPtr<IUriBuilder> builder;
    baseUri->BuildUpon((IUriBuilder**)&builder);
    builder->AppendEncodedPath(pathSegment);
    return builder->Build(result);
}

ECode Uri::GetCanonicalUri(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    String scheme;
    GetScheme(&scheme);
    if (!scheme.IsNull() && scheme.Equals("file")) {
        String path;
        GetPath(&path);

        String canonicalPath;
        AutoPtr<IFile> file, tmpFile;
        ECode ec = CFile::New(path, (IFile**)&file);
        if (FAILED(ec)) goto _Exit_;
        ec = file->GetCanonicalPath(&canonicalPath);
        if (FAILED(ec)) goto _Exit_;

        AutoPtr<IEnvironment> env;
        // TODO: Waiting for CEnvironment
        assert(0);
        // CEnvironment::AcquireSingleton((IEnvironment**)&env);
        Boolean isEmulated;
        if (env->IsExternalStorageEmulated(&isEmulated), isEmulated) {
            AutoPtr<IFile> dirFile;
            env->GetLegacyExternalStorageDirectory((IFile**)&dirFile);
            assert(dirFile != NULL);
            String legacyPath;
            dirFile->ToString(&legacyPath);

            // Splice in user-specific path when legacy path is found
            if (!canonicalPath.IsNull() && canonicalPath.StartWith(legacyPath)) {
                dirFile = NULL;
                env->GetExternalStorageDirectory((IFile**)&dirFile);
                assert(dirFile != NULL);
                String dirPath, mode;
                dirFile->ToString(&dirPath);
                mode = canonicalPath.Substring(legacyPath.GetLength() + 1);
                CFile::New(dirPath, mode, (IFile**)&tmpFile);
                return Uri::FromFile(tmpFile, result);
            }
        }

        CFile::New(canonicalPath, (IFile**)&tmpFile);
        return Uri::FromFile(tmpFile, result);
    }

_Exit_:
    *result = THIS_PROBE(IUri);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Uri::CheckFileUriExposed(
    /* [in] */ const String& location)
{
    String s;
    GetScheme(&s);
    if (String("file").Equals(s)) {
        // TODO: Waiting for CStrictMode
        assert(0);
        // CStrictMode::OnFileUriExposed(location);
    }
    return NOERROR;
}

ECode Uri::IsPathPrefixMatch(
    /* [in] */ IUri* prefix,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (!Ptr(IUri::Probe(this))->Func(IUri::GetScheme).Equals(Ptr(prefix)->Func(IUri::GetScheme))) FUNC_RETURN(FALSE)
    if (!Ptr(IUri::Probe(this))->Func(IUri::GetAuthority).Equals(Ptr(prefix)->Func(IUri::GetAuthority))) FUNC_RETURN(FALSE)
    AutoPtr<IList> seg;
    GetPathSegments((IList**)&seg);
    AutoPtr<IList> prefixSeg;
    prefix->GetPathSegments((IList**)&prefixSeg);
    Int32 prefixSize;
    prefixSeg->GetSize(&prefixSize);
    if (Ptr(seg)->Func(seg->GetSize) < prefixSize) FUNC_RETURN(FALSE)
    for (Int32 i = 0; i < prefixSize; i++) {
        AutoPtr<IInterface> iSeg;
        seg->Get(i, (IInterface**)&iSeg);
        AutoPtr<IInterface> iPrefixSeg;
        prefixSeg->Get(i, (IInterface**)&iPrefixSeg);
        if (!Objects::Equals(iSeg, iPrefixSeg)) {
            FUNC_RETURN(FALSE)
        }
    }
    FUNC_RETURN(TRUE)
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
