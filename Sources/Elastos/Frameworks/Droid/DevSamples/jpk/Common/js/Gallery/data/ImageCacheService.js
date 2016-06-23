module.exports = (function(){

function _extend(a,b){for(var p in a)b[p]=a[p];}

//constructor
// ImageCacheService::ImageCacheService(
//     /* [in] */ Int32 w,
//     /* [in] */ Int32 h)
//     : mImgWidth(w)
//     , mImgHeight(h)
// {
//     Logger::D(TAG, "ImageCacheService()-----");
//     AutoPtr<IFile> cacheDir;
//     CFile::New(CACHE_PATH, (IFile**)&cacheDir);
//     assert(cacheDir != NULL);
//     Boolean ret = FALSE;
//     cacheDir->Exists(&ret);
//     if (!ret) {
//         Logger::D(TAG, "ImageCacheService()-----Mkdirs");
//         cacheDir->Mkdirs(&ret);
//         if (!ret) {
//             Logger::E(TAG, "ImageCacheService()-----make directory failed!");
//         }
//     }

//     AutoPtr<ArrayOf<IFile*> > files;
//     cacheDir->ListFiles((ArrayOf<IFile*>**)&files);
//     if (files != NULL && files->GetLength() > 0) {
//         Int32 len = files->GetLength();
//         IFile* file;
//         String name;
//         Boolean ret = FALSE;
//         for (Int32 i = 0; i < len; i++) {
//             file = (*files)[i];
//             file->CanRead(&ret);
//             if (ret) {
//                 file->IsFile(&ret);
//                 if (ret) {
//                     file->GetName(&name);
//                     Logger::D(TAG, "file name:%s", name.string());
//                     mCachePathList.PushBack(name);
//                 }
//             }
//         }
//     }
//     UpdateCacheFilesIfNecessary();
//     Logger::D(TAG, "ImageCacheService()-----mCachePathList---size:%d", mCachePathList.GetSize());
// }
function ImageCacheService(w, h) {
    _initProp(this);
}

//public methods

// ECode ImageCacheService::GetImageData(
//     /* [in] */ Int32 key,
//     /* [in] */ Boolean isHigh,
//     /* [in] */ Int64 len,
//     /* [in] */ Int64 modify,
//     /* [out] */ IBitmap** bitmap)
// {
//     VALIDATE_NOT_NULL(bitmap);
//     Mutex::Autolock lock(mLock);
//     String keyStr = StringUtils::Int32ToString(key);
//     keyStr += "_";
//     keyStr += StringUtils::Int64ToString(len);
//     Logger::D(TAG, "GetImageData()-------keyStr:%s,%lld", keyStr.string(), modify);
//     List<String>::Iterator it = mCachePathList.Begin();
//     String fileName;
//     for (; it != mCachePathList.End();) {
//         fileName = *it;
//         if (fileName.StartWith(keyStr)) {
//             if (isHigh && fileName.Contains("#")) {
//                 if (fileName.EndWith(StringUtils::Int64ToString(modify))) {
//                     AutoPtr<IBitmapFactory> factory;
//                     String path(CACHE_PATH);
//                     path += fileName;
//                     Logger::D(TAG, "GetImageData()-------get full image cache file, path:%s", path.string());
//                     CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory);
//                     return factory->DecodeFile(path, NULL, bitmap);
//                 }
//                 else {
//                     Logger::W(TAG, "GetImageData()------1-not the same modify:%lld,fileName:%s", modify, fileName.string());
//                 }
//             }
//             else if (!isHigh && fileName.Contains("$")) {
//                 if (fileName.EndWith(StringUtils::Int64ToString(modify))) {
//                     AutoPtr<IBitmapFactory> factory;
//                     String path(CACHE_PATH);
//                     path += fileName;
//                     Logger::D(TAG, "GetImageData()-------get thumb image cache file, path:%s", path.string());
//                     CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory);
//                     return factory->DecodeFile(path, NULL, bitmap);
//                 }
//                 else {
//                     Logger::W(TAG, "GetImageData()------2-not the same modify:%lld,fileName:%s", modify, fileName.string());
//                 }
//             }
//         }
//         ++it;
//     }

//     *bitmap = NULL;
//     return NOERROR;
// }
function GetImageData(key, isHigh, len, modify) {
    elog("====ImageCacheService::GetImageData====begin========");

    var bitmap;

    return bitmap;
}

// ECode ImageCacheService::PutImageData(
//     /* [in] */ Int32 key,
//     /* [in] */ Boolean isHigh,
//     /* [in] */ Int64 len,
//     /* [in] */ Int64 modify,
//     /* [in] */ IBitmap* bitmap)
// {
//     Mutex::Autolock lock(mLock);
//     String imgStr = StringUtils::Int32ToString(key);
//     imgStr += "_";
//     imgStr += StringUtils::Int64ToString(len);
//     if (isHigh) {
//         imgStr += "#"; // full image
//     }
//     else {
//         imgStr += "$"; // thumb image
//     }
//     imgStr += StringUtils::Int64ToString(modify);
//     Logger::D(TAG, "PutImageData()-------imgStr:%s", imgStr.string());
//     AutoPtr<IFile> file;
//     CFile::New(CACHE_PATH, imgStr, (IFile**)&file);
//     assert(file != NULL);
//     Boolean ret = FALSE;
//     file->Exists(&ret);
//     if (ret) {
//         Logger::W(TAG, "PutImageData()----file:%s, is exist!", imgStr.string());
//         return NOERROR;
//     }
//     AutoPtr<IFileOutputStream> outStream;
//     ECode ec = CFileOutputStream::New(file, (IFileOutputStream**)&outStream);
//     if (FAILED(ec)) {
//         Logger::E(TAG, "Error: failed to create FileOutputStream %08x", ec);
//         return NOERROR;
//     }
//     assert(outStream != NULL);
//     ec = bitmap->Compress(BitmapCompressFormat_JPEG, 100, outStream, &ret);
//     outStream->Close();
//     if (!ret || ec != NOERROR) {
//         Logger::W(TAG, "PutImageData()---Compress--failed! ec:%x", ec);
//         DeleteFile(imgStr);
//     }
//     else {
//         mCachePathList.PushBack(imgStr);
//     }

//     Logger::D(TAG, "PutImageData()+++++++");
//     return ec;
// }
function PutImageData(key, isHigh, len, modify, bitmap){
    elog("====ImageCacheService::PutImageData====begin========");
}

// ECode ImageCacheService::UpdateCache()
// {
//     Logger::D(TAG, "UpdateCache()----1--size:%d", mCachePathList.GetSize());
//     if (mCachePathList.GetSize() > 200) {
//         DataSourceHelper::sCurrentPath = CACHE_PATH;
//         mCachePathList.Sort(DataSourceHelper::TimeComparator);
//         List<String>::Iterator it = mCachePathList.Begin();
//         String fileName;
//         for (Int32 i = 0; it != mCachePathList.End(); i++) {
//             fileName = *it;
//             if (!fileName.Contains("$") && !fileName.Contains("#")) {
//                 DeleteFile(fileName);
//                 it = mCachePathList.Erase(it);
//                 continue;
//             }
//             else if (i > 100) {
//                 if (fileName.Contains("$") && fileName.Contains("#"))
//                     continue;
//                 DeleteFile(fileName);
//                 it = mCachePathList.Erase(it);
//                 continue;
//             }
//             ++it;
//         }
//     }
//     Logger::D(TAG, "UpdateCache()----2--size:%d", mCachePathList.GetSize());
//     return NOERROR;
// }
function UpdateCache(){
    elog("====ImageCacheService::UpdateCache====begin========");
}

// void ImageCacheService::UpdateCacheFilesIfNecessary()
// {
//     List<String>::Iterator it = mCachePathList.Begin();
//     Boolean isNewConfigFile = FALSE;
//     Boolean hasConfigFile = FALSE;
//     String fileName;
//     for (; it != mCachePathList.End();) {
//         fileName = *it;
//         if (fileName.Contains("$") && fileName.Contains("#")) {
//             String strWidth = fileName.Substring(0, fileName.IndexOf("$"));
//             String strHeight = fileName.Substring(fileName.IndexOf("$") + 1, fileName.IndexOf("#"));
//             Int32 oldWidth = StringUtils::ParseInt32(strWidth);
//             Int32 oldHeight = StringUtils::ParseInt32(strHeight);
//             Logger::D(TAG, "UpdateCacheFilesIfNecessary()--oldW:%d,oldH:%d,newW:%d,newH:%d",
//                     oldWidth, oldHeight, mImgWidth, mImgHeight);
//             if ((oldWidth > 0 && oldHeight > 0) && (mImgWidth != oldWidth || mImgHeight != oldHeight)) {
//                 DeleteFile(fileName);
//                 it = mCachePathList.Erase(it);
//                 CreateConfigFile(mImgWidth, mImgHeight);
//                 isNewConfigFile = TRUE;
//             }
//             hasConfigFile = TRUE;
//             break;
//         }
//         ++it;
//     }
//     if (!hasConfigFile) {
//         CreateConfigFile(mImgWidth, mImgHeight);
//         isNewConfigFile = TRUE;
//     }
//     if (isNewConfigFile) {
//         ClearCacheFiles();
//     }
// }
//private methods
function _UpdateCacheFilesIfNecessary(){
    elog("====ImageCacheService::_UpdateCacheFilesIfNecessary====begin========");

}

// void ImageCacheService::DeleteFile(
//     /* [in] */ const String& fileName)
// {
//     Logger::D(TAG, "DeleteFile()------fileName:%s", fileName.string());
//     AutoPtr<IFile> file;
//     CFile::New(CACHE_PATH, fileName, (IFile**)&file);
//     assert(file != NULL);
//     Boolean ret = FALSE;
//     file->Exists(&ret);
//     if (ret) {
//         file->Delete(&ret);
//     }
// }
function _DeleteFile(filePath){
    elog("====ImageCacheService::_DeleteFile====begin========");

}

// void ImageCacheService::CreateConfigFile(
//     /* [in] */ Int32 w,
//     /* [in] */ Int32 h)
// {
//     String configFile(StringUtils::Int32ToString(w));
//     configFile += "$";
//     configFile += StringUtils::Int32ToString(h);
//     configFile += "#";
//     Logger::D(TAG, "CreateConfigFile()-------configFile:%s", configFile.string());
//     AutoPtr<IFile> file;
//     CFile::New(CACHE_PATH, configFile, (IFile**)&file);
//     assert(file != NULL);
//     Boolean ret = FALSE;
//     file->Exists(&ret);
//     if (ret) {
//         Logger::W(TAG, "CreateConfigFile()----configFile:%s, is exist!", configFile.string());
//         return;
//     }
//     ECode ec = file->CreateNewFile(&ret);
//     if (!ret || ec != NOERROR) {
//         Logger::W(TAG, "CreateConfigFile()----create configFile failed!");
//     }
// }
function _CreateConfigFile(w, h){
    elog("====ImageCacheService::_CreateConfigFile====begin========");

}

// void ImageCacheService::ClearCacheFiles()
// {
//     Logger::D(TAG, "ClearCacheFiles()------");
//     List<String>::Iterator it = mCachePathList.Begin();
//     String fileName;
//     for (; it != mCachePathList.End();) {
//         fileName = *it;
//         if (fileName.Contains("$") && fileName.Contains("#")) {
//             ++it;
//             continue;
//         }
//         DeleteFile(fileName);
//         it = mCachePathList.Erase(it);
//     }
// }
function _ClearCacheFiles(){
    elog("====ImageCacheService::_ClearCacheFiles====begin========");

}

//private properties
var _prop = [];
function _initProp(o){
    o.idx = _prop.length;
    _prop.push({
        mImgWidth : 0,          //Int32
        mImgHeight : 0,         //Int32
        mCachePathList : [],    //List<String>
        mLock : null,           //Mutex
    });
}

//static properties
_extend(ImageCacheService, {
    TAG : null,
    CACHE_PATH : null,
});

//static methods
//_extend(ImageCacheService, {
//    fn : fn,
//});

//public methods
_extend(ImageCacheService.prototype, {
    GetImageData : GetImageData,
    PutImageData : PutImageData,
    UpdateCache : UpdateCache
});

return ImageCacheService;

})();   //module.exports
