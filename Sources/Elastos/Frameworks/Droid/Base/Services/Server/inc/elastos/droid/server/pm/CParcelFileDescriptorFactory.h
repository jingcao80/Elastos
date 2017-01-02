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

#ifndef __ELASTOS_DROID_SERVER_PM_CPARCELFILEDESCRIPTORFACTORY_H__
#define __ELASTOS_DROID_SERVER_PM_CPARCELFILEDESCRIPTORFACTORY_H__

#include "_Elastos_Droid_Server_Pm_CParcelFileDescriptorFactory.h"
#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::Os::IIParcelFileDescriptorFactory;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

CarClass(CParcelFileDescriptorFactory)
    , public Object
    , public IIParcelFileDescriptorFactory
    , public IBinder
{
public:
    CARAPI constructor(
      /* [in] */ IFile* codeFile);

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI Open(
        /* [in] */ const String& name,
        /* [in] */ Int32 mode,
        /* [out] */ IParcelFileDescriptor** pfd);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    AutoPtr<IFile> mCodeFile;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_PM_CPARCELFILEDESCRIPTORFACTORY_H__
