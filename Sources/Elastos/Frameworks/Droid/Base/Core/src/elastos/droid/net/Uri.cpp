
#include "_Elastos.Droid.Os.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Libcore.h>
#include <Elastos.CoreLibrary.Net.h>
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/net/CStringUri.h"
#include "elastos/droid/net/COpaqueUri.h"
#include "elastos/droid/net/CHierarchicalUri.h"
#include "elastos/droid/net/CUriBuilder.h"
#include "elastos/droid/net/ReturnOutValue.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/Environment.h"
#include "elastos/droid/os/CStrictMode.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Objects.h>
#include <elastos/utility/etl/HashSet.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::CStrictMode;
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
CAR_INTERFACE_IMPL(StringUri, AbstractHierarchicalUri, IStringUri)

StringUri::StringUri()
    : mCachedSsi(Uri::NOT_CALCULATED)
    , mCachedFsi(Uri::NOT_CALCULATED)
    , mScheme(AbstractPart::NOT_CACHED)
{}

ECode StringUri::constructor()
{
    return NOERROR;
}

ECode StringUri::constructor(
    /* [in] */ const String& uriString)
{
    if (uriString.IsNull()) {
        Logger::E("Uri", "uriString is null.");
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

    return CStringUri::New(str, uri);
}

ECode StringUri::WriteToParcel(
    /* [in] */ IParcel* parcel)
{
    parcel->WriteInt32(1); // id 1
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

    if (mUriString.GetLength() == (ssi + 1)) {
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

    Boolean cached = (!mScheme.Equals(AbstractPart::NOT_CACHED));
    *scheme = cached ? mScheme : (mScheme = ParseScheme());
    return NOERROR;
}

String StringUri::ParseScheme()
{
    Int32 ssi = FindSchemeSeparator();
    return ssi == NOT_FOUND ? String(NULL) : mUriString.Substring(0, ssi);
}

AutoPtr<Part> StringUri::GetSsp()
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

    AutoPtr<Part> part = GetSsp();
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

    AutoPtr<Part> part = GetSsp();
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

AutoPtr<Part> StringUri::GetAuthorityPart()
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

    AutoPtr<Part> part = GetAuthorityPart();
    if (part) {
        *authority = part->GetEncoded();
    }
    return NOERROR;
}

ECode StringUri::GetAuthority(
    /* [out] */ String* authority)
{
    VALIDATE_NOT_NULL(authority);

    AutoPtr<Part> part = GetAuthorityPart();
    assert(part != NULL);
    *authority = part->GetDecoded();
    return NOERROR;
}

AutoPtr<PathPart> StringUri::GetPathPart()
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

    AutoPtr<PathPart> part = GetPathPart();
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

    AutoPtr<PathPart> part = GetPathPart();
    if (part) {
        *path = part->GetEncoded();
    }
    return NOERROR;
}

ECode StringUri::GetPathSegments(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<PathPart> part = GetPathPart();
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
        Boolean schemeOnly = (ssi + 1) == charArray->GetLength();
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

AutoPtr<Part> StringUri::GetQueryPart()
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

    AutoPtr<Part> part = GetQueryPart();
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

    AutoPtr<Part> part = GetQueryPart();
    if (part) {
        *query = part->GetDecoded();
    }
    return NOERROR;
}

AutoPtr<Part> StringUri::GetFragmentPart()
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

    AutoPtr<Part> part = GetFragmentPart();
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

    AutoPtr<Part> part = GetFragmentPart();
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
        AutoPtr<Part> p = GetAuthorityPart();
        builder->Authority(p.Get());
        AutoPtr<PathPart> pp = GetPathPart();
        builder->Path(pp.Get());
        p = GetQueryPart();
        builder->Query(p.Get());
        p = GetFragmentPart();
        builder->Fragment(p.Get());
    }
    else {
        AutoPtr<Part> p = GetSsp();
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
CAR_INTERFACE_IMPL(OpaqueUri, Uri, IOpaqueUri)

ECode OpaqueUri::constructor()
{
    return NOERROR;
}

ECode OpaqueUri::constructor(
    /* [in] */ const String& scheme,
    /* [in] */ Handle32 ssp,
    /* [in] */ Handle32 fragment)
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

    AutoPtr<Part> p1, p2;
    Part::ReadFrom(parcel, (Part**)&p1);
    Part::ReadFrom(parcel, (Part**)&p2);

    return COpaqueUri::New(str, (Handle32)p1.Get(), (Handle32)p2.Get(), result);
}

