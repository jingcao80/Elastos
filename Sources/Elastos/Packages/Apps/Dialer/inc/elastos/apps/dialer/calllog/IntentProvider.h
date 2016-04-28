
#ifndef __ELASTOS_APPS_DIALER_CALLLOG_INTENTPROVIDER_H__
#define __ELASTOS_APPS_DIALER_CALLLOG_INTENTPROVIDER_H__

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace CallLog {

/**
 * Used to create an intent to attach to an action in the call log.
 * <p>
 * The intent is constructed lazily with the given information.
 */
class IntentProvider
    : public Object
    , public IIntentProvider
{
private:
    class ReturnCallIntentProvider
        : public Object
        , public IIntentProvider
    {
    public:
        ReturnCallIntentProvider(
            /* [in] */ const String& number,
            /* [in] */ IPhoneAccountHandle* accountHandle);

        CARAPI GetIntent(
            /* [in] */ IContext* context,
            /* [out] */ IIntent** intent);

    private:
        String mNumber;
        AutoPtr<IPhoneAccountHandle> mAccountHandle;
    };

    class ReturnVideoCallIntentProvider
        : public Object
        , public IIntentProvider
    {
    public:
        ReturnVideoCallIntentProvider(
            /* [in] */ const String& number,
            /* [in] */ IPhoneAccountHandle* accountHandle);

        CARAPI GetIntent(
            /* [in] */ IContext* context,
            /* [out] */ IIntent** intent);

    private:
        String mNumber;
        AutoPtr<IPhoneAccountHandle> mAccountHandle;
    };

    class PlayVoicemailIntentProvider
        : public Object
        , public IIntentProvider
    {
    public:
        PlayVoicemailIntentProvider(
            /* [in] */ Int64 rowId,
            /* [in] */ const String& voicemailUri);

        CARAPI GetIntent(
            /* [in] */ IContext* context,
            /* [out] */ IIntent** intent);
    private:
        Int64 mRowId;
        String mVoicemailUri;
    };

    class CallDetailIntentProvider
        : public Object
        , public IIntentProvider
    {
    public:
        CallDetailIntentProvider(
            /* [in] */ Int64 id,
            /* [in] */ ArrayOf<Int64>* extraIds,
            /* [in] */ const String& voicemailUri);

        CARAPI GetIntent(
            /* [in] */ IContext* context,
            /* [out] */ IIntent** intent);
    private:
        Int64 mId;
        AutoPtr<ArrayOf<Int64> > mExtraIds;
        String mVoicemailUri;
    };

public:

    static CARPAI_(AutoPtr<IIntentProvider>) GetReturnCallIntentProvider(
        /* [in] */ const String& number);

    static CARPAI_(AutoPtr<IIntentProvider>) GetReturnCallIntentProvider(
        /* [in] */ const String& number,
        /* [in] */ IPhoneAccountHandle* accountHandle);

    static CARPAI_(AutoPtr<IIntentProvider>) GetReturnVideoCallIntentProvider(
        /* [in] */ const String& number);

    static CARPAI_(AutoPtr<IIntentProvider>) GetReturnVideoCallIntentProvider(
        /* [in] */ const String& number,
        /* [in] */ IPhoneAccountHandle* accountHandle);

    static CARPAI_(AutoPtr<IIntentProvider>) GetPlayVoicemailIntentProvider(
        /* [in] */ Int64 rowId,
        /* [in] */ const String& voicemailUri);

    /**
     * Retrieves the call details intent provider for an entry in the call log.
     *
     * @param id The call ID of the first call in the call group.
     * @param extraIds The call ID of the other calls grouped together with the call.
     * @param voicemailUri If call log entry is for a voicemail, the voicemail URI.
     * @return The call details intent provider.
     */
    static AutoPtr<IIntentProvider> GetCallDetailIntentProvider(
        /* [in] */ Int64 id,
        /* [in] */ ArrayOf<Int64>* extraIds,
        /* [in] */ const String& voicemailUri);

private:
    static const String TAG; // = IntentProvider.class.getSimpleName();
};

} // CallLog
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CALLLOG_INTENTPROVIDER_H__
