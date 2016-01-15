
#ifndef __LIBCORE_IO_DELETEONEXIT_H__
#define __LIBCORE_IO_DELETEONEXIT_H__

#include "Elastos.CoreLibrary.Libcore.h"
#include "Thread.h"
#include "utility/CArrayList.h"

using Elastos::Core::Thread;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;

namespace Libcore {
namespace IO {

class DeleteOnExit
    : public Thread
    , public IDeleteOnExit
{
public:
    CAR_INTERFACE_DECL()

    static CARAPI GetInstance(
    /* [out] */ IDeleteOnExit** instance);

    CARAPI AddFile(
        /* [in] */ const String& filename);

    CARAPI Run();

private:
    DeleteOnExit();

    static Object sLockLock;

    /**
     * Our singleton instance.
     */
    static AutoPtr<IDeleteOnExit> sInstance;

    /**
     * Our list of files scheduled for deletion.
     */
    AutoPtr<IArrayList> mFiles;// = new ArrayList<String>();
};

} // namespace IO
} // namespace Libcore

#endif // __CDELETEONEXIT_H__
