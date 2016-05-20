
#include "PKIXParameters.h"
#include "CPKIXParameters.h"
#include "CPKIXBuilderParameters.h"
#include "CTrustAnchor.h"
#include "CHashSet.h"
#include "CArrayList.h"
#include "StringBuilder.h"
#include "CDate.h"

using Elastos::Core::ICharSequence;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IEnumeration;
using Elastos::Utility::IListIterator;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashSet;
using Elastos::Utility::CDate;

#define INNER_CLONER(INTERFACE_NAME, CLASS_NAME) do { \
    AutoPtr<INTERFACE_NAME> clonedObj; \
    CLASS_NAME::New((INTERFACE_NAME**)&clonedObj); \
    /* copy fields containing references to mutable objects */ \
    if (mCertStores) { \
        AutoPtr<IArrayList> cs; \
        CArrayList::New(mCertStores.Get(), (IArrayList**)&cs); \
        ((PKIXParameters*)(CLASS_NAME*)clonedObj.Get())->mCertStores = cs; \
    } \
    if (mCertPathCheckers) { \
        AutoPtr<IArrayList> al; \
        CArrayList::New(mCertPathCheckers.Get(), (IArrayList**)&al); \
        ((PKIXParameters*)(CLASS_NAME*)clonedObj.Get())->mCertPathCheckers = al; \
    } \
    *obj = clonedObj.Get(); \
    REFCOUNT_ADD(*obj) \
} while(0);

