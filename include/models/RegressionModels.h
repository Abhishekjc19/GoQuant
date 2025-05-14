#pragma once

#include <vector>
#include <deque>
#include <cmath>

namespace GoQuant {

class RegressionModels {
public:
    struct DataPoint {
        double x;  // Independent variable (e.g., order size)
        double y;  // Dependent variable (e.g., slippage)
    };

    // Linear regression model
    class LinearRegression {
    public:
        void fit(const std::vector<DataPoint>& data);
        double predict(double x) const;
        double getRSquared() const;

    private:
        double m_slope;
        double m_intercept;
        double m_rSquared;
    };

    // Quantile regression model
    class QuantileRegression {
    public:
        QuantileRegression(double quantile = 0.5);
        void fit(const std::vector<DataPoint>& data);
        double predict(double x) const;

    private:
        double m_quantile;
        double m_slope;
        double m_intercept;
    };

    // Logistic regression for Maker/Taker prediction
    class LogisticRegression {
    public:
        void fit(const std::vector<DataPoint>& data, const std::vector<bool>& labels);
        double predictProbability(double x) const;
        bool predict(double x) const;

    private:
        double m_slope;
        double m_intercept;
        static double sigmoid(double x);
    };

    // Slippage estimator using multiple models
    class SlippageEstimator {
    public:
        SlippageEstimator();
        void update(const std::deque<DataPoint>& historicalData);
        double estimateSlippage(double orderSize) const;
        double getConfidence() const;

    private:
        LinearRegression m_linearModel;
        QuantileRegression m_quantileModel;
        std::deque<DataPoint> m_historicalData;
        static constexpr size_t MAX_HISTORY_SIZE = 1000;
    };

    // Maker/Taker predictor
    class MakerTakerPredictor {
    public:
        MakerTakerPredictor();
        void update(const std::deque<DataPoint>& historicalData, 
                   const std::deque<bool>& makerLabels);
        double predictMakerProportion(double orderSize) const;

    private:
        LogisticRegression m_model;
        std::deque<DataPoint> m_historicalData;
        std::deque<bool> m_makerLabels;
        static constexpr size_t MAX_HISTORY_SIZE = 1000;
    };
};

} // namespace GoQuant 