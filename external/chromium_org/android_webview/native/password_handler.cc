#include "android_webview/native/password_handler.h"

#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/threading/platform_thread.h"
#include "components/autofill/content/common/autofill_messages.h"
#include "components/autofill/core/common/password_autofill_util.h"
#include "android_webview/native/password_handler_delegate.h"
#include "content/public/browser/navigation_entry.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/frame_navigate_params.h"
#include "net/cert/cert_status_flags.h"

using content::WebContents;
using autofill::PasswordForm;

DEFINE_WEB_CONTENTS_USER_DATA_KEY(android_webview::PasswordHandler);

namespace android_webview {

const char cSpdyProxyRealm[] = "/SpdyProxy";

//static
void PasswordHandler::CreateForWebContentsAndDelegate(
        content::WebContents* contents,
        PasswordHandlerDelegate* delegate) {
    LOG(WARNING) << "create-->contents = "
                 << contents
                 << ", delegate = "
                 << delegate;
    if (FromWebContents(contents)) {
        return;
    }

    LOG(WARNING) << "attaching to web_contents ";
    contents->SetUserData(UserDataKey(),
                          new PasswordHandler(contents, delegate));
}

PasswordHandler::PasswordHandler(content::WebContents* web_contents,
                    PasswordHandlerDelegate* delegate)
    : content::WebContentsObserver(web_contents),
      web_contents_(web_contents),
      delegate_(delegate),
      can_save(kCanNot){
}

PasswordHandler::~PasswordHandler() {}

bool PasswordHandler::IsEnabledSavePassword() {
    if (delegate_ == NULL) {
        LOG(WARNING) << "OnPasswordFormsRendered delegate is null!!";
        return false;
    }
    return delegate_->IsSavePasswordEnabled();
}

void PasswordHandler::DidNavigateAnyFrame(
      const content::LoadCommittedDetails& details,
      const content::FrameNavigateParams& params) {
}

bool PasswordHandler::OnMessageReceived(const IPC::Message& message) {
    bool handled = true;
    IPC_BEGIN_MESSAGE_MAP(PasswordHandler, message)
        IPC_MESSAGE_HANDLER(AutofillHostMsg_PasswordFormsParsed,
                            OnPasswordFormsParsed)
        IPC_MESSAGE_HANDLER(AutofillHostMsg_PasswordFormsRendered,
                            OnPasswordFormsRendered)
        IPC_MESSAGE_HANDLER(AutofillHostMsg_PasswordFormSubmitted,
                            OnPasswordFormSubmitted)
        IPC_MESSAGE_UNHANDLED(handled = false)
    IPC_END_MESSAGE_MAP()
    return handled;
}

void PasswordHandler::OnPasswordFormSubmitted(
    const PasswordForm& password_form) {
    LOG(WARNING) << "OnPasswordFormSubmitted can_save = "
                 << can_save;
    // setting off
    if (!IsEnabledSavePassword()) {
        LOG(WARNING) << "OnPasswordFormSubmitted IsEnabledSavePassword is false!!";
        can_save = kCanNot;
        return;
    }
    // Try to save the password if one was submitted.
    if (password_form.origin.is_valid()) {
        // invalid param
        if (password_form.password_value.empty()) {
            LOG(WARNING) << "OnPasswordFormSubmitted password_value is empty!!";
            can_save = kCanNot;
            return;
        }
        // auto-complete attribute
        bool ret = autofill::ShouldIgnoreAutocompleteOffForPasswordFields();
        LOG(WARNING) << "OnPasswordFormSubmitted password_autocomplete_set switch = "
                     << (ret == true ? 1 : 0);
        if (!ret && !password_form.password_autocomplete_set) {
            LOG(WARNING) << "OnPasswordFormSubmitted password_autocomplete_set is false!!";
            can_save = kCanNot;
            return;
        }
    } else {
        LOG(WARNING) << "OnPasswordFormSubmitted password is not submitted!!";
        can_save = kCanNot;
        return;
    }
    LOG(WARNING) << "OnPasswordFormSubmitted can_save = "
                 << can_save;
    if (can_save == kRequesting) {
        saved_data->username_value = password_form.username_value;
        saved_data->password_value = password_form.password_value;
        can_save = kNewOne;
        return;
    }
    LOG(WARNING) << "OnPasswordFormSubmitted user element = "
                 << password_form.username_element;
    LOG(WARNING) << "OnPasswordFormSubmitted pwd element = "
                 << password_form.password_element;
    if (can_save == kData) {
        // never remember
        if (requested_data->basic_data.fields[0].value.empty()) {
            LOG(WARNING) << "OnPasswordFormSubmitted never remember!!";
            can_save = kCanNot;
            return;
        }
        if (requested_data->basic_data.fields[0].value == 
                password_form.username_value) {
            LOG(WARNING) << "OnPasswordFormSubmitted update item username = "
                         << password_form.username_value;
            saved_data->username_value = password_form.username_value;
            saved_data->password_value = password_form.password_value;
            can_save = kUpdate;
            return;
        }
        LOG(WARNING) << "OnPasswordFormSubmitted new item username = "
                     << password_form.username_value;
        saved_data->username_value = password_form.username_value;
        saved_data->password_value = password_form.password_value;
        can_save = kNewOne;
    }
}

void PasswordHandler::OnGetRequestedData(
        const autofill::PasswordFormFillData& data) {
    LOG(WARNING) << "OnGetRequestedData request password returned";
    requested_data.reset(new autofill::PasswordFormFillData(data));
    if (saved_data.get() == NULL) {
        return;
    }
    saved_data->username_value = requested_data->basic_data.fields[0].value;
    saved_data->password_value = requested_data->basic_data.fields[1].value;
    if (requested_data->basic_data.fields[0].value.empty()) {
        LOG(WARNING) << "OnGetRequestedData never remember!!";
        can_save = kCanNot;
    } else {
        can_save = kData;
    }
}

void PasswordHandler::OnPasswordFormsParsed(
    const std::vector<PasswordForm>& forms) {
    LOG(WARNING) << "OnPasswordFormsParsed called";
    if (!IsEnabledSavePassword()) {
        LOG(WARNING) << "OnPasswordFormsParsed IsEnabledSavePassword is false";
        return;
    }
    // Ask the SSLManager for current security.
    //bool had_ssl_error = delegate_->DidLastPageLoadEncounterSSLErrors();
    PasswordHandlerDelegate::GetRequestedData callBack = base::Bind(
        &PasswordHandler::OnGetRequestedData, base::Unretained(this));

    for (std::vector<PasswordForm>::const_iterator iter = forms.begin();
         iter != forms.end(); ++iter) {
        // Don't involve the password manager if this form corresponds to
        // SpdyProxy authentication, as indicated by the realm.
        LOG(WARNING) << "requesting data16.. <"
                     << iter->username_element
                     << ">, <"
                     << iter->username_value
                     << ">, <"
                     << iter->password_element
                     << ">, <"
                     << iter->password_value;
        LOG(WARNING) << "requesting data.. <"
                     << iter->signon_realm
                     << ">, <"
                     << iter->original_signon_realm
                     << ">, <"
                     << iter->origin
                     << ">, <"
                     << iter->action;
        if (EndsWith(iter->signon_realm, cSpdyProxyRealm, true)) {
            continue;
        }
        can_save = kRequesting;
        saved_data.reset(new autofill::PasswordForm(*iter));
        delegate_->GetPasswordRequest(*iter, callBack);
    }
}

void PasswordHandler::OnPasswordFormsRendered(
    const std::vector<PasswordForm>& visible_forms,
    bool did_stop_loading) {
    LOG(WARNING) << "OnPasswordFormsRendered called";
    if (!IsEnabledSavePassword()) {
        LOG(WARNING) << "OnPasswordFormsRendered IsEnabledSavePassword is false";
        return;
    }
    LOG(WARNING) << "OnPasswordFormsRendered can_save = "
                 << can_save;
    // changing login info when remember, should not save the old info before
    // the new navigation arrived. remove (can_save == kData)
    if (can_save == kUpdate || can_save == kNewOne) {
        delegate_->AddSavePasswordPopupIfPermitted(
            *(saved_data.get()), 
            (can_save == kNewOne ? true : false));
        saved_data.reset();
        can_save = kCanNot;
    }
}

} // namespace
