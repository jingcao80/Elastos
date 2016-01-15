#ifndef __ELASTOS_DROID_VIEW_CDISPLISTHELPER_H__
#define __ELASTOS_DROID_VIEW_CDISPLISTHELPER_H__

#include "_Elastos_Droid_View_CDispListHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/HashMap.h>
#include "Elastos.Core.h"
#include <elastos/utility/logging/Logger.h>

#define HDMI_DEFAULT_FORMAT DISP_FORMAT_HDMI_720P_60HZ
#define CVBS_DEFAULT_FORMAT DISP_FORMAT_NTSC
#define YPBPR_DEFAULT_FORMAT DISP_FORMAT_YPBPR_720P_60HZ
#define VGA_DEFAULT_FORMAT DISP_FORMAT_VGA_1024_768

using Elastos::Utility::CObjectMap;
using Elastos::Utility::IObjectMap;
using Elastos::Core::CObjectContainer;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Utility::Etl::HashMap;

#define HASH_EQUALTO_FUNC_FOR_AUTOPTR_IDISPLISTDISPFORMAT
DEFINE_EQUALTO_FUNC_FOR_AUTOPTR(Elastos::Droid::View::IDispListDispFormat)
#endif

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CDispListHelper)
{
public:
    CDispListHelper();

    CARAPI constructor();

    CARAPI GetDispList(
        /* [out] */ IObjectContainer** list);

    CARAPI GetItemStringIdList(
        /* [out] */ IObjectMap** map);

    CARAPI ItemCode2Name(
        /* [in] */ IDispListDispFormat* item,
        /* [out] */ String* rst);

    CARAPI ItemName2Code(
        /* [in] */ const String& name,
        /* [out] */ IDispListDispFormat** rst);

    CARAPI IsHDMI(
        /* [in] */ IDispListDispFormat* fmt,
        /* [out] */ Boolean* rst);

    CARAPI IsCVBS(
        /* [in] */ IDispListDispFormat* fmt,
        /* [out] */ Boolean* rst);

    CARAPI IsYPbPr(
        /* [in] */ IDispListDispFormat* fmt,
        /* [out] */ Boolean* rst);

    CARAPI IsVGA(
        /* [in] */ IDispListDispFormat* fmt,
        /* [out] */ Boolean* rst);

    CARAPI GetAdvancedDisplayType(
        /* [in] */ IDispListDispFormat* fmt,
        /* [out] */ Int32* rst);

    CARAPI GetStaticDispFormat(
        /* [in] */ Int32 formatId,
        /* [out] */ IDispListDispFormat** rst);

private:

    CARAPI_(AutoPtr<IDispListDispFormat>) InitFormat(
        /* [in] */ Int32 type,
        /* [in] */ Int32 format);

    CARAPI_(AutoPtr<IInteger32>) TransFromResource(
        /* [in] */ const Int32 value);

    CARAPI InitAllDispFormat();
    CARAPI InitStrMap();
    CARAPI InitItemArray();
    CARAPI InitShortCutArray();
    CARAPI InitShortCutStrMap();
    CARAPI InitShortCutFormatMap();
    CARAPI InitCode2NameMap();
    CARAPI InitName2CodeMap();

private:
    AutoPtr<IDispListDispFormat> DISP_FORMAT_NTSC             ;
    AutoPtr<IDispListDispFormat> DISP_FORMAT_PAL              ;
    AutoPtr<IDispListDispFormat> DISP_FORMAT_YPBPR_480I       ;
    AutoPtr<IDispListDispFormat> DISP_FORMAT_YPBPR_480P       ;
    AutoPtr<IDispListDispFormat> DISP_FORMAT_YPBPR_576I       ;
    AutoPtr<IDispListDispFormat> DISP_FORMAT_YPBPR_576P       ;
    AutoPtr<IDispListDispFormat> DISP_FORMAT_YPBPR_720P_50HZ  ;
    AutoPtr<IDispListDispFormat> DISP_FORMAT_YPBPR_720P_60HZ  ;
    AutoPtr<IDispListDispFormat> DISP_FORMAT_YPBPR_1080P_50HZ ;
    AutoPtr<IDispListDispFormat> DISP_FORMAT_YPBPR_1080P_60HZ ;
    AutoPtr<IDispListDispFormat> DISP_FORMAT_HDMI_480I        ;
    AutoPtr<IDispListDispFormat> DISP_FORMAT_HDMI_480P        ;
    AutoPtr<IDispListDispFormat> DISP_FORMAT_HDMI_576I        ;
    AutoPtr<IDispListDispFormat> DISP_FORMAT_HDMI_576P        ;
    AutoPtr<IDispListDispFormat> DISP_FORMAT_HDMI_720P_50HZ   ;
    AutoPtr<IDispListDispFormat> DISP_FORMAT_HDMI_720P_60HZ   ;
    AutoPtr<IDispListDispFormat> DISP_FORMAT_HDMI_1080I_50HZ  ;
    AutoPtr<IDispListDispFormat> DISP_FORMAT_HDMI_1080I_60HZ  ;
    AutoPtr<IDispListDispFormat> DISP_FORMAT_HDMI_1080P_24HZ  ;
    AutoPtr<IDispListDispFormat> DISP_FORMAT_HDMI_1080P_50HZ  ;
    AutoPtr<IDispListDispFormat> DISP_FORMAT_HDMI_1080P_60HZ  ;
    AutoPtr<IDispListDispFormat> DISP_FORMAT_VGA_640_480      ;
    AutoPtr<IDispListDispFormat> DISP_FORMAT_VGA_800_600      ;
    AutoPtr<IDispListDispFormat> DISP_FORMAT_VGA_1024_768     ;
    AutoPtr<IDispListDispFormat> DISP_FORMAT_VGA_1280_720     ;
    AutoPtr<IDispListDispFormat> DISP_FORMAT_VGA_1280_1024    ;
    AutoPtr<IDispListDispFormat> DISP_FORMAT_VGA_1360_768     ;
    AutoPtr<IDispListDispFormat> DISP_FORMAT_VGA_1440_900     ;
    AutoPtr<IDispListDispFormat> DISP_FORMAT_VGA_1680_1050    ;
    AutoPtr<IDispListDispFormat> DISP_FORMAT_VGA_1920_1080    ;
    AutoPtr<IObjectMap> mStrMap;
    AutoPtr<IObjectContainer> mItemArray;
    AutoPtr<ArrayOf<IInteger32*> > mShortCutArray;
    HashMap<AutoPtr<IInteger32>, AutoPtr<IInteger32> > mShortCutStrMap;
    HashMap<AutoPtr<IInteger32>, AutoPtr<IDispListDispFormat> >  mShortCutFormatMap;
    HashMap<AutoPtr<IDispListDispFormat>, String> mCode2NameMap;
    HashMap<String, AutoPtr<IDispListDispFormat> >  mName2CodeMap;

    typedef HashMap<AutoPtr<IDispListDispFormat>, String>::Iterator C2NIterator;
    typedef HashMap<String, AutoPtr<IDispListDispFormat> >::Iterator N2CIterator;
    friend class CDispList;
};

}// namespace View
}// namespace Droid
}// namespace Elastos

#endif
