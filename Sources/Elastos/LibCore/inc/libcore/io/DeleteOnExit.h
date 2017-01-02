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
