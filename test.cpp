#include <iostream>
#include <string>
#include <curl/curl.h>
#include <json/json.h> // Assuming you have a JSON library, like JsonCpp

class DeribitAPI {
private:
    std::string apiKey;
    std::string apiSecret;
    std::string baseUrl = "https://test.deribit.com/api/v2/";

    std::string makeRequest(const std::string& endpoint, const std::string& method, const std::string& payload) {
        CURL* curl;
        CURLcode res;
        std::string readBuffer;

        curl = curl_easy_init();
        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, (baseUrl + endpoint).c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, nullptr); // Set headers if needed
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
        }
        return readBuffer;
    }

public:
    DeribitAPI(const std::string& key, const std::string& secret) : apiKey(key), apiSecret(secret) {}

    std::string placeOrder(const std::string& instrument, double amount, const std::string& side) {
        Json::Value order;
        order["jsonrpc"] = "2.0";
        order["id"] = 1;
        order["method"] = "private/buy"; // or "private/sell"
        order["params"]["instrument_name"] = instrument;
        order["params"]["amount"] = amount;
        order["params"]["type"] = "limit"; // or "market"
        
        return makeRequest("private/buy", "POST", order.toStyledString());
    }

    std::string cancelOrder(const std::string& orderId) {
        Json::Value order;
        order["jsonrpc"] = "2.0";
        order["id"] = 1;
        order["method"] = "private/cancel";
        order["params"]["order_id"] = orderId;

        return makeRequest("private/cancel", "POST", order.toStyledString());
    }

    std::string modifyOrder(const std::string& orderId, double newAmount) {
        Json::Value order;
        order["jsonrpc"] = "2.0";
        order["id"] = 1;
        order["method"] = "private/edit";
        order["params"]["order_id"] = orderId;
        order["params"]["amount"] = newAmount;

        return makeRequest("private/edit", "POST", order.toStyledString());
    }

    std::string getOrderBook(const std::string& instrument) {
        return makeRequest("public/get_order_book?instrument_name=" + instrument, "GET", "");
    }

    std::string getCurrentPositions() {
        return makeRequest("private/get_positions", "GET", "");
    }
};

int main() {
    DeribitAPI api("YOUR_API_KEY", "YOUR_API_SECRET");

    // Example usage:
    std::string orderResponse = api.placeOrder("BTC-PERPETUAL", 1.0, "buy");
    std::cout << "Order Response: " << orderResponse << std::endl;

    std::string orderBook = api.getOrderBook("BTC-PERPETUAL");
    std::cout << "Order Book: " << orderBook << std::endl;

    std::string positions = api.getCurrentPositions();
    std::cout << "Current Positions: " << positions << std::endl;

    return 0;
}
