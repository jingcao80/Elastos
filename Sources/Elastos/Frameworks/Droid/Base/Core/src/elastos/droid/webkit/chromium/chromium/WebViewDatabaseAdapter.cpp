
namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Chromium {
namespace Chromium {

WebViewDatabaseAdapter::WebViewDatabaseAdapter(
    /* [in] */ AwFormDatabase* formDatabase,
    /* [in] */ HttpAuthDatabase* httpAuthDatabase)
    : mFormDatabase(formDatabase)
    , mHttpAuthDatabase(httpAuthDatabase)
{
}

//@Override
ECode WebViewDatabaseAdapter::HasUsernamePassword(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // This is a deprecated API: intentional no-op.
    *result = FALSE;
    return NOERROR;
}

//@Override
ECode WebViewDatabaseAdapter::ClearUsernamePassword()
{
    // This is a deprecated API: intentional no-op.
    return NOERROR;
}

//@Override
ECode WebViewDatabaseAdapter::HasHttpAuthUsernamePassword(
    /* [out] */ Boolean* result)
{
#if 0
    return mHttpAuthDatabase.hasHttpAuthUsernamePassword();
#endif
    return E_NOT_IMPLEMENTED;
}

//@Override
ECode WebViewDatabaseAdapter::ClearHttpAuthUsernamePassword()
{
    mHttpAuthDatabase->ClearHttpAuthUsernamePassword();
    return NOERROR;
}

//@Override
ECode WebViewDatabaseAdapter::HasFormData(
    /* [out] */ Boolean* result)
{
#if 0
    return mFormDatabase.hasFormData();
#endif
    return E_NOT_IMPLEMENTED;
}

//@Override
ECode WebViewDatabaseAdapter::ClearFormData()
{
    mFormDatabase->ClearFormData();
    return NOERROR;
}

} // namespace Chromium
} // namespace Chromium
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
