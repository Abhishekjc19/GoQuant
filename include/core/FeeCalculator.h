#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace GoQuant {

class FeeCalculator {
public:
    struct FeeTier {
        double makerFee;
        double takerFee;
        double minVolume;  // Minimum 30-day trading volume in USD
    };

    FeeCalculator();

    // Set fee tier based on exchange and trading volume
    void setFeeTier(const std::string& exchange, double tradingVolume);

    // Calculate fees for a trade
    double calculateFees(double orderSize, bool isMaker) const;

    // Get current fee tier
    const FeeTier& getCurrentFeeTier() const;

private:
    // Fee tiers for different exchanges
    std::unordered_map<std::string, std::vector<FeeTier>> m_feeTiers;
    FeeTier m_currentTier;

    // Initialize fee tiers for different exchanges
    void initializeFeeTiers();
};

} // namespace GoQuant 