#ifndef __ELASTOS_DROID_SETTINGS_DREAMBACKEND_H__
#define __ELASTOS_DROID_SETTINGS_DREAMBACKEND_H__

#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Graphics.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Service::Dreams::IIDreamManager;
using Elastos::Core::IComparator;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {

class DreamBackend
    : public Object
{
public:
    class DreamInfo
        : public Object
    {
    public:
        DreamInfo();

        //@Override
        CARAPI ToString(
            /* [out] */ String* str);

    public:
        AutoPtr<ICharSequence> mCaption;
        AutoPtr<IDrawable> mIcon;
        Boolean mIsActive;
        AutoPtr<IComponentName> mComponentName;
        AutoPtr<IComponentName> mSettingsComponentName;
    };

private:
    class DreamInfoComparator
        : public Object
        , public IComparator
    {
    public:
        TO_STRING_IMPL("DreamBackend::DreamInfoComparator")

        CAR_INTERFACE_DECL()

        DreamInfoComparator(
            /* [in] */ IComponentName* defaultDream);

        //@Override
        CARAPI Compare(
            /* [in] */ IInterface* _lhs,
            /* [in] */ IInterface* _rhs,
            /* [out] */ Int32* result);

    private:
        CARAPI_(String) SortKey(
            /* [in] */ DreamInfo* di);

    private:
        AutoPtr<IComponentName> mDefaultDream;
    };

public:
    TO_STRING_IMPL("DreamBackend")

    DreamBackend(
        /* [in] */ IContext* context);

    virtual CARAPI_(AutoPtr<IList>) /*List<DreamInfo>*/ GetDreamInfos();

    virtual CARAPI_(AutoPtr<IComponentName>) GetDefaultDream();

    virtual CARAPI_(AutoPtr<ICharSequence>) GetActiveDreamName();

    virtual CARAPI_(Boolean) IsEnabled();

    virtual CARAPI SetEnabled(
        /* [in] */ Boolean value);

    virtual CARAPI_(Boolean) IsActivatedOnDock();

    virtual CARAPI SetActivatedOnDock(
        /* [in] */ Boolean value);

    virtual CARAPI_(Boolean) IsActivatedOnSleep();

    virtual CARAPI SetActivatedOnSleep(
        /* [in] */ Boolean value);

    virtual CARAPI SetActiveDream(
        /* [in] */ IComponentName* dream);

    virtual CARAPI_(AutoPtr<IComponentName>) GetActiveDream();

    virtual CARAPI LaunchSettings(
        /* [in] */ DreamInfo* dreamInfo);

    virtual CARAPI Preview(
        /* [in] */ DreamInfo* dreamInfo);

    virtual CARAPI StartDreaming();

private:
    CARAPI_(Boolean) GetBoolean(
        /* [in] */ const String& key,
        /* [in] */ Boolean def);

    CARAPI_(void) SetBoolean(
        /* [in] */ const String& key,
        /* [in] */ Boolean value);

    static CARAPI_(AutoPtr<IComponentName>) GetDreamComponentName(
        /* [in] */ IResolveInfo* resolveInfo);

    static CARAPI_(AutoPtr<IComponentName>) GetSettingsComponentName(
        /* [in] */ IPackageManager* pm,
        /* [in] */ IResolveInfo* resolveInfo);

    static CARAPI_(void) Logd(
        /* [in] */ const String& msg,
        /* [in] */ ArrayOf<IInterface*>* args);

private:
    static const String TAG;

    IContext* mContext;
    AutoPtr<IIDreamManager> mDreamManager;
    AutoPtr<DreamInfoComparator> mComparator;
    Boolean mDreamsEnabledByDefault;
    Boolean mDreamsActivatedOnSleepByDefault;
    Boolean mDreamsActivatedOnDockByDefault;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_DREAMBACKEND_H__