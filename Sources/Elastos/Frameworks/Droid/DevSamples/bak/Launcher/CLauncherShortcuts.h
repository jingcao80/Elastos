
#ifndef __CLAUNCHERSHORTCUTS_H__
#define __CLAUNCHERSHORTCUTS_H__

#include "elastos/droid/app/Activity.h"
#include "_CLauncherShortcuts.h"
#include <elastos/utility/etl/List.h>

using Elastos::IO::IFile;
using Elastos::Droid::App::Activity;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Launcher {

class CLauncherShortcuts : public Activity
{
public:
    CARAPI OnListItemClick(Int32 id);
    ~CLauncherShortcuts();
protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    typedef HashMap<String, String> StringMap;
    AutoPtr<List< AutoPtr<StringMap> > > mWebappsList;
    AutoPtr<IFile> mWebappsFolder;
};

} // namespace Launcher
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CLAUNCHERSHORTCUTS_H__
