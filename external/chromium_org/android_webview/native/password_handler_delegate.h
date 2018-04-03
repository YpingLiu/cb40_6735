#ifndef ANDROID_WEBVIEW_NATIVE_PASSWORD_HALDLER_DELEGATE_H_
#define ANDROID_WEBVIEW_NATIVE_PASSWORD_HALDLER_DELEGATE_H_

#include "base/callback_forward.h"

namespace autofill {
struct PasswordFormFillData;
struct PasswordForm;
}  // namespace autofill

namespace android_webview {

class PasswordHandlerDelegate {
public:
    PasswordHandlerDelegate() {}
    virtual ~PasswordHandlerDelegate() {}

    typedef base::Callback<void(const autofill::PasswordFormFillData&)> GetRequestedData;

    // Fill forms matching |form_data| in |web_contents|.  By default, goes
    // through the RenderViewHost to FillPasswordForm.  Tests can override this
    // to sever the dependency on the entire rendering stack.
    virtual void FillPasswordForm(
        const autofill::PasswordFormFillData& form_data) = 0;

    // A mechanism to show an popup window in the current tab at our request.
    virtual void AddSavePasswordPopupIfPermitted(
                    const autofill::PasswordForm& info, bool permitted) = 0;

    // To request the saved password
    virtual void GetPasswordRequest(
                    const autofill::PasswordForm& info, 
                    const GetRequestedData& callback) = 0;

    // If any SSL certificate errors were encountered as a result of the last
    // page load.
    virtual bool DidLastPageLoadEncounterSSLErrors() = 0;

    // To check whether save password setting is enabled.
    virtual bool IsSavePasswordEnabled() = 0;
};

}

#endif  // ANDROID_WEBVIEW_NATIVE_PASSWORD_HALDLER_DELEGATE_H_

