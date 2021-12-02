#include "tlgrm.h"
#include <string>
#include <nlohmann/json.hpp>
#include <wx\debug.h>
#define CURL_STATICLIB
#include "curl/curl.h"
#include "credentials.h"

using namespace nlohmann;
using std::string;

namespace tlgrm
{
	//TODO: MB: hack, bcs I don't know how to through data to CURL callbacks
	optional<size_t> retChatId = {};
	optional<string> findingTlgrmLogin = {};

	bool hasKey(const nlohmann::json& jsonObj, const std::string& key)
	{
		return jsonObj.find(key) != jsonObj.end();
	}
	const std::string ok = "ok";
	const std::string result = "result";
	const std::string login = "username";
	const std::string updateId = "update_id";
	const std::string message = "message";
	const std::string messageId = "message_id";
	const std::string id = "id";
	const std::string chat = "chat";
	const std::string from = "from";

	void UpdateChatIdFromRawJson(const char* rawJsonData)
	{
		using namespace nlohmann;
		retChatId = {};
		// just a place for the cast

	//	size_t        rc = 0;
	//	std::string* stp = reinterpret_cast<std::string*>(userdata);
		  // construct the JSON root object
		const json jsonObj{ json::parse(rawJsonData) };

		if (!tlgrm::hasKey(jsonObj, tlgrm::ok) || !jsonObj[tlgrm::ok].get<bool>())
		{
			return;
		}
		if (!tlgrm::hasKey(jsonObj, tlgrm::result))
		{
			return;
		}
		const auto resultArray{ jsonObj[tlgrm::result] };
		if (!resultArray.is_array())
		{
		}
		for (const auto& resultElem : resultArray)
		{
			wxASSERT(tlgrm::hasKey(resultElem, tlgrm::updateId));
			wxASSERT(tlgrm::hasKey(resultElem, tlgrm::message));
			const auto& messageNode{ resultElem[tlgrm::message] };
			wxASSERT(tlgrm::hasKey(messageNode, tlgrm::chat));
			const auto& chatNode{ messageNode[tlgrm::chat] };
			wxASSERT(tlgrm::findingTlgrmLogin.has_value());
			if (tlgrm::hasKey(chatNode, tlgrm::login) && chatNode[tlgrm::login] == findingTlgrmLogin.value())
			{
				wxASSERT(tlgrm::hasKey(chatNode, tlgrm::id));
				const auto& chatIdNode{ chatNode[tlgrm::id] };
				wxASSERT(chatIdNode.is_number_unsigned());
				retChatId = chatIdNode.get<size_t>();
				bool test = false;
			}
		}
	}

	size_t tlgrmReadCallback(char* data, size_t size, size_t nmemb, void* up)
	{
		const auto retVal = size * nmemb;
		UpdateChatIdFromRawJson(data);
		return retVal;
	}
}





namespace tlgrm 
{
	optional<int> getChatId(const std::string& tlgrmLogin)
	{
		curl_global_init(CURL_GLOBAL_ALL);
		if (CURL* curl = curl_easy_init())
		{
			curl_easy_setopt(curl, CURLOPT_URL, credentials::telegramUpdateUrl.c_str());
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, tlgrmReadCallback);
			curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "http");
			//curl_easy_setopt(curl, CUROPT_READDATA, )
			//struct curl_slist* headers = NULL;
			//curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
			//curl_easy_setopt(curl, CURLOPT_HOST, "LocalHost");

			std::string readBuffer;
			char buffer[10000] = {};

			tlgrm::findingTlgrmLogin = tlgrmLogin;

			const CURLcode res{ curl_easy_perform(curl) };
			wxASSERT(res == CURLcode::CURLE_OK);

			curl_easy_cleanup(curl);
		}
		findingTlgrmLogin = {};
		return retChatId;
	}
}