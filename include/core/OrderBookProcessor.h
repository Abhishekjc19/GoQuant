/**
 * @file OrderBookProcessor.h
 * @brief Header file for the OrderBookProcessor class
 * 
 * This file defines the OrderBookProcessor class and related structures for
 * processing and analyzing order book data in real-time.
 * 
 * @author GoQuant Team
 * @version 1.0
 * @date 2024
 */

#pragma once

#include <QObject>
#include <vector>
#include <deque>
#include <mutex>
#include <nlohmann/json.hpp>

namespace GoQuant {

/**
 * @brief Represents a single price level in the order book
 * 
 * This structure holds the price and quantity information for a single
 * level in either the ask (sell) or bid (buy) side of the order book.
 */
struct OrderBookLevel {
    double price;    ///< Price at this level
    double quantity; ///< Available quantity at this price
};

/**
 * @brief Represents a complete order book snapshot
 * 
 * This structure holds the complete state of an order book at a specific
 * point in time, including all ask and bid levels.
 */
struct OrderBook {
    std::vector<OrderBookLevel> asks;  ///< List of ask (sell) orders
    std::vector<OrderBookLevel> bids;  ///< List of bid (buy) orders
    std::string timestamp;             ///< ISO format timestamp
    std::string exchange;              ///< Exchange identifier
    std::string symbol;                ///< Trading pair symbol
};

/**
 * @brief Processes and analyzes order book data in real-time
 * 
 * This class handles the processing of order book updates and provides
 * methods for calculating various market metrics such as market impact,
 * slippage, and maker/taker proportions. It maintains a history of order
 * book snapshots for analysis and emits signals when updates occur.
 */
class OrderBookProcessor : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructs a new OrderBookProcessor instance
     * 
     * @param parent Parent QObject for Qt signal/slot system
     */
    explicit OrderBookProcessor(QObject *parent = nullptr);
    ~OrderBookProcessor();

    /**
     * @brief Processes incoming order book data
     * 
     * @param data JSON object containing order book data
     */
    void processOrderBook(const nlohmann::json& data);

    /**
     * @brief Retrieves the most recent order book snapshot
     * 
     * @return OrderBook Current order book state
     */
    OrderBook getLatestOrderBook() const;
    
    /**
     * @brief Calculates market impact for a given order size
     * 
     * @param quantity Order size in base currency
     * @param isBuy True for buy orders, false for sell orders
     * @return double Market impact as a percentage of mid price
     */
    double calculateMarketImpact(double quantity, bool isBuy) const;
    
    /**
     * @brief Calculates slippage for a given order size
     * 
     * @param quantity Order size in base currency
     * @param isBuy True for buy orders, false for sell orders
     * @return double Slippage as a percentage of mid price
     */
    double calculateSlippage(double quantity, bool isBuy) const;
    
    /**
     * @brief Calculates the proportion of maker vs taker orders
     * 
     * @return double Proportion of maker orders (0.0 to 1.0)
     */
    double calculateMakerTakerProportion() const;

signals:
    /// Emitted when the order book is updated
    void orderBookUpdated(const OrderBook& orderBook);
    /// Emitted when market impact is calculated
    void marketImpactUpdated(double impact);
    /// Emitted when slippage is calculated
    void slippageUpdated(double slippage);
    /// Emitted when maker/taker proportion is calculated
    void makerTakerProportionUpdated(double proportion);

private:
    OrderBook m_currentOrderBook;              ///< Current order book state
    std::deque<OrderBook> m_orderBookHistory;  ///< Historical order book snapshots
    mutable std::mutex m_mutex;                ///< Mutex for thread safety
    
    static constexpr size_t HISTORY_SIZE = 1000;  ///< Maximum history size
    
    /**
     * @brief Updates the current order book with new data
     * 
     * @param data JSON object containing order book data
     */
    void updateOrderBook(const nlohmann::json& data);

    /**
     * @brief Maintains the order book history within size limits
     */
    void maintainHistory();
};

} // namespace GoQuant 