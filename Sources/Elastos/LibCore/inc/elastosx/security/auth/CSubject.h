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

#ifndef __ELASTOSX_SECURITY_AUTH_CSUBJECT_H__
#define __ELASTOSX_SECURITY_AUTH_CSUBJECT_H__

#include "Elastos.CoreLibrary.Security.h"
#include "_Elastosx_Security_Auth_CSubject.h"
#include "AbstractSet.h"
#if 0 // TODO: Waiting for AccessControlContext
#include "CAccessControlContext.h"
#endif

using Elastos::IO::IObjectInputStream;
using Elastos::IO::IObjectOutputStream;
using Elastos::Security::EIID_IPrivilegedAction;
using Elastos::Security::IAccessControlContext;
using Elastos::Security::IDomainCombiner;
using Elastos::Security::IPermission;
using Elastos::Security::IPrivilegedAction;
using Elastos::Security::IPrivilegedExceptionAction;
#if 0 // TODO: Waiting for AccessControlContext
using Elastos::Security::CAccessControlContext;
#endif
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::ILinkedList;
using Elastos::Utility::ISet;
using Elastos::Utility::AbstractSet;
using Elastosx::Security::Auth::ISubjectDomainCombiner;
using Elastos::Core::Object;

namespace Elastosx {
namespace Security {
namespace Auth {

CarClass(CSubject)
    , public Object
    , public ISubject
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Runs the code defined by {@code action} using the permissions granted to
     * the {@code Subject} itself and to the code as well.
     *
     * @param subject
     *            the distinguished {@code Subject}.
     * @param action
     *            the code to be run.
     * @return the {@code Object} returned when running the {@code action}.
     */
    // @SuppressWarnings("unchecked")
    static CARAPI DoAs(
        /* [in] */ ISubject *subject,
        /* [in] */ IPrivilegedAction *action,
        /* [out] */ IInterface **obj);

    /**
     * Run the code defined by {@code action} using the permissions granted to
     * the {@code Subject} and to the code itself, additionally providing a more
     * specific context.
     *
     * @param subject
     *            the distinguished {@code Subject}.
     * @param action
     *            the code to be run.
     * @param context
     *            the specific context in which the {@code action} is invoked.
     *            if {@code null} a new {@link AccessControlContext} is
     *            instantiated.
     * @return the {@code Object} returned when running the {@code action}.
     */
    // @SuppressWarnings("unchecked")
    static CARAPI DoAsPrivileged(
        /* [in] */ ISubject *subject,
        /* [in] */ IPrivilegedAction *action,
        /* [in] */ IAccessControlContext *context,
        /* [out] */ IInterface **obj);

    /**
     * Runs the code defined by {@code action} using the permissions granted to
     * the subject and to the code itself.
     *
     * @param subject
     *            the distinguished {@code Subject}.
     * @param action
     *            the code to be run.
     * @return the {@code Object} returned when running the {@code action}.
     * @throws PrivilegedActionException
     *             if running the {@code action} throws an exception.
     */
    // @SuppressWarnings("unchecked")
    static CARAPI DoAs(
        /* [in] */ ISubject *subject,
        /* [in] */ IPrivilegedExceptionAction *action,
        /* [out] */ IInterface **obj);

    /**
     * Runs the code defined by {@code action} using the permissions granted to
     * the subject and to the code itself, additionally providing a more
     * specific context.
     *
     * @param subject
     *            the distinguished {@code Subject}.
     * @param action
     *            the code to be run.
     * @param context
     *            the specific context in which the {@code action} is invoked.
     *            if {@code null} a new {@link AccessControlContext} is
     *            instantiated.
     * @return the {@code Object} returned when running the {@code action}.
     * @throws PrivilegedActionException
     *             if running the {@code action} throws an exception.
     */
    // @SuppressWarnings("unchecked")
    static CARAPI DoAsPrivileged(
        /* [in] */ ISubject *subject,
        /* [in] */ IPrivilegedExceptionAction *action,
        /* [in] */ IAccessControlContext *context,
        /* [out] */ IInterface **obj);

    /**
     * Returns the {@code Subject} that was last associated with the {@code
     * context} provided as argument.
     *
     * @param context
     *            the {@code context} that was associated with the
     *            {@code Subject}.
     * @return the {@code Subject} that was last associated with the {@code
     *         context} provided as argument.
     */
    static CARAPI GetSubject(
        /* [in] */ IAccessControlContext *context,
        /* [out] */ ISubject **subject);

    /**
     * Checks two Subjects for equality. More specifically if the principals,
     * public and private credentials are equal, equality for two {@code
     * Subjects} is implied.
     *
     * @param obj
     *            the {@code Object} checked for equality with this {@code
     *            Subject}.
     * @return {@code true} if the specified {@code Subject} is equal to this
     *         one.
     */
    // @Override
    CARAPI Equals(
        /* [in] */ IInterface *obj,
        /* [out] */ Boolean *isEqual);

    /**
     * Returns this {@code Subject}'s {@link Principal}.
     *
     * @return this {@code Subject}'s {@link Principal}.
     */
    CARAPI GetPrincipals(
        /* [out] */ ISet **principals);

