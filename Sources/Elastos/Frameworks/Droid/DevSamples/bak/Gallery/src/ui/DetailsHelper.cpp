#include "DetailsHelper.h"
#include "../../R.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringUtils.h>
#include "DialogDetailsView.h"

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Gallery {

// ba461079-a7f6-44ec-a5ef-3e78296dfa39
extern "C" const InterfaceID EIID_ICloseListener =
    { 0xba461079, 0xa7f6, 0x44ec, { 0xa5, 0xef, 0x3e, 0x78, 0x29, 0x6d, 0xfa, 0x39 } };

const String DetailsHelper::TAG("DetailsHelper");

CAR_INTERFACE_IMPL(ICloseListener, IInterface)

String DetailsHelper::GetDetailsName(
    /* [in] */ IContext* context,
    /* [in] */ Int32 key)
{
    Logger::D(TAG, "GetDetailsName--");
    String str;
    switch (key) {
        case MediaDetails::INDEX_TITLE:
            context->GetString(R::string::title, &str);
            break;
        case MediaDetails::INDEX_DESCRIPTION:
            context->GetString(R::string::description, &str);
            break;
        case MediaDetails::INDEX_DATETIME:
            context->GetString(R::string::time, &str);
            break;
        case MediaDetails::INDEX_LOCATION:
            context->GetString(R::string::location, &str);
            break;
        case MediaDetails::INDEX_PATH:
            context->GetString(R::string::path, &str);
            break;
        case MediaDetails::INDEX_WIDTH:
            context->GetString(R::string::width, &str);
            break;
        case MediaDetails::INDEX_HEIGHT:
            context->GetString(R::string::height, &str);
            break;
        case MediaDetails::INDEX_ORIENTATION:
            context->GetString(R::string::orientation, &str);
            break;
        case MediaDetails::INDEX_DURATION:
            context->GetString(R::string::duration, &str);
            break;
        case MediaDetails::INDEX_MIMETYPE:
            context->GetString(R::string::mimetype, &str);
            break;
        case MediaDetails::INDEX_SIZE:
            context->GetString(R::string::file_size, &str);
            break;
        case MediaDetails::INDEX_MAKE:
            context->GetString(R::string::maker, &str);
            break;
        case MediaDetails::INDEX_MODEL:
            context->GetString(R::string::model, &str);
            break;
        case MediaDetails::INDEX_FLASH:
            context->GetString(R::string::flash, &str);
            break;
        case MediaDetails::INDEX_APERTURE:
            context->GetString(R::string::aperture, &str);
            break;
        case MediaDetails::INDEX_FOCAL_LENGTH:
            context->GetString(R::string::focal_length, &str);
            break;
        case MediaDetails::INDEX_WHITE_BALANCE:
            context->GetString(R::string::white_balance, &str);
            break;
        case MediaDetails::INDEX_EXPOSURE_TIME:
            context->GetString(R::string::exposure_time, &str);
            break;
        case MediaDetails::INDEX_ISO:
            context->GetString(R::string::iso, &str);
            break;
        default:
            str = "Unknown key";
            str += StringUtils::ToString(key);
            break;
        }
    return str;
}

DetailsHelper::DetailsHelper(
    /* [in] */ IActivity* activity,
    // /* [in] */ IDetailsSource* source)
    /* [in] */ ArrayOf<String>* source)
{
    mContainer = new DialogDetailsView(activity, source);
}

void DetailsHelper::ReloadDetails()
{
    mContainer->ReloadDetails();
}

void DetailsHelper::SetCloseListener(
    /* [in] */ ICloseListener* listener)
{
    mContainer->SetCloseListener(listener);
}

void DetailsHelper::Show()
{
    mContainer->Show();
}

void DetailsHelper::Hide()
{
    mContainer->Hide();
}

} // namespace Gallery
