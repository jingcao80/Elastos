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

#ifndef __ELASTOS_SECURITY_SECURECLASSLOADER_H__
#define __ELASTOS_SECURITY_SECURECLASSLOADER_H__

#include "Elastos.CoreLibrary.Security.h"
#include "ProtectionDomain.h"
#include "ClassLoader.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Core::ClassLoader;
using Elastos::Core::IClassLoader;
using Elastos::IO::IByteBuffer;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Security {

/**
 * {@code SecureClassLoader} represents a {@code ClassLoader} which associates
 * the classes it loads with a code source and provide mechanisms to allow the
 * relevant permissions to be retrieved.
 */
class SecureClassLoader
    : public ClassLoader
{
protected:
    /**
     * Constructs a new instance of {@code SecureClassLoader}. The default
     * parent {@code ClassLoader} is used.
     */
    SecureClassLoader();

    /**
     * Constructs a new instance of {@code SecureClassLoader} with the specified
     * parent {@code ClassLoader}.
     *
     * @param parent
     *            the parent {@code ClassLoader}.
     */
    CARAPI constructor(
        /* [in] */ IClassLoader* parent);

    /**
     * Returns the {@code PermissionCollection} for the specified {@code
     * CodeSource}.
     *
     * @param codesource
     *            the code source.
     * @return the {@code PermissionCollection} for the specified {@code
     *         CodeSource}.
     */
    virtual CARAPI_(AutoPtr<IPermissionCollection>) GetPermissions(
        /* [in] */ ICodeSource* codesource);

    /**
     * Constructs a new class from an array of bytes containing a class
     * definition in class file format with an optional {@code CodeSource}.
     *
     * @param name
     *            the name of the new class.
     * @param b
     *            a memory image of a class file.
     * @param off
     *            the start offset in b of the class data.
     * @param len
     *            the length of the class data.
     * @param cs
     *            the {@code CodeSource}, or {@code NULL}.
     * @return a new class.
     * @throws IndexOutOfBoundsException
     *             if {@code off} or {@code len} are not valid in respect to
     *             {@code b}.
     * @throws ClassFormatError
     *             if the specified data is not valid class data.
     * @throws SecurityException
     *             if the package to which this class is to be added, already
     *             contains classes which were signed by different certificates,
     *             or if the class name begins with "java."
     */
    virtual CARAPI DefineClass(
        /* [in] */ const String& name,
        /* [in] */ ArrayOf<Byte>* b,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len,
        /* [in] */ ICodeSource* cs,
        /* [out] */ IClassInfo** result);

    /**
     * Constructs a new class from an array of bytes containing a class
     * definition in class file format with an optional {@code CodeSource}.
     *
     * @param name
     *            the name of the new class.
     * @param b
     *            a memory image of a class file.
     * @param cs
     *            the {@code CodeSource}, or {@code NULL}.
     * @return a new class.
     * @throws ClassFormatError
     *             if the specified data is not valid class data.
     * @throws SecurityException
     *             if the package to which this class is to be added, already
     *             contains classes which were signed by different certificates,
     *             or if the class name begins with "java."
     */
    virtual CARAPI DefineClass(
        /* [in] */ const String& name,
        /* [in] */ IByteBuffer* b,
        /* [in] */ ICodeSource* cs,
        /* [out] */ IClassInfo** result);

private:
    // Constructs and caches ProtectionDomain for the given CodeSource
    // object.<br>
    // It calls {@link getPermissions()} to get a set of permissions.
    //
    // @param cs CodeSource object
    // @return ProtectionDomain for the passed CodeSource object
    CARAPI_(AutoPtr<ProtectionDomain>) GetPD(
        /* [in] */ ICodeSource* cs);

private:
    // A cache of ProtectionDomains for a given CodeSource
    AutoPtr<IHashMap> mPds; //HashMap<CodeSource, ProtectionDomain>
    Object PDS;
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_SECURECLASSLOADER_H__
