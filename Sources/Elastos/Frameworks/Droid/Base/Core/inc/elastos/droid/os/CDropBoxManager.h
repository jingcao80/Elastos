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

#ifndef __ELASTOS_DROID_OS_CDROPBOXMANAGER_H__
#define __ELASTOS_DROID_OS_CDROPBOXMANAGER_H__

#include "_Elastos_Droid_Os_CDropBoxManager.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/os/ParcelFileDescriptor.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/Math.h>
#include "elastos/droid/os/CDropBoxManagerEntry.h"

using Elastos::Core::Math;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Internal::Os::IDropBoxManagerService;
using Elastos::IO::IFile;
using Elastos::IO::ICloseable;
using Elastos::Droid::Os::IDropBoxManagerEntry;
using Elastos::Droid::Os::CDropBoxManagerEntry;

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CDropBoxManager)
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /** {@hide} */
    CARAPI constructor(
        /* [in] */ IDropBoxManagerService* service);

    /**
     * Stores human-readable text.  The data may be discarded eventually (or even
     * immediately) if space is limited, or ignored entirely if the tag has been
     * blocked (see {@link #isTagEnabled}).
     *
     * @param tag describing the type of entry being stored
     * @param data value to store
     */
    CARAPI AddText(
        /* [in] */ const String& tag,
        /* [in] */ const String& data);

    /**
     * Stores binary data, which may be ignored or discarded as with {@link #addText}.
     *
     * @param tag describing the type of entry being stored
     * @param data value to store
     * @param flags describing the data
     */
    CARAPI AddData(
        /* [in] */ const String& tag,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 flags);

    /**
     * Stores the contents of a file, which may be ignored or discarded as with
     * {@link #addText}.
     *
     * @param tag describing the type of entry being stored
     * @param file to read from
     * @param flags describing the data
     * @throws IOException if the file can't be opened
     */
    CARAPI AddFile(
        /* [in] */ const String& tag,
        /* [in] */ IFile* file,
        /* [in] */ Int32 flags);

    /**
     * Checks any blacklists (set in system settings) to see whether a certain
     * tag is allowed.  Entries with disabled tags will be dropped immediately,
     * so you can save the work of actually constructing and sending the data.
     *
     * @param tag that would be used in {@link #addText} or {@link #addFile}
     * @return whether events with that tag would be accepted
     */
    CARAPI IsTagEnabled(
        /* [in] */ const String& tag,
        /* [out] */ Boolean* isTagEnabled);

    /**
     * Gets the next entry from the drop box <em>after</em> the specified time.
     * Requires <code>android.permission.READ_LOGS</code>.  You must always call
     * {@link Entry#close()} on the return value!
     *
     * @param tag of entry to look for, null for all tags
     * @param msec time of the last entry seen
     * @return the next entry, or null if there are no more entries
     */
    CARAPI GetNextEntry(
        /* [in] */ const String& tag,
        /* [in] */ Int64 msec,
        /* [out] */ IDropBoxManagerEntry** nextEntry);

    // TODO: It may be useful to have some sort of notification mechanism
    // when data is added to the dropbox, for demand-driven readers --
    // for now readers need to poll the dropbox to find new data.

//protected:
    /**
     * Create a dummy instance for testing.  All methods will fail unless
     * overridden with an appropriate mock implementation.  To obtain a
     * functional instance, use {@link android.content.Context#getSystemService}.
     */
    CARAPI constructor();

private:
    static const String TAG;
    AutoPtr<IDropBoxManagerService> mService;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CDROPBOXMANAGER_H__