ECode OpaqueUri::WriteToParcel(
    /* [in] */ IParcel* parcel)
{
    VALIDATE_NOT_NULL(parcel);
    parcel->WriteInt32(2);
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
    Part::ReadFrom(parcel, (Part**)&mSsp);
    mFragment = NULL;
    Part::ReadFrom(parcel, (Part**)&mFragment);
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
    Boolean cached = (!mCachedString.IsNull() && !mCachedString.Equals(AbstractPart::NOT_CACHED));
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
//              PathSegments
//====================================================================================
AutoPtr<PathSegments> PathSegments::sEMPTY = new PathSegments(ArrayOf<String>::Alloc(0), 0);

CAR_INTERFACE_IMPL(PathSegments, AbstractList, IRandomAccess)

PathSegments::PathSegments(
    /* [in] */ ArrayOf<String>* segments,
    /* [in] */ Int32 size)
    : mSegments(segments)
    , mSize(size)
{}

ECode PathSegments::Get(
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

ECode PathSegments::GetSize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSize;
    return NOERROR;
}

ECode PathSegments::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("PathSegments{");
    sb += "size:";
    sb += mSize;
    if (mSegments) {
        sb += ", [";
        for (Int32 i = 0; i < mSegments->GetLength(); ++i) {
            sb += ", ";
            sb += (*mSegments)[i];
        }
        sb += "]";
    }
    sb += "}";
    *str = sb.ToString();
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

AutoPtr<PathSegments> Uri::PathSegmentsBuilder::Build()
{
    if (mSegments.IsEmpty()) {
        return PathSegments::sEMPTY;
    }

    // try {
    AutoPtr<ArrayOf<String> > segments = ArrayOf<String>::Alloc(mSegments.GetSize());
    if (!segments) {
        return PathSegments::sEMPTY;
    }

    List<String>::Iterator itor;
    Int32 i = 0;
    for (itor = mSegments.Begin(); itor != mSegments.End(); ++itor, ++i) {
        (*segments)[i] = *itor;
    }

    AutoPtr<PathSegments> pathSegments = new PathSegments(segments, mSize);

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
    : mHost(AbstractPart::NOT_CACHED)
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

AutoPtr<Part> AbstractHierarchicalUri::GetUserInfoPart()
{
    if (mUserInfo == NULL) {
        String result = ParseUserInfo();
        mUserInfo = Part::FromEncoded(result);
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

    Boolean cached = !mHost.Equals(AbstractPart::NOT_CACHED);

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
CAR_INTERFACE_IMPL(HierarchicalUri, AbstractHierarchicalUri, IHierarchicalUri)

HierarchicalUri::HierarchicalUri()
{
    mUriString = AbstractPart::NOT_CACHED;
}

ECode HierarchicalUri::constructor()
{
    return NOERROR;
}

ECode HierarchicalUri::constructor(
    /* [in] */ const String& scheme,
    /* [in] */ Handle32 authority,
    /* [in] */ Handle32 path,
    /* [in] */ Handle32 query,
    /* [in] */ Handle32 fragment)
{
    mScheme = scheme;
    mAuthority = (Part*)authority;
    if (mAuthority == NULL) mAuthority = Part::sNULL;
    mPath = (PathPart*)path;
    if (mPath == NULL) mPath = PathPart::sNULL;
    mQuery = (Part*)query;
    if (mQuery == NULL) mQuery = Part::sNULL;
    mFragment = (Part*)fragment;
    if (mFragment == NULL) mFragment = Part::sNULL;
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

    AutoPtr<Part> p1, p3, p4;
    AutoPtr<PathPart> p2;
    Part::ReadFrom(parcel, (Part**)&p1);
    PathPart::ReadFrom(parcel, (PathPart**)&p2);
    Part::ReadFrom(parcel, (Part**)&p3);
    Part::ReadFrom(parcel, (Part**)&p4);

    return CHierarchicalUri::New(str, (Handle32)p1.Get(), (Handle32)p2.Get(),
        (Handle32)p3.Get(), (Handle32)p4.Get(), result);
}

ECode HierarchicalUri::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    Int32 id;
    parcel->ReadInt32(&id);
    parcel->ReadString(&mScheme);

    mAuthority = NULL;
    Part::ReadFrom(parcel, (Part**)&mAuthority);

    mPath = NULL;
    PathPart::ReadFrom(parcel, (PathPart**)&mPath);

    mQuery = NULL;
    Part::ReadFrom(parcel, (Part**)&mQuery);

    mFragment = NULL;
    Part::ReadFrom(parcel, (Part**)&mFragment);
    return NOERROR;
}

ECode HierarchicalUri::WriteToParcel(
    /* [in] */ IParcel* parcel)
{
    parcel->WriteInt32(3);
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

AutoPtr<Part> HierarchicalUri::GetSsp()
{
    if (mSsp == NULL) {
        mSsp = Part::FromEncoded(MakeSchemeSpecificPart());
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
    *query = mQuery->GetDecoded();
    return NOERROR;
}

ECode HierarchicalUri::GetEncodedQuery(
    /* [out] */ String* query)
{
    VALIDATE_NOT_NULL(query);
    *query = mQuery->GetEncoded();
    return NOERROR;
}

ECode HierarchicalUri::GetFragment(
    /* [out] */ String* fragment)
{
    VALIDATE_NOT_NULL(fragment);
    *fragment = mFragment->GetEncoded();
    return NOERROR;
}

ECode HierarchicalUri::GetEncodedFragment(
    /* [out] */ String* fragment)
{
    VALIDATE_NOT_NULL(fragment);
    *fragment = mFragment->GetEncoded();
    return NOERROR;
}

// return a list of string
ECode HierarchicalUri::GetPathSegments(
    /* [out, callee] */ IList** segments)
{
    VALIDATE_NOT_NULL(segments);
    *segments = NULL;

    AutoPtr<PathSegments> pathSegments = mPath->GetPathSegments();
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

    Boolean cached = !mUriString.Equals(AbstractPart::NOT_CACHED);
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
    /* [in] */ Part* opaquePart)
{
    mOpaquePart = (Part*)opaquePart;
    return NOERROR;
}

ECode UriBuilder::OpaquePart(
    /* [in] */ const String& opaquePart)
{
    return OpaquePart(Part::FromDecoded(opaquePart));
}

ECode UriBuilder::EncodedOpaquePart(
    /* [in] */ const String& opaquePart)
{
    return OpaquePart(Part::FromEncoded(opaquePart));
}

ECode UriBuilder::Authority(
    /* [in] */ Part* authority)
{
    // This URI will be hierarchical.
    mOpaquePart = NULL;
    mAuthority = authority;
    return NOERROR;
}

ECode UriBuilder::Authority(
    /* [in] */ const String& authority)
{
    return Authority(Part::FromDecoded(authority));
}

ECode UriBuilder::EncodedAuthority(
    /* [in] */ const String& authority)
{
    return Authority(Part::FromEncoded(authority));
}

ECode UriBuilder::Path(
    /* [in] */ PathPart* path)
{
    // This URI will be hierarchical.
    mOpaquePart = NULL;
    mPath = path;
    return NOERROR;
}

ECode UriBuilder::Path(
    /* [in] */ const String& path)
{
    return Path(PathPart::FromDecoded(path));
}

ECode UriBuilder::EncodedPath(
    /* [in] */ const String& path)
{
    return Path(PathPart::FromEncoded(path));
}

ECode UriBuilder::AppendPath(
    /* [in] */ const String& newSegment)
{
    return Path(PathPart::AppendDecodedSegment(mPath, newSegment));
}

ECode UriBuilder::AppendEncodedPath(
    /* [in] */ const String& newSegment)
{
    return Path(PathPart::AppendEncodedSegment(mPath, newSegment));
}

ECode UriBuilder::Query(
    /* [in] */ Part* query)
{
    // This URI will be hierarchical.
    mOpaquePart = NULL;
    mQuery = query;
    return NOERROR;
}

ECode UriBuilder::Query(
    /* [in] */ const String& query)
{
    return Query(Part::FromDecoded(query));
}

ECode UriBuilder::EncodedQuery(
    /* [in] */ const String& query)
{
    return Query(Part::FromEncoded(query));
}

ECode UriBuilder::Fragment(
    /* [in] */ Part* fragment)
{
    mFragment = fragment;
    return NOERROR;
}

ECode UriBuilder::Fragment(
    /* [in] */ const String& fragment)
{
    return Fragment(Part::FromDecoded(fragment));
}

ECode UriBuilder::EncodedFragment(
    /* [in] */ const String& fragment)
{
    return Fragment(Part::FromEncoded(fragment));
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
        mQuery = Part::FromEncoded(encodedParameter);
        return NOERROR;
    }

    String oldQuery = mQuery->GetEncoded();
    if (oldQuery.IsNullOrEmpty()) {
        mQuery = Part::FromEncoded(encodedParameter);
    } else {
        sb.Reset();
        sb += oldQuery;
        sb += "&";
        sb += encodedParameter;
        mQuery = Part::FromEncoded(sb.ToString());
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

    if (mOpaquePart != NULL) {
        if (mScheme.IsNull()) {
            Logger::E("UriBuilder", "An opaque URI must have a scheme.");
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }

        return COpaqueUri::New(mScheme, (Handle32)mOpaquePart.Get(),
            (Handle32)mFragment.Get(), result);
    }
    else {

        // Hierarchical URIs should not return null for getPath().
        AutoPtr<PathPart> path = mPath;
        if (path == NULL || path == PathPart::sNULL) {
            path = PathPart::sEMPTY;
        }
        else {
            // If we have a scheme and/or authority, the path must
            // be absolute. Prepend it with a '/' if necessary.
            Boolean bval;
            HasSchemeOrAuthority(&bval);
            if (bval) {
                path = PathPart::MakeAbsolute(mPath);
            }
        }

        return CHierarchicalUri::New(mScheme, (Handle32)mAuthority.Get(),
            (Handle32)mPath.Get(), (Handle32)mQuery.Get(), (Handle32)mFragment.Get(), result);
    }

    return NOERROR;
}

ECode UriBuilder::HasSchemeOrAuthority(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = !mScheme.IsNull()
            || (mAuthority != NULL && mAuthority != Part::sNULL);
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
//              AbstractPart
//====================================================================================

INIT_PROI_1 const String AbstractPart::NOT_CACHED("NOT CACHED");
INIT_PROI_1 const AutoPtr<Part> Part::sNULL = new EmptyPart(String(NULL));
INIT_PROI_1 const AutoPtr<Part> Part::sEMPTY = new EmptyPart(String(""));

CAR_INTERFACE_IMPL(AbstractPart, Object, IAbstractPart)

AbstractPart::AbstractPart(
    /* [in] */ const String& encoded,
    /* [in] */ const String& decoded)
    : mEncoded(encoded)
    , mDecoded(decoded)
{}

String AbstractPart::GetDecoded()
{
    // @SuppressWarnings("StringEquality")
    if (NOT_CACHED.Equals(mDecoded)) {
        Uri::Decode(mEncoded, &mDecoded);
    }
    return mDecoded;
}

ECode AbstractPart::WriteTo(
    /* [in] */ IParcel* parcel)
{
    // @SuppressWarnings("StringEquality")
    Boolean hasEncoded = !NOT_CACHED.Equals(mEncoded);

    // @SuppressWarnings("StringEquality")
    Boolean hasDecoded = !NOT_CACHED.Equals(mDecoded);

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
        Logger::E("AbstractPart", "Neither encoded nor decoded");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

//====================================================================================
//              EmptyPart
//====================================================================================
EmptyPart::EmptyPart(
    /* [in] */ const String& value)
    : Part(value, value)
{}

ECode EmptyPart::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = TRUE;
    return NOERROR;
}

//====================================================================================
//              Part
//====================================================================================

Part::Part(
    /* [in] */ const String& encoded,
    /* [in] */ const String& decoded)
    : AbstractPart(encoded, decoded)
{}

Boolean Part::IsEmpty()
{
    return FALSE;
}

String Part::GetEncoded()
{
    // @SuppressWarnings("StringEquality")
    if (NOT_CACHED.Equals(mEncoded)) {
        Uri::Encode(mDecoded, &mEncoded);
    }
    return mEncoded;
}

ECode Part::ReadFrom(
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

AutoPtr<Part> Part::NonNull(
    /* [in] */ Part* part)
{
    return part == NULL ? sNULL.Get() : part;
}

/**
 * Creates a part from the encoded string.
 *
 * @param encoded part string
 */
AutoPtr<Part> Part::FromEncoded(
    /* [in] */ const String& encoded)
{
    return From(encoded, NOT_CACHED);
}

/**
 * Creates a part from the decoded string.
 *
 * @param decoded part string
 */
AutoPtr<Part> Part::FromDecoded(
    /* [in] */ const String& decoded)
{
    return From(NOT_CACHED, decoded);
}

AutoPtr<Part> Part::From(
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
//              PathPart
//====================================================================================
INIT_PROI_1 const AutoPtr<PathPart> PathPart::sNULL = new PathPart(String(NULL), String(NULL));
INIT_PROI_1 const AutoPtr<PathPart> PathPart::sEMPTY = new PathPart(String(""), String(""));

PathPart::PathPart(
    /* [in] */ const String& encoded,
    /* [in] */ const String& decoded)
    : AbstractPart(encoded, decoded)
{}

String PathPart::GetEncoded()
{
    // @SuppressWarnings("StringEquality")
    if (NOT_CACHED.Equals(mEncoded)) {
        // Uri::Encode(mDecoded, &mEncoded);
        Uri::Encode(mDecoded, String("/"), &mEncoded);
    }

    return mEncoded;
}

AutoPtr<PathSegments> PathPart::GetPathSegments()
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

AutoPtr<PathPart> PathPart::AppendEncodedSegment(
    /* [in] */ PathPart* oldPart,
    /* [in] */ const String& newSegment)
{
    // If there is no old path, should we make the new path relative
    // or absolute? I pick absolute.

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

AutoPtr<PathPart> PathPart::AppendDecodedSegment(
    /* [in] */ PathPart* oldPart,
    /* [in] */ const String& decoded)
{
    String encoded;
    Uri::Encode(decoded, &encoded);

    // TODO: Should we reuse old PathSegments? Probably not.
    return AppendEncodedSegment(oldPart, encoded);
}

ECode PathPart::ReadFrom(
    /* [in] */ IParcel* parcel,
    /* [out] */ PathPart** result)
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

AutoPtr<PathPart> PathPart::FromEncoded(
    /* [in] */ const String& encoded)
{
    return From(encoded, NOT_CACHED);
}

AutoPtr<PathPart> PathPart::FromDecoded(
    /* [in] */ const String& decoded)
{
    return From(NOT_CACHED, decoded);
}

AutoPtr<PathPart> PathPart::From(
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
AutoPtr<PathPart> PathPart::MakeAbsolute(
    /* [in] */ PathPart* oldPart)
{
    // @SuppressWarnings("StringEquality")
    Boolean encodedCached = !oldPart->mEncoded.Equals(NOT_CACHED);

    // We don't care which version we use, and we don't want to force
    // unneccessary encoding/decoding.
    String oldPath = encodedCached ? oldPart->mEncoded : oldPart->mDecoded;
    if (oldPath.IsNullOrEmpty() || oldPath.StartWith("/")) {
        return oldPart;
    }

    // Prepend encoded string if present.
    String newEncoded = encodedCached ? String("/") + oldPart->mEncoded : NOT_CACHED;

    // Prepend decoded string if present.
    // @SuppressWarnings("StringEquality")
    Boolean decodedCached = !oldPart->mDecoded.Equals(
        NOT_CACHED);
    String newDecoded = decodedCached ? String("/") + oldPart->mDecoded : NOT_CACHED;

    return new PathPart(newEncoded, newDecoded);
}

//====================================================================================
// Uri
//====================================================================================
CAR_INTERFACE_IMPL_3(Uri, Object, IParcelable, IComparable, IUri)

const String Uri::LOG("Uri");
const Char32 Uri::HEX_DIGITS[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
const Int32 Uri::NOT_FOUND = -1;
const Int32 Uri::NOT_CALCULATED = -2;
const String Uri::NOT_HIERARCHICAL("This isn't a hierarchical URI.");
const String Uri::DEFAULT_ENCODING("UTF-8");

AutoPtr<IUri> Uri::CreateEmpty()
{
    assert(Part::sNULL != NULL);
    assert(PathPart::sEMPTY != NULL);
    AutoPtr<IUri> rev;
    CHierarchicalUri::New(String(NULL), (Handle32)Part::sNULL.Get(),
        (Handle32)PathPart::sEMPTY.Get(), (Handle32)Part::sNULL.Get(),
        (Handle32)Part::sNULL.Get(), (IUri**)&rev);
    return rev;
}
INIT_PROI_2 const AutoPtr<IUri> Uri::EMPTY = CreateEmpty();

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
    *result = FALSE;
    IUri* uri = IUri::Probe(o);
    if (uri == NULL) return NOERROR;

    if (uri == (IUri*)this) {
        *result = TRUE;
        return NOERROR;
    }

    String s1, s2 = Object::ToString(o);
    ToString(&s1);
    *result = s1.Equals(s2);
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
    /* [in] */ IInterface* o,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    String s1, s2 = Object::ToString(o);
    ToString(&s1);
    *result = s1.Compare(s2);
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
            builder.AppendChar(':');
            if (ssp != NULL) {
                for (Int32 i=0; i<ssp.GetLength(); i++) {
                    Char32 c = ssp.GetChar(i);
                    if (c == '-' || c == '@' || c == '.') {
                        builder.AppendChar(c);
                    } else {
                        builder.AppendChar('x');
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
        builder.AppendChar(':');
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
    return CStringUri::New(uriString, result);
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
    return CHierarchicalUri::New(String("file"), (Handle32)Part::sEMPTY.Get(), (Handle32)path.Get(),
        (Handle32)Part::sNULL.Get(), (Handle32)Part::sNULL.Get(), result);
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
    AutoPtr<Part> sspPart = Part::FromDecoded(ssp);
    AutoPtr<Part> fragmentPart = Part::FromDecoded(fragment);
    return COpaqueUri::New(scheme, (Handle32)sspPart.Get(), (Handle32)fragmentPart.Get(), result);
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
        *result = this;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    String lowerScheme = scheme.ToLowerCase(); // String lowerScheme = scheme.toLowerCase(Locale.ROOT);

    if (scheme.Equals(lowerScheme)) {
        *result = this;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    AutoPtr<IUriBuilder> builder;
    BuildUpon((IUriBuilder**)&builder);
    FAIL_RETURN(builder->Scheme(lowerScheme));
    FAIL_RETURN(builder->Build(result));
    return NOERROR;
}

ECode Uri::ReadFromParcel(
   /* [in] */ IParcel* parcel,
   /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = NULL;

   Int32 type;
   parcel->ReadInt32(&type);

   switch (type) {
       case 0: return NOERROR;
       case 1: return StringUri::ReadFrom(parcel, uri);
       case 2: return OpaqueUri::ReadFrom(parcel, uri);
       case 3: return HierarchicalUri::ReadFrom(parcel, uri);
   }

    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode Uri::WriteToParcel(
    /* [in] */ IParcel* out,
    /* [in] */ IUri* uri)
{
    if (uri == NULL) {
        out->WriteInt32(0);
    }
    else {
        IParcelable* p = IParcelable::Probe(uri);
        assert(p != NULL);
        p->WriteToParcel(out);
    }

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

        if (Environment::IsExternalStorageEmulated()) {
            AutoPtr<IFile> dirFile = Environment::GetLegacyExternalStorageDirectory();
            assert(dirFile != NULL);
            String legacyPath;
            dirFile->ToString(&legacyPath);

            // Splice in user-specific path when legacy path is found
            if (!canonicalPath.IsNull() && canonicalPath.StartWith(legacyPath)) {
                dirFile = Environment::GetExternalStorageDirectory();
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
    *result = this;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Uri::CheckFileUriExposed(
    /* [in] */ const String& location)
{
    String s;
    GetScheme(&s);
    if (s.Equals("file")) {
        AutoPtr<IStrictMode> sm;
        CStrictMode::AcquireSingleton((IStrictMode**)&sm);
        sm->OnFileUriExposed(location);
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
            *result = FALSE;
             return NOERROR;
        }
    }
    *result = TRUE;
     return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