    /**
     * Returns this {@code Subject}'s {@link Principal} which is a subclass of
     * the {@code Class} provided.
     *
     * @param c
     *            the {@code Class} as a criteria which the {@code Principal}
     *            returned must satisfy.
     * @return this {@code Subject}'s {@link Principal}. Modifications to the
     *         returned set of {@code Principal}s do not affect this {@code
     *         Subject}'s set.
     */
    CARAPI GetPrincipals(
        /* [in] */ IInterface *pC,
        /* [out] */ ISet **principals);

    /**
     * Returns the private credentials associated with this {@code Subject}.
     *
     * @return the private credentials associated with this {@code Subject}.
     */
    CARAPI GetPrivateCredentials(
        /* [out] */ ISet **credentials);

    /**
     * Returns this {@code Subject}'s private credentials which are a subclass
     * of the {@code Class} provided.
     *
     * @param c
     *            the {@code Class} as a criteria which the private credentials
     *            returned must satisfy.
     * @return this {@code Subject}'s private credentials. Modifications to the
     *         returned set of credentials do not affect this {@code Subject}'s
     *         credentials.
     */
    CARAPI GetPrivateCredentials(
        /* [in] */ const ClassID& id,
        /* [out] */ ISet **credentials);

    /**
     * Returns the public credentials associated with this {@code Subject}.
     *
     * @return the public credentials associated with this {@code Subject}.
     */
    CARAPI GetPublicCredentials(
        /* [out] */ ISet **credentials);

    /**
     * Returns this {@code Subject}'s public credentials which are a subclass of
     * the {@code Class} provided.
     *
     * @param c
     *            the {@code Class} as a criteria which the public credentials
     *            returned must satisfy.
     * @return this {@code Subject}'s public credentials. Modifications to the
     *         returned set of credentials do not affect this {@code Subject}'s
     *         credentials.
     */
    CARAPI GetPublicCredentials(
        /* [in] */ const ClassID& id,
        /* [out] */ ISet **credentials);

    /**
     * Returns a hash code of this {@code Subject}.
     *
     * @return a hash code of this {@code Subject}.
     */
    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32 *hashCode);

    /**
     * Prevents from modifications being done to the credentials and {@link
     * Principal} sets. After setting it to read-only this {@code Subject} can
     * not be made writable again. The destroy method on the credentials still
     * works though.
     */
    CARAPI SetReadOnly();

    /**
     * Returns whether this {@code Subject} is read-only or not.
     *
     * @return whether this {@code Subject} is read-only or not.
     */
    CARAPI IsReadOnly(
        /* [out] */ Boolean *isReadOnly);

    /**
     * Returns a {@code String} representation of this {@code Subject}.
     *
     * @return a {@code String} representation of this {@code Subject}.
     */
    // @Override
    CARAPI ToString(
        /* [out] */ String *str);

    /**
     * The default constructor initializing the sets of public and private
     * credentials and principals with the empty set.
     */
    CARAPI constructor();

    /**
     * The constructor for the subject, setting its public and private
     * credentials and principals according to the arguments.
     *
     * @param readOnly
     *            {@code true} if this {@code Subject} is read-only, thus
     *            preventing any modifications to be done.
     * @param subjPrincipals
     *            the set of Principals that are attributed to this {@code
     *            Subject}.
     * @param pubCredentials
     *            the set of public credentials that distinguish this {@code
     *            Subject}.
     * @param privCredentials
     *            the set of private credentials that distinguish this {@code
     *            Subject}.
     */
    CARAPI constructor(
        /* [in] */ Boolean readOnly,
        /* [in] */ ISet *subjPrincipals,
        /* [in] */ ISet *pubCredentials,
        /* [in] */ ISet *privCredentials);

private:
    class SecureSet
        : public AbstractSet
    {
    public:
        /*
         * verifies specified element, checks set state, and security permission
         * to modify set before adding new element
         */
        // @Override
        CARAPI Add(
            /* [in] */ IInterface *o,
            /* [out] */ Boolean *ret);

        // returns an instance of SecureIterator
        // @Override
        CARAPI GetIterator(
            /* [out] */ IIterator **it);

        // @Override
        CARAPI RetainAll(
            /* [in] */ ICollection *c,
            /* [out] */ Boolean *ret);

        /**
         * return set with elements that are instances or subclasses of the
         * specified class
         */
         /*
            protected final <E> Set<E> get(final Class<E> c)
        */
        SecureSet(
            /* [in] */ IPermission *perm,
            /* [in] */ Handle32 host);

        SecureSet(
            /* [in] */ IPermission *perm,
            /* [in] */ ICollection *s,
            /* [in] */ Handle32 host);

        /**
         * return set with elements that are instances or subclasses of the
         * specified class
         */
        CARAPI Get(
            /* [in] */ const ClassID& id,
            /* [out] */ ISet **obj);

        CARAPI GetSize(
            /* [out] */ Int32* size);

        /**
         * Represents iterator for subject's secure set
         */
        class SecureIterator
            : public Object
            , public IIterator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI HasNext(
                /* [out] */ Boolean *hasNext);

            CARAPI GetNext(
                /* [out] */ IInterface **next);

            /**
             * checks set state, and security permission to modify set before
             * removing current element
             */
            CARAPI Remove();

            SecureIterator(
                /* [in] */ IIterator *iterator,
                /* [in] */ Handle32 host);

        protected:
            AutoPtr<IIterator> mIterator;
            Handle32 mHost;
        };

