#pragma once

#include <vector>
#include <cmath>

namespace GoQuant {

class AlmgrenChriss {
public:
    struct Parameters {
        double volatility;      // Market volatility
        double permanentImpact; // Permanent market impact parameter
        double temporaryImpact; // Temporary market impact parameter
        double riskAversion;    // Risk aversion parameter
        double timeHorizon;     // Trading time horizon
    };

    AlmgrenChriss(const Parameters& params);
    
    // Calculate optimal trading trajectory
    std::vector<double> calculateOptimalTrajectory(
        double initialPosition,
        double targetPosition,
        int numSteps
    ) const;

    // Calculate expected market impact
    double calculateMarketImpact(
        double quantity,
        double price,
        double timeHorizon
    ) const;

    // Calculate total cost including market impact
    double calculateTotalCost(
        double quantity,
        double price,
        double timeHorizon
    ) const;

private:
    Parameters m_params;

    // Helper functions
    double calculatePermanentImpact(double quantity) const;
    double calculateTemporaryImpact(double quantity) const;
    double calculateRiskCost(double quantity, double timeHorizon) const;
};

} // namespace GoQuant 