
#include "CookieStoreImpl.h"
#include "CURI.h"
#include "CHttpCookie.h"
#include "CArrayList.h"
#include "CHashMap.h"
#include "Collections.h"
#include "AutoLock.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Collections;
using Elastos::Utility::IMapEntry;

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(CookieStoreImpl, Object, ICookieStore)

CookieStoreImpl::CookieStoreImpl()
{
    CHashMap::New((IMap**)&mMap);
}

CookieStoreImpl::~CookieStoreImpl()
{
    mMap->Clear();
}

ECode CookieStoreImpl::Add(
    /* [in] */ IURI* uri,
    /* [in] */ IHttpCookie* cookie)
{
    {    AutoLock syncLock(this);
        if (cookie == NULL) {
            return E_NULL_POINTER_EXCEPTION;
        }

        AutoPtr<IURI> cookiesUri = GetCookiesUri(uri);

        AutoPtr<IList> cookies;
        AutoPtr<IInterface> outObj;
        mMap->Get(cookiesUri, (IInterface**)&outObj);
        cookies = IList::Probe(outObj);
        if (cookies == NULL) {
            CArrayList::New((IList**)&cookies);
            mMap->Put(cookiesUri, cookies);
        }
        else {
            cookies->Remove(cookie);
        }
        cookies->Add(cookie);
    }
    return NOERROR;
}

AutoPtr<IURI> CookieStoreImpl::GetCookiesUri(
    /* [in] */ IURI* uri)
{
    if (uri == NULL) {
        return NULL;
    }

    String host;
    uri->GetHost(&host);
    AutoPtr<IURI> cookiesUri;
    ECode ec = CURI::New(String("http"), host, String(NULL), String(NULL), (IURI**)&cookiesUri);
    if (FAILED(ec)) {
        cookiesUri = uri;
    }

    return cookiesUri;
}

ECode CookieStoreImpl::Get(
    /* [in] */ IURI* uri,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);

    {    AutoLock syncLock(this);
        if (uri == NULL) {
            return E_NULL_POINTER_EXCEPTION;
        }

        AutoPtr<IList> outresult;
        CArrayList::New((IList**)&outresult);

        // get cookies associated with given URI. If none, returns an empty list
        AutoPtr<IList> cookiesForUri;
        AutoPtr<IInterface> outObj;
        mMap->Get(uri, (IInterface**)&outObj);
        cookiesForUri = IList::Probe(outObj);
        if (cookiesForUri != NULL) {
            AutoPtr<IIterator> iterator;
            cookiesForUri->GetIterator((IIterator**)&iterator);

            Boolean isflag = FALSE, hasExpired = FALSE;
            while (iterator->HasNext(&isflag), isflag) {
                AutoPtr<IHttpCookie> cookie;
                AutoPtr<IInterface> nextface;
                iterator->GetNext((IInterface**)&nextface);
                cookie = IHttpCookie::Probe(nextface);
                if (cookie->HasExpired(&hasExpired), hasExpired) {
                    iterator->Remove();// remove expired cookies
                } else {
                    outresult->Add(cookie);
                }
            }
        }

        // get all cookies that domain matches the URI
        AutoPtr<ArrayOf<IInterface*> > entries;
        AutoPtr<ISet> entrySet;
        mMap->GetEntrySet((ISet**)&entrySet);
        (ICollection::Probe(entrySet))->ToArray((ArrayOf<IInterface*>**)&entries);
        for (Int32 i = 0; i < entries->GetLength(); i++) {
            AutoPtr<IMapEntry> entry = IMapEntry::Probe((*entries)[i]);
            AutoPtr<IInterface> keyface;
            AutoPtr<IInterface> valueface;
            AutoPtr<IList> entryCookies;
            entry->GetKey((IInterface**)&keyface);

            Boolean isEqual = FALSE;
            if (IObject::Probe(uri)->Equals(keyface, &isEqual), isEqual) {
                continue; // skip the given URI; we've already handled it
            }
            entry->GetValue((IInterface**)&valueface);
            entryCookies = IList::Probe(valueface);

            AutoPtr<IIterator> iterator;
            entryCookies->GetIterator((IIterator**)&iterator);

            Boolean isflag = FALSE, hasExpired = FALSE;
            while (iterator->HasNext(&isflag), isflag) {
                AutoPtr<IHttpCookie> cookie;
                AutoPtr<IInterface> nextface;
                iterator->GetNext((IInterface**)&nextface);
                cookie = IHttpCookie::Probe(nextface);

                String attr, host;
                cookie->GetDomainAttr(&attr);
                uri->GetHost(&host);
                if (!CHttpCookie::IsDomainMatches(attr, host)) {
                    continue;
                }

                if (cookie->HasExpired(&hasExpired), hasExpired) {
                    iterator->Remove();// remove expired cookies
                } else {
                    Boolean found = FALSE;
                    outresult->Contains(cookie, &found);
                    if (!found) {
                        outresult->Add(cookie);
                    }
                }
            }
        }

        return Collections::UnmodifiableList(outresult, result);
    }
    return NOERROR;
}

