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

#ifndef __ELASTOS_UTILITY_JAR_ELASTOS_UTILITY_CJARFILE_H__
#define __ELASTOS_UTILITY_JAR_ELASTOS_UTILITY_CJARFILE_H__

#include "_Elastos_Utility_Jar_CJarFile.h"
#include "ZipFile.h"
#include "JarVerifier.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Zip::ZipFile;
using Elastos::Utility::Zip::IZipEntry;
using Elastos::Utility::Etl::HashMap;
using Elastos::IO::IInputStream;
using Elastos::IO::IFile;

namespace Elastos {
namespace Utility {
namespace Jar {

CarClass(CJarFile)
    , public ZipFile
    , public IJarFile
{
public:
    class JarFileInputStream
        : public FilterInputStream
    {
    public:
        JarFileInputStream();

        CARAPI constructor(
            /* [in] */ IInputStream* is,
            /* [in] */ Int64 size,
            /* [in] */ JarVerifier::VerifierEntry* e);

        CARAPI Available(
            /* [out] */ Int32* val);

        CARAPI Read(
            /* [out] */ Int32* val);

        CARAPI Read(
            /* [in] */ ArrayOf<Byte>* buf,
            /* [in] */ Int32 off,
            /* [in] */ Int32 nbytes,
            /* [out] */ Int32* val);

        CARAPI Skip(
            /* [in] */ Int64 byteCount,
            /* [out] */ Int64* val);

    private:
        AutoPtr<JarVerifier::VerifierEntry> mEntry;
        Int64 mCount;
        Boolean mDone;
    };

    class JarFileEnumerator
        : public Object
        , public IEnumeration
    {
    public:
        CAR_INTERFACE_DECL()

        JarFileEnumerator(
            /* [in] */ IEnumeration* enu,
            /* [in] */ CJarFile* jarFile);

        CARAPI HasMoreElements(
            /* [out] */ Boolean * hasMore);

        CARAPI GetNextElement(
            /* [out] */ IInterface ** obj);

        CARAPI GetNextElement(
            /* [out] */ IJarEntry ** obj);

    public:
        AutoPtr<IEnumeration> mZipEntry;
        CJarFile* mJarFile;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CJarFile();

    CARAPI constructor(
        /* [in] */ IFile* file);

    CARAPI constructor(
        /* [in] */ IFile* file,
        /* [in] */ Boolean verify);

    CARAPI constructor(
        /* [in] */ IFile* file,
        /* [in] */ Boolean verify,
        /* [in] */ Int32 mode);

    CARAPI constructor(
        /* [in] */ const String& filename);

    CARAPI constructor(
        /* [in] */ const String& filename,
        /* [in] */ Boolean verify);

    /**
     * Return an enumeration containing the {@code JarEntrys} contained in this
     * {@code JarFile}.
     *
     * @return the {@code Enumeration} containing the JAR entries.
     * @throws IllegalStateException
     *             if this {@code JarFile} is closed.
     */
    CARAPI GetEntries(
        /* [out] */ IEnumeration** entries);

    /**
     * Return the {@code JarEntry} specified by its name or {@code null} if no
     * such entry exists.
     *
     * @param name
     *            the name of the entry in the JAR file.
     * @return the JAR entry defined by the name.
     */
    CARAPI GetJarEntry(
        /* [in] */ const String& name,
        /* [out] */ IJarEntry** jarEntry);

    /**
     * Returns the {@code Manifest} object associated with this {@code JarFile}
     * or {@code null} if no MANIFEST entry exists.
     *
     * @return the MANIFEST.
     * @throws IOException
     *             if an error occurs reading the MANIFEST file.
     * @throws IllegalStateException
     *             if the jar file is closed.
     * @see Manifest
     */
    CARAPI GetManifest(
        /* [out] */ IManifest** manifest);

    /**
     * Return an {@code InputStream} for reading the decompressed contents of
     * ZIP entry.
     *
     * @param ze
     *            the ZIP entry to be read.
     * @return the input stream to read from.
     * @throws IOException
     *             if an error occurred while creating the input stream.
     */
    CARAPI GetInputStream(
        /* [in] */ IZipEntry* entry,
        /* [out] */ IInputStream** is);

    /**
     * Return the {@code JarEntry} specified by name or {@code null} if no such
     * entry exists.
     *
     * @param name
     *            the name of the entry in the JAR file.
     * @return the ZIP entry extracted.
     */
    CARAPI GetEntry(
        /* [in] */ const String& entryName,
        /* [out] */ IZipEntry** entry);

    /**
     * Closes this {@code JarFile}.
     *
     * @throws IOException
     *             if an error occurs.
     */
    CARAPI Close();

private:
    /**
     * Called by the JarFile constructors, Reads the contents of the
     * file's META-INF/ directory and picks out the MANIFEST.MF file and
     * verifier signature files if they exist.
     *
     * @throws IOException
     *             if there is a problem reading the jar file entries.
     * @return a map of entry names to their {@code byte[]} content.
     */
    static AutoPtr<HashMap<String, AutoPtr<ArrayOf<Byte> > > > ReadMetaEntries(
        /* [in] */ ZipFile* file,
        /* [in] */ Boolean verificationRequired);

    static Boolean EndsWithIgnoreCase(
        /* [in] */ const String& s,
        /* [in] */ const String& suffix);

    /**
     * Returns all the ZipEntry's that relate to files in the
     * JAR's META-INF directory.
     */
    static CARAPI_(AutoPtr<ArrayOf<IZipEntry*> >) GetMetaEntries(
        /* [in] */ IZipFile* zipFile);

public:
    // The directory containing the manifest.
    static const String META_DIR;

    AutoPtr<JarVerifier> mVerifier;

private:
    // The manifest after it has been read from the JAR.
    AutoPtr<IManifest> mManifest;

    // The entry for the MANIFEST.MF file before the first call to getManifest().
    AutoPtr<ArrayOf<Byte> > mManifestBytes;

    // The entry for the MANIFEST.MF file before it is read.
    AutoPtr<IZipEntry> mManifestEntry;

    Boolean mClosed;
};

} // namespace Jar
} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_JAR_ELASTOS_UTILITY_CJARFILE_H__
