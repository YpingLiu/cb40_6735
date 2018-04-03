#ifndef ANDROID_WEBVIEW_NATIVE_PASSWORD_HALDLER_H_
#define ANDROID_WEBVIEW_NATIVE_PASSWORD_HALDLER_H_

#include <vector>

#include <jni.h>
#include "base/memory/scoped_ptr.h"
#include "components/autofill/core/common/password_form_fill_data.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_observer.h"
#include "content/public/browser/web_contents_user_data.h"
#include "components/autofill/core/common/password_form.h"
#include "android_webview/native/password_handler_delegate.h"

namespace content {
class WebContents;
}

namespace android_webview {

class PasswordHandler
    : public content::WebContentsObserver,
      public content::WebContentsUserData<PasswordHandler> {

public:
    static void CreateForWebContentsAndDelegate(
        content::WebContents* contents,
        PasswordHandlerDelegate* delegate);

    virtual ~PasswordHandler();

    bool IsEnabledSavePassword();
    // content::WebContentsObserver overrides.
    virtual void DidNavigateAnyFrame(
        const content::LoadCommittedDetails& details,
        const content::FrameNavigateParams& params) OVERRIDE;
    virtual bool OnMessageReceived(const IPC::Message& message) OVERRIDE;

  void OnPasswordFormsParsed(
      const std::vector<autofill::PasswordForm>& forms);
  void OnPasswordFormsRendered(
      const std::vector<autofill::PasswordForm>& visible_forms,
      bool did_stop_loading);
  void OnPasswordFormSubmitted(
      const autofill::PasswordForm& password_form);
  void OnGetRequestedData(
        const autofill::PasswordFormFillData& data);
protected:
    PasswordHandler(content::WebContents* web_contents,
                    PasswordHandlerDelegate* delegate);

private:
    enum SaveOption {
        kCanNot = 0,
        kRequesting,
        kData,
        kUpdate,
        kNewOne
    };

    content::WebContents* web_contents_;
    PasswordHandlerDelegate* delegate_;
    SaveOption can_save;
    scoped_ptr<autofill::PasswordFormFillData> requested_data;
    scoped_ptr<autofill::PasswordForm> saved_data;
}; // class PasswordHandler

} // namespace content

#endif  // ANDROID_WEBVIEW_NATIVE_PASSWORD_HALDLER_H_
