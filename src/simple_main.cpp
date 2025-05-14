#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>

// Simple OrderBook structure
struct OrderBookLevel {
    double price;
    double quantity;
};

struct OrderBook {
    std::vector<OrderBookLevel> asks;
    std::vector<OrderBookLevel> bids;
    std::string timestamp;
    std::string exchange;
    std::string symbol;
};

// Simple OrderBookProcessor
class OrderBookProcessor {
public:
    void processOrderBook(const OrderBook& data) {
        m_currentOrderBook = data;
        std::cout << "Order book updated for " << data.symbol << std::endl;
    }

    OrderBook getLatestOrderBook() const {
        return m_currentOrderBook;
    }

    double calculateMarketImpact(double quantity, bool isBuy) const {
        const auto& levels = isBuy ? m_currentOrderBook.asks : m_currentOrderBook.bids;
        if (levels.empty()) return 0.0;

        double remainingQuantity = quantity;
        double weightedPrice = 0.0;
        double totalQuantity = 0.0;

        for (const auto& level : levels) {
            if (remainingQuantity <= 0.0) break;
            double executedQuantity = std::min(remainingQuantity, level.quantity);
            weightedPrice += level.price * executedQuantity;
            totalQuantity += executedQuantity;
            remainingQuantity -= executedQuantity;
        }

        if (totalQuantity == 0.0) return 0.0;
        double averagePrice = weightedPrice / totalQuantity;
        double midPrice = (levels[0].price + levels[0].price) / 2.0;
        return std::abs(averagePrice - midPrice) / midPrice;
    }

private:
    OrderBook m_currentOrderBook;
};

// Simple FeeCalculator
class FeeCalculator {
public:
    double calculateFees(double orderSize, bool isMaker) const {
        double feeRate = isMaker ? 0.0008 : 0.0010; // 0.08% maker fee, 0.10% taker fee
        return orderSize * feeRate;
    }
};

// Main function
int main() {
    std::cout << "Starting GoQuant Trade Simulator (Simple Version)" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;

    OrderBookProcessor orderBookProcessor;
    FeeCalculator feeCalculator;

    // Create sample order book
    OrderBook orderBook;
    orderBook.symbol = "BTC-USDT";
    orderBook.exchange = "OKX";
    orderBook.timestamp = "2024-03-20T10:00:00Z";

    // Add some sample asks
    orderBook.asks = {
        {50000.0, 1.5},
        {50001.0, 2.0},
        {50002.0, 1.0}
    };

    // Add some sample bids
    orderBook.bids = {
        {49999.0, 2.0},
        {49998.0, 1.5},
        {49997.0, 1.0}
    };

    // Process order book
    orderBookProcessor.processOrderBook(orderBook);

    // Calculate and display market impact
    double marketImpact = orderBookProcessor.calculateMarketImpact(1.0, true);
    std::cout << "Market impact for 1 BTC: " << marketImpact * 100 << "%" << std::endl;

    // Calculate and display fees
    double orderSize = 1.0; // 1 BTC
    double makerFee = feeCalculator.calculateFees(orderSize, true);
    double takerFee = feeCalculator.calculateFees(orderSize, false);
    std::cout << "Fees for " << orderSize << " BTC:" << std::endl;
    std::cout << "  Maker fee: " << makerFee << " BTC" << std::endl;
    std::cout << "  Taker fee: " << takerFee << " BTC" << std::endl;

    std::cout << "\nSimulation complete!" << std::endl;
    return 0;
} 