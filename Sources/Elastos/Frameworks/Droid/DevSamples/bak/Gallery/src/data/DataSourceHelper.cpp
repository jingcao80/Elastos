#include "DataSourceHelper.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::Utility::Logging::Logger;

namespace Gallery {

const String DataSourceHelper::ROOT_PATH = String("/mnt");
const String DataSourceHelper::PATH_SPLITE = String("/");
const String DataSourceHelper::SOURCE_PATH = String("source_path");
const String DataSourceHelper::SOURCE_DESC = String("source_desc");
const String DataSourceHelper::SOURCE_INDEX = String("source_index");
const String DataSourceHelper::TAG("DataSourceHelper");

Int32 DataSourceHelper::sVersion = 0;
String DataSourceHelper::sRootPath;
String DataSourceHelper::sCurrentPath;
Boolean DataSourceHelper::sInited = FALSE;

AutoPtr<List<String> > DataSourceHelper::sPath = new List<String>();
AutoPtr<HashMap<String, AutoPtr<List<String> > > > DataSourceHelper::sImageSourcePathMap
    = new HashMap<String, AutoPtr<List<String> > >();

DataSourceHelper::~DataSourceHelper()
{
    ClearAll();
}

void DataSourceHelper::SetRootPath(
    /* [in] */ const String& rootPath)
{
    Logger::D(TAG, "SetRootPath()---rootPath:%s", rootPath.string());
    ClearAll();
    sRootPath = rootPath;
}

AutoPtr<HashMap<String, AutoPtr<List<String> > > > DataSourceHelper::GetImageFileMap()
{
    Init();
    Logger::D(TAG, "GetImageFileMap()---count:%d", sImageSourcePathMap->GetSize());
    return sImageSourcePathMap;
}

AutoPtr<List<String> > DataSourceHelper::GetImagePathList()
{
    Init();
    Logger::D(TAG, "GetImagePathList()---count:%d", sPath->GetSize());
    if (sPath->GetSize() > 1) {
        SortListByTime();
    }
    return sPath;
}

AutoPtr<List<String> > DataSourceHelper::GetItemList(
    /* [in] */ const String& key)
{
    Init();
    AutoPtr<List<String> > list;
    HashMap<String, AutoPtr<List<String> > >::Iterator it = sImageSourcePathMap->Find(key);
    if (it != sImageSourcePathMap->End()) {
        list = it->mSecond;
    }

    Logger::D(TAG, "GetItemList()--key:%s, list:%p, mapSize:%d", key.string(), list.Get(), sImageSourcePathMap->GetSize());
    return list;
}

Boolean DataSourceHelper::TimeComparator(
    /* [in] */ const String& lhs,
    /* [in] */ const String& rhs)
{
    AutoPtr<IFile> file1;
    AutoPtr<IFile> file2;
    String strL(sCurrentPath);
    String strR(sCurrentPath);
    strL += lhs;
    strR += rhs;
    CFile::New(strL, (IFile**)&file1);
    CFile::New(strR, (IFile**)&file2);
    Int64 last1 = 0;
    Int64 last2 = 0;
    file1->LastModified(&last1);
    file2->LastModified(&last2);
    Int64 diff = last1 - last2;

    if (diff > 0) {
        return TRUE;
    }
    else if (diff == 0) {
        Int32 result = 0;
        if (sCurrentPath.IsNullOrEmpty()) {
            String lhsDir;
            String rhsDir;
            file1->GetParent(&lhsDir);
            file2->GetParent(&rhsDir);
            String lhsSub = lhsDir.Substring(lhsDir.LastIndexOf(PATH_SPLITE) + 1, lhsDir.GetLength());
            String rhsSub = rhsDir.Substring(rhsDir.LastIndexOf(PATH_SPLITE) + 1, rhsDir.GetLength());
            result = lhsSub.Compare(rhsSub);
        }
        else {
            result = lhs.Compare(rhs);
        }
        return result > 0 ? TRUE : FALSE;
    }
    else {
        return FALSE;
    }
}

Boolean DataSourceHelper::IsImageFile(
    /* [in] */ const String& fileName)
{
    // Logger::D(TAG, "IsImageFile--fileName:%s", fileName.string());
    Boolean result = FALSE;
    if (fileName.IsNullOrEmpty()) return result;
    String end = fileName.Substring(fileName.LastIndexOf(".") + 1, fileName.GetLength());
    end = end.ToLowerCase();
    if (end.Equals("jpg") || end.Equals("png") || end.Equals("jpeg") || end.Equals("bmp")
            || end.Equals("webp")) {
        result = TRUE;
    }
    return result;
}

void DataSourceHelper::Init()
{
    if (sInited)
        return;
    Logger::D(TAG, "Init()----");
    AutoPtr<IFile> dir;
    CFile::New(sRootPath, (IFile**)&dir);
    Boolean ret = FALSE;
    if (dir == NULL || (dir->IsDirectory(&ret), !ret)) {
        Logger::E(TAG, "sRootPath:%s is not a valid directory!", sRootPath.string());
        return;
    }

    GetImageFileMap(sRootPath);
    sInited = TRUE;
}

void DataSourceHelper::GetImageFileMap(
    /* [in] */ const String& rootPath)
{
    // Logger::D(TAG, "GetImageFileMap--rootPath:%s", rootPath.string());
    AutoPtr<IFile> dir;
    CFile::New(rootPath, (IFile**)&dir);
    AutoPtr< ArrayOf<IFile*> > files;
    dir->ListFiles((ArrayOf<IFile*>**)&files);
    if (files != NULL && files->GetLength() > 0) {
        AutoPtr<List<String> > itemList = new List<String>();
        Int32 length = files->GetLength();
        String absPath, name;
        IFile* file;
        Boolean ret;
        for (Int32 i = 0; i < length; i++) {
            file = (*files)[i];
            file->CanRead(&ret);
            if (ret) {
                file->IsDirectory(&ret);
                if (ret) {
                    file->GetAbsolutePath(&absPath);
                    GetImageFileMap(absPath);
                }
                else {
                    file->GetName(&name);
                    if (IsImageFile(name)) {
                        // Logger::D(TAG, "  > Image %d: %s", itemList->GetSize(), name.string());
                        itemList->PushBack(name);
                    }
                }
            }
        }

        if (itemList->IsEmpty() == FALSE) {
            Logger::D(TAG, "GetImageFileMap()--rootPath:%s, has %d images.", rootPath.string(), itemList->GetSize());
            if (itemList->GetSize() > 1) {
                sCurrentPath = rootPath;
                sCurrentPath += PATH_SPLITE;
                itemList->Sort(TimeComparator);
            }
            (*sImageSourcePathMap)[rootPath] = itemList;
            sPath->PushBack(rootPath);
        }
    }
}

void DataSourceHelper::ClearAll()
{
    Logger::D(TAG, "ClearAll()---");
    sPath->Clear();
    sImageSourcePathMap->Clear();
    sInited = FALSE;
}

void DataSourceHelper::SortListByTime()
{
    Logger::D(TAG, "SortListByTime()-----");
    AutoPtr<HashMap<String, AutoPtr<List<String> > > > pathMap = sImageSourcePathMap;
    AutoPtr<List<String> > pathList = sPath;
    List<String> albumList;
    AutoPtr<List<String> > fileList;
    List<String>::Iterator it = pathList->Begin();
    for (; it != pathList->End(); ++it) {
        String path(*it);
        HashMap<String, AutoPtr<List<String> > >::Iterator iter = pathMap->Find(path);
        fileList = iter->mSecond;
        path += PATH_SPLITE;
        path += fileList->GetFront();
        // Logger::D(TAG, "SortListByTime()-----path:%s", path.string());
        albumList.PushBack(path);
    }

    sCurrentPath = "";
    albumList.Sort(TimeComparator);
    Logger::D(TAG, "SortListByTime()----albumList-size:%d", albumList.GetSize());
    sPath->Clear();
    List<String>::Iterator iter = albumList.Begin();
    for (; iter != albumList.End(); ++iter) {
        String path(*iter);
        sPath->PushBack(path.Substring(0, path.LastIndexOf(DataSourceHelper::PATH_SPLITE)));
    }
    Logger::D(TAG, "SortListByTime()----sPath-size:%d", sPath->GetSize());
}

} // namespace Gallery
