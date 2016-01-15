
#ifndef __ELASTOS_IO_FILEREADER_H__
#define __ELASTOS_IO_FILEREADER_H__

#include "InputStreamReader.h"

namespace Elastos {
namespace IO {

/**
 * A specialized {@link Reader} that reads from a file in the file system.
 * All read requests made by calling methods in this class are directly
 * forwarded to the equivalent function of the underlying operating system.
 * Since this may induce some performance penalty, in particular if many small
 * read requests are made, a FileReader is often wrapped by a
 * BufferedReader.
 *
 * @see BufferedReader
 * @see FileWriter
 */
class FileReader
    : public InputStreamReader
    , public IFileReader
{
public:
	CAR_INTERFACE_DECL()

    /**
     * Constructs a new FileReader on the given {@code file}.
     *
     * @param file
     *            a File to be opened for reading characters from.
     * @throws FileNotFoundException
     *             if {@code file} does not exist.
     */
    CARAPI constructor(
        /* [in] */ IFile* file);

    /**
     * Construct a new FileReader on the given FileDescriptor {@code fd}. Since
     * a previously opened FileDescriptor is passed as an argument, no
     * FileNotFoundException can be thrown.
     *
     * @param fd
     *            the previously opened file descriptor.
     */
    CARAPI constructor(
        /* [in] */ IFileDescriptor* file);

    /**
     * Construct a new FileReader on the given file named {@code filename}.
     *
     * @param filename
     *            an absolute or relative path specifying the file to open.
     * @throws FileNotFoundException
     *             if there is no file named {@code filename}.
     */
    CARAPI constructor(
        /* [in] */ const String& file);
};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_FILEREADER_H__
