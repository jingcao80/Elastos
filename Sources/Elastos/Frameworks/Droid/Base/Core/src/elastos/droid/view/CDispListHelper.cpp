#include "elastos/droid/view/CDispListHelper.h"
#include "elastos/droid/view/CDispListDispFormat.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace View {

CDispListHelper::CDispListHelper()
{
    InitAllDispFormat();
    InitStrMap();
    InitCode2NameMap();
    InitName2CodeMap();
    InitItemArray();
    InitShortCutArray();
    InitShortCutStrMap();
    InitShortCutFormatMap();
}

ECode CDispListHelper::constructor()
{
    return NOERROR;
}

ECode CDispListHelper::GetDispList(
    /* [out] */ IObjectContainer** list)
{
    VALIDATE_NOT_NULL(list);
    *list = mItemArray;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode CDispListHelper::GetItemStringIdList(
    /* [out] */ IObjectMap** map)
{
    VALIDATE_NOT_NULL(map);
    *map = mStrMap;
    REFCOUNT_ADD(*map);
    return NOERROR;
}

ECode CDispListHelper::ItemCode2Name(
    /* [in] */ IDispListDispFormat* item,
    /* [out] */ String* rst)
{
    VALIDATE_NOT_NULL(rst);
    *rst = String(NULL);
    AutoPtr<IDispListDispFormat> key = item;
    C2NIterator it = mCode2NameMap.Find(key);
    if(it != mCode2NameMap.End())
    {
        *rst = it->mSecond;
    }
    return NOERROR;
}

ECode CDispListHelper::ItemName2Code(
    /* [in] */ const String& name,
    /* [out] */ IDispListDispFormat** rst)
{
    VALIDATE_NOT_NULL(rst);
    *rst = NULL;
    N2CIterator it = mName2CodeMap.Find(name);
    if(it != mName2CodeMap.End())
    {
        *rst = it->mSecond;
        REFCOUNT_ADD(*rst);
    }
    return NOERROR;
}

ECode CDispListHelper::IsHDMI(
    /* [in] */ IDispListDispFormat* fmt,
    /* [out] */ Boolean* rst)
{
    *rst = FALSE;
    Int32 outputType;
    fmt->GetOutputType(&outputType);
    if(outputType == IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_HDMI)
    {
        *rst = TRUE;
    }
    return NOERROR;
}

ECode CDispListHelper::IsCVBS(
    /* [in] */ IDispListDispFormat* fmt,
    /* [out] */ Boolean* rst)
{
    *rst = FALSE;
    Boolean equals;
    if((fmt->Equals(DISP_FORMAT_NTSC, &equals), equals) || (fmt->Equals(DISP_FORMAT_PAL, &equals), equals))
    {
        *rst = TRUE;
    }
    return NOERROR;
}

ECode CDispListHelper::IsYPbPr(
    /* [in] */ IDispListDispFormat* fmt,
    /* [out] */ Boolean* rst)
{
    *rst = FALSE;
    Boolean equals;
    if((fmt->Equals(DISP_FORMAT_YPBPR_480I, &equals), equals)
       || (fmt->Equals(DISP_FORMAT_YPBPR_480P, &equals), equals)
       || (fmt->Equals(DISP_FORMAT_YPBPR_576I, &equals), equals)
       || (fmt->Equals(DISP_FORMAT_YPBPR_576P, &equals), equals)
       || (fmt->Equals(DISP_FORMAT_YPBPR_720P_50HZ, &equals), equals)
       || (fmt->Equals(DISP_FORMAT_YPBPR_720P_60HZ, &equals), equals)
       || (fmt->Equals(DISP_FORMAT_YPBPR_1080P_50HZ, &equals), equals)
       || (fmt->Equals(DISP_FORMAT_YPBPR_1080P_60HZ, &equals), equals))
    {
        *rst = TRUE;
    }
    return NOERROR;
}

ECode CDispListHelper::IsVGA(
    /* [in] */ IDispListDispFormat* fmt,
    /* [out] */ Boolean* rst)
{
    *rst = FALSE;
    Int32 outputType;
    fmt->GetOutputType(&outputType);
    if(outputType == IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_VGA)
    {
        *rst = TRUE;
    }
    return NOERROR;
}

ECode CDispListHelper::GetAdvancedDisplayType(
    /* [in] */ IDispListDispFormat* fmt,
    /* [out] */ Int32* rst)
{
    Boolean isType;
    if(IsHDMI(fmt, &isType), isType){
        *rst = IDispList::ADVANCED_DISPLAY_TYPE_HDMI;
        return NOERROR;
    }else if(IsCVBS(fmt, &isType), isType){
        *rst = IDispList::ADVANCED_DISPLAY_TYPE_CVBS;
        return NOERROR;
    }else if(IsYPbPr(fmt, &isType), isType){
        *rst = IDispList::ADVANCED_DISPLAY_TYPE_YPBPR;
        return NOERROR;
    }else if(IsVGA(fmt, &isType), isType){
        *rst = IDispList::ADVANCED_DISPLAY_TYPE_VGA;
        return NOERROR;
    }else{
        *rst = IDispList::ADVANCED_DISPLAY_TYPE_UNKNOWN;
        return NOERROR;
    }
}

ECode CDispListHelper::GetStaticDispFormat(
    /* [in] */ Int32 formatId,
    /* [out] */ IDispListDispFormat** rst)
{
    switch(formatId)
    {
        case IDispList::DISP_FORMAT_NTSC_ID : {
            *rst = DISP_FORMAT_NTSC;
            REFCOUNT_ADD(*rst)
            return NOERROR;
        }
        case IDispList::DISP_FORMAT_PAL_ID : {
            *rst = DISP_FORMAT_PAL;
            REFCOUNT_ADD(*rst)
            return NOERROR;
        }
        case IDispList::DISP_FORMAT_YPBPR_480I_ID : {
            *rst = DISP_FORMAT_YPBPR_480I;
            REFCOUNT_ADD(*rst)
            return NOERROR;
        }
        case IDispList::DISP_FORMAT_YPBPR_480P_ID : {
            *rst = DISP_FORMAT_YPBPR_480P;
            REFCOUNT_ADD(*rst)
            return NOERROR;
        }
        case IDispList::DISP_FORMAT_YPBPR_576I_ID : {
            *rst = DISP_FORMAT_YPBPR_576I;
            REFCOUNT_ADD(*rst)
            return NOERROR;
        }
        case IDispList::DISP_FORMAT_YPBPR_576P_ID : {
            *rst = DISP_FORMAT_YPBPR_576P;
            REFCOUNT_ADD(*rst)
            return NOERROR;
        }
        case IDispList::DISP_FORMAT_HDMI_720P_50HZ_ID : {
            *rst = DISP_FORMAT_HDMI_720P_50HZ;
            REFCOUNT_ADD(*rst)
            return NOERROR;
        }
        case IDispList::DISP_FORMAT_HDMI_720P_60HZ_ID : {
            *rst = DISP_FORMAT_HDMI_720P_60HZ;
            REFCOUNT_ADD(*rst)
            return NOERROR;
        }
        case IDispList::DISP_FORMAT_HDMI_1080I_50HZ_ID : {
            *rst = DISP_FORMAT_HDMI_1080I_50HZ;
            REFCOUNT_ADD(*rst)
            return NOERROR;
        }
        case IDispList::DISP_FORMAT_HDMI_1080I_60HZ_ID : {
            *rst = DISP_FORMAT_HDMI_1080I_60HZ;
            REFCOUNT_ADD(*rst)
            return NOERROR;
        }
        case IDispList::DISP_FORMAT_HDMI_1080P_24HZ_ID : {
            *rst = DISP_FORMAT_HDMI_1080P_24HZ;
            REFCOUNT_ADD(*rst)
            return NOERROR;
        }
        case IDispList::DISP_FORMAT_HDMI_1080P_50HZ_ID : {
            *rst = DISP_FORMAT_HDMI_1080P_50HZ;
            REFCOUNT_ADD(*rst)
            return NOERROR;
        }
        case IDispList::DISP_FORMAT_HDMI_1080P_60HZ_ID : {
            *rst = DISP_FORMAT_HDMI_1080P_60HZ;
            REFCOUNT_ADD(*rst)
            return NOERROR;
        }
        case IDispList::DISP_FORMAT_VGA_640_480_ID : {
            *rst = DISP_FORMAT_VGA_640_480;
            REFCOUNT_ADD(*rst)
            return NOERROR;
        }
        case IDispList::DISP_FORMAT_VGA_800_600_ID : {
            *rst = DISP_FORMAT_VGA_800_600;
            REFCOUNT_ADD(*rst)
            return NOERROR;
        }
        case IDispList::DISP_FORMAT_VGA_1024_768_ID : {
            *rst = DISP_FORMAT_VGA_1024_768;
            REFCOUNT_ADD(*rst)
            return NOERROR;
        }
        case IDispList::DISP_FORMAT_VGA_1280_720_ID : {
            *rst = DISP_FORMAT_VGA_1280_720;
            REFCOUNT_ADD(*rst)
            return NOERROR;
        }
        case IDispList::DISP_FORMAT_VGA_1280_1024_ID : {
            *rst = DISP_FORMAT_VGA_1280_1024;
            REFCOUNT_ADD(*rst)
            return NOERROR;
        }
        case IDispList::DISP_FORMAT_VGA_1360_768_ID : {
            *rst = DISP_FORMAT_VGA_1360_768;
            REFCOUNT_ADD(*rst)
            return NOERROR;
        }
        case IDispList::DISP_FORMAT_VGA_1440_900_ID : {
            *rst = DISP_FORMAT_VGA_1440_900;
            REFCOUNT_ADD(*rst)
            return NOERROR;
        }
        case IDispList::DISP_FORMAT_VGA_1680_1050_ID : {
            *rst = DISP_FORMAT_VGA_1680_1050;
            REFCOUNT_ADD(*rst)
            return NOERROR;
        }
        case IDispList::DISP_FORMAT_VGA_1920_1080_ID : {
            *rst = DISP_FORMAT_VGA_1920_1080;
            REFCOUNT_ADD(*rst)
            return NOERROR;
        }
        default : {
            (*rst) = NULL;
            return NOERROR;
        }
    }
}

AutoPtr<IDispListDispFormat> CDispListHelper::InitFormat(
    /* [in] */ Int32 type,
    /* [in] */ Int32 format)
{
    AutoPtr<IDispListDispFormat> rst;
    CDispListDispFormat::New(type, format, (IDispListDispFormat**)&rst);
    return rst;
}

ECode CDispListHelper::InitAllDispFormat()
{
    DISP_FORMAT_NTSC = InitFormat(IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_TV  , IDisplayManagerAw::DISPLAY_TVFORMAT_NTSC);
    DISP_FORMAT_PAL = InitFormat(IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_TV  , IDisplayManagerAw::DISPLAY_TVFORMAT_PAL);
    DISP_FORMAT_YPBPR_480I = InitFormat(IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_TV  , IDisplayManagerAw::DISPLAY_TVFORMAT_480I);
    DISP_FORMAT_YPBPR_480P = InitFormat(IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_TV  , IDisplayManagerAw::DISPLAY_TVFORMAT_480P);
    DISP_FORMAT_YPBPR_576I = InitFormat(IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_TV  , IDisplayManagerAw::DISPLAY_TVFORMAT_576I);
    DISP_FORMAT_YPBPR_576P = InitFormat(IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_TV  , IDisplayManagerAw::DISPLAY_TVFORMAT_576P);
    DISP_FORMAT_YPBPR_720P_50HZ = InitFormat(IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_TV  , IDisplayManagerAw::DISPLAY_TVFORMAT_720P_50HZ);
    DISP_FORMAT_YPBPR_720P_60HZ = InitFormat(IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_TV  , IDisplayManagerAw::DISPLAY_TVFORMAT_720P_60HZ);
    DISP_FORMAT_YPBPR_1080P_50HZ = InitFormat(IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_TV  , IDisplayManagerAw::DISPLAY_TVFORMAT_1080P_50HZ);
    DISP_FORMAT_YPBPR_1080P_60HZ = InitFormat(IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_TV  , IDisplayManagerAw::DISPLAY_TVFORMAT_1080P_60HZ);
    DISP_FORMAT_HDMI_480I = InitFormat(IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_HDMI, IDisplayManagerAw::DISPLAY_TVFORMAT_480I);
    DISP_FORMAT_HDMI_480P = InitFormat(IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_HDMI, IDisplayManagerAw::DISPLAY_TVFORMAT_480P);
    DISP_FORMAT_HDMI_576I = InitFormat(IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_HDMI, IDisplayManagerAw::DISPLAY_TVFORMAT_576I);
    DISP_FORMAT_HDMI_576P = InitFormat(IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_HDMI, IDisplayManagerAw::DISPLAY_TVFORMAT_576P);
    DISP_FORMAT_HDMI_720P_50HZ = InitFormat(IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_HDMI, IDisplayManagerAw::DISPLAY_TVFORMAT_720P_50HZ);
    DISP_FORMAT_HDMI_720P_60HZ = InitFormat(IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_HDMI, IDisplayManagerAw::DISPLAY_TVFORMAT_720P_60HZ);
    DISP_FORMAT_HDMI_1080I_50HZ = InitFormat(IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_HDMI, IDisplayManagerAw::DISPLAY_TVFORMAT_1080I_50HZ);
    DISP_FORMAT_HDMI_1080I_60HZ = InitFormat(IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_HDMI, IDisplayManagerAw::DISPLAY_TVFORMAT_1080I_60HZ);
    DISP_FORMAT_HDMI_1080P_24HZ = InitFormat(IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_HDMI, IDisplayManagerAw::DISPLAY_TVFORMAT_1080P_24HZ);
    DISP_FORMAT_HDMI_1080P_50HZ = InitFormat(IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_HDMI, IDisplayManagerAw::DISPLAY_TVFORMAT_1080P_50HZ);
    DISP_FORMAT_HDMI_1080P_60HZ = InitFormat(IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_HDMI, IDisplayManagerAw::DISPLAY_TVFORMAT_1080P_60HZ);
    DISP_FORMAT_VGA_640_480 = InitFormat(IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_VGA , IDisplayManagerAw::DISPLAY_VGA_H640_V480);
    DISP_FORMAT_VGA_800_600 = InitFormat(IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_VGA , IDisplayManagerAw::DISPLAY_VGA_H800_V600);
    DISP_FORMAT_VGA_1024_768 = InitFormat(IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_VGA , IDisplayManagerAw::DISPLAY_VGA_H1024_V768);
    DISP_FORMAT_VGA_1280_720 = InitFormat(IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_VGA , IDisplayManagerAw::DISPLAY_VGA_H1280_V720);
    DISP_FORMAT_VGA_1280_1024 = InitFormat(IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_VGA , IDisplayManagerAw::DISPLAY_VGA_H1280_V1024);
    DISP_FORMAT_VGA_1360_768 = InitFormat(IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_VGA , IDisplayManagerAw::DISPLAY_VGA_H1360_V768);
    DISP_FORMAT_VGA_1440_900 = InitFormat(IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_VGA , IDisplayManagerAw::DISPLAY_VGA_H1440_V900);
    DISP_FORMAT_VGA_1680_1050 = InitFormat(IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_VGA , IDisplayManagerAw::DISPLAY_VGA_H1680_V1050);
    DISP_FORMAT_VGA_1920_1080 = InitFormat(IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_VGA , IDisplayManagerAw::DISPLAY_VGA_H1920_V1080);
    return NOERROR;
}

ECode CDispListHelper::InitStrMap()
{
    CObjectMap::New((IObjectMap**)&mStrMap);
    mStrMap->Put(DISP_FORMAT_NTSC, TransFromResource(R::string::disp_format_ntsc));
    mStrMap->Put(DISP_FORMAT_PAL, TransFromResource(R::string::disp_format_pal));
    mStrMap->Put(DISP_FORMAT_YPBPR_480I, TransFromResource(R::string::disp_format_ypbpr_480i));
    mStrMap->Put(DISP_FORMAT_YPBPR_480P, TransFromResource(R::string::disp_format_ypbpr_480p));
    mStrMap->Put(DISP_FORMAT_YPBPR_576I, TransFromResource(R::string::disp_format_ypbpr_576i));
    mStrMap->Put(DISP_FORMAT_YPBPR_576P, TransFromResource(R::string::disp_format_ypbpr_576p));
    mStrMap->Put(DISP_FORMAT_YPBPR_720P_50HZ, TransFromResource(R::string::disp_format_ypbpr_720p_50hz));
    mStrMap->Put(DISP_FORMAT_YPBPR_720P_60HZ, TransFromResource(R::string::disp_format_ypbpr_720p_60hz));
    mStrMap->Put(DISP_FORMAT_YPBPR_1080P_60HZ, TransFromResource(R::string::disp_format_ypbpr_1080p_60hz));
    mStrMap->Put(DISP_FORMAT_YPBPR_1080P_50HZ, TransFromResource(R::string::disp_format_ypbpr_1080p_50hz));
    mStrMap->Put(DISP_FORMAT_HDMI_480I, TransFromResource(R::string::disp_format_hdmi_480i));
    mStrMap->Put(DISP_FORMAT_HDMI_480P, TransFromResource(R::string::disp_format_hdmi_480p));
    mStrMap->Put(DISP_FORMAT_HDMI_576I, TransFromResource(R::string::disp_format_hdmi_576i));
    mStrMap->Put(DISP_FORMAT_HDMI_576P, TransFromResource(R::string::disp_format_hdmi_576p));
    mStrMap->Put(DISP_FORMAT_HDMI_720P_50HZ, TransFromResource(R::string::disp_format_hdmi_720p_50hz));
    mStrMap->Put(DISP_FORMAT_HDMI_720P_60HZ, TransFromResource(R::string::disp_format_hdmi_720p_60hz));
    mStrMap->Put(DISP_FORMAT_HDMI_1080I_50HZ, TransFromResource(R::string::disp_format_hdmi_1080i_50hz));
    mStrMap->Put(DISP_FORMAT_HDMI_1080I_60HZ, TransFromResource(R::string::disp_format_hdmi_1080i_60hz));
    mStrMap->Put(DISP_FORMAT_HDMI_1080P_24HZ, TransFromResource(R::string::disp_format_hdmi_1080p_24hz));
    mStrMap->Put(DISP_FORMAT_HDMI_1080P_50HZ, TransFromResource(R::string::disp_format_hdmi_1080p_50hz));
    mStrMap->Put(DISP_FORMAT_HDMI_1080P_60HZ, TransFromResource(R::string::disp_format_hdmi_1080p_60hz));
    mStrMap->Put(DISP_FORMAT_VGA_640_480, TransFromResource(R::string::disp_format_vga_640_480));
    mStrMap->Put(DISP_FORMAT_VGA_800_600, TransFromResource(R::string::disp_format_vga_800_600));
    mStrMap->Put(DISP_FORMAT_VGA_1024_768, TransFromResource(R::string::disp_format_vga_1024_768));
    mStrMap->Put(DISP_FORMAT_VGA_1280_720, TransFromResource(R::string::disp_format_vga_1280_720));
    mStrMap->Put(DISP_FORMAT_VGA_1280_1024, TransFromResource(R::string::disp_format_vga_1280_1024));
    mStrMap->Put(DISP_FORMAT_VGA_1360_768, TransFromResource(R::string::disp_format_vga_1360_768));
    mStrMap->Put(DISP_FORMAT_VGA_1440_900, TransFromResource(R::string::disp_format_vga_1440_900));
    mStrMap->Put(DISP_FORMAT_VGA_1680_1050, TransFromResource(R::string::disp_format_vga_1680_1050));
    mStrMap->Put(DISP_FORMAT_VGA_1920_1080, TransFromResource(R::string::disp_format_vga_1920_1080));
    return NOERROR;
}

AutoPtr<IInteger32> CDispListHelper::TransFromResource(
    /* [in] */ Int32 value)
{
    AutoPtr<IInteger32> rst;
    CInteger32::New(value, (IInteger32**)&rst);
    return rst;
}

ECode CDispListHelper::InitItemArray()
{
    CObjectContainer::New((IObjectContainer**)&mItemArray);
    // mItemArray->Add(DISP_FORMAT_NTSC);
    // mItemArray->Add(DISP_FORMAT_PAL);
    mItemArray->Add(DISP_FORMAT_NTSC);
    mItemArray->Add(DISP_FORMAT_PAL);
    // mItemArray->Add(DISP_FORMAT_YPBPR_480I      );
    // mItemArray->Add(DISP_FORMAT_YPBPR_480P      );
    // mItemArray->Add(DISP_FORMAT_YPBPR_576I      );
    // mItemArray->Add(DISP_FORMAT_YPBPR_576P      );
    // mItemArray->Add(DISP_FORMAT_YPBPR_720P_50HZ );
    // mItemArray->Add(DISP_FORMAT_YPBPR_720P_60HZ );
    // mItemArray->Add(DISP_FORMAT_YPBPR_1080P_50HZ);
    // mItemArray->Add(DISP_FORMAT_YPBPR_1080P_60HZ);
    mItemArray->Add(DISP_FORMAT_HDMI_480I);
    mItemArray->Add(DISP_FORMAT_HDMI_480P);
    mItemArray->Add(DISP_FORMAT_HDMI_576I);
    mItemArray->Add(DISP_FORMAT_HDMI_576P);
    mItemArray->Add(DISP_FORMAT_HDMI_720P_50HZ);
    mItemArray->Add(DISP_FORMAT_HDMI_720P_60HZ);
    mItemArray->Add(DISP_FORMAT_HDMI_1080I_50HZ);
    mItemArray->Add(DISP_FORMAT_HDMI_1080I_60HZ);
    mItemArray->Add(DISP_FORMAT_HDMI_1080P_24HZ);
    mItemArray->Add(DISP_FORMAT_HDMI_1080P_50HZ);
    mItemArray->Add(DISP_FORMAT_HDMI_1080P_60HZ);
    mItemArray->Add(DISP_FORMAT_VGA_640_480);
    mItemArray->Add(DISP_FORMAT_VGA_800_600);
    mItemArray->Add(DISP_FORMAT_VGA_1024_768);
    mItemArray->Add(DISP_FORMAT_VGA_1280_720);
    mItemArray->Add(DISP_FORMAT_VGA_1280_1024);
    mItemArray->Add(DISP_FORMAT_VGA_1360_768);
    mItemArray->Add(DISP_FORMAT_VGA_1440_900);
    mItemArray->Add(DISP_FORMAT_VGA_1680_1050);
    mItemArray->Add(DISP_FORMAT_VGA_1920_1080);
    return NOERROR;
}

ECode CDispListHelper::InitShortCutArray()
{
    mShortCutArray = ArrayOf<IInteger32*>::Alloc(3);
    AutoPtr<IInteger32> temp;
    CInteger32::New(IDispList::ADVANCED_DISPLAY_TYPE_HDMI, (IInteger32**)&temp);
    mShortCutArray->Set(0, temp);
    temp = NULL;
    CInteger32::New(IDispList::ADVANCED_DISPLAY_TYPE_CVBS, (IInteger32**)&temp);
    mShortCutArray->Set(1, temp);
    temp = NULL;
    CInteger32::New(IDispList::ADVANCED_DISPLAY_TYPE_VGA, (IInteger32**)&temp);
    mShortCutArray->Set(2, temp);
    return NOERROR;
}

ECode CDispListHelper::InitShortCutStrMap()
{
    AutoPtr<IInteger32> key;
    CInteger32::New(IDispList::ADVANCED_DISPLAY_TYPE_HDMI, (IInteger32**)&key);
    AutoPtr<IInteger32> value = TransFromResource(R::string::display_type_hmdi);
    mShortCutStrMap[key] = value;

    key = NULL;
    CInteger32::New(IDispList::ADVANCED_DISPLAY_TYPE_CVBS, (IInteger32**)&key);
    value = TransFromResource(R::string::display_type_cvbs);
    mShortCutStrMap[key] = value;

    key = NULL;
    CInteger32::New(IDispList::ADVANCED_DISPLAY_TYPE_YPBPR, (IInteger32**)&key);
    value = TransFromResource(R::string::display_type_ypbpr);
    mShortCutStrMap[key] = value;

    key = NULL;
    CInteger32::New(IDispList::ADVANCED_DISPLAY_TYPE_VGA, (IInteger32**)&key);
    value = TransFromResource(R::string::display_type_vga);
    mShortCutStrMap[key] = value;
    return NOERROR;
}

ECode CDispListHelper::InitShortCutFormatMap()
{
    AutoPtr<IInteger32> key;
    CInteger32::New(IDispList::ADVANCED_DISPLAY_TYPE_HDMI, (IInteger32**)&key);
    mShortCutFormatMap[key] = HDMI_DEFAULT_FORMAT;

    key = NULL;
    CInteger32::New(IDispList::ADVANCED_DISPLAY_TYPE_CVBS, (IInteger32**)&key);
    mShortCutFormatMap[key] = CVBS_DEFAULT_FORMAT;

    key = NULL;
    CInteger32::New(IDispList::ADVANCED_DISPLAY_TYPE_YPBPR, (IInteger32**)&key);
    mShortCutFormatMap[key] = YPBPR_DEFAULT_FORMAT;

    key = NULL;
    CInteger32::New(IDispList::ADVANCED_DISPLAY_TYPE_VGA, (IInteger32**)&key);
    mShortCutFormatMap[key] = VGA_DEFAULT_FORMAT;
    return NOERROR;
}

ECode CDispListHelper::InitCode2NameMap()
{
    mCode2NameMap[DISP_FORMAT_NTSC] = IDispList::DISP_FORMAT_NAME_NTSC;
    mCode2NameMap[DISP_FORMAT_PAL] = IDispList::DISP_FORMAT_NAME_PAL;
    mCode2NameMap[DISP_FORMAT_YPBPR_480I] = IDispList::DISP_FORMAT_NAME_YPBPR_480I;
    mCode2NameMap[DISP_FORMAT_YPBPR_480P] = IDispList::DISP_FORMAT_NAME_YPBPR_480P;
    mCode2NameMap[DISP_FORMAT_YPBPR_576I] = IDispList::DISP_FORMAT_NAME_YPBPR_576I;
    mCode2NameMap[DISP_FORMAT_YPBPR_576P] = IDispList::DISP_FORMAT_NAME_YPBPR_576P;
    mCode2NameMap[DISP_FORMAT_YPBPR_720P_50HZ] = IDispList::DISP_FORMAT_NAME_YPBPR_720P_50HZ;
    mCode2NameMap[DISP_FORMAT_YPBPR_720P_60HZ] = IDispList::DISP_FORMAT_NAME_YPBPR_720P_60HZ;
    mCode2NameMap[DISP_FORMAT_YPBPR_1080P_60HZ] = IDispList::DISP_FORMAT_NAME_YPBPR_1080P_50HZ;
    mCode2NameMap[DISP_FORMAT_YPBPR_1080P_50HZ] = IDispList::DISP_FORMAT_NAME_YPBPR_1080P_60HZ;
    mCode2NameMap[DISP_FORMAT_HDMI_480I] = IDispList::DISP_FORMAT_NAME_HDMI_480I;
    mCode2NameMap[DISP_FORMAT_HDMI_480P] = IDispList::DISP_FORMAT_NAME_HDMI_480P;
    mCode2NameMap[DISP_FORMAT_HDMI_576I] = IDispList::DISP_FORMAT_NAME_HDMI_576I;
    mCode2NameMap[DISP_FORMAT_HDMI_576P] = IDispList::DISP_FORMAT_NAME_HDMI_576P;
    mCode2NameMap[DISP_FORMAT_HDMI_720P_50HZ] = IDispList::DISP_FORMAT_NAME_HDMI_720P_50HZ;
    mCode2NameMap[DISP_FORMAT_HDMI_720P_60HZ] = IDispList::DISP_FORMAT_NAME_HDMI_720P_60HZ;
    mCode2NameMap[DISP_FORMAT_HDMI_1080I_50HZ] = IDispList::DISP_FORMAT_NAME_HDMI_1080I_50HZ;
    mCode2NameMap[DISP_FORMAT_HDMI_1080I_60HZ] = IDispList::DISP_FORMAT_NAME_HDMI_1080I_60HZ;
    mCode2NameMap[DISP_FORMAT_HDMI_1080P_24HZ] = IDispList::DISP_FORMAT_NAME_HDMI_1080P_24HZ;
    mCode2NameMap[DISP_FORMAT_HDMI_1080P_50HZ] = IDispList::DISP_FORMAT_NAME_HDMI_1080P_50HZ;
    mCode2NameMap[DISP_FORMAT_HDMI_1080P_60HZ] = IDispList::DISP_FORMAT_NAME_HDMI_1080P_60HZ;
    mCode2NameMap[DISP_FORMAT_VGA_640_480] = IDispList::DISP_FORMAT_NAME_VGA_640_480;
    mCode2NameMap[DISP_FORMAT_VGA_800_600] = IDispList::DISP_FORMAT_NAME_VGA_800_600;
    mCode2NameMap[DISP_FORMAT_VGA_1024_768] = IDispList::DISP_FORMAT_NAME_VGA_1024_768;
    mCode2NameMap[DISP_FORMAT_VGA_1280_720] = IDispList::DISP_FORMAT_NAME_VGA_1280_720;
    mCode2NameMap[DISP_FORMAT_VGA_1280_1024] = IDispList::DISP_FORMAT_NAME_VGA_1280_1024;
    mCode2NameMap[DISP_FORMAT_VGA_1360_768] = IDispList::DISP_FORMAT_NAME_VGA_1360_768;
    mCode2NameMap[DISP_FORMAT_VGA_1440_900] = IDispList::DISP_FORMAT_NAME_VGA_1440_900;
    mCode2NameMap[DISP_FORMAT_VGA_1680_1050] = IDispList::DISP_FORMAT_NAME_VGA_1680_1050;
    mCode2NameMap[DISP_FORMAT_VGA_1920_1080] = IDispList::DISP_FORMAT_NAME_VGA_1920_1080;
    return NOERROR;
}
ECode CDispListHelper::InitName2CodeMap()
{
    mName2CodeMap[IDispList::DISP_FORMAT_NAME_NTSC] = DISP_FORMAT_NTSC;
    mName2CodeMap[IDispList::DISP_FORMAT_NAME_PAL] = DISP_FORMAT_PAL;
    mName2CodeMap[IDispList::DISP_FORMAT_NAME_YPBPR_480I] = DISP_FORMAT_YPBPR_480I;
    mName2CodeMap[IDispList::DISP_FORMAT_NAME_YPBPR_480P] = DISP_FORMAT_YPBPR_480P;
    mName2CodeMap[IDispList::DISP_FORMAT_NAME_YPBPR_576I] = DISP_FORMAT_YPBPR_576I;
    mName2CodeMap[IDispList::DISP_FORMAT_NAME_YPBPR_576P] = DISP_FORMAT_YPBPR_576P;
    mName2CodeMap[IDispList::DISP_FORMAT_NAME_YPBPR_720P_50HZ] = DISP_FORMAT_YPBPR_720P_50HZ;
    mName2CodeMap[IDispList::DISP_FORMAT_NAME_YPBPR_720P_60HZ] = DISP_FORMAT_YPBPR_720P_60HZ;
    mName2CodeMap[IDispList::DISP_FORMAT_NAME_YPBPR_1080P_50HZ] = DISP_FORMAT_YPBPR_1080P_60HZ;
    mName2CodeMap[IDispList::DISP_FORMAT_NAME_YPBPR_1080P_60HZ] = DISP_FORMAT_YPBPR_1080P_50HZ;
    mName2CodeMap[IDispList::DISP_FORMAT_NAME_HDMI_480I] = DISP_FORMAT_HDMI_480I;
    mName2CodeMap[IDispList::DISP_FORMAT_NAME_HDMI_480P] = DISP_FORMAT_HDMI_480P;
    mName2CodeMap[IDispList::DISP_FORMAT_NAME_HDMI_576I] = DISP_FORMAT_HDMI_576I;
    mName2CodeMap[IDispList::DISP_FORMAT_NAME_HDMI_576P] = DISP_FORMAT_HDMI_576P;
    mName2CodeMap[IDispList::DISP_FORMAT_NAME_HDMI_720P_50HZ] = DISP_FORMAT_HDMI_720P_50HZ;
    mName2CodeMap[IDispList::DISP_FORMAT_NAME_HDMI_720P_60HZ] = DISP_FORMAT_HDMI_720P_60HZ;
    mName2CodeMap[IDispList::DISP_FORMAT_NAME_HDMI_1080I_50HZ] = DISP_FORMAT_HDMI_1080I_50HZ;
    mName2CodeMap[IDispList::DISP_FORMAT_NAME_HDMI_1080I_60HZ] = DISP_FORMAT_HDMI_1080I_60HZ;
    mName2CodeMap[IDispList::DISP_FORMAT_NAME_HDMI_1080P_24HZ] = DISP_FORMAT_HDMI_1080P_24HZ;
    mName2CodeMap[IDispList::DISP_FORMAT_NAME_HDMI_1080P_50HZ] = DISP_FORMAT_HDMI_1080P_50HZ;
    mName2CodeMap[IDispList::DISP_FORMAT_NAME_HDMI_1080P_60HZ] = DISP_FORMAT_HDMI_1080P_60HZ;
    mName2CodeMap[IDispList::DISP_FORMAT_NAME_VGA_640_480] = DISP_FORMAT_VGA_640_480;
    mName2CodeMap[IDispList::DISP_FORMAT_NAME_VGA_800_600] = DISP_FORMAT_VGA_800_600;
    mName2CodeMap[IDispList::DISP_FORMAT_NAME_VGA_1024_768] = DISP_FORMAT_VGA_1024_768;
    mName2CodeMap[IDispList::DISP_FORMAT_NAME_VGA_1280_720] = DISP_FORMAT_VGA_1280_720;
    mName2CodeMap[IDispList::DISP_FORMAT_NAME_VGA_1280_1024] = DISP_FORMAT_VGA_1280_1024;
    mName2CodeMap[IDispList::DISP_FORMAT_NAME_VGA_1360_768] = DISP_FORMAT_VGA_1360_768;
    mName2CodeMap[IDispList::DISP_FORMAT_NAME_VGA_1440_900] = DISP_FORMAT_VGA_1440_900;
    mName2CodeMap[IDispList::DISP_FORMAT_NAME_VGA_1680_1050] = DISP_FORMAT_VGA_1680_1050;
    mName2CodeMap[IDispList::DISP_FORMAT_NAME_VGA_1920_1080] = DISP_FORMAT_VGA_1920_1080;
    return NOERROR;
}

}// namespace View
}// namespace Droid
}// namespace Elastos
