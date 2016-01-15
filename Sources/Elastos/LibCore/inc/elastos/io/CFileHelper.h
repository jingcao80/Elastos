#ifndef __ELASTOS_IO_CFILEHELPER_H__
#define __ELASTOS_IO_CFILEHELPER_H__

#include "_Elastos_IO_CFileHelper.h"
#include "Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace IO {

CarClass(CFileHelper)
    , public Singleton
    , public IFileHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Creates an empty temporary file using the given prefix and suffix as part
     * of the file name. If {@code suffix} is null, {@code .tmp} is used. This
     * method is a convenience method that calls
     * {@link #createTempFile(String, String, File)} with the third argument
     * being {@code null}.
     *
     * @param prefix
     *            the prefix to the temp file name.
     * @param suffix
     *            the suffix to the temp file name.
     * @return the temporary file.
     * @throws IOException
     *             if an error occurs when writing the file.
     */
    CARAPI CreateTempFile(
        /* [in] */ const String& prefix,
        /* [in] */ const String& suffix,
        /* [out] */ IFile** file);

    /**
     * Creates an empty temporary file in the given directory using the given
     * prefix and suffix as part of the file name. If {@code suffix} is null, {@code .tmp} is used.
     *
     * <p>Note that this method does <i>not</i> call {@link #deleteOnExit}, but see the
     * documentation for that method before you call it manually.
     *
     * @param prefix
     *            the prefix to the temp file name.
     * @param suffix
     *            the suffix to the temp file name.
     * @param directory
     *            the location to which the temp file is to be written, or
     *            {@code null} for the default location for temporary files,
     *            which is taken from the "java.io.tmpdir" system property. It
     *            may be necessary to set this property to an existing, writable
     *            directory for this method to work properly.
     * @return the temporary file.
     * @throws IllegalArgumentException
     *             if the length of {@code prefix} is less than 3.
     * @throws IOException
     *             if an error occurs when writing the file.
     */
    CARAPI CreateTempFile(
        /* [in] */ const String& prefix,
        /* [in] */ const String& suffix,
        /* [in] */ IFile* directory,
        /* [out] */ IFile** file);

    /**
     * The system-dependent character used to separate components in filenames ('/').
     * Use of this (rather than hard-coding '/') helps portability to other operating systems.
     *
     * <p>This field is initialized from the system property "file.separator".
     * Later changes to that property will have no effect on this field or this class.
     */
    CARAPI GetSeparatorChar(
        /* [out] */ Char32* separatorChar);

    /**
     * The system-dependent string used to separate components in filenames ('/').
     * See {@link #separatorChar}.
     */
    CARAPI GetSeparator(
        /* [out] */ String* separator);

    /**
     * The system-dependent character used to separate components in search paths (':').
     * This is used to split such things as the PATH environment variable and classpath
     * system properties into lists of directories to be searched.
     *
     * <p>This field is initialized from the system property "path.separator".
     * Later changes to that property will have no effect on this field or this class.
     */
    CARAPI GetPathSeparatorChar(
        /* [out] */ Char32* pathSeparatorChar);

    /**
     * The system-dependent string used to separate components in search paths (":").
     * See {@link #pathSeparatorChar}.
     */
    CARAPI GetPathSeparator(
        /* [out] */ String* pathSeparator);

    /**
     * Returns the file system roots. On Android and other Unix systems, there is
     * a single root, {@code /}.
     */
    CARAPI ListRoots(
        /* [out, callee] */ ArrayOf<IFile*>** files);
};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_CFILEHELPER_H__
