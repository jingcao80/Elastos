
#include "org/apache/http/protocol/UriPatternMatcher.h"
#include "elastos/core/CString.h"
#include "elastos/utility/CHashMap.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::IHashMap;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IIterator;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

CAR_INTERFACE_IMPL(UriPatternMatcher, Object, IUriPatternMatcher)

ECode UriPatternMatcher::Init()
{
    AutoPtr<IHashMap> hm;
    CHashMap::New((IHashMap**)&hm);
    mHandlerMap = IMap::Probe(hm);
    return NOERROR;
}

ECode UriPatternMatcher::Register(
    /* [in] */ const String& pattern,
    /* [in] */ IInterface* handler)
{
    if (pattern.IsNull()) {
        Logger::E("UriPatternMatcher", "URI request pattern may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (handler == NULL) {
        Logger::E("UriPatternMatcher", "HTTP request handelr may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ICharSequence> cs;
    CString::New(pattern, (ICharSequence**)&cs);
    mHandlerMap->Put(cs, handler);
    return NOERROR;
}

ECode UriPatternMatcher::Unregister(
    /* [in] */ const String& pattern)
{
    if (pattern.IsNull()) {
        return NOERROR;
    }
    AutoPtr<ICharSequence> cs;
    CString::New(pattern, (ICharSequence**)&cs);
    mHandlerMap->Remove(cs);
    return NOERROR;
}

ECode UriPatternMatcher::SetHandlers(
    /* [in] */ IMap* map)
{
    if (map == NULL) {
        Logger::E("UriPatternMatcher", "Map of handlers may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mHandlerMap->Clear();
    mHandlerMap->PutAll(map);
    return NOERROR;
}

ECode UriPatternMatcher::Lookup(
    /* [in] */ const String& _requestURI,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value)
    *value = NULL;

    String requestURI = _requestURI;
    if (requestURI == NULL) {
        Logger::E("UriPatternMatcher", "Request URI may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    //Strip away the query part part if found
    Int32 index = requestURI.IndexOf("?");
    if (index != -1) {
        requestURI = requestURI.Substring(0, index);
    }

    // direct match?
    AutoPtr<ICharSequence> cs;
    CString::New(requestURI, (ICharSequence**)&cs);
    AutoPtr<IInterface> handler;
    mHandlerMap->Get(cs, (IInterface**)&handler);
    if (handler == NULL) {
        // pattern match?
        String bestMatch = String(NULL);
        AutoPtr<ISet> keySet;
        mHandlerMap->GetKeySet((ISet**)&keySet);
        AutoPtr<IIterator> it;
        keySet->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> value;
            it->GetNext((IInterface**)&value);
            AutoPtr<ICharSequence> patternCs = ICharSequence::Probe(value);
            String pattern;
            patternCs->ToString(&pattern);
            Boolean isMatch;
            if (MatchUriRequestPattern(pattern, requestURI, &isMatch), isMatch) {
                // we have a match. is it any better?
                if (bestMatch.IsNull()
                        || (bestMatch.GetLength() < pattern.GetLength())
                        || (bestMatch.GetLength() == pattern.GetLength() && pattern.EndWith("*"))) {
                    handler = NULL;
                    mHandlerMap->Get(patternCs, (IInterface**)&handler);
                    bestMatch = pattern;
                }
            }
        }
    }
    *value = handler;
    REFCOUNT_ADD(*value)
    return NOERROR;
}

ECode UriPatternMatcher::MatchUriRequestPattern(
    /* [in] */ const String& pattern,
    /* [in] */ const String& requestUri,
    /* [out] */ Boolean* isMatch)
{
    VALIDATE_NOT_NULL(isMatch)
    if (pattern.Equals("*")) {
        *isMatch = TRUE;
    }
    else {
        *isMatch =
                (pattern.EndWith("*") && requestUri.StartWith(pattern.Substring(0, pattern.GetLength() - 1))) ||
                (pattern.StartWith("*") && requestUri.EndWith(pattern.Substring(1, pattern.GetLength())));
    }
    return NOERROR;
}

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org