ECode CookieStoreImpl::GetCookies(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);

    {    AutoLock syncLock(this);
        AutoPtr<IList> outresult;
        CArrayList::New((IList**)&outresult);

        AutoPtr<ICollection> collection;
        AutoPtr<ArrayOf<IInterface*> > entries;
        mMap->GetValues((ICollection**)&collection);
        collection->ToArray((ArrayOf<IInterface*>**)&entries);
        for (Int32 i = 0; i < entries->GetLength(); i++) {
            AutoPtr<IList> entry = IList::Probe((*entries)[i]);
            AutoPtr<IIterator> iterator;
            entry->GetIterator((IIterator**)&iterator);

            Boolean isflag = FALSE, hasExpired = FALSE;
            while (iterator->HasNext(&isflag), isflag) {
                AutoPtr<IHttpCookie> cookie;
                AutoPtr<IInterface> nextface;
                iterator->GetNext((IInterface**)&nextface);
                cookie = IHttpCookie::Probe(nextface);
                if (cookie->HasExpired(&hasExpired), hasExpired) {
                    iterator->Remove();// remove expired cookies
                } else {
                    Boolean found = FALSE;
                    outresult->Contains(cookie, &found);
                    if (!found) {
                        outresult->Add(cookie);
                    }
                }
            }
        }
        return Collections::UnmodifiableList(outresult, result);
    }

    return NOERROR;
}

ECode CookieStoreImpl::GetURIs(
    /* [out] */ IList** URIs)
{
    VALIDATE_NOT_NULL(URIs);

    {    AutoLock syncLock(this);
        AutoPtr<IList> outresult;
        AutoPtr<ISet> keyset;
        mMap->GetKeySet((ISet**)&keyset);
        CArrayList::New(ICollection::Probe(keyset), (IList**)&outresult);

        Boolean isflag;
        outresult->Remove(NULL, &isflag); // sigh
        return Collections::UnmodifiableList(outresult, URIs);
    }
    return NOERROR;
}

ECode CookieStoreImpl::Remove(
    /* [in] */ IURI* uri,
    /* [in] */ IHttpCookie* cookie,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);

    {    AutoLock syncLock(this);
        if (cookie == NULL) {
            return E_NULL_POINTER_EXCEPTION;
        }

        AutoPtr<IURI> cookiesUri = GetCookiesUri(uri);

        AutoPtr<IList> cookies;
        AutoPtr<IInterface> outObj;
        mMap->Get(cookiesUri, (IInterface**)&outObj);
        cookies = IList::Probe(outObj);

        if (cookies != NULL) {
            cookies->Remove(cookie, succeeded);
        }
        else {
            *succeeded = FALSE;
        }
    }
    return NOERROR;
}

ECode CookieStoreImpl::RemoveAll(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);

    {    AutoLock syncLock(this);
        Boolean isEmpty;
        mMap->IsEmpty(&isEmpty);
        *succeeded = !isEmpty;
        mMap->Clear();
    }

    return NOERROR;
}

} // namespace Net
} // namespace Elastos
