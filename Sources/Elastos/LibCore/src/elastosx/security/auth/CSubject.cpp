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

#include "CSubject.h"
#include "CAuthPermission.h"
#include "CAccessController.h"
#include "CAccessControlContext.h"
#include "CSubjectDomainCombiner.h"
#include "StringBuilder.h"
#include "CLinkedList.h"

using Elastos::Core::StringBuilder;
using Elastos::Security::CAccessController;
using Elastos::Security::CAccessControlContext;
using Elastos::Security::IAccessController;
using Elastos::Security::IProtectionDomain;
using Elastos::Utility::EIID_IIterator;
using Elastos::Utility::EIID_ISet;
using Elastos::Utility::CLinkedList;
using Elastosx::Security::Auth::CSubjectDomainCombiner;
using Elastos::Utility::EIID_ICollection;
using Elastos::Core::IInteger16;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::IByte;
using Elastos::Core::IFloat;
using Elastos::Core::IDouble;
using Elastos::Core::IChar32;
using Elastos::Core::IBoolean;
using Elastos::Security::EIID_IPrincipal;

namespace Elastosx {
namespace Security {
namespace Auth {

CAR_OBJECT_IMPL(CSubject)

CAR_INTERFACE_IMPL(CSubject, Object, ISubject)

const Int64 CSubject::serialVersionUID = -8308522755600156056L;

const Int64 CSubject::SecureSet::serialVersionUID = 7911754171111800359L;

AutoPtr<IPermission> GetNewAuthPermission(const String& name)
{
    AutoPtr<IPermission> rev;
    CAuthPermission::New(name, (IPermission**)&rev);
    return rev;
}

const AutoPtr<IPermission> CSubject::_AS_PRIVILEGED = GetNewAuthPermission(String("doAsPrivileged"));

const AutoPtr<IPermission> CSubject::_SUBJECT = GetNewAuthPermission(String("getSubject"));

const AutoPtr<IPermission> CSubject::_PRINCIPALS = GetNewAuthPermission(String("modifyPrincipals"));

const AutoPtr<IPermission> CSubject::_PRIVATE_CREDENTIALS = GetNewAuthPermission(String("modifyPrivateCredentials"));

const AutoPtr<IPermission> CSubject::_PUBLIC_CREDENTIALS = GetNewAuthPermission(String("modifyPublicCredentials"));

const AutoPtr<IPermission> CSubject::_READ_ONLY = GetNewAuthPermission(String("setReadOnly"));

const AutoPtr<IPermission> CSubject::_AS = GetNewAuthPermission(String("doAs"));

ECode CSubject::DoAs(
    /* [in] */ ISubject *subject,
    /* [in] */ IPrivilegedAction *action,
    /* [out] */ IInterface **obj)
{
    VALIDATE_NOT_NULL(obj)
    *obj = NULL;
    VALIDATE_NOT_NULL(subject)
    VALIDATE_NOT_NULL(action)

    AutoPtr<IAccessController> ac;
    AutoPtr<IAccessControlContext> acc;
    CAccessController::AcquireSingleton((IAccessController**)&ac);
    ac->GetContext((IAccessControlContext**)&acc);
    return DoAs_PrivilegedAction(subject, action, acc, obj);
}

ECode CSubject::DoAsPrivileged(
    /* [in] */ ISubject* subject,
    /* [in] */ IPrivilegedAction* action,
    /* [in] */ IAccessControlContext* context,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    *obj = NULL;
    VALIDATE_NOT_NULL(subject)
    VALIDATE_NOT_NULL(action)

    if (NULL == context) {
        AutoPtr<ArrayOf<IProtectionDomain*> > context = ArrayOf<IProtectionDomain*>::Alloc(0);
        AutoPtr<IAccessControlContext> acc;
        CAccessControlContext::New(context, (IAccessControlContext**)&acc);
        return DoAs_PrivilegedAction(subject, action, acc, obj);
    }
    return DoAs_PrivilegedAction(subject, action, context, obj);
}

ECode CSubject::DoAs(
    /* [in] */ ISubject* subject,
    /* [in] */ IPrivilegedExceptionAction* action,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    *obj = NULL;
    VALIDATE_NOT_NULL(subject)
    VALIDATE_NOT_NULL(action)

    AutoPtr<IAccessController> ac;
    AutoPtr<IAccessControlContext> acc;
    CAccessController::AcquireSingleton((IAccessController**)&ac);
    ac->GetContext((IAccessControlContext**)&acc);
    return DoAs_PrivilegedExceptionAction(subject, action, acc, obj);
}

ECode CSubject::DoAsPrivileged(
    /* [in] */ ISubject* subject,
    /* [in] */ IPrivilegedExceptionAction* action,
    /* [in] */ IAccessControlContext* context,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    *obj = NULL;
    VALIDATE_NOT_NULL(subject)
    VALIDATE_NOT_NULL(action)

    if (NULL == context) {
        AutoPtr<ArrayOf<IProtectionDomain*> > pd = ArrayOf<IProtectionDomain*>::Alloc(0);
        AutoPtr<IAccessControlContext> acc;
        CAccessControlContext::New(pd, (IAccessControlContext**)&acc);
        return DoAs_PrivilegedExceptionAction(subject, action, acc, obj);
    }
    return DoAs_PrivilegedExceptionAction(subject, action, context, obj);
}

ECode CSubject::GetSubject(
    /* [in] */ IAccessControlContext* context,
    /* [out] */ ISubject** subject)
{
    VALIDATE_NOT_NULL(subject)
    *subject = NULL;

    if (NULL == context) {
        return E_NULL_POINTER_EXCEPTION;
    }
    class InnerSub_PrivilegedAction
        : public Object
        , public IPrivilegedAction
    {
    public:
        CAR_INNER_INTERFACE_IMPL(Object, IPrivilegedAction)

        InnerSub_PrivilegedAction(IAccessControlContext* context)
            : mContext(context)
        {}

        ECode Run(
            /* [out] */ IInterface** result)
        {
            VALIDATE_NOT_NULL(result)
            *result = NULL;

            AutoPtr<IDomainCombiner> dc;
            mContext->GetDomainCombiner((IDomainCombiner**)&dc);
            *result = dc.Get();
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
    private:
        AutoPtr<IAccessControlContext> mContext;
    };
    AutoPtr<IPrivilegedAction> action = new InnerSub_PrivilegedAction(context);
    AutoPtr<IAccessController> ac;
    CAccessController::AcquireSingleton((IAccessController**)&ac);
    AutoPtr<IInterface> cmb;
    ac->DoPrivileged(action, (IInterface**)&cmb);
    AutoPtr<IDomainCombiner> combiner = IDomainCombiner::Probe(cmb);
    if (NULL == combiner || !ISubjectDomainCombiner::Probe(combiner.Get())) {
        return NOERROR;
    }
    return ISubjectDomainCombiner::Probe(combiner.Get())->GetSubject(subject);
}

ECode CSubject::Equals(
    /* [in] */ IInterface *obj,
    /* [out] */ Boolean *isEqual)
{
    VALIDATE_NOT_NULL(isEqual)
    *isEqual = FALSE;

    if (TO_IINTERFACE(this) == obj) {
        *isEqual = TRUE;
    }

    ClassID thisID, objID;
    this->GetClassID(&thisID);
    IObject::Probe(obj)->GetClassID(&objID);

    if (NULL == obj || thisID != objID) {
        return NOERROR;
    }

    ISubject* thatObj = (ISubject*)obj->Probe(EIID_ISubject);
    if (NULL == thatObj) {
        return NOERROR;
    }

    if (this == thatObj) {
        *isEqual = TRUE;
        return NOERROR;
    }

    CSubject* that = (CSubject*)thatObj;
    Boolean equals1, equals2, equals3;
    mPrincipals->Equals(that->mPrincipals, &equals1);
    mPublicCredentials->Equals((ISet*)that->mPublicCredentials, &equals2);
    mPrivateCredentials->Equals((ISet*)that->mPrivateCredentials, &equals3);
    if (equals1 && equals2 && equals3) {
        *isEqual = TRUE;
        return NOERROR;
    }
    return NOERROR;
}

ECode CSubject::GetPrincipals(
    /* [out] */ ISet **principals)
{
    VALIDATE_NOT_NULL(principals)

    *principals = mPrincipals;
    REFCOUNT_ADD(*principals)
    return NOERROR;
}

ECode CSubject::GetPrincipals(
    /* [in] */ IInterface *c,
    /* [out] */ ISet **principals)
{
    VALIDATE_NOT_NULL(principals)
    *principals = NULL;
    VALIDATE_NOT_NULL(c)

    ClassID clsId;
    IObject::Probe(c)->GetClassID(&clsId);
    return ((SecureSet*)mPrincipals.Get())->Get(clsId, principals);
}

ECode CSubject::GetPrivateCredentials(
    /* [out] */ ISet **credentials)
{
    VALIDATE_NOT_NULL(credentials)
    *credentials = mPrivateCredentials;
    REFCOUNT_ADD(*credentials)
    return NOERROR;
}

ECode CSubject::GetPrivateCredentials(
    /* [in] */ const ClassID& id,
    /* [out] */ ISet **credentials)
{
    VALIDATE_NOT_NULL(credentials)
    *credentials = NULL;

    return mPrivateCredentials->Get(id, credentials);
}

ECode CSubject::GetPublicCredentials(
    /* [out] */ ISet **credentials)
{
    VALIDATE_NOT_NULL(credentials)

    *credentials = mPublicCredentials;
    REFCOUNT_ADD(*credentials)
    return NOERROR;
}

ECode CSubject::GetPublicCredentials(
    /* [in] */ const ClassID& id,
    /* [out] */ ISet **credentials)
{
    VALIDATE_NOT_NULL(credentials)
    *credentials = NULL;

    return mPublicCredentials->Get(id, credentials);
}

ECode CSubject::GetHashCode(
    /* [out] */ Int32 *hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    *hashCode = 0;

    Int32 hc;
    mPrincipals->GetHashCode(hashCode);
    mPrivateCredentials->GetHashCode(&hc);
    *hashCode += hc;
    mPublicCredentials->GetHashCode(&hc);
    *hashCode += hc;
    return NOERROR;
}

ECode CSubject::SetReadOnly()
{
    mReadOnly = TRUE;
    return NOERROR;
}

ECode CSubject::IsReadOnly(
    /* [out] */ Boolean *isReadOnly)
{
    VALIDATE_NOT_NULL(isReadOnly)

    *isReadOnly = mReadOnly;
    return NOERROR;
}

ECode CSubject::ToString(
    /* [out] */ String *str)
{
    VALIDATE_NOT_NULL(str)
    *str = String(NULL);

    StringBuilder buf("Subject:\n");
    AutoPtr<IIterator> it;
    mPrincipals->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while(it->HasNext(&hasNext), hasNext) {
        buf.Append("\tPrincipal: ");
        AutoPtr<IInterface> elem;
        it->GetNext((IInterface**)&elem);
        buf.Append(elem);
        buf.AppendChar('\n');
    }

    it = NULL;
    mPublicCredentials->GetIterator((IIterator**)&it);
    while(it->HasNext(&hasNext), hasNext) {
        buf.Append("\tPublic Credential: ");
        AutoPtr<IInterface> elem;
        it->GetNext((IInterface**)&elem);
        buf.Append(elem);
        buf.AppendChar('\n');
    }

    Int32 offset;
    buf.GetLength(&offset);
    offset -= 1;
    it = NULL;
    mPrivateCredentials->GetIterator((IIterator**)&it);
    while(it->HasNext(&hasNext), hasNext) {
        buf.Append("\tPrivate Credential: ");
        AutoPtr<IInterface> elem;
        it->GetNext((IInterface**)&elem);
        buf.Append(elem);
        buf.AppendChar('\n');
    }
    return buf.ToString(str);
}

ECode CSubject::constructor()
{
    mPrincipals = new SecureSet(_PRINCIPALS, (Handle32)this);
    mPublicCredentials = new SecureSet(_PUBLIC_CREDENTIALS, (Handle32)this);
    mPrivateCredentials = new SecureSet(_PRIVATE_CREDENTIALS, (Handle32)this);
    mReadOnly = FALSE;
    return NOERROR;
}

ECode CSubject::constructor(
    /* [in] */ Boolean readOnly,
    /* [in] */ ISet *subjPrincipals,
    /* [in] */ ISet *pubCredentials,
    /* [in] */ ISet *privCredentials)
{
    VALIDATE_NOT_NULL(subjPrincipals)
    VALIDATE_NOT_NULL(pubCredentials)
    VALIDATE_NOT_NULL(privCredentials)

    if (NULL == subjPrincipals) {
        //throw new NullPointerException("subjPrincipals == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    else if (NULL == pubCredentials) {
        //throw new NullPointerException("pubCredentials == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    else if (NULL == privCredentials) {
        return E_NULL_POINTER_EXCEPTION;
    }

    mPrincipals = new SecureSet(_PRINCIPALS, (ICollection*) subjPrincipals->Probe(EIID_ICollection), (Handle32)this);
    mPublicCredentials = new SecureSet(_PUBLIC_CREDENTIALS, (ICollection*) pubCredentials->Probe(EIID_ICollection), (Handle32)this);
    mPrivateCredentials = new SecureSet(_PRIVATE_CREDENTIALS, (ICollection*) privCredentials->Probe(EIID_ICollection), (Handle32)this);
    mReadOnly = readOnly;
    return NOERROR;
}

ECode CSubject::DoAs_PrivilegedAction(
    /* [in] */ ISubject *subject,
    /* [in] */ IPrivilegedAction *action,
    /* [in] */ IAccessControlContext * const context,
    /* [out] */ IInterface **ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = NULL;
    VALIDATE_NOT_NULL(subject)
    VALIDATE_NOT_NULL(context)
    VALIDATE_NOT_NULL(action)

    AutoPtr<IAccessControlContext> newContext;
    AutoPtr<ISubjectDomainCombiner> combiner;
    if (NULL == subject) {
        // performance optimization
        // if subject is null there is nothing to combine
        combiner = NULL;
    }
    else {
        CSubjectDomainCombiner::New(subject, (ISubjectDomainCombiner**)&combiner);
    }

    class InnerSub_PrivilegedAction
        : public Object
        , public IPrivilegedAction
    {
    public:
        CAR_INNER_INTERFACE_IMPL(Object, IPrivilegedAction)

        InnerSub_PrivilegedAction(IAccessControlContext* const context, ISubjectDomainCombiner* combiner)
                : mContext(context), mCombiner(combiner) {}
        ECode Run(
            /* [out] */ IInterface** result)
        {
            VALIDATE_NOT_NULL(result)
            *result = NULL;

            AutoPtr<IAccessControlContext> cc;
            CAccessControlContext::New(mContext, IDomainCombiner::Probe(mCombiner)
                    , (IAccessControlContext**)&cc);
            *result = cc;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    private:
        AutoPtr<IAccessControlContext> mContext;
        AutoPtr<ISubjectDomainCombiner> mCombiner;
    };
    AutoPtr<IPrivilegedAction> dccAction = new InnerSub_PrivilegedAction(context, combiner);
    AutoPtr<IAccessController> ac;
    CAccessController::AcquireSingleton((IAccessController**)&ac);
    AutoPtr<IInterface> priv;
    ac->DoPrivileged(dccAction, (IInterface**)&priv);
    newContext = IAccessControlContext::Probe(priv);
    return ac->DoPrivileged(action, newContext, ret);
}

ECode CSubject::DoAs_PrivilegedExceptionAction(
    /* [in] */ ISubject *subject,
    /* [in] */ IPrivilegedExceptionAction *action,
    /* [in] */ IAccessControlContext * const context,
    /* [out] */ IInterface **ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = NULL;
    VALIDATE_NOT_NULL(context)
    VALIDATE_NOT_NULL(action)

    AutoPtr<IAccessControlContext> newContext;
    AutoPtr<ISubjectDomainCombiner> combiner;
    if (NULL == subject) {
        // performance optimization
        // if subject is null there is nothing to combine
        combiner = NULL;
    }
    else {
        CSubjectDomainCombiner::New(subject, (ISubjectDomainCombiner**)&combiner);
    }

    class InnerSub_PrivilegedAction
        : public Object
        , public IPrivilegedAction
    {
    public:
        CAR_INNER_INTERFACE_IMPL(Object, IPrivilegedAction)

        InnerSub_PrivilegedAction(IAccessControlContext* const context, ISubjectDomainCombiner* combiner)
                : mContext(context), mCombiner(combiner) {}
        ECode Run(
            /* [out] */ IInterface** result)
        {
            VALIDATE_NOT_NULL(result)
            *result = NULL;

            AutoPtr<IAccessControlContext> cc;
            CAccessControlContext::New(mContext, IDomainCombiner::Probe(mCombiner)
                    , (IAccessControlContext**)&cc);
            *result = cc;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    private:
        AutoPtr<IAccessControlContext> mContext;
        AutoPtr<ISubjectDomainCombiner> mCombiner;
    };

    AutoPtr<IPrivilegedAction> dccAction = new InnerSub_PrivilegedAction(context, combiner);
    AutoPtr<IAccessController> ac;
    CAccessController::AcquireSingleton((IAccessController**)&ac);
    AutoPtr<IInterface> priv;
    ac->DoPrivileged(dccAction, (IInterface**)&priv);
    newContext = IAccessControlContext::Probe(priv);
    return ac->DoPrivileged(action, newContext, ret);
}

ECode CSubject::CheckState()
{
    if (mReadOnly) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

/*
 * verifies specified element, checks set state, and security permission
 * to modify set before adding new element
 */
ECode CSubject::SecureSet::Add(
    /* [in] */ IInterface *o,
    /* [out] */ Boolean *ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = FALSE;
    VALIDATE_NOT_NULL(o)

    VerifyElement(o);
    ((CSubject*)mHost)->CheckState();
    Boolean isContained;
    if ((mElements->Contains(o, &isContained), !isContained)) {
        mElements->Add(o, &isContained);
        *ret = TRUE;
        return NOERROR;
    }
    return NOERROR;
}

ECode CSubject::SecureSet::GetIterator(
    /* [out] */ IIterator **it)
{
    VALIDATE_NOT_NULL(it)
    *it = NULL;

    if (mPermission == _PRIVATE_CREDENTIALS) {
        /*
        * private credential set requires iterator with additional
        * security check (PrivateCredentialPermission)
        */
        AutoPtr<IIterator> iter;
        mElements->GetIterator((IIterator**)&iter);
        *it = new SecureIterator(iter, mHost);
        REFCOUNT_ADD(*it)
        return NOERROR;
    }
    AutoPtr<IIterator> iter;
    mElements->GetIterator((IIterator**)&iter);
    class InnerSub_SecureIterator
        : public SecureIterator
    {
    public:
        InnerSub_SecureIterator(
            /* [in] */ IIterator *iterator,
            /* [in] */ Handle32 host)
            : SecureIterator(iterator, host)
        {}
        /*
         * checks permission to access next private credential moves
         * to the next element even SecurityException was thrown
         */
        // @Override
        ECode GetNext(
            /* [out] */ IInterface **next)
        {
            VALIDATE_NOT_NULL(next)
            *next = NULL;

            return mIterator->GetNext(next);
        }


    };
    *it = new InnerSub_SecureIterator(iter, mHost);
    REFCOUNT_ADD(*it)
    return NOERROR;
}

ECode CSubject::SecureSet::RetainAll(
    /* [in] */ ICollection *c,
    /* [out] */ Boolean *ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = FALSE;

    if (NULL == c) {
        return E_NULL_POINTER_EXCEPTION;
    }
    return AbstractSet::RetainAll(c, ret);
}

CSubject::SecureSet::SecureSet(
    /* [in] */ IPermission *perm,
    /* [in] */ Handle32 host)
    : mPermission(perm)
    , mHost(host)
{
    CLinkedList::New((ILinkedList**)&mElements);
}

CSubject::SecureSet::SecureSet(
    /* [in] */ IPermission *perm,
    /* [in] */ ICollection *s,
    /* [in] */ Handle32 host)
    : mPermission(perm)
    , mHost(host)
{
    // Subject's constructor receives a Set, we can trusts if a set is from bootclasspath,
    // and not to check whether it contains duplicates or not
    Boolean trust = FALSE;
    AutoPtr<IIterator> it;
    s->GetIterator((IIterator**)&it);
    Boolean has_next;
    AutoPtr<IInterface> o;
    if (it->HasNext(&has_next), has_next) {
        it->GetNext((IInterface**)&o);
        trust = (IInteger16::Probe(o) != NULL)
                || (IInteger32::Probe(o) != NULL)
                || (IInteger64::Probe(o) != NULL)
                || (IByte::Probe(o) != NULL)
                || (IFloat::Probe(o) != NULL)
                || (IDouble::Probe(o) != NULL)
                || (IChar32::Probe(o) != NULL)
                || (IBoolean::Probe(o) != NULL);
    } else {
        return;
    }

    do
    {
        VerifyElement(o);
        Boolean is_contains;
        if (trust || (mElements->Contains(o, &is_contains), is_contains)) {
            mElements->Add(o);
        }
        it->GetNext((IInterface**)&o);
    }while(it->HasNext(&has_next), has_next);
}

ECode CSubject::SecureSet::Get(
    /* [in] */ const ClassID& id,
    /* [out] */ ISet **obj)
{
    VALIDATE_NOT_NULL(obj)
    *obj = NULL;

    class InnerSet : public AbstractSet {
    public:
        InnerSet(const ClassID& id)
            : mId(id)
        {
            CLinkedList::New((ILinkedList**)&mElements);
        }

        // @Override
        ECode Add(
            /* [in] */ IInterface* o,
            /* [out] */ Boolean* rev)
        {
            VALIDATE_NOT_NULL(rev)
            *rev = FALSE;
            VALIDATE_NOT_NULL(o)

            if (o->Probe((REIID)mId) == NULL) {
                // throw new IllegalArgumentException("Invalid type: " + o.getClass());
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            Boolean had;
            if (mElements->Contains(o, &had), had) {
                *rev = FALSE;
                return NOERROR;
            }
            mElements->Add(o);
            *rev = TRUE;
            return NOERROR;
        }

        // @Override
        ECode GetIterator(
            /* [out] */ IIterator** it)
        {
            VALIDATE_NOT_NULL(it)
            *it = NULL;

            return mElements->GetIterator(it);
        }

        // @Override
        ECode RetainAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* rev)
        {
            VALIDATE_NOT_NULL(rev)
            *rev = FALSE;
            VALIDATE_NOT_NULL(c)

            if (NULL == c) {
                // throw new NullPointerException("c == null");
                return E_NULL_POINTER_EXCEPTION;
            }
            return AbstractSet::RetainAll(c, rev);
        }

        // @Override
        ECode GetSize(
            /* [out] */ Int32* size)
        {
            VALIDATE_NOT_NULL(size)
            *size = 0;

            return mElements->GetSize(size);
        }

    protected:
        CARAPI_(String) GetClassName() { return String("CSubject::InnerSet"); }

    private:
        AutoPtr<ILinkedList> mElements;
        ClassID mId;
    };

    AutoPtr<AbstractSet> s = new InnerSet(id);

    // FIXME must have permissions for requested priv. credentials
    AutoPtr<IIterator> it;
    this->GetIterator((IIterator**)&it);
    Boolean has_next;
    AutoPtr<IInterface> o;
    Boolean tmp;
    while(it->HasNext(&has_next), has_next)
    {
        it->GetNext((IInterface**)&o);
        if (o->Probe((REIID)id) != NULL) {
            s->Add(o->Probe((REIID)id), &tmp);
        }
    }
    *obj = s;
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode CSubject::SecureSet::VerifyElement(
    /* [in] */ IInterface *o)
{
    if (NULL == o) {
        return E_NULL_POINTER_EXCEPTION;
    }
    if (mPermission == _PRINCIPALS && o->Probe(EIID_IPrincipal) == NULL) {
        // throw new IllegalArgumentException("Element is not instance of java.security.Principal");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CSubject::SecureSet::SecureIterator, Object, IIterator)

ECode CSubject::SecureSet::SecureIterator::HasNext(
    /* [out] */ Boolean *hasNext)
{
    VALIDATE_NOT_NULL(hasNext)
    *hasNext = FALSE;

    return mIterator->HasNext(hasNext);
}

ECode CSubject::SecureSet::SecureIterator::GetNext(
    /* [out] */ IInterface **next)
{
    VALIDATE_NOT_NULL(next)
    *next = NULL;

    return mIterator->GetNext(next);
}

/**
 * checks set state, and security permission to modify set before
 * removing current element
 */
ECode CSubject::SecureSet::SecureIterator::Remove()
{
    ((CSubject*)mHost)->CheckState();
    return mIterator->Remove();
}

CSubject::SecureSet::SecureIterator::SecureIterator(
    /* [in] */ IIterator *iterator,
    /* [in] */ Handle32 host)
    : mIterator(iterator)
    , mHost(host)
{}

ECode CSubject::SecureSet::GetSize(
            /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = 0;

    return mElements->GetSize(size);
}

} // namespace Auth
} // namespace Security
} // namespace Elastosx