namespace Elastos {
namespace Security {
namespace Cert {

CAR_INTERFACE_IMPL_2(PKIXParameters, Object, IPKIXParameters, ICertPathParameters, ICloneable)
PKIXParameters::PKIXParameters()
    : mRevocationEnabled(TRUE)
    , mExplicitPolicyRequired(FALSE)
    , mPolicyMappingInhibited(FALSE)
    , mAnyPolicyInhibited(FALSE)
    , mPolicyQualifiersRejected(TRUE)
{}

PKIXParameters::~PKIXParameters()
{}

ECode PKIXParameters::GetTrustAnchors(
    /* [out] */ ISet **trustAnchors)
{
    //Todo
    //return Collections.unmodifiableSet(trustAnchors);
    return E_NOT_IMPLEMENTED;
}

ECode PKIXParameters::SetTrustAnchors(
    /* [in] */ ISet *trustAnchors)
{
    if (!trustAnchors) {
        return E_NULL_POINTER_EXCEPTION;
    }
    CheckTrustAnchors(trustAnchors);
    // make shallow copy
    mTrustAnchors = NULL;
    return CHashSet::New(trustAnchors, (ISet**)&mTrustAnchors);
}

ECode PKIXParameters::IsAnyPolicyInhibited(
    /* [out] */ Boolean *anyPolicyInhibited)
{
    VALIDATE_NOT_NULL(anyPolicyInhibited)
    *anyPolicyInhibited = mAnyPolicyInhibited;
}

ECode PKIXParameters::SetAnyPolicyInhibited(
    /* [in] */ Boolean anyPolicyInhibited)
{
    mAnyPolicyInhibited = anyPolicyInhibited;
    return NOERROR;
}

ECode PKIXParameters::GetCertPathCheckers(
    /* [out] */ IList **certPathCheckers)
{
    if (mCertPathCheckers == NULL) {
        // set to empty List if has not been set yet
        CArrayList::New((IList**)&mCertPathCheckers);
    }
    Boolean isEmpty;
    if ((mCertPathCheckers->IsEmpty(&isEmpty), isEmpty)) {
        // no content - no need to copy,
        // just return immutable view of the same
        // empty List each time
        //Todo
        //return Collections.unmodifiableList(certPathCheckers);
        return E_NOT_IMPLEMENTED;
    }
    // List is not empty - do deep copy
    AutoPtr<IArrayList> modifiableList;
    CArrayList::New((IArrayList**)&modifiableList);
    AutoPtr<IListIterator> it;
    mCertPathCheckers->GetListIterator((IListIterator**)&it);
    Boolean hasNext;
    while ((it->HasNext(&hasNext), hasNext)) {
        AutoPtr<IInterface> elem, clonedElem;
        it->GetNext((IInterface**)&elem);
        IPKIXCertPathChecker::Probe(elem)->Clone((IInterface**)&clonedElem);
        Boolean isModified;
        modifiableList->Add(clonedElem, &isModified);
    }
    //return Collections.unmodifiableList(modifiableList);
    return E_NOT_IMPLEMENTED;
}

ECode PKIXParameters::SetCertPathCheckers(
    /* [in] */ IList *certPathCheckers)
{
    Boolean isEmpty;
    if (certPathCheckers == NULL || (certPathCheckers->IsEmpty(&isEmpty), isEmpty)) {
        // empty list or null provided
        if (mCertPathCheckers &&
            (mCertPathCheckers->IsEmpty(&isEmpty), !isEmpty)) {
                // discard non-empty list
                mCertPathCheckers = NULL;
            }
            return NOERROR;
    }
    // non-empty list provided - do deep copy
    mCertPathCheckers = NULL;
    CArrayList::New((IList**)&mCertPathCheckers);
    AutoPtr<IListIterator> it;
    certPathCheckers->GetListIterator((IListIterator**)&it);
    Boolean hasNext;
    while ((it->HasNext(&hasNext), hasNext)) {
        AutoPtr<IInterface> elem, clonedElem;
        it->GetNext((IInterface**)&elem);
        IPKIXCertPathChecker::Probe(elem)->Clone((IInterface**)&clonedElem);
        Boolean isModified;
        mCertPathCheckers->Add(clonedElem, &isModified);
    }
    return NOERROR;
}

ECode PKIXParameters::AddCertPathChecker(
    /* [in] */ IPKIXCertPathChecker *checker)
{
    if (checker == NULL) {
        // do nothing if null provided
        return NOERROR;
    }
    if (mCertPathCheckers == NULL) {
        // set to empty List if has not been set yet
        mCertPathCheckers = NULL;
        CArrayList::New((IList**)&mCertPathCheckers);
    }
    // add a copy to avoid possible modifications
    AutoPtr<IInterface> elem;
    checker->Clone((IInterface**)&elem);
    Boolean isModified;
    return mCertPathCheckers->Add(elem, &isModified);
}

ECode PKIXParameters::GetCertStores(
    /* [out] */ IList **certStores)
{
    if (mCertStores == NULL) {
        // set to empty List if has not been set yet
        CArrayList::New((IList**)&mCertStores);
    }
    Boolean isEmpty;
    if ((mCertStores->IsEmpty(&isEmpty), isEmpty)) {
        // no content - no need to copy,
        // just return immutable view of the same
        // empty List each time
        //return Collections.unmodifiableList(certStores);
        return E_NOT_IMPLEMENTED;
    }
    // List is not empty - do shallow copy
    AutoPtr<IArrayList> modifiableList;
    CArrayList::New(mCertStores, (IArrayList**)&modifiableList);
    //return Collections.unmodifiableList(modifiableList);
    return E_NOT_IMPLEMENTED;
}

ECode PKIXParameters::SetCertStores(
    /* [in] */ IList *certStores)
{
    Boolean isEmpty;
    if (!certStores || (certStores->IsEmpty(&isEmpty), isEmpty)) {
        // empty list or null provided
        if (mCertStores && (mCertStores->IsEmpty(&isEmpty), !isEmpty)) {
            // discard non-empty list
            mCertStores = NULL;
        }
        return NOERROR;
    }
    // non-empty list provided - do shallow copy
    mCertStores = NULL;
    return CArrayList::New(certStores, (IList**)&mCertStores);
}

ECode PKIXParameters::AddCertStore(
    /* [in] */ ICertStore *store)
{
    if (store == NULL) {
        // do nothing if null provided
        return NOERROR;
    }
    if (mCertStores == NULL) {
        // set to empty List if has not been set yet
        CArrayList::New((IList**)&mCertStores);
    }
    // add store
    Boolean isModified;
    return mCertStores->Add(store, &isModified);
}

ECode PKIXParameters::GetDate(
    /* [out] */ IDate **date)
{
    VALIDATE_NOT_NULL(date)
    if (!mDate) {
        *date = NULL;
    }
    else {
        AutoPtr<IInterface> clonedObj;
        Elastos::Core::ICloneable::Probe(mDate)->Clone((IInterface**)&clonedObj);
        *date = IDate::Probe(clonedObj);
        REFCOUNT_ADD(*date)
    }
    return NOERROR;
}

ECode PKIXParameters::SetDate(
    /* [in] */ IDate *date)
{
    if (date == NULL) {
        mDate = NULL;
        return NOERROR;
    }
    else {
        Int64 tm;
        date->GetTime(&tm);
        mDate = NULL;
        return CDate::New(tm, (IDate**)&mDate);
    }
}

ECode PKIXParameters::IsExplicitPolicyRequired(
    /* [out] */ Boolean *explicitPolicyRequired)
{
    VALIDATE_NOT_NULL(explicitPolicyRequired)
    *explicitPolicyRequired = mExplicitPolicyRequired;
    return NOERROR;
}

ECode PKIXParameters::SetExplicitPolicyRequired(
    /* [in] */ Boolean explicitPolicyRequired)
{
    mExplicitPolicyRequired = explicitPolicyRequired;
    return NOERROR;
}

ECode PKIXParameters::GetInitialPolicies(
    /* [out] */ ISet **initialPolicies)
{
    VALIDATE_NOT_NULL(initialPolicies)
    if (mInitialPolicies == NULL) {
        // set to empty Set if has not been set yet
        CHashSet::New((ISet**)&mInitialPolicies);
    }
    Boolean isEmpty;
    if ((mInitialPolicies->IsEmpty(&isEmpty), isEmpty)) {
        // no content - no need to copy,
        // just return immutable view of the same
        // empty Set each time
        //return Collections.unmodifiableSet(initialPolicies);
        return E_NOT_IMPLEMENTED;
    }
    // List is not empty - do shallow copy
    AutoPtr<ISet> modifiableSet;
    CHashSet::New(mInitialPolicies, (ISet**)&modifiableSet);
    //return Collections.unmodifiableSet(modifiableSet);
    return E_NOT_IMPLEMENTED;
}

ECode PKIXParameters::SetInitialPolicies(
    /* [in] */ ISet *initialPolicies)
{
    Boolean isEmpty;
    if (!initialPolicies || (initialPolicies->IsEmpty(&isEmpty), isEmpty)) {
        // empty list or null provided
        if (mInitialPolicies && (mInitialPolicies->IsEmpty(&isEmpty), !isEmpty)) {
            // discard non-empty list
            mInitialPolicies = NULL;
        }
        return NOERROR;
    }
    // non-empty list provided - do shallow copy
    mInitialPolicies = NULL;
    return CHashSet::New(initialPolicies, (ISet**)&mInitialPolicies);
}

ECode PKIXParameters::IsPolicyMappingInhibited(
    /* [out] */ Boolean *policyMappingInhibited)
{
    VALIDATE_NOT_NULL(policyMappingInhibited)
    *policyMappingInhibited = mPolicyMappingInhibited;
    return NOERROR;
}

ECode PKIXParameters::SetPolicyMappingInhibited(
    /* [in] */ Boolean policyMappingInhibited)
{
    mPolicyMappingInhibited = policyMappingInhibited;
    return NOERROR;
}

ECode PKIXParameters::GetPolicyQualifiersRejected(
    /* [out] */ Boolean * policyQualifiersRejected)
{
    VALIDATE_NOT_NULL(policyQualifiersRejected)
    *policyQualifiersRejected = mPolicyQualifiersRejected;
    return NOERROR;
}

ECode PKIXParameters::SetPolicyQualifiersRejected(
    /* [in] */ Boolean policyQualifiersRejected)
{
    mPolicyQualifiersRejected = policyQualifiersRejected;
    return NOERROR;
}

ECode PKIXParameters::IsRevocationEnabled(
    /* [out] */ Boolean *revocationEnabled)
{
    VALIDATE_NOT_NULL(revocationEnabled)
    *revocationEnabled = mRevocationEnabled;
    return NOERROR;
}

ECode PKIXParameters::SetRevocationEnabled(
    /* [in] */ Boolean revocationEnabled)
{
    mRevocationEnabled = revocationEnabled;
    return NOERROR;
}

ECode PKIXParameters::GetSigProvider(
    /* [out] */ String *sigProvider)
{
    VALIDATE_NOT_NULL(sigProvider)
    *sigProvider = mSigProvider;
    return NOERROR;
}

ECode PKIXParameters::SetSigProvider(
    /* [in] */ const String& sigProvider)
{
    mSigProvider = sigProvider;
    return NOERROR;
}

ECode PKIXParameters::GetTargetCertConstraints(
    /* [out] */ ICertSelector **targetCertConstraints)
{
    VALIDATE_NOT_NULL(targetCertConstraints)
    AutoPtr<IInterface> cs;
    if (mTargetCertConstraints) {
        mTargetCertConstraints->Clone((IInterface**)&cs);
    }
    *targetCertConstraints = ICertSelector::Probe(cs);
    REFCOUNT_ADD(*targetCertConstraints)
    return NOERROR;
}

ECode PKIXParameters::SetTargetCertConstraints(
    /* [in] */ ICertSelector *targetCertConstraints)
{
    if (targetCertConstraints == NULL) {
        mTargetCertConstraints = NULL;
    }
    else {
        AutoPtr<IInterface> cc;
        targetCertConstraints->Clone((IInterface**)&cc);
        mTargetCertConstraints = ICertSelector::Probe(cc);
    }
    return NOERROR;
}

ECode PKIXParameters::Clone(
    /* [out] */ IInterface **obj)
{
    VALIDATE_NOT_NULL(obj)
    // do shallow copy first
    if (IPKIXParameters::Probe(this)) {
        INNER_CLONER(IPKIXParameters, CPKIXParameters)
        return NOERROR;
    }
    else if (IPKIXBuilderParameters::Probe(this)) {
        INNER_CLONER(IPKIXBuilderParameters, CPKIXBuilderParameters)
        return NOERROR;
    }
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode PKIXParameters::ToString(
    /* [out] */ String *str)
{
    StringBuilder sb("[\n Trust Anchors: ");
    sb.AppendObject(mTrustAnchors.Get());
    sb.AppendCStr("\n Revocation Enabled: ");
    sb.AppendBoolean(mRevocationEnabled);
    sb.AppendCStr("\n Explicit Policy Required: ");
    sb.AppendBoolean(mExplicitPolicyRequired);
    sb.AppendCStr("\n Policy Mapping Inhibited: ");
    sb.AppendBoolean(mPolicyMappingInhibited);
    sb.AppendCStr("\n Any Policy Inhibited: ");
    sb.AppendBoolean(mAnyPolicyInhibited);
    sb.AppendCStr("\n Policy Qualifiers Rejected: ");
    sb.AppendBoolean(mPolicyQualifiersRejected);
    sb.AppendCStr("\n Initial Policy OIDs: ");
    Boolean isEmpty;
    if (!mInitialPolicies || (mInitialPolicies->IsEmpty(&isEmpty), isEmpty)) {
        sb.AppendCStr("any");
    }
    else {
        String policies = Object::ToString(mInitialPolicies);
        sb.AppendString(policies);
    }
    sb.AppendCStr("\n Cert Stores: ");
    if (mCertStores == NULL || (mCertStores->IsEmpty(&isEmpty), isEmpty)) {
        sb.AppendCStr("no");
    }
    else {
        String certStore = Object::ToString(mCertStores.Get());
        sb.AppendString(certStore);
    }
    sb.AppendCStr("\n Validity Date: ");
    sb.AppendObject(mDate.Get());
    sb.AppendCStr("\n Cert Path Checkers: ");
    if (!mCertPathCheckers || (mCertPathCheckers->IsEmpty(&isEmpty), isEmpty)) {
        sb.AppendCStr("no");
    }
    else {
        String certPathCheckers = Object::ToString(mCertPathCheckers.Get());
        sb.AppendString(certPathCheckers);
    }
    sb.AppendCStr("\n Signature Provider: ");
    sb.AppendString(mSigProvider);
    sb.AppendCStr("\n Target Certificate Constraints: ");
    sb.AppendObject(mTargetCertConstraints.Get());
    sb.AppendCStr("\n]");
    return sb.ToString(str);
}

ECode PKIXParameters::constructor(
    /* [in] */ ISet *trustAnchors)
{
    if (!trustAnchors) {
        return E_NULL_POINTER_EXCEPTION;
    }
    CheckTrustAnchors(trustAnchors);
    return CHashSet::New(trustAnchors, (ISet**)&mTrustAnchors);
}

ECode PKIXParameters::constructor(
    /* [in] */ IKeyStore *keyStore)
{
    if (keyStore == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    // Will throw KeyStoreException if
    // keyStore has not been initialized (loaded)
    Int32 size;
    keyStore->GetSize(&size);
    if (size == 0) {
        return E_INVALID_ALGORITHM_PARAMETER_EXCEPTION;
    }
    // keyStore is not null and loaded
    CHashSet::New((ISet**)&mTrustAnchors);
    AutoPtr<IEnumeration> enumeration;
    keyStore->Aliases((IEnumeration**)&enumeration);
    Boolean hasNext;
    while ((enumeration->HasMoreElements(&hasNext), hasNext)) {
        AutoPtr<IInterface> elem;
        enumeration->NextElement((IInterface**)&elem);
        String alias;
        ICharSequence::Probe(elem)->ToString(&alias);
        Boolean isEntry;
        keyStore->IsCertificateEntry(alias, &isEntry);
        if (isEntry) {
            // this is trusted certificate entry
            // check if it is X509Certificate
            AutoPtr<ICertificate> c;
            keyStore->GetCertificate(alias, (ICertificate**)&c);
            // add only X509Certificate
            // ignore all other types
            if (IX509Certificate::Probe(c.Get()) != NULL) {
                AutoPtr<ITrustAnchor> trustAnchor;
                AutoPtr<ArrayOf<Byte> > nullArray = ArrayOf<Byte>::Alloc(0);
                CTrustAnchor::New(IX509Certificate::Probe(c.Get()), *nullArray, (ITrustAnchor**)&trustAnchor);
                Boolean isModified;
                mTrustAnchors->Add(trustAnchor.Get(), &isModified);
            }
        }
    }
    return CheckTrustAnchors(mTrustAnchors);
}

ECode PKIXParameters::CheckTrustAnchors(
    /* [in] */ ISet *trustAnchors)
{
    Boolean empty;
    trustAnchors->IsEmpty(&empty);
    if (empty) {
        return E_INVALID_ALGORITHM_PARAMETER_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Cert
} // namespace Security
} // namespace Elastos

