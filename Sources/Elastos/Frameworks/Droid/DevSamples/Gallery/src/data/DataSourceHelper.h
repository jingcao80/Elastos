
#ifndef __DATASOURCEHELPER_H__
#define __DATASOURCEHELPER_H__

#include "elastos/droid/app/Activity.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;

namespace Gallery {

class DataSourceHelper
    : public ElRefBase
{
public:
    ~DataSourceHelper();

    static void SetRootPath(
        /* [in] */ const String& rootPath);

    static AutoPtr<HashMap<String, AutoPtr<List<String> > > > GetImageFileMap();

    static AutoPtr< List<String> > GetImagePathList();

    static AutoPtr< List<String> > GetItemList(
        /* [in] */ const String& key);

    static Int32 TimeComparator(
        /* [in] */ const String& lhs,
        /* [in] */ const String& rhs);

    static Boolean IsImageFile(
        /* [in] */ const String& fileName);

private:
    static void Init();

    static void GetImageFileMap(
        /* [in] */ const String& dir);

    static void ClearAll();

    static void SortListByTime();

public:
    static const String ROOT_PATH;
    static const String PATH_SPLITE;
    static const String SOURCE_PATH;
    static const String SOURCE_DESC;
    static const String SOURCE_INDEX;

    static String sCurrentPath;

private:
    static const String TAG;
    static Int32 sVersion;
    static String sRootPath;
    static Boolean sInited;

    static AutoPtr<List<String> > sPath;
    static AutoPtr<HashMap<String, AutoPtr<List<String> > > > sImageSourcePathMap;
};

} // namespace Gallery

#endif // __DATASOURCEHELPER_H__
