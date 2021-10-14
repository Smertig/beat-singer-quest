#include "lyrics_loader.h"

#include <bs/utils/strings.h>
#include <bs/utils/logger.h>
#include <bs/utils/config.h>

#include <extern/beatsaber-hook/shared/utils/il2cpp-utils.hpp>

#include <System/Action_1.hpp>
#include <UnityEngine/Networking/DownloadHandler.hpp>
#include <UnityEngine/Networking/UnityWebRequest.hpp>
#include <UnityEngine/Networking/UnityWebRequestAsyncOperation.hpp>

#include <functional>

// TODO: remove after updating to beatsaber-hook 2.3.2
template <class T, class F>
T MakeDelegate(F&& f) {
    return il2cpp_utils::MakeDelegate<T>(classof(T), std::forward<F>(f));
}

void loadLyrics(std::string_view song, std::string_view artist, std::function<void(std::optional<std::string>)> callback) {
    using UnityEngine::Networking::UnityWebRequest;

    std::string qTrack  = utils::to_utf8(UnityWebRequest::EscapeURL(utils::to_il2cpp(song)));
    std::string qArtist = utils::to_utf8(UnityWebRequest::EscapeURL(utils::to_il2cpp(artist)));

    std::string url = string_format(
            "https://apic-desktop.musixmatch.com/ws/1.1/macro.subtitles.get?"
            "format=json&"
            "q_track=%s&"
            "q_artist=%s&"
            "user_language=en&"
            "userblob_id=aG9va2VkIG9uIGEgZmVlbGluZ19ibHVlIHN3ZWRlXzE3Mg&"
            "subtitle_format=mxm&"
            "app_id=web-desktop-app-v1.0&"
            "usertoken=%s",
            qTrack.c_str(), qArtist.c_str(), config::getMusicMatchAPIToken().c_str()
    );

    auto req = UnityWebRequest::Get(utils::to_il2cpp(url));
    getLogger().debug("web-request: %p", req);
    CRASH_UNLESS(req);

    req->SetRequestHeader(utils::to_il2cpp("Cookie"), utils::to_il2cpp("x-mxm-token-guid=cd25ed55-85ea-445b-83cd-c4b173e20ce7"));

    auto async_op = req->SendWebRequest();
    getLogger().info("async operation: %p", async_op);
    CRASH_UNLESS(async_op);

    std::function lambda = [callback = std::move(callback)](UnityEngine::AsyncOperation* raw_op) {
        auto op = static_cast<UnityEngine::Networking::UnityWebRequestAsyncOperation*>(raw_op);

        getLogger().info("Async operation completed. op: %p", op);

        auto req = op->webRequest;
        getLogger().info("Inside async: req: %p", req);

        getLogger().info("Response code: %d", static_cast<int>(req->get_responseCode()));
        getLogger().info("Progress: %f", req->get_downloadProgress());

        auto text = req->get_downloadHandler()->get_text();
        if (text) {
            callback(utils::to_utf8(text));
        }
        else {
            callback(std::nullopt);
        }
    };

    auto complete_callback = MakeDelegate<System::Action_1<UnityEngine::AsyncOperation*>*>(lambda);
    async_op->add_completed(complete_callback);
}
