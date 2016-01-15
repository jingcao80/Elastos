
namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Chromium {
namespace Chromium {

//===============================================================
//             WebStorageAdapter::InnerValueCallback
//===============================================================

WebStorageAdapter::InnerValueCallback::InnerValueCallback(
    /* [in] */ WebStorageAdapter* owner,
    /* [in] */ const IValueCallback* callback)
    : mOwner(owner)
    , mCallback(callback)
{
}

ECode WebStorageAdapter::InnerValueCallback::OnReceiveValue(
    /* [in] */ IInterface* value)
{
    Map<String, Origin> originsMap = new HashMap<String, Origin>();
    for (int i = 0; i < origins.mOrigins.length; ++i) {
        Origin origin = new Origin(origins.mOrigins[i], origins.mQuotas[i],
                origins.mUsages[i]) {
            // Intentionally empty to work around cross-package protected visibility
            // of Origin constructor.
        };
        originsMap.put(origins.mOrigins[i], origin);
    }
    callback.onReceiveValue(originsMap);
    return NOERROR;
}

//===============================================================
//                     WebStorageAdapter
//===============================================================

WebStorageAdapter::WebStorageAdapter(
    /* [in] */ AwQuotaManagerBridge* quotaManagerBridge)
    : mQuotaManagerBridge(quotaManagerBridge)
{
}

//@Override
ECode WebStorageAdapter::GetOrigins(
    /* [in] */ const IValueCallback* callback)
{
    AutoPtr<IValueCallback> valueCallback = new InnerValueCallback(this, callback);
    mQuotaManagerBridge->GetOrigins(valueCallback);

    return NOERROR;
}

//@Override
ECode WebStorageAdapter::GetUsageForOrigin(
    /* [in] */ const String& origin,
    /* [in] */ IValueCallback* callback)
{
    mQuotaManagerBridge->GetUsageForOrigin(origin, callback);
    return NOERROR;
}

//@Override
ECode WebStorageAdapter::GetQuotaForOrigin(
    /* [in] */ const String& origin,
    /* [in] */ IValueCallback* callback)
{
    mQuotaManagerBridge->GetQuotaForOrigin(origin, callback);
    return NOERROR;
}

//@Override
ECode WebStorageAdapter::SetQuotaForOrigin(
    /* [in] */ const String& origin,
    /* [in] */ Int64 quota)
{
    // Intentional no-op for deprecated method.
    return NOERROR;
}

//@Override
ECode WebStorageAdapter::DeleteOrigin(
    /* [in] */ const String& origin)
{
    mQuotaManagerBridge->DeleteOrigin(origin);
    return NOERROR;
}

//@Override
ECode WebStorageAdapter::DeleteAllData()
{
    mQuotaManagerBridge->DeleteAllData();
    return NOERROR;
}

} // namespace Chromium
} // namespace Chromium
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
