
#ifndef __ELASTOS_IO_FILEWRITER_H__
#define __ELASTOS_IO_FILEWRITER_H__

#include "OutputStreamWriter.h"

namespace Elastos {
namespace IO {

class FileWriter
    : public OutputStreamWriter
    , public IFileWriter
{
public:
	CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IFile* file);

    CARAPI constructor(
        /* [in] */ IFile* file,
        /* [in] */ Boolean append);

    CARAPI constructor(
        /* [in] */ IFileDescriptor* fd);

    CARAPI constructor(
        /* [in] */ const String& filename);

    CARAPI constructor(
        /* [in] */ const String& filename,
        /* [in] */ Boolean append);
};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_FILEWRITER_H__
