
#include "CNamespaceSupport.h"
#include "CArrayList.h"
#include "Collections.h"
#include "CString.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::Collections;
using Elastos::Utility::IList;

namespace Org {
namespace Xml {
namespace Sax {
namespace Helpers {

static AutoPtr<IEnumeration> InitEMPTY_ENUMERATION()
{
    AutoPtr<IList> list = Collections::EMPTY_LIST;
    AutoPtr<IEnumeration> enu;
    Collections::Enumeration(ICollection::Probe(list), (IEnumeration**)&enu);
    return enu;
}

AutoPtr<IEnumeration> CNamespaceSupport::EMPTY_ENUMERATION;

CAR_INTERFACE_IMPL(CNamespaceSupport, Object, INamespaceSupport)

CAR_OBJECT_IMPL(CNamespaceSupport)

ECode CNamespaceSupport::Reset()
{
    mContexts = ArrayOf<Context*>::Alloc(32);
    mNamespaceDeclUris = false;
    mContextPos = 0;
    mCurrentContext = new Context(this);
    mContexts->Set(mContextPos, mCurrentContext);
    mCurrentContext->DeclarePrefix(String("xml"), XMLNS);

    return NOERROR;
}

ECode CNamespaceSupport::PushContext()
{
    Int32 max = mContexts->GetLength();

    (*mContexts)[mContextPos]->mDeclsOK = FALSE;
    mContextPos++;

    // Extend the array if necessary
    if (mContextPos >= max) {
        AutoPtr<ArrayOf<Context*> > newContexts = ArrayOf<Context*>::Alloc(max*2);
        newContexts->Copy(0, mContexts, 0, max);
        max *= 2;
        mContexts = newContexts;
    }

    // Allocate the context if necessary.
    mCurrentContext = (*mContexts)[mContextPos];
    if (mCurrentContext == NULL) {
        mCurrentContext = new Context(this);
        mContexts->Set(mContextPos, mCurrentContext);
    }

    // Set the parent, if any.
    if (mContextPos > 0) {
        mCurrentContext->SetParent((*mContexts)[mContextPos - 1]);
    }
    return NOERROR;
}

ECode CNamespaceSupport::PopContext()
{
    (*mContexts)[mContextPos]->Clear();
    mContextPos--;
    if (mContextPos < 0) {
        // throw new EmptyStackException();
        return E_SAX_EMPTYSTACK_EXCEPTION;
    }
    mCurrentContext = (*mContexts)[mContextPos];
    return NOERROR;
}

ECode CNamespaceSupport::DeclarePrefix(
    /* [in] */ const String& prefix,
    /* [in] */ const String& uri,
    /* [out] */ Boolean* isPrefixLegal)
{
    VALIDATE_NOT_NULL(isPrefixLegal)

    if (prefix.Equals("xml") || prefix.Equals("xmlns")) {
        *isPrefixLegal = FALSE;
    }
    else {
        mCurrentContext->DeclarePrefix(prefix, uri);
        *isPrefixLegal = TRUE;
    }
    return NOERROR;
}

ECode CNamespaceSupport::ProcessName(
    /* [in] */ const String& qName,
    /* [in] */ ArrayOf<String>* parts,
    /* [in] */ Boolean isAttribute,
    /* [out, callee] */ ArrayOf<String>** nName)
{
    VALIDATE_NOT_NULL(nName)
    *nName = NULL;
    VALIDATE_NOT_NULL(parts)
    assert(parts->GetLength() >= 3);

    AutoPtr< ArrayOf<String> > myParts = mCurrentContext->ProcessName(qName, isAttribute);
    if (myParts != NULL) {
        assert(myParts->GetLength() >= 3);
        (*parts)[0] = (*myParts)[0];
        (*parts)[1] = (*myParts)[1];
        (*parts)[2] = (*myParts)[2];

        *nName = parts;
        REFCOUNT_ADD(*nName)
    }
    return NOERROR;
}

ECode CNamespaceSupport::GetURI(
    /* [in] */ const String& prefix,
    /* [out] */ String* URI)
{
    VALIDATE_NOT_NULL(URI)

    *URI = mCurrentContext->GetURI(prefix);
    return NOERROR;
}

ECode CNamespaceSupport::GetPrefixes(
    /* [out] */ IEnumeration** prefixes)
{
    VALIDATE_NOT_NULL(prefixes)

    AutoPtr<IEnumeration> res = mCurrentContext->GetPrefixes();
    *prefixes = res;
    REFCOUNT_ADD(*prefixes)

    return NOERROR;
}

ECode CNamespaceSupport::GetPrefix(
    /* [in] */ const String& uri,
    /* [out] */ String* prefix)
{
    VALIDATE_NOT_NULL(prefix)

    *prefix = mCurrentContext->GetPrefix(uri);
    return NOERROR;
}

ECode CNamespaceSupport::GetPrefixes(
    /* [in] */ const String& uri,
    /* [out] */ IEnumeration** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IArrayList> prefixes;
    CArrayList::New((IArrayList**)&prefixes);

    AutoPtr<IEnumeration> allPrefixes;
    GetPrefixes((IEnumeration**)&allPrefixes);

    Boolean hasMore;
    while (allPrefixes->HasMoreElements(&hasMore), hasMore) {
        AutoPtr<IInterface> obj;
        allPrefixes->GetNextElement((IInterface**)&obj);
        ICharSequence* seq =  ICharSequence::Probe(obj);
        String prefix, us;
        seq->ToString(&prefix);
        GetURI(prefix, &us);
        if (uri.Equals(us)) {
            AutoPtr<ICharSequence> csq;
            CString::New(prefix, (ICharSequence**)&csq);
            prefixes->Add(csq);
        }
    }

    *result = IEnumeration::Probe(prefixes);
    REFCOUNT_ADD(*result)

    return NOERROR;
}

ECode CNamespaceSupport::GetDeclaredPrefixes(
    /* [out] */ IEnumeration** prefixes)
{
    VALIDATE_NOT_NULL(prefixes)

    AutoPtr<IEnumeration> res = mCurrentContext->GetDeclaredPrefixes();
    *prefixes = res;
    REFCOUNT_ADD(*prefixes)

    return NOERROR;
}

ECode CNamespaceSupport::SetNamespaceDeclUris(
    /* [in] */ Boolean value)
{
    if (mContextPos != 0) {
        // throw new IllegalStateException ();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (value == mNamespaceDeclUris)
        return NOERROR;
    mNamespaceDeclUris = value;
    if (value)
        mCurrentContext->DeclarePrefix(String("xmlns"), NSDECL);
    else {
        mCurrentContext = new Context(this);
        (*mContexts)[mContextPos] = mCurrentContext;
        mCurrentContext->DeclarePrefix(String("xml"), XMLNS);
    }
    return NOERROR;
}

ECode CNamespaceSupport::IsNamespaceDeclUris(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mNamespaceDeclUris;
    return NOERROR;
}

ECode CNamespaceSupport::constructor()
{
    return Reset();
}

CNamespaceSupport::Context::Context(
    /* [in] */ CNamespaceSupport* parent)
{
    AutoPtr<IWeakReferenceSource> wrs = (IWeakReferenceSource*)parent->Probe(EIID_IWeakReferenceSource);
    wrs->GetWeakReference((IWeakReference**)&mWeakParentHolder);
    mDeclSeen = FALSE;
    mParentContext = NULL;

    mDefaultNS = NULL;
    mDeclsOK = TRUE;

    CopyTables();
}

CNamespaceSupport::Context::~Context()
{
}

void CNamespaceSupport::Context::SetParent (
    /* [in] */ Context* parent)
{
    mParentContext = parent;
    mPrefixTable = parent->mPrefixTable;
    mUriTable = parent->mUriTable;
    mElementNameTable = parent->mElementNameTable;
    mAttributeNameTable = parent->mAttributeNameTable;
    mDefaultNS = parent->mDefaultNS;
    mDeclSeen = FALSE;
    mDeclsOK = TRUE;
}

void CNamespaceSupport::Context::Clear ()
{
    mParentContext = NULL;
    mDefaultNS = String(NULL);
}

ECode CNamespaceSupport::Context::DeclarePrefix(
    /* [in] */ const String& prefix,
    /* [in] */ const String& uri)
{
    // Lazy processing...
    if (!mDeclsOK) {
        // throw new IllegalStateException ("can't declare any more prefixes in this context");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (!mDeclSeen) {
       CopyTables();
    }

    if (mDeclarations == NULL) {
        CArrayList::New((IArrayList**)&mDeclarations);
    }

    if (prefix.Equals("")) {
        if (uri.Equals("")) {
            mDefaultNS = String(NULL);
        }
        else {
            mDefaultNS = uri;
        }
    }
    else {
        mPrefixTable[prefix] = uri;
        mUriTable[uri] = prefix; // may wipe out another prefix
    }

    AutoPtr<ICharSequence> seq;
    CString::New(prefix, (ICharSequence**)&seq);
    return mDeclarations->Add(seq);
}

AutoPtr< ArrayOf<String> > CNamespaceSupport::Context::ProcessName (
    /* [in] */ const String& qName,
    /* [in] */ Boolean isAttribute)
{
    AutoPtr< ArrayOf<String> > name;
    HashMap<String, AutoPtr< ArrayOf<String> > >* table;

    // detect errors in call sequence
    mDeclsOK = FALSE;

    // Select the appropriate table.
    if (isAttribute) {
        table = &mAttributeNameTable;
    }
    else {
        table = &mElementNameTable;
    }

    // Start by looking in the cache, and
    // return immediately if the name
    // is already known in this content
    name = (*table)[qName];
    if (name != NULL) {
        return name;
    }

    // We haven't seen this name in this
    // context before.  Maybe in the parent
    // context, but we can't assume prefix
    // bindings are the same.
    name = ArrayOf<String>::Alloc(3);
    (*name)[2] = qName;// qName.intern();
    Int32 index = qName.IndexOf(':');

    // No prefix.
    if (index == -1) {
        if (isAttribute) {
            (*name)[0] = "";

            if (qName.Equals("xmlns")) {
                AutoPtr<INamespaceSupport> ns;
                mWeakParentHolder->Resolve(EIID_INamespaceSupport, (IInterface**)&ns);
                if (ns != NULL) {
                    CNamespaceSupport* holder = (CNamespaceSupport*)ns.Get();
                    if (holder->mNamespaceDeclUris) {
                        (*name)[0] = NSDECL;
                    }
                }
            }
        }
        else if (mDefaultNS.IsNull()) {
            (*name)[0] = "";
        }
        else {
            (*name)[0] = mDefaultNS;
        }
        (*name)[1] = (*name)[2];
    }
    else { // Prefix
        String prefix = qName.Substring(0, index);
        String local = qName.Substring(index+1);
        String uri;
        if (prefix.Equals("")) {
            uri = mDefaultNS;
        }
        else {
            uri = mPrefixTable[prefix];
        }
        if (uri.IsNull() || (!isAttribute && prefix.Equals("xmlns"))) {
           return NULL;
        }
        (*name)[0] = uri;
        (*name)[1] = local; // local.intern();
    }

    // Save in the cache for future use.
    // (Could be shared with parent context...)
    (*table)[(*name)[2]] = name;
    return name;
}


String CNamespaceSupport::Context::GetURI (
    /* [in] */ const String& prefix)
{
    if (prefix.Equals("")) {
        return mDefaultNS;
    }
    else if (mPrefixTable.IsEmpty()) {
        return String(NULL);
    }
    else {
        return mPrefixTable[prefix];
    }
    return String("");
}

String CNamespaceSupport::Context::GetPrefix (
    /* [in] */ const String& uri)
{
    if (mUriTable.IsEmpty()) {
        return String(NULL);
    } else {
        return mUriTable[uri];
    }
    return String("");
}

AutoPtr<IEnumeration> CNamespaceSupport::Context::GetDeclaredPrefixes()
{
    if (mDeclarations == NULL) {
        if (EMPTY_ENUMERATION == NULL) {
            EMPTY_ENUMERATION = InitEMPTY_ENUMERATION();
        }
        return EMPTY_ENUMERATION;
    }
    else {
        AutoPtr<IEnumeration> enu;
        Collections::Enumeration(ICollection::Probe(mDeclarations), (IEnumeration**)&enu);
        return enu;
    }
}

AutoPtr<IEnumeration> CNamespaceSupport::Context::GetPrefixes ()
{
    if (mPrefixTable.IsEmpty()) {
        if (EMPTY_ENUMERATION == NULL) {
            EMPTY_ENUMERATION = InitEMPTY_ENUMERATION();
        }
        return EMPTY_ENUMERATION;
    }
    else {
        AutoPtr<IArrayList> prefixes;
        CArrayList::New((IArrayList**)&prefixes);

        HashMap<String, String>::Iterator iter = mPrefixTable.Begin();
        while (iter != mPrefixTable.End()) {
            AutoPtr<ICharSequence> csq;
            CString::New(iter->mFirst, (ICharSequence**)&csq);
            prefixes->Add(csq);
            iter++;
        }

        AutoPtr<IEnumeration> enu;
        Collections::Enumeration(ICollection::Probe(prefixes), (IEnumeration**)&enu);
        return enu;
    }
}

void CNamespaceSupport::Context::CopyTables ()
{
    if (!mPrefixTable.IsEmpty()) {
        assert(0 && "TODO");
        // mPrefixTable = (Hashtable)mPrefixTable.clone();
    }
    else {
        // mPrefixTable = new Hashtable();
    }
    if (!mUriTable.IsEmpty()) {
        // mUriTable = (Hashtable)mUriTable.clone();
    }
    else {
        // mUriTable = new Hashtable();
    }
    mElementNameTable.Clear();
    mAttributeNameTable.Clear();
    mDeclSeen = TRUE;
}

} // namespace Helpers
} // namespace Sax
} // namespace Xml
} // namespace Org
