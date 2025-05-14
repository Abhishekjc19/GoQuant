#pragma once

#include <QObject>
#include <vector>
#include <deque>
#include <mutex>
#include <nlohmann/json.hpp>

namespace GoQuant {

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

class OrderBookProcessor : public QObject {
    Q_OBJECT

public:
    explicit OrderBookProcessor(QObject *parent = nullptr);
    ~OrderBookProcessor();

    void processOrderBook(const nlohmann::json& data);
    OrderBook getLatestOrderBook() const;
    
    // Market impact calculation
    double calculateMarketImpact(double quantity, bool isBuy) const;
    
    // Slippage calculation
    double calculateSlippage(double quantity, bool isBuy) const;
    
    // Maker/Taker proportion calculation
    double calculateMakerTakerProportion() const;

signals:
    void orderBookUpdated(const OrderBook& orderBook);
    void marketImpactUpdated(double impact);
    void slippageUpdated(double slippage);
    void makerTakerProportionUpdated(double proportion);

private:
    OrderBook m_currentOrderBook;
    std::deque<OrderBook> m_orderBookHistory;
    mutable std::mutex m_mutex;
    
    static constexpr size_t HISTORY_SIZE = 1000;
    
    void updateOrderBook(const nlohmann::json& data);
    void maintainHistory();
};

} // namespace GoQuant 