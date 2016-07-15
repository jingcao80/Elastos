module.exports = function(aoElastos){
//--------common definition----begin----
    var CObject = aoElastos.CObject;

    var CString = aoElastos.Core.CString;
    var Droid_New = aoElastos.Droid.New;
    var Core_New = aoElastos.Core.New;

    var R = aoElastos.Application.R;
//--------common definition----end----

//--------.h----begin----

// #ifndef __UTILS_H__
// #define __UTILS_H__

// #include "app/Activity.h"

// using Elastos::Droid::Widget::IToastHelper;
    //TODO:
    //var IToastHelper = CReflection.getClass("Elastos::Droid::Widget::IToastHelper");
    //var IToastHelper = Elastos.Droid.Widget.IToastHelper;

// namespace Gallery {

// class Utils
    function Utils() {};
//     : public ElRefBase
// {
// public:
//     static void MakeToast(
    Utils.MakeToast = function(
//         /* [in] */ IContext* context,
            context,
//         /* [in] */ const String& text,
            text,
//         /* [in] */ Int32 toastType);
            toastType){};

// private:
//     static const String TAG;
    var TAG;
//     static AutoPtr<IToastHelper> sToastHelper;
    var sToastHelper;
// };

// } // namespace Gallery

// #endif // __UTILS_H__

//--------.h----end----

//--------.cpp----begin----

// #include "Utils.h"
// #include <elastos/Logger.h>
// #include <elastos/StringBuilder.h>

// using Elastos::Core::StringBuilder;
// using Elastos::Droid::Widget::CToastHelper;
// using Elastos::Droid::Widget::IToast;
// using Elastos::Utility::Logging::Logger;

// namespace Gallery {

// const String Utils::TAG("Utils");
    TAG = "Utils";
// AutoPtr<IToastHelper> Utils::sToastHelper;
    sToastHelper = null;

// void Utils::MakeToast(
    Utils.MakeToast = function (
//     /* [in] */ IContext* context,
        context,
//     /* [in] */ const String& text,
        text,
//     /* [in] */ Int32 toastType)
        toastType)
// {
    {
//     Logger::D(TAG, "MakeToast()---");
        elog(TAG + "MakeToast()---");
//     if (sToastHelper == NULL) {
        if (sToastHelper == null) {
//         CToastHelper::AcquireSingleton((IToastHelper**)&sToastHelper);
            sToastHelper = Droid_New("Elastos.Droid.Widget.CToastHelper");
//     }
        }
//     StringBuilder sb(text);
        var sb = text;
//     AutoPtr<ICharSequence> seq = sb.ToCharSequence();
        var seq = CString(sb);
//     AutoPtr<IToast> toast;
        var toast;
//     sToastHelper->MakeText(context, seq, toastType, (IToast**)&toast);
        toast = sToastHelper.MakeText(context, seq, toastType);
//     toast->Show();
        toast.Show();
// }
    }

// } // namespace Gallery

//--------.cpp----end----

    return Utils;
};
