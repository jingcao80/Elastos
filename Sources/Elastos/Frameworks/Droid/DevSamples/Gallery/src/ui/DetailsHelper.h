
#ifndef __DETAILSHELPER_H__
#define __DETAILSHELPER_H__

#include "elastos/droid/app/Activity.h"
#include "../data/MediaDetails.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IContext;

namespace Gallery {

extern "C" const InterfaceID EIID_ICloseListener;

class IDetailsSource
    : public ElRefBase
{
public:
    CARAPI virtual Size(
        /* [out] */ Int32* size) = 0;

    CARAPI virtual SetIndex(
        /* [out] */ Int32* index) = 0;

    CARAPI virtual GetDetails(
        /* [out] */ MediaDetails** mediaDetails) = 0;
};

class ICloseListener
    : public ElRefBase
    , public IInterface
{
public:
    CAR_INTERFACE_DECL()

    CARAPI virtual OnClose() = 0;
};

class IDetailsViewContainer
    : public ElRefBase
{
public:
    CARAPI virtual ReloadDetails() = 0;

    CARAPI virtual SetCloseListener(
        /* [in] */ ICloseListener* listener) = 0;

    CARAPI virtual Show() = 0;

    CARAPI virtual Hide() = 0;
};

class DetailsHelper
    : public ElRefBase
{
public:
    static String GetDetailsName(
        /* [in] */ IContext* context,
        /* [in] */ Int32 key);

    DetailsHelper(
        /* [in] */ IActivity* activity,
        ///* [in] */ IDetailsSource* source,
        /* [in] */ ArrayOf<String>* source);

    CARAPI_(void) ReloadDetails();

    CARAPI_(void) SetCloseListener(
        /* [in] */ ICloseListener* listener);

    CARAPI_(void) Show();

    CARAPI_(void) Hide();

private:
    static const String TAG;

private:
    AutoPtr<IDetailsViewContainer> mContainer;

};

} // namespace Gallery

#endif // __DETAILSHELPER_H__
