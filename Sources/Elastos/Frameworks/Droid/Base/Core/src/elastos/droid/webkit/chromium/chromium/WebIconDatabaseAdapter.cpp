
namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Chromium {
namespace Chromium {

//@Override
ECode WebIconDatabaseAdapter::Open(
    /* [in] */ const String& path)
{
    AwContents::SetShouldDownloadFavicons();
    return NOERROR;
}

//@Override
ECode WebIconDatabaseAdapter::Close()
{
    // Intentional no-op.
    return NOERROR;
}

//@Override
ECode WebIconDatabaseAdapter::RemoveAllIcons()
{
    // Intentional no-op: we have no database so nothing to remove.
    return NOERROR;
}

//@Override
ECode WebIconDatabaseAdapter::RequestIconForPageUrl(
    /* [in] */ const String& url,
    /* [in] */ IIconListener* listener)
{
    // Intentional no-op.
    return NOERROR;
}

//@Override
ECode WebIconDatabaseAdapter::BulkRequestIconForPageUrl(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& where,
    /* [in] */ IIconListener* listener)
{
    // Intentional no-op: hidden in base class.
    return NOERROR;
}

//@Override
ECode WebIconDatabaseAdapter::RetainIconForPageUrl(
    /* [in] */ const String& url)
{
    // Intentional no-op.
    return NOERROR;
}

//@Override
ECode WebIconDatabaseAdapter::ReleaseIconForPageUrl(
    /* [in] */ const String& url)
{
    // Intentional no-op.
    return NOERROR;
}

} // namespace Chromium
} // namespace Chromium
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
