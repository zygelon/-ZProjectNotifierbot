#include "tlgrm.h"
#include "xlog.h"
#include <nlohmann/json.hpp>
#include <wx\debug.h>
#define CURL_STATICLIB
#include "curl/curl.h"
#include "credentials.h"
#include <wx/string.h>

using namespace nlohmann;

//https://api.telegram.org/bot{token}/getUpdates

namespace tlgrm
{
	using std::string;
	namespace wconst
	{
		const std::string getUpdatesStr = "getUpdates";
		const std::string tlgrmApiUrl = "https://api.telegram.org/";
		const std::string sendMessage{ "sendMessage" };

		const std::string ok{ "ok" };
		const std::string result = "result";
		const std::string login = "username";
		const std::string updateId = "update_id";
		const std::string message = "message";
		const std::string messageId = "message_id";
		const std::string id = "id";
		const std::string chat = "chat";
		const std::string text = "text";
		const std::string nextArgComma = ", ";
		const std::string equal = "=";
	}
	//TODO: MB: hack, bcs I don't know how to through data to CURL callbacks
	optional<size_t> retChatId = {};
	optional<std::string> findingTlgrmLogin = {};
	bool isCurlInitilized = false;

	bool hasKey(const nlohmann::json& jsonObj, const std::string& key)
	{
		return jsonObj.find(key) != jsonObj.end();
	}

	void initCurlIfNeeded()
	{
		if (!isCurlInitilized)
		{
			isCurlInitilized = true;
			curl_global_init(CURL_GLOBAL_ALL);
		}
	}

	void updateChatIdFromRawJson(const char* rawJsonData)
	{
		using namespace nlohmann;
		const wxString logPrefix = L"Telegram getUpdates ";
		initCurlIfNeeded();

		if (!findingTlgrmLogin.has_value())
		{
			const wxString warningMessage{ logPrefix + wxString{L"finding login is empty"} };
			xlog(ELogType::warning, warningMessage);
			return;
		}
		retChatId = {};
		// just a place for the cast

		const json jsonObj{ json::parse(rawJsonData) };
		auto jdebug = jsonObj[wconst::ok];
		if (!tlgrm::hasKey(jsonObj, wconst::ok) || !jsonObj[wconst::ok].get<bool>())
		{
			const wxString warningMessage{ logPrefix + wxString{L"does not have 'ok'"} };
			xlog(ELogType::warning, warningMessage);
			return;
		}
		if (!tlgrm::hasKey(jsonObj, wconst::result))
		{
			xlog(ELogType::warning, logPrefix + wxString{ L"does not have 'result'" });
			return;
		}
		const auto resultArray{ jsonObj[wconst::result] };
		if (!resultArray.is_array())
		{
			const wxString warningMessage = logPrefix + wxString{ " Result array is empty" };
			xlog(ELogType::warning, warningMessage);
			return;
		}
		for (const auto& resultElem : resultArray)
		{
			wxASSERT(tlgrm::hasKey(resultElem, wconst::updateId));
			wxASSERT(tlgrm::hasKey(resultElem, wconst::message));
			const auto& messageNode{ resultElem[wconst::message] };
			wxASSERT(tlgrm::hasKey(messageNode, wconst::chat));
			const auto& chatNode{ messageNode[wconst::chat] };
			wxASSERT(tlgrm::findingTlgrmLogin.has_value());
			wxASSERT(tlgrm::findingTlgrmLogin.has_value());

			const std::string tlgrmLogin{ findingTlgrmLogin.value() };
			if (tlgrm::hasKey(chatNode, wconst::login) && chatNode[wconst::login] == tlgrmLogin)
			{
				wxASSERT(tlgrm::hasKey(chatNode, wconst::id));
				const auto& chatIdNode{ chatNode[wconst::id] };
				wxASSERT(chatIdNode.is_number_unsigned());

				retChatId = chatIdNode.get<size_t>();

				const auto logMessage{wxString::Format(wxT("%sfound chat with login = %s, chat_id = %i"), logPrefix, tlgrmLogin, retChatId.value())};
				xlog(ELogType::info, logMessage);
			}
		}
	}

	size_t tlgrmReadCallback(char* data, size_t size, size_t nmemb, void* up)
	{
		const auto retVal = size * nmemb;
		updateChatIdFromRawJson(data);
		return retVal;
	}
}

namespace tlgrm
{
	optional<int> getChatId(const std::string& tlgrmLogin)
	{
		initCurlIfNeeded();
		if (CURL* curl = curl_easy_init())
		{
			const auto& url = wconst::tlgrmApiUrl + credentials::botToken + wconst::getUpdatesStr;
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			//curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, tlgrmReadCallback);
			//curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "http");

			tlgrm::findingTlgrmLogin = tlgrmLogin;

			const CURLcode res{ curl_easy_perform(curl) };
			const wxString logPrefix = L"getChatId ";
			if (res == CURLcode::CURLE_OK)
			{
				xlog(ELogType::info, logPrefix + L" Http responce OK");
			}
			else
			{
				xlog(ELogType::warning, logPrefix + "http responce with error");
			}
			curl_easy_cleanup(curl);
		}
		findingTlgrmLogin.reset();
		return retChatId;
	}

	size_t tlgrmReadCallbackDebug(char* data, size_t size, size_t nmemb, void* up)
	{
		const auto retVal = size * nmemb;
		return retVal;
	}

	//Hack, if curl does not work
	void systemSendMessage(const std::string& message, const size_t chatId)
	{
		wxASSERT(false);
		const auto& urlBase = wconst::tlgrmApiUrl + credentials::botToken + wconst::sendMessage;
		const auto& urlParams = "?chat_id=" + std::to_string(chatId) + "&text=" + message;
		const auto& curlUrl = urlBase + urlParams;
		const auto& curlUrl2 = '"' + curlUrl + '"';
		//xlog(ELogType::info, curlUrl2);
		system((std::string("curl ") + curlUrl2).c_str());
	}

	bool sendMessage(const std::string& message, const size_t chatId)
	{
		initCurlIfNeeded();
		const auto& urlBase = wconst::tlgrmApiUrl + credentials::botToken + wconst::sendMessage;
		const auto& urlParams = "?chat_id=" + std::to_string(chatId) + "&text=" + message;
		auto curlUrl =  urlBase + urlParams;
		//
		std::replace(curlUrl.begin(), curlUrl.end(), ' ', '+');
		if (CURL* curl = curl_easy_init())
		{
			curl_easy_setopt(curl, CURLOPT_URL, curlUrl.c_str());
			
			const CURLcode res{ curl_easy_perform(curl) };
			wxASSERT(res == CURLcode::CURLE_OK);
			curl_easy_cleanup(curl);
			return res == CURLcode::CURLE_OK;
		};
		return false;
	}
}
