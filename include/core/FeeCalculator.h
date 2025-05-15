/**
 * @file FeeCalculator.h
 * @brief Header file for the FeeCalculator class
 * 
 * This file defines the FeeCalculator class for computing trading fees based on
 * exchange-specific fee tiers and trading volumes.
 * 
 * @author GoQuant Team
 * @version 1.0
 * @date 2024
 */

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace GoQuant {

/**
 * @brief Calculates trading fees based on exchange-specific fee tiers
 * 
 * This class manages fee calculations for different exchanges, taking into account
 * trading volume-based fee tiers and maker/taker order types.
 */
class FeeCalculator {
public:
    /**
     * @brief Represents a fee tier structure for an exchange
     * 
     * Each tier defines the maker and taker fees applicable for a given
     * minimum trading volume threshold.
     */
    struct FeeTier {
        double makerFee;   ///< Fee rate for maker orders (e.g., 0.0008 for 0.08%)
        double takerFee;   ///< Fee rate for taker orders (e.g., 0.0010 for 0.10%)
        double minVolume;  ///< Minimum 30-day trading volume in USD required for this tier
    };

    /**
     * @brief Constructs a new FeeCalculator instance
     * 
     * Initializes the fee calculator with default fee tiers for supported exchanges.
     */
    FeeCalculator();

    /**
     * @brief Sets the current fee tier based on trading volume
     * 
     * @param exchange Exchange name (e.g., "OKX")
     * @param tradingVolume Total trading volume in USD
     */
    void setFeeTier(const std::string& exchange, double tradingVolume);

    /**
     * @brief Calculates trading fees for an order
     * 
     * @param orderSize Size of the order in base currency
     * @param isMaker True for maker orders, false for taker orders
     * @return double Fee amount in base currency
     */
    double calculateFees(double orderSize, bool isMaker) const;

    /**
     * @brief Retrieves the current fee tier information
     * 
     * @return const FeeTier& Reference to the current fee tier structure
     */
    const FeeTier& getCurrentFeeTier() const;

private:
    std::unordered_map<std::string, std::vector<FeeTier>> m_feeTiers;  ///< Fee tiers for different exchanges
    FeeTier m_currentTier;  ///< Currently active fee tier

    /**
     * @brief Initializes fee tiers for supported exchanges
     * 
     * Sets up the fee structure for different exchanges and their VIP tiers.
     */
    void initializeFeeTiers();
};

} // namespace GoQuant 