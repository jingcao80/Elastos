// wuweizuo automatic build .cpp file from .java file.

#include "elastos/droid/webkit/native/content/browser/FileDescriptorInfo.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

//=====================================================================
//                          FileDescriptorInfo
//=====================================================================
FileDescriptorInfo::FileDescriptorInfo(
    /* [in] */ Int32 id,
    /* [in] */ Int32 fd,
    /* [in] */ Boolean autoClose)
    : mId(id)
    , mFd(fd)
    , mAutoClose(autoClose)
{
}

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
