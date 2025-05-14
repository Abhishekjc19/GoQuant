import { useState, useEffect } from 'react';
import { OrderBook, MetricsData, FeeTier } from '../types';
import { INITIAL_METRICS } from '../constants';

interface UseTradeMetricsParams {
  orderBook: OrderBook;
  quantity: number;
  feeTier: FeeTier;
  orderType: 'market' | 'limit';
  volatility: number;
}

export default function useTradeMetrics({
  orderBook,
  quantity,
  feeTier,
  orderType,
  volatility
}: UseTradeMetricsParams) {
  const [metrics, setMetrics] = useState<MetricsData>(INITIAL_METRICS);

  useEffect(() => {
    // Only calculate if we have order book data and a positive quantity
    if (!orderBook || !orderBook.asks.length || !orderBook.bids.length || quantity <= 0) {
      return;
    }

    // Calculate slippage
    const calculateSlippage = () => {
      if (orderType === 'limit') return 0;
      
      // For market orders, estimate slippage based on depth
      let remainingQuantity = quantity;
      let totalCost = 0;
      
      for (const ask of orderBook.asks) {
        const fillQuantity = Math.min(remainingQuantity, ask.size);
        totalCost += fillQuantity * ask.price;
        remainingQuantity -= fillQuantity;
        
        if (remainingQuantity <= 0) break;
      }
      
      // If we can't fill the entire order with the available depth
      if (remainingQuantity > 0) {
        // Estimate price for the remaining quantity (with a premium)
        const lastPrice = orderBook.asks[orderBook.asks.length - 1].price;
        totalCost += remainingQuantity * lastPrice * 1.05;
      }
      
      // Calculate effective price
      const effectivePrice = totalCost / quantity;
      
      // Calculate slippage as percentage from the best ask
      const bestAskPrice = orderBook.asks[0].price;
      const slippagePercentage = ((effectivePrice - bestAskPrice) / bestAskPrice) * 100;
      
      return slippagePercentage;
    };

    // Calculate fees
    const calculateFees = () => {
      // Estimate execution price
      const executionPrice = orderBook.asks[0].price;
      const executionValue = quantity * executionPrice;
      
      // Apply fee tier
      const feePercentage = orderType === 'market' ? feeTier.taker : feeTier.maker;
      return (executionValue * feePercentage) / 100;
    };

    // Calculate market impact
    const calculateMarketImpact = () => {
      // Simplified Almgren-Chriss model
      // Impact = C * σ * (Q/V)^0.5
      // where:
      // - C is a constant (typical value ~0.3)
      // - σ is volatility
      // - Q is order quantity
      // - V is market volume
      
      const C = 0.3;
      const marketVolume = orderBook.bids.reduce((sum, bid) => sum + bid.size, 0) + 
                           orderBook.asks.reduce((sum, ask) => sum + ask.size, 0);
      
      // Adjusted volatility from the volatility parameter (0-100 scale to percentage)
      const adjustedVolatility = volatility / 100;
      
      return C * adjustedVolatility * Math.sqrt(quantity / marketVolume) * 100;
    };

    // Calculate Maker/Taker split
    const calculateMakerTakerSplit = () => {
      // For limit orders, assume higher maker percentage
      // For market orders, assume 100% taker
      if (orderType === 'market') {
        return { maker: 0, taker: 100 };
      }
      
      // For limit orders, use a logistic function based on volatility
      // Higher volatility means more likely to be taker (gets filled quickly)
      const normalizedVolatility = volatility / 100;
      
      // Logistic function: 1 / (1 + e^(-k(x-x0)))
      // where k controls steepness and x0 is the midpoint
      const k = 10; // Steepness
      const x0 = 0.5; // Midpoint
      
      // Calculate taker percentage
      const takerPercentage = 100 * (1 / (1 + Math.exp(-k * (normalizedVolatility - x0))));
      const makerPercentage = 100 - takerPercentage;
      
      return { maker: makerPercentage, taker: takerPercentage };
    };

    // Calculate all metrics
    const slippage = calculateSlippage();
    const fees = calculateFees();
    const marketImpact = calculateMarketImpact();
    const netCost = slippage + fees + marketImpact;
    const { maker, taker } = calculateMakerTakerSplit();
    
    // Simulated latency calculation (not based on real network performance)
    const latency = 2 + Math.random() * 5;

    setMetrics({
      slippage,
      fees,
      marketImpact,
      netCost,
      makerPercentage: maker,
      takerPercentage: taker,
      latency
    });
  }, [orderBook, quantity, feeTier, orderType, volatility]);

  return metrics;
}