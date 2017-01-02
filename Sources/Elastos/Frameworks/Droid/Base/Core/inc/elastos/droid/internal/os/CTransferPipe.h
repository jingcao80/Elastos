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

#ifndef __ELASTOS_DROID_INTERNAL_OS_CTRANSFERPIPE_H__
#define __ELASTOS_DROID_INTERNAL_OS_CTRANSFERPIPE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Os.h"
#include <Elastos.CoreLibrary.IO.h>
#include "_Elastos_Droid_Internal_Os_CTransferPipe.h"
#include <elastos/core/Object.h>

using Elastos::IO::IFileDescriptor;
using Elastos::Core::IRunnable;
using Elastos::Core::IThread;
using Elastos::Core::CThread;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

/**
 * Helper for transferring data through a pipe from a client app.
 */
CarClass(CTransferPipe)
    , public Object
    , public ITransferPipe
    , public IRunnable
{
public:
    interface ICaller : public IInterface
    {
        virtual CARAPI Go(
            /* [in] */ IInterface* iface,
            /* [in] */ IFileDescriptor* fd,
            /* [in] */ const String& prefix,
            /* [in] */ ArrayOf<String>* args);
    };

public:
    CTransferPipe();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI Run();

    CARAPI_(AutoPtr<IParcelFileDescriptor>) GetReadFd();

    CARAPI GetWriteFd(
        /* [out] */ IParcelFileDescriptor** writeFd);

    CARAPI SetBufferPrefix(
        /* [in] */ const String& prefix);

    static CARAPI Go(
        /* [in] */ ICaller* caller,
        /* [in] */ IInterface* iface,
        /* [in] */ IFileDescriptor* out,
        /* [in] */ const String& prefix,
        /* [in] */ ArrayOf<String>* args);

    static CARAPI Go(
        /* [in] */ ICaller* caller,
        /* [in] */ IInterface* iface,
        /* [in] */ IFileDescriptor* out,
        /* [in] */ const String& prefix,
        /* [in] */ ArrayOf<String>* args,
        /* [in] */ Int64 timeout);

    static CARAPI GoDump(
        /* [in] */ IBinder* binder,
        /* [in] */ IFileDescriptor* out,
        /* [in] */ ArrayOf<String>* args);

    static CARAPI GoDump(
        /* [in] */ IBinder* binder,
        /* [in] */ IFileDescriptor* out,
        /* [in] */ ArrayOf<String>* args,
        /* [in] */ Int64 timeout);

    CARAPI Go(
        /* [in] */ IFileDescriptor* out);

    CARAPI Go(
        /* [in] */ IFileDescriptor* out,
        /* [in] */ Int64 timeout);

    CARAPI CloseFd(
        /* [in] */ Int32 num);

    CARAPI Kill();

public:
    static const String TAG;
    static const Boolean DEBUG = FALSE;

    static const Int64 DEFAULT_TIMEOUT = 5000;  // 5 seconds

    AutoPtr<IThread> mThread;
    AutoPtr<ArrayOf<IParcelFileDescriptor*> > mFds;

    AutoPtr<IFileDescriptor> mOutFd;
    Int64 mEndTime;
    String mFailure;
    Boolean mComplete;

    String mBufferPrefix;
};

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_OS_CTRANSFERPIPE_H__
