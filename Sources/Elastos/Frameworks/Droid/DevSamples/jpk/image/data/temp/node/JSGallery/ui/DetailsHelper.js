module.exports = function(aoElastos){
//--------common definition----begin----
    var CObject = aoElastos.CObject;

    var CString = aoElastos.Core.CString;
    var Droid_New = aoElastos.Droid.New;
    var Core_New = aoElastos.Core.New;

    var R = aoElastos.Application.R;
//--------common definition----end----

    var DetailsHelper;

//--------.h----begin----

// #ifndef __DETAILSHELPER_H__
// #define __DETAILSHELPER_H__

// #include "app/Activity.h"
// #include "../data/MediaDetails.h"
    var MediaDetails = require("../data/MediaDetails.js")(aoElastos);

// using Elastos::Droid::App::IActivity;
// using Elastos::Droid::Content::IContext;

// namespace Gallery {

// extern "C" const InterfaceID EIID_ICloseListener;

// class IDetailsSource
//     : public ElRefBase
    var IDetailsSource = function() {}; //no constructor
// {
// public:
//     CARAPI virtual Size(
//         /* [out] */ Int32* size) = 0;

//     CARAPI virtual SetIndex(
//         /* [out] */ Int32* index) = 0;

//     CARAPI virtual GetDetails(
//         /* [out] */ MediaDetails** mediaDetails) = 0;
// };

// class ICloseListener
//     : public ElRefBase
//     , public IInterface
    var ICloseListener = function () {};    //no constructor
// {
// public:
//     CAR_INTERFACE_DECL()

//     CARAPI virtual OnClose() = 0;
// };

// class IDetailsViewContainer
//     : public ElRefBase
    var IDetailsViewContainer = function () {}; //no constructor
// {
// public:
//     CARAPI virtual ReloadDetails() = 0;

//     CARAPI virtual SetCloseListener(
//         /* [in] */ ICloseListener* listener) = 0;

//     CARAPI virtual Show() = 0;

//     CARAPI virtual Hide() = 0;
// };

// class DetailsHelper
//     : public ElRefBase
    var DetailsHelper;
// {
// public:
//     static String GetDetailsName(
//         /* [in] */ IContext* context,
//         /* [in] */ Int32 key);

//     DetailsHelper(
    var _DetailsHelper;
    DetailsHelper = function(
//         /* [in] */ IActivity* activity,
        activity,
//         ///* [in] */ IDetailsSource* source,
//         /* [in] */ ArrayOf<String>* source);
        source)
    {
        _DetailsHelper(this, activity, source);
    }

//     CARAPI_(void) ReloadDetails();

//     CARAPI_(void) SetCloseListener(
//         /* [in] */ ICloseListener* listener);

//     CARAPI_(void) Show();

//     CARAPI_(void) Hide();

// private:
//     static const String TAG;
    var TAG;

// private:
//     AutoPtr<IDetailsViewContainer> mContainer;

// };

// } // namespace Gallery

// #endif // __DETAILSHELPER_H__

//--------.h----end----

//--------.cpp----begin----

// #include "DetailsHelper.h"
// #include "../../R.h"
// #include <elastos/Logger.h>
// #include <elastos/StringUtils.h>
// #include "DialogDetailsView.h"
    var DialogDetailsView = require("./DialogDetailsView.js")(aoElastos);

// using Elastos::Core::StringUtils;
// using Elastos::Utility::Logging::Logger;

// namespace Gallery {

// // ba461079-a7f6-44ec-a5ef-3e78296dfa39
// extern "C" const InterfaceID EIID_ICloseListener =
//     { 0xba461079, 0xa7f6, 0x44ec, { 0xa5, 0xef, 0x3e, 0x78, 0x29, 0x6d, 0xfa, 0x39 } };
    var EIID_ICloseListener = [ 0xba461079, 0xa7f6, 0x44ec, [ 0xa5, 0xef, 0x3e, 0x78, 0x29, 0x6d, 0xfa, 0x39 ] ];

// const String DetailsHelper::TAG("DetailsHelper");
    TAG = "DetailsHelper";

// CAR_INTERFACE_IMPL(ICloseListener, IInterface)

// String DetailsHelper::GetDetailsName(
    DetailsHelper.prototype.GetDetailsName = function(
//     /* [in] */ IContext* context,
        context,
//     /* [in] */ Int32
        key)
// {
    {
//     Logger::D(TAG, "GetDetailsName--");
        elog(TAG + "GetDetailsName--");
//     String str;
        var str;
//     switch (key) {
        switch (key) {
//         case MediaDetails::INDEX_TITLE:
            case MediaDetails.INDEX_TITLE:
//             context->GetString(R::string::title, &str);
                str = context.GetString(R.string.title);
//             break;
                break;
//         case MediaDetails::INDEX_DESCRIPTION:
            case MediaDetails.INDEX_DESCRIPTION:
//             context->GetString(R::string::description, &str);
                str = context.GetString(R.string.description);
//             break;
                break;
//         case MediaDetails::INDEX_DATETIME:
            case MediaDetails.INDEX_DATETIME:
//             context->GetString(R::string::time, &str);
                str = context.GetString(R.string.time);
//             break;
                break;
//         case MediaDetails::INDEX_LOCATION:
            case MediaDetails.INDEX_LOCATION:
//             context->GetString(R::string::location, &str);
                str = context.GetString(R.string.location);
//             break;
                break;
//         case MediaDetails::INDEX_PATH:
            case MediaDetails.INDEX_PATH:
//             context->GetString(R::string::path, &str);
                str = context.GetString(R.string.path);
//             break;
                break;
//         case MediaDetails::INDEX_WIDTH:
            case MediaDetails.INDEX_WIDTH:
//             context->GetString(R::string::width, &str);
                str = context.GetString(R.string.width);
//             break;
                break;
//         case MediaDetails::INDEX_HEIGHT:
            case MediaDetails.INDEX_HEIGHT:
//             context->GetString(R::string::height, &str);
                str = context.GetString(R.string.height);
//             break;
                break;
//         case MediaDetails::INDEX_ORIENTATION:
            case MediaDetails.INDEX_ORIENTATION:
//             context->GetString(R::string::orientation, &str);
                str = context.GetString(R.string.orientation);
//             break;
                break;
//         case MediaDetails::INDEX_DURATION:
            case MediaDetails.INDEX_DURATION:
//             context->GetString(R::string::duration, &str);
                str = context.GetString(R.string.duration);
//             break;
                break;
//         case MediaDetails::INDEX_MIMETYPE:
            case MediaDetails.INDEX_MIMETYPE:
//             context->GetString(R::string::mimetype, &str);
                str = context.GetString(R.string.mimetype);
//             break;
                break;
//         case MediaDetails::INDEX_SIZE:
            case MediaDetails.INDEX_SIZE:
//             context->GetString(R::string::file_size, &str);
                str = context.GetString(R.string.file_size);
//             break;
                break;
//         case MediaDetails::INDEX_MAKE:
            case MediaDetails.INDEX_MAKE:
//             context->GetString(R::string::maker, &str);
                str = context.GetString(R.string.maker);
//             break;
                break;
//         case MediaDetails::INDEX_MODEL:
            case MediaDetails.INDEX_MODEL:
//             context->GetString(R::string::model, &str);
                str = context.GetString(R.string.model);
//             break;
                break;
//         case MediaDetails::INDEX_FLASH:
            case MediaDetails.INDEX_FLASH:
//             context->GetString(R::string::flash, &str);
                str = context.GetString(R.string.flash);
//             break;
                break;
//         case MediaDetails::INDEX_APERTURE:
            case MediaDetails.INDEX_APERTURE:
//             context->GetString(R::string::aperture, &str);
                str = context.GetString(R.string.aperture);
//             break;
                break;
//         case MediaDetails::INDEX_FOCAL_LENGTH:
            case MediaDetails.INDEX_FOCAL_LENGTH:
//             context->GetString(R::string::focal_length, &str);
                str = context.GetString(R.string.focal_length);
//             break;
                break;
//         case MediaDetails::INDEX_WHITE_BALANCE:
            case MediaDetails.INDEX_WHITE_BALANCE:
//             context->GetString(R::string::white_balance, &str);
                str = context.GetString(R.string.white_balance);
//             break;
                break;
//         case MediaDetails::INDEX_EXPOSURE_TIME:
            case MediaDetails.INDEX_EXPOSURE_TIME:
//             context->GetString(R::string::exposure_time, &str);
                str = context.GetString(R.string.exposure_time);
//             break;
                break;
//         case MediaDetails::INDEX_ISO:
            case MediaDetails.INDEX_ISO:
//             context->GetString(R::string::iso, &str);
                str = context.GetString(R.string.iso);
//             break;
                break;
//         default:
            default:
//             str = "Unknown key";
                str = "Unknown key";
//             str += StringUtils::Int32ToString(key);
                str += key;
//             break;
//         }
            }
//     return str;
        return str;
// }
    }

// DetailsHelper::DetailsHelper(
    _DetailsHelper = function(that,
//     /* [in] */ IActivity* activity,
        activity,
//     // /* [in] */ IDetailsSource* source)
//     /* [in] */ ArrayOf<String>* source)
        source)
// {
    {
//     mContainer = new DialogDetailsView(activity, source);
        that.mContainer = new DialogDetailsView(activity, source);
// }
    }

// void DetailsHelper::ReloadDetails()
    DetailsHelper.prototype.ReloadDetails = function()
// {
    {
//     mContainer->ReloadDetails();
        this.mContainer.ReloadDetails();
// }
    }

// void DetailsHelper::SetCloseListener(
    DetailsHelper.prototype.SetCloseListener = function(
//     /* [in] */ ICloseListener* listener)
        listener)
// {
    {
//     mContainer->SetCloseListener(listener);
        this.mContainer.SetCloseListener(listener);
// }
    }

// void DetailsHelper::Show()
    DetailsHelper.prototype.Show = function()
// {
    {
//     mContainer->Show();
        this.mContainer.Show();
// }
    }

// void DetailsHelper::Hide()
    DetailsHelper.prototype.Hide = function()
// {
    {
//     mContainer->Hide();
        this.mContainer.Hide();
// }
    }

// } // namespace Gallery

//--------.cpp----end----

    return DetailsHelper;
};  //exports