module.exports = function(aoElastos){

var JS_MULTI_READY = false;

//common definition
var CString = aoElastos.Core.CString;
var Droid_New = aoElastos.Droid.New;
var Core_New = aoElastos.Core.New;

function extend (a,b) {
    for (var p in b) a[p] = b[p];
}

var sFileName = '/data/temp/node/JSGallery/data/ImageCacheService.js';
var ImageCacheService = require(sFileName)(aoElastos);

//main class
function AsyncImageLoader () {};

//private static property:
var TAG = "AsyncImageLoader";
//var sImgWidth = 320;
//var sImgHeight = 320;
var sImgWidth = 952;
var sImgHeight = 1648;

var sCacheService=null;

var sImageCache = {};
var sImageSampleStatus = {};
var sHighImageCache = {};
var sHighImageWH = {};
var sImageLoaderParamCache = {};

var sLock = null;
var sStatusLock = null;
var sImageWHLock = null;

//class
function ILoadImageCallback () {
    this.ImageLoaded = function (aoImageDrawable, aoImageView) {
        ///* [in] */ IDrawable* imageDrawable,
        ///* [in] */ IImageView* imageView) = 0;
    };
}

//class
function ImageLoaderParam (aoImageView, aoCallback) {
    this.mImageView = aoImageView;  //IImageView
    this.mCallback = aoCallback;    //ILoadImageCallback
}

//private internal class
var ImageLoaderThread = (function(){
    function _ImageLoaderThread (abIsHigh, asImageUrl) {
        this.mIsHigh = abIsHigh;
        this.mImageUrl = asImageUrl;
    }

    //private method
    function LoadImageFromUrl (path) {
        var cacheService = GetCacheService();
        var last = 0;
        var file = Core_New("Elastos.IO.CFile", path);
        //var last = file.LastModified();
        var last = file.GetLastModified();

        //var hashkey = path.GetHashCode();
        var hashkey = file.GetHashCode();
        if (hashkey < 0) hashkey = 0 - hashkey;
        var len = file.GetLength();
        var bitmap = cacheService.GetImageData(hashkey, this.mIsHigh, len, last);
        if (bitmap) {
            //Mutex::Autolock lock(sStatusLock);
            sImageSampleStatus[path] = true;
            var bitmapDrawable = Droid_New("Elastos.Droid.Graphics.Drawable.CBitmapDrawable",bitmap);

            return bitmapDrawable;
        }

        var opt;
        var inSampleSize = 0;

        var opt = Droid_New("Elastos.Droid.Graphics.CBitmapFactoryOptions");
        opt.SetInJustDecodeBounds(true);

        var factory = Droid_New("Elastos.Droid.Graphics.CBitmapFactory");

        var bitmap = factory.DecodeFile(path, opt);

        if (this.mIsHigh) {
            //inSampleSize = ComputeSampleSize(opt, -1, 1280 * 672 * 4);
            inSampleSize = ComputeSampleSize(opt, -1, 1648 * 952 * 4);
        }
        else {
            inSampleSize = ComputeSampleSize(opt, -1, sImgWidth * sImgHeight);
        }

        opt.SetInSampleSize(inSampleSize);
        opt.SetInJustDecodeBounds(false);

        //Mutex::Autolock lock(sStatusLock);
        sImageSampleStatus[path] = (inSampleSize > 1 || this.mIsHigh);

        //bitmap = null;
        bitmap = factory.DecodeFile(path, opt);

        var bitmapDrawable = Droid_New("Elastos.Droid.Graphics.Drawable.CBitmapDrawable",bitmap);
        if (inSampleSize > 1 && bitmap != null) {
            cacheService.PutImageData(hashkey, this.mIsHigh, len, last, bitmap);
        }

        return bitmapDrawable;
    }

    function ComputeSampleSize (options, minSideLength, maxNumOfPixels) {
        var initialSize = ComputeInitialSampleSize(options, minSideLength, maxNumOfPixels);
        var roundedSize = 0;

        if (initialSize <= 8 ) {
            roundedSize = 1;
            while (roundedSize < initialSize) {
                roundedSize <<= 1;
            }
        }
        else {
            roundedSize = (initialSize + 7) / 8 * 8;
        }

        return roundedSize;
    }

    function ComputeInitialSampleSize (options, minSideLength, maxNumOfPixels) {
        var w = options.GetOutWidth();
        var h = options.GetOutHeight();

        if (this.mIsHigh) {
            //Mutex::Autolock lock(sImageWHLock);
            sHighImageWH[mImageUrl] = [w, h];
        }

        var lowerBound = (maxNumOfPixels == -1/*UNCONSTRAINED*/) ? 1 :
            Math.ceil(Math.sqrt(w * h / maxNumOfPixels));
        var upperBound = (minSideLength == -1/*UNCONSTRAINED*/) ? 128 :
            Math.min(Math.Floor(w / minSideLength),
                Math.floor(h / minSideLength));

        if (upperBound < lowerBound) {
            // return the larger one when there is no overlapping zone.
            return lowerBound;
        }

        if ((maxNumOfPixels == -1/*UNCONSTRAINED*/) && (minSideLength == -1/*UNCONSTRAINED*/)) {
            return 1;
        }
        else if (minSideLength == -1/*UNCONSTRAINED*/) {
            return lowerBound;
        }
        else {
            return upperBound;
        }

    }

    //public method:
    extend(_ImageLoaderThread.prototype, {
        _LoadImageFromUrl : LoadImageFromUrl,

        start : function () {
            if(!JS_MULTI_READY) {
                return this.Run();
            }
            else {
                this.Run();
            }
        },

        Run : function () {
            var drawable = this._LoadImageFromUrl(this.mImageUrl);

            AsyncImageLoader.DrawableLoaded(this.mImageUrl, this.mIsHigh, drawable);

            if(!JS_MULTI_READY) {
                return drawable;
            }
        },
    }); //extend

    return _ImageLoaderThread;
})();   //ImageLoaderThread

//public static
function LoadDrawable (imageUrl, isHigh, imageView, imageCallback) {
    var drawable;

    //Mutex::Autolock lock(sLock);

    if (isHigh) {
        drawable = sHighImageCache[imageUrl];
    }
    else {
        drawable = sImageCache[imageUrl];
    }
    if (drawable) return drawable;

    var key = this.MakeImageParamKey(imageUrl, isHigh);
    var list = sImageLoaderParamCache[key];
    if (list) {
        var needAdd = true;
        for (var i=0, im=list.length; i<im; i++) {
            if (list[i] == imageView) {
                needAdd = false;
                break;
            }
        }
        if (needAdd) {
            list.push({
                mImageView : imageView,
                mCallback : imageCallback,
            });
        }
    }
    else {
        sImageLoaderParamCache[key] = [
            {
                mImageView : imageView,
                mCallback : imageCallback,
            }
        ];
    }

    var loadRunnable = new ImageLoaderThread(isHigh, imageUrl);

    if(JS_MULTI_READY) {
        loadRunnable.start();
    }
    else {
        drawable = loadRunnable.start();
    }

    return drawable;
}

function DrawableLoaded (imageUrl, isHigh, drawable){
    //Mutex::Autolock lock(sLock);

    if (isHigh) {
        sHighImageCache[imageUrl] = drawable;
    }
    else {
        sImageCache[imageUrl] = drawable;
    }

    var key = this.MakeImageParamKey(imageUrl, isHigh);
    var list = sImageLoaderParamCache[key];
    if (list) {
        for (var i=0, im=list.length; i<im; i++) {
            var param = list[i];    //ImageLoaderParam
            param.mCallback.ImageLoaded(drawable, param.mImageView);
        }
    }
}

function NeedLoadHighDrawable (imageUrl) {
    //Mutex::Autolock lock(sStatusLock);
    //return sImageSampleStatus[imageUrl];
    return true;
}

// Boolean AsyncImageLoader::GetOrigionWidthAndHeight(
function GetOrigionWidthAndHeight(
//     /* [in] */ const String& imageUrl,
        imageUrl,
//     /* [out] */ Int32* w,
//     /* [out] */ Int32* h)
        r)  //r:{w:w,h:h}   more than one out para, must transfer throught object reference
{

//     // VALIDATE_NOT_NULL(w);
//     *w = 0;
        r.w = 0;
//     // VALIDATE_NOT_NULL(h);
//     *h = 0;
        r.h = 0;

//     Mutex::Autolock lock(sImageWHLock);
        //TODO:multi thread opt
//     HashMap<String, AutoPtr< ArrayOf<Int32> > >::Iterator it = sHighImageWH.Find(imageUrl);
        var it = sHighImageWH[imageUrl];
//     if (it != sHighImageWH.End()) {
        if (it) {
//         AutoPtr< ArrayOf<Int32> > tmpArray = it->mSecond;
            var tmpArray = it;
//         *w = (*tmpArray)[0];
            r.w = tmpArray[0];
//         *h = (*tmpArray)[1];
            r.h = tmpArray[1];
//         return TRUE;
            return true;
//     }
        }
//     AutoPtr<IBitmapFactoryOptions> opt;
        var opt;

//     CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&opt);
        opt = Droid_New("Elastos.Droid.Graphics.CBitmapFactoryOptions");
//     opt->SetInJustDecodeBounds(TRUE);
        opt.SetInJustDecodeBounds(true);
//     AutoPtr<IBitmapFactory> factory;
        var factory;
//     CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory);
        factory = Droid_New("Elastos.Droid.Graphics.CBitmapFactory");
//     AutoPtr<IBitmap> bitmap;
        var bitmap;
//     if (FAILED(factory->DecodeFile(imageUrl, opt, (IBitmap**)&bitmap)))
//         return FALSE;
        try {
            bitmap = factory.DecodeFile(imageUrl, opt);
        }
        catch(e) {
            return false;
        }
//     opt->GetOutWidth(w);
        r.w = opt.GetOutWidth();
//     opt->GetOutHeight(h);
        r.h = opt.GetOutHeight();
//     return TRUE;
        return true;
}

function ClearAll () {
    sImageCache = {};
    sImageSampleStatus = {};
    sHighImageCache = {};
    sHighImageWH = {};
}

function MakeImageParamKey (imageUrl, isHigh) {
    return imageUrl + isHigh;
}

function UpdateImageCache (albumArray) {
    var cacheService = GetCacheService();
    var album = albumArray;
    cacheService.UpdateCache();
    var len = album.length;
    var isMatch = false;
    for (var i=0,im=album.length;i<im;i++) {
        var key = album[i];
        if(sImageCache[key]){
            sImageCache[key]=null;
            delete sImageCache[key];
        }
    }
}

function GetCacheService () {
    if (sCacheService===null) {
        sCacheService = new ImageCacheService(sImgWidth, sImgHeight);
    }
    return sCacheService;
}

extend(AsyncImageLoader, {
    //public static:
    LoadDrawable : LoadDrawable,
    DrawableLoaded : DrawableLoaded,
    NeedLoadHighDrawable : NeedLoadHighDrawable,
    GetOrigionWidthAndHeight : GetOrigionWidthAndHeight,
    ClearAll : ClearAll,
    MakeImageParamKey : MakeImageParamKey,
    UpdateImageCache : UpdateImageCache,
    GetCacheService : GetCacheService,
});

return AsyncImageLoader;

};   //module.exports
