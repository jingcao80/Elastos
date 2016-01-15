
#include "DeleteOnExit.h"
#include "CString.h"
#include "AutoLock.h"
#include "CFile.h"
#include "utility/Collections.h"

// #include "utility/CCollections.h"


using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::Utility::IList;
using Elastos::Utility::Collections;

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;

namespace Libcore {
namespace IO {

CAR_INTERFACE_IMPL(DeleteOnExit, Thread, IDeleteOnExit)

AutoPtr<IDeleteOnExit> DeleteOnExit::sInstance;
Object DeleteOnExit::sLockLock;

ECode DeleteOnExit::GetInstance(
    /* [out] */ IDeleteOnExit** instance)
{
    VALIDATE_NOT_NULL(instance)
    AutoLock lock(sLockLock);
    if (NULL == sInstance) {
        AutoPtr<DeleteOnExit> doe = new DeleteOnExit();
        doe->constructor();
        sInstance = doe;
    }
    *instance = sInstance;
    REFCOUNT_ADD(*instance)
    return NOERROR;
}

ECode DeleteOnExit::AddFile(
    /* [in] */ const String& filename)
{
    AutoLock lock(this);
    AutoPtr<ICharSequence> cs;
    CString::New(filename, (ICharSequence**)&cs);
    Boolean isContained;
    AutoPtr<ICollection> collection = ICollection::Probe(mFiles);
    collection->Contains(cs, &isContained);
    if (!isContained) {
        collection->Add(cs, &isContained);
    }
    return NOERROR;
}

DeleteOnExit::DeleteOnExit()
{
    CArrayList::New((IArrayList**)&mFiles);
}

ECode DeleteOnExit::Run()
{
    // CCollections::AcquireSingleton((ICollections**)&col);
    AutoPtr<IList> list = IList::Probe(mFiles);
    Collections::Sort(list);
    Int32 size;
    AutoPtr<ICollection> collection = ICollection::Probe(mFiles);
    collection->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr<IInterface> elem;
        list->Get(i, (IInterface**)&elem);
        String fileName;
        ICharSequence::Probe(elem)->ToString(&fileName);
        AutoPtr<IFile> file;
        CFile::New(fileName, (IFile**)&file);
        Boolean isDeleted;
        FAIL_RETURN(file->Delete(&isDeleted))
    }
    return NOERROR;
}

} // namespace IO
} // namespace Libcore


