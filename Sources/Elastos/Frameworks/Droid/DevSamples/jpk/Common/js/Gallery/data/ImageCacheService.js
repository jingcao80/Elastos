module.exports = function(aoElastos){

//common definition
var CString = aoElastos.Core.CString;
var Droid_New = aoElastos.Droid.New;
var Core_New = aoElastos.Core.New;
function _extend(a,b){for(var p in b)a[p]=b[p];}

//constructor
// ImageCacheService::ImageCacheService(
//     /* [in] */ Int32 w,
//     /* [in] */ Int32 h)
//     : mImgWidth(w)
//     , mImgHeight(h)
// {
function ImageCacheService(w, h) {
    elog("====ImageCacheService::ImageCacheService====begin========");

    _initProp(this);

    var prop = _prop[this.idx];
    prop.mImgWidth = w;
    prop.mImgHeight = h;

//     Logger::D(TAG, "ImageCacheService()-----");
    elog("====ImageCacheService::ImageCacheService=====");
//     AutoPtr<IFile> cacheDir;
//     CFile::New(CACHE_PATH, (IFile**)&cacheDir);
    var cacheDir = Core_New("Elastos.IO.CFile", ImageCacheService.CACHE_PATH);
//     assert(cacheDir != NULL);
//     Boolean ret = FALSE;

    elog("====ImageCacheService::ImageCacheService=====GetPath.0====");
    var path = cacheDir.GetPath();
    elog("====ImageCacheService::ImageCacheService=====GetPath.1===="+path);


    var ret = false;
//     cacheDir->Exists(&ret);
    ret = cacheDir.Exists();
//     if (!ret) {
    if (!ret) {
//         Logger::D(TAG, "ImageCacheService()-----Mkdirs");
        elog("====ImageCacheService::ImageCacheService====Mkdirs");
//         cacheDir->Mkdirs(&ret);
        ret = cacheDir.Mkdirs();
//         if (!ret) {
        if (!ret) {
//             Logger::E(TAG, "ImageCacheService()-----make directory failed!");
            elog("====ImageCacheService::ImageCacheService====make directory failed! ECode:"+"0x"+ret.toString(16).toUpperCase());
//         }
        }
//     }
    }

//     AutoPtr<ArrayOf<IFile*> > files;
//     cacheDir->ListFiles((ArrayOf<IFile*>**)&files);
    elog("====ImageCacheService::ImageCacheService=====ListFiles====begin====");
    var files = cacheDir.ListFiles();
    elog("====ImageCacheService::ImageCacheService=====ListFiles====end====");
//     if (files != NULL && files->GetLength() > 0) {
    if (files && files.length) {
//         Int32 len = files->GetLength();
        var len = files.length;
//         IFile* file;
        var file;
//         String name;
        var name;
//         Boolean ret = FALSE;
        var ret = false;
//         for (Int32 i = 0; i < len; i++) {
        for (var i = 0; i < len; i++) {
//             file = (*files)[i];
            file = files[i];
//             file->CanRead(&ret);
            ret = file.CanRead();
//             if (ret) {
            if (ret) {
//                 file->IsFile(&ret);
                ret = file.IsFile();
//                 if (ret) {
                if (ret) {
//                     file->GetName(&name);
                    name = file.GetName();
//                     Logger::D(TAG, "file name:%s", name.string());
                    elog(ImageCacheService.TAG + "====" + "file name:" + name);
//                     mCachePathList.PushBack(name);
                    prop.mCachePathList.push(name);
//                 }
                }
//             }
            }
//         }
        }
//     }
    }
//     UpdateCacheFilesIfNecessary();
    _UpdateCacheFilesIfNecessary(this);
//     Logger::D(TAG, "ImageCacheService()-----mCachePathList---size:%d", mCachePathList.GetSize());
    elog(ImageCacheService.TAG + "====" + "ImageCacheService()-----mCachePathList---size:" + prop.mCachePathList.length);
// }
}

//public methods

// ECode ImageCacheService::GetImageData(
//     /* [in] */ Int32 key,
//     /* [in] */ Boolean isHigh,
//     /* [in] */ Int64 len,
//     /* [in] */ Int64 modify,
//     /* [out] */ IBitmap** bitmap)
// {
function GetImageData(key, isHigh, len, modify) {
    elog("====ImageCacheService::GetImageData====begin========");

    var bitmap = null;

    var prop = _prop[this.idx];

//     VALIDATE_NOT_NULL(bitmap);
//     Mutex::Autolock lock(mLock);
//     String keyStr = StringUtils::Int32ToString(key);
//     keyStr += "_";
//     keyStr += StringUtils::Int64ToString(len);
    var keyStr = key + '_' + len;
//     Logger::D(TAG, "GetImageData()-------keyStr:%s,%lld", keyStr.string(), modify);
//     List<String>::Iterator it = mCachePathList.Begin();
//     String fileName;
    var fileName;
    for (var i=0,im=prop.mCachePathList.length;i<im;i++) {
//     for (; it != mCachePathList.End();) {
//         fileName = *it;
        fileName = prop.mCachePathList[i];
//         if (fileName.StartWith(keyStr)) {
        if (fileName.indexOf(keyStr)==0) {
//             if (isHigh && fileName.Contains("#")) {
            var _modify = '' + modify;
            if (isHigh && fileName.indexOf("#")>-1) {
//                 if (fileName.EndWith(StringUtils::Int64ToString(modify))) {
                if (fileName.lastIndexOf(_modify)+_modify.length==fileName.length) {
//                     AutoPtr<IBitmapFactory> factory;
//                     String path(CACHE_PATH);
//                     path += fileName;
                    var path = ImageCacheService.CACHE_PATH + fileName;
//                     Logger::D(TAG, "GetImageData()-------get full image cache file, path:%s", path.string());
//                     CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory);
                    var factory = Droid_New("Elastos.Droid.Graphics.CBitmapFactory");
//                     return factory->DecodeFile(path, NULL, bitmap);
                    elog("====ImageCacheService::GetImageData====begin========DecodeFile.1.1 path:"+path);
                    //bitmap = factory.DecodeFile(path, null);
                    bitmap = factory.DecodeFile(path);
                    elog("====ImageCacheService::GetImageData====begin========DecodeFile.1.2");
//                 }
                } else {
//                 else {
//                     Logger::W(TAG, "GetImageData()------1-not the same modify:%lld,fileName:%s", modify, fileName.string());
                    elog("ImageCacheService::GetImageData------1-not the same modify:"+modify+","+fileName);
//                 }
                }
//             }
            }
//             else if (!isHigh && fileName.Contains("$")) {
            else if (!isHigh && fileName.indexOf("$")>-1) {
//                 if (fileName.EndWith(StringUtils::Int64ToString(modify))) {
                if (fileName.lastIndexOf(_modify)+_modify.length==fileName.length) {
//                     AutoPtr<IBitmapFactory> factory;
//                     String path(CACHE_PATH);
//                     path += fileName;
                    var path = ImageCacheService.CACHE_PATH + fileName;
//                     Logger::D(TAG, "GetImageData()-------get thumb image cache file, path:%s", path.string());
//                     CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory);
                    var factory = Droid_New("Elastos.Droid.Graphics.CBitmapFactory");
//                     return factory->DecodeFile(path, NULL, bitmap);
                    elog("====ImageCacheService::GetImageData====begin========DecodeFile.2.1 path:"+path);
                    //bitmap = factory.DecodeFile(path, null);
                    bitmap = factory.DecodeFile(path);
                    elog("====ImageCacheService::GetImageData====begin========DecodeFile.2.2");
//                 }
                }
                else {
//                 else {
//                     Logger::W(TAG, "GetImageData()------2-not the same modify:%lld,fileName:%s", modify, fileName.string());
                    elog("ImageCacheService::GetImageData------2-not the same modify:"+modify+","+fileName);
//                 }
                }
//             }
            }
//         }
        }
//         ++it;
//     }

    }

    elog("====ImageCacheService::GetImageData====end========");

//     *bitmap = NULL;
    return bitmap;
//     return NOERROR;
// }
}

// ECode ImageCacheService::PutImageData(
//     /* [in] */ Int32 key,
//     /* [in] */ Boolean isHigh,
//     /* [in] */ Int64 len,
//     /* [in] */ Int64 modify,
//     /* [in] */ IBitmap* bitmap)
// {
function PutImageData(key, isHigh, len, modify, bitmap){
    elog("====ImageCacheService::PutImageData====begin========");

    var TAG = ImageCacheService.TAG;
    var prop = _prop[this.idx];

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
    var imgStr = key + "_" + len + (isHigh?"#":"$") + modify;

//     Logger::D(TAG, "PutImageData()-------imgStr:%s", imgStr.string());
    elog(TAG + "PutImageData()-------imgStr:" + imgStr);

//     AutoPtr<IFile> file;
//     CFile::New(CACHE_PATH, imgStr, (IFile**)&file);
//     assert(file != NULL);
    var file = Core_New("Elastos.IO.CFile", ImageCacheService.CACHE_PATH, imgStr);

//     Boolean ret = FALSE;
//     file->Exists(&ret);
//     if (ret) {
//         Logger::W(TAG, "PutImageData()----file:%s, is exist!", imgStr.string());
//         return NOERROR;
//     }
    var ret = file.Exists();
    if(ret) {
        elog(TAG + "PutImageData()-------file:" + imgStr + ", is exist!");
        return;
    }

//     AutoPtr<IFileOutputStream> outStream;
//     ECode ec = CFileOutputStream::New(file, (IFileOutputStream**)&outStream);
//     if (FAILED(ec)) {
//         Logger::E(TAG, "Error: failed to create FileOutputStream %08x", ec);
//         return NOERROR;
//     }
//     assert(outStream != NULL);
    var outStream;
    try {
        outStream = Core_New("Elastos.IO.CFileOutputStream", file);
    } catch (e) {
        elog(TAG + "Error: failed to create FileOutputStream " + JSON.stringify(e));
        return;
    }

//     ec = bitmap->Compress(BitmapCompressFormat_JPEG, 100, outStream, &ret);
//     outStream->Close();
//     if (!ret || ec != NOERROR) {
//         Logger::W(TAG, "PutImageData()---Compress--failed! ec:%x", ec);
//         DeleteFile(imgStr);
//     }
//     else {
//         mCachePathList.PushBack(imgStr);
//     }

    //Elastos::Droid::Graphics::BitmapCompressFormat_JPEG
    // enum BitmapCompressFormat {
    //     BitmapCompressFormat_JPEG    = 0,
    //     BitmapCompressFormat_PNG     = 1,
    //     BitmapCompressFormat_WEBP    = 2,
    // }
    var BitmapCompressFormat_JPEG = 0;

    var ret;
    try {
        ret = bitmap.Compress(BitmapCompressFormat_JPEG, 100, outStream);
        prop.mCachePathList.push(imgStr);
    } catch (e) {
        elog(TAG + "PutImageData()---Compress--failed! " + JSON.stringify(e));
        _DeleteFile(imgStr);
        return;
    }

//     Logger::D(TAG, "PutImageData()+++++++");
//     return ec;
    elog(TAG + "PutImageData()+++++++");
// }
}

// ECode ImageCacheService::UpdateCache()
// {
function UpdateCache(){
    elog("====ImageCacheService::UpdateCache====begin========");

    var prop = _prop[this.idx];

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
}

//private methods

// void ImageCacheService::UpdateCacheFilesIfNecessary()
// {
function _UpdateCacheFilesIfNecessary(ao_this){
    elog("====ImageCacheService::_UpdateCacheFilesIfNecessary====begin========");

    var prop = _prop[ao_this.idx];

//     List<String>::Iterator it = mCachePathList.Begin();
//     Boolean isNewConfigFile = FALSE;
//     Boolean hasConfigFile = FALSE;
//     String fileName;
    var isNewConfigFile = false;
    var hasConfigFile = false;
    var fileName;

//     for (; it != mCachePathList.End();) {
    var _list = [];
    for (var i=0,im=prop.mCachePathList.length;i<im;i++) {

//         fileName = *it;
        fileName = prop.mCachePathList[i];
//         if (fileName.Contains("$") && fileName.Contains("#")) {
        if (fileName.indexOf("$")>-1 && fileName.indexOf("#")>-1) {
//             String strWidth = fileName.Substring(0, fileName.IndexOf("$"));
//             String strHeight = fileName.Substring(fileName.IndexOf("$") + 1, fileName.IndexOf("#"));
//             Int32 oldWidth = StringUtils::ParseInt32(strWidth);
//             Int32 oldHeight = StringUtils::ParseInt32(strHeight);
//             Logger::D(TAG, "UpdateCacheFilesIfNecessary()--oldW:%d,oldH:%d,newW:%d,newH:%d",
//                     oldWidth, oldHeight, mImgWidth, mImgHeight);
            var s = fileName.split("#")[0].split("$");
            var oldWidth = parseInt(s[0]);
            var oldHeight = parseInt(s[1]);

//             if ((oldWidth > 0 && oldHeight > 0) && (mImgWidth != oldWidth || mImgHeight != oldHeight)) {
//                 DeleteFile(fileName);
//                 it = mCachePathList.Erase(it);
//                 CreateConfigFile(mImgWidth, mImgHeight);
//                 isNewConfigFile = TRUE;
//             }
            if ((oldWidth > 0 && oldHeight > 0) && (prop.mImgWidth != oldWidth || prop.mImgHeight != oldHeight)) {
                _DeleteFile(fileName);
                _CreateConfigFile(prop.mImgWidth, prop.mImgHeight);
                isNewConfigFile = true;
            }

//             hasConfigFile = TRUE;
            hasConfigFile = true;
//             break;
            break;
//         }
        }
//         ++it;
//     }
    }
    prop.mCachePathList = _list;

//     if (!hasConfigFile) {
//         CreateConfigFile(mImgWidth, mImgHeight);
//         isNewConfigFile = TRUE;
//     }
    if (!hasConfigFile) {
        _CreateConfigFile(prop.mImgWidth, prop.mImgHeight);
        isNewConfigFile = true;
    }

//     if (isNewConfigFile) {
//         ClearCacheFiles();
//     }
    if (isNewConfigFile) {
        _ClearCacheFiles(ao_this);
    }
// }
}

// void ImageCacheService::DeleteFile(
//     /* [in] */ const String& fileName)
// {
function _DeleteFile(fileName){
    elog("====ImageCacheService::_DeleteFile====begin========");

//     Logger::D(TAG, "DeleteFile()------fileName:%s", fileName.string());
//     AutoPtr<IFile> file;
//     CFile::New(CACHE_PATH, fileName, (IFile**)&file);
//     assert(file != NULL);
    var file = Core_New("Elastos.IO.CFile", fileName);

//     Boolean ret = FALSE;
//     file->Exists(&ret);
    var ret = file.Exists();
//     if (ret) {
//         file->Delete(&ret);
//     }
    if(ret) {
        ret = file.Delete();
    }
// }
}

// void ImageCacheService::CreateConfigFile(
//     /* [in] */ Int32 w,
//     /* [in] */ Int32 h)
// {
function _CreateConfigFile(w, h){
    elog("====ImageCacheService::_CreateConfigFile====begin========");

//     String configFile(StringUtils::Int32ToString(w));
//     configFile += "$";
//     configFile += StringUtils::Int32ToString(h);
//     configFile += "#";
//     Logger::D(TAG, "CreateConfigFile()-------configFile:%s", configFile.string());
    var configFile = w+"$"+h+"#";

//     AutoPtr<IFile> file;
//     CFile::New(CACHE_PATH, configFile, (IFile**)&file);
//     assert(file != NULL);
    var file = Core_New("Elastos.IO.CFile", ImageCacheService.CACHE_PATH, configFile);

//     Boolean ret = FALSE;
//     file->Exists(&ret);
    var ret = file.Exists();
//     if (ret) {
//         Logger::W(TAG, "CreateConfigFile()----configFile:%s, is exist!", configFile.string());
//         return;
//     }
    if (ret) {
        elog("ImageCacheService CreateConfigFile()----configFile:"+configFile+", is exist!");
        return;
    }

//     ECode ec = file->CreateNewFile(&ret);
//     if (!ret || ec != NOERROR) {
//         Logger::W(TAG, "CreateConfigFile()----create configFile failed!");
//     }
    try {
        ret = file.CreateNewFile();
    } catch(e) {
        elog("ImageCacheService CreateConfigFile()----create configFile failed!");
    }
// }
}

// void ImageCacheService::ClearCacheFiles()
// {
function _ClearCacheFiles(ao_this){
    elog("====ImageCacheService::_ClearCacheFiles====begin========");

    var prop = _prop[ao_this.idx];

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

    var _list = [];
    var fileName;
    for (var i=0,im=prop.mCachePathList.length;i<im;i++) {
        fileName = prop.mCachePathList[i];
        if (fileName.indexOf("$")>-1 && fileName.indexOf("#")>-1) {
            _list.push(fileName);
        } else {
            _DeleteFile(fileName);
        }
    }
    prop.mCachePathList = _list;

// }

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
    TAG : "ImageCacheService",
    CACHE_PATH : "/data/temp/diskcache/",
    //CACHE_PATH : "/mnt/sdcard/Android/data/JSGallery/cache/",
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

};   //module.exports
