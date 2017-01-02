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

#ifndef __ELASTOS_UTILITY_PREFS_FILEPREFERENCESIMPL_H__
#define __ELASTOS_UTILITY_PREFS_FILEPREFERENCESIMPL_H__

#include "Elastos.CoreLibrary.IO.h"
#include "AbstractPreferences.h"

using Elastos::IO::IFile;
using Elastos::IO::IFilenameFilter;

namespace Elastos {
namespace Utility {
namespace Prefs {

class FilePreferencesImpl
    : public AbstractPreferences
{
private:
    class FilenameFilter
        : public Object
        , public IFilenameFilter
    {
    public:
        CAR_INTERFACE_DECL()

        FilenameFilter(
            /* [in] */ FilePreferencesImpl* host);

        CARAPI Accept(
            /* [in] */ IFile* dir,
            /* [in] */ const String& filename,
            /* [out] */ Boolean* succeeded);

    private:
        AutoPtr<FilePreferencesImpl> mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CARAPI ToString(
        /* [out] */ String* info);

    /*
     * --------------------------------------------------------------
     * Constructors
     * --------------------------------------------------------------
     */

    /**
     * Construct root <code>FilePreferencesImpl</code> instance rooted
     * at the given path.
     *
     * @hide
     */
    FilePreferencesImpl(
        /* [in] */ const String& path,
        /* [in] */ Boolean isUserNode);

    CARAPI_(void) InitPrefs();

protected:
    // @Override
    CARAPI ChildrenNamesSpi(
        /* [out, callee] */ ArrayOf<String>** list) /*throws BackingStoreException*/;

    // @Override
    CARAPI_(AutoPtr<AbstractPreferences>) ChildSpi(
        /* [in] */ const String& name);

    // @Override
    CARAPI FlushSpi() /*throws BackingStoreException*/;

    // @Override
    CARAPI_(String) GetSpi(
        /* [in] */ const String& key);

    // @Override
    CARAPI KeysSpi(
        /* [out, callee] */ ArrayOf<String>** spi)/* throws BackingStoreException*/;

    // @Override
    CARAPI PutSpi(
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    // @Override
    CARAPI RemoveNodeSpi() /*throws BackingStoreException */;

    // @Override
    CARAPI RemoveSpi(
        /* [in] */ const String& key);

    // @Override
    CARAPI SyncSpi()/* throws BackingStoreException */;

private:
    /**
     * Construct a prefs using given parent and given name
     */
    FilePreferencesImpl(
        /* [in] */ AbstractPreferences* parent,
        /* [in] */ const String& name);

private:
    //prefs file name
    static const String PREFS_FILE_NAME;

    //file path for this preferences node
    String mPath;

    //internal cache for prefs key-value pair
    AutoPtr<IProperties> mPrefs;

    //file represents this preferences node
    AutoPtr<IFile> mPrefsFile;

    //parent dir for this preferences node
    AutoPtr<IFile> mDir;

    //cache for removed prefs key-value pair
    AutoPtr<ISet> mRemoved;

    //cache for updated prefs key-value pair
    AutoPtr<ISet> mUpdated;
};

} // namespace Prefs
} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY__PREFS_FILEPREFERENCESIMPL_H__
