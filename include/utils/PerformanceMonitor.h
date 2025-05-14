#pragma once

#include <QObject>
#include <chrono>
#include <deque>
#include <mutex>
#include <string>

namespace GoQuant {

class PerformanceMonitor : public QObject {
    Q_OBJECT

public:
    struct Metric {
        std::string name;
        double value;
        std::chrono::system_clock::time_point timestamp;
    };

    explicit PerformanceMonitor(QObject *parent = nullptr);
    ~PerformanceMonitor();

    // Record metrics
    void recordMetric(const std::string& name, double value);
    void recordLatency(const std::string& operation, double milliseconds);

    // Get statistics
    double getAverageMetric(const std::string& name) const;
    double getMinMetric(const std::string& name) const;
    double getMaxMetric(const std::string& name) const;
    double getPercentileMetric(const std::string& name, double percentile) const;

    // Get latency statistics
    double getAverageLatency(const std::string& operation) const;
    double getMinLatency(const std::string& operation) const;
    double getMaxLatency(const std::string& operation) const;
    double getPercentileLatency(const std::string& operation, double percentile) const;

    // Clear history
    void clearHistory();

signals:
    void metricUpdated(const std::string& name, double value);
    void latencyUpdated(const std::string& operation, double milliseconds);

private:
    struct MetricHistory {
        std::deque<Metric> values;
        static constexpr size_t MAX_HISTORY_SIZE = 1000;
    };

    std::unordered_map<std::string, MetricHistory> m_metrics;
    std::unordered_map<std::string, MetricHistory> m_latencies;
    mutable std::mutex m_mutex;

    void maintainHistory(MetricHistory& history);
    double calculatePercentile(const std::deque<Metric>& values, double percentile) const;
};

} // namespace GoQuant 