        // verifies new set element
        CARAPI VerifyElement(
            /* [in] */ IInterface *o);

    protected:
        CARAPI_(String) GetClassName() { return String("CSubject::SecureSet"); }

    private:
        /**
         * Compatibility issue: see comments for setType variable
         */
        static const Int64 serialVersionUID;// = 7911754171111800359L;

        AutoPtr<ILinkedList> mElements;

        /*
         * Is used to define a set type for serialization.
         *
         * A type can be principal, priv. or pub. credential set. The spec.
         * doesn't clearly says that priv. and pub. credential sets can be
         * serialized and what classes they are. It is only possible to figure
         * out from writeObject method comments that priv. credential set is
         * serializable and it is an instance of SecureSet class. So pub.
         * credential was implemented by analogy
         *
         * Compatibility issue: the class follows its specified serial form.
         * Also according to the serialization spec. adding new field is a
         * compatible change. So is ok for principal set (because the default
         * value for integer is zero). But priv. or pub. credential set it is
         * not compatible because most probably other implementations resolve
         * this issue in other way
         */
        Int32 mSetType;

        // Defines principal set for serialization.
        static const Int32 SET_Principal = 0;

        // Defines private credential set for serialization.
        static const Int32 SET_PrivCred = 1;

        // Defines public credential set for serialization.
        static const Int32 SET_PubCred = 2;

        // permission required to modify set
        // transient
        AutoPtr<IPermission> mPermission;

        Handle32 mHost;

    };

    class PrivilegedAction
        : public IPrivilegedAction
        , public Object
    {
    public:
        PrivilegedAction(
            /* [in] */ IAccessControlContext *context)
            : mAcc(context)
        {}

        PInterface Probe(
            /* [in] */ REIID riid)
        {
            if (riid == EIID_IInterface) {
                return (IInterface*)(IPrivilegedAction*)this;
            }
            else if (riid == EIID_IPrivilegedAction) {
                return (IPrivilegedAction*)this;
            }
            return NULL;
        }

        UInt32 AddRef()
        {
            return ElRefBase::AddRef();
        }

        UInt32 Release()
        {
            return ElRefBase::Release();
        }

        ECode GetInterfaceID(
            /* [in] */ IInterface* object,
            /* [out] */ InterfaceID* iid)
        {
            VALIDATE_NOT_NULL(iid)
            if (object == (IInterface*)(IPrivilegedAction*)this) {
                *iid = EIID_IPrivilegedAction;
            }
            else {
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            return NOERROR;
        }

        ECode Run(
            /* [out] */ IInterface **dc)
        {
            AutoPtr<IDomainCombiner> tmpdc;
            ECode ec = mAcc->GetDomainCombiner((IDomainCombiner**)&tmpdc);
            *dc = tmpdc.Get();
            REFCOUNT_ADD(*dc)
            return ec;
        }
    protected:
        AutoPtr<IAccessControlContext> mAcc;
    };

    // instantiates a new context and passes it to AccessController
    // @SuppressWarnings("unchecked")
    static CARAPI DoAs_PrivilegedAction(
        /* [in] */ ISubject *subject,
        /* [in] */ IPrivilegedAction *action,
        /* [in] */ IAccessControlContext * const context,
        /* [out] */ IInterface **ret);

    // instantiates a new context and passes it to AccessController
    // @SuppressWarnings("unchecked")
    static CARAPI DoAs_PrivilegedExceptionAction(
        /* [in] */ ISubject *subject,
        /* [in] */ IPrivilegedExceptionAction *action,
        /* [in] */ IAccessControlContext * const context,
        /* [out] */ IInterface **ret);

    CARAPI ReadObject(
        /* [in] */ IObjectInputStream *is);

    CARAPI WriteObject(
        /* [in] */ IObjectOutputStream *os);

    CARAPI CheckState();

private:
    static const Int64 serialVersionUID;

    static const AutoPtr<IPermission> _AS;

    static const AutoPtr<IPermission> _AS_PRIVILEGED;

    static const AutoPtr<IPermission> _SUBJECT;

    static const AutoPtr<IPermission> _PRINCIPALS;

    static const AutoPtr<IPermission> _PRIVATE_CREDENTIALS;

    static const AutoPtr<IPermission> _PUBLIC_CREDENTIALS;

    static const AutoPtr<IPermission> _READ_ONLY;

    AutoPtr<ISet> mPrincipals;

    Boolean mReadOnly;

    // set of private credentials
    // transient
    AutoPtr<SecureSet> mPrivateCredentials;

    // set of public credentials
    // transient
    AutoPtr<SecureSet> mPublicCredentials;
};

} // namespace Auth
} // namespace Security
} // namespace Elastosx

#endif // __ELASTOSX_SECURITY_AUTH_CSUBJECT_H__
