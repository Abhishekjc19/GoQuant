export interface OrderBookEntry {
  price: number;
  size: number;
  total?: number;
}

export interface OrderBook {
  bids: OrderBookEntry[];
  asks: OrderBookEntry[];
  timestamp: number;
}

export interface ExchangeOption {
  value: string;
  label: string;
}

export interface AssetPair {
  value: string;
  label: string;
}

export interface OrderType {
  value: "market" | "limit";
  label: string;
}

export interface FeeTier {
  value: string;
  label: string;
  maker: number;
  taker: number;
}

export interface MetricsData {
  slippage: number;
  fees: number;
  marketImpact: number;
  netCost: number;
  makerPercentage: number;
  takerPercentage: number;
  latency: number;
}