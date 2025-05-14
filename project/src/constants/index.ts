import { AssetPair, ExchangeOption, FeeTier, OrderType } from "../types";

export const EXCHANGES: ExchangeOption[] = [
  { value: "okx", label: "OKX" },
  { value: "binance", label: "Binance" },
  { value: "bybit", label: "Bybit" },
];

export const ASSET_PAIRS: AssetPair[] = [
  { value: "BTC-USDT", label: "BTC-USDT" },
  { value: "ETH-USDT", label: "ETH-USDT" },
  { value: "SOL-USDT", label: "SOL-USDT" },
  { value: "XRP-USDT", label: "XRP-USDT" },
  { value: "BNB-USDT", label: "BNB-USDT" },
];

export const ORDER_TYPES: OrderType[] = [
  { value: "market", label: "Market" },
  { value: "limit", label: "Limit" },
];

export const FEE_TIERS: FeeTier[] = [
  { value: "tier1", label: "Regular", maker: 0.1, taker: 0.1 },
  { value: "tier2", label: "VIP 1", maker: 0.08, taker: 0.1 },
  { value: "tier3", label: "VIP 2", maker: 0.06, taker: 0.08 },
  { value: "tier4", label: "VIP 3", maker: 0.04, taker: 0.06 },
];

export const MOCK_ORDERBOOK = {
  bids: [
    { price: 61253.5, size: 1.2 },
    { price: 61252.8, size: 0.8 },
    { price: 61252.1, size: 2.3 },
    { price: 61251.7, size: 1.5 },
    { price: 61250.9, size: 3.1 },
  ],
  asks: [
    { price: 61254.1, size: 0.9 },
    { price: 61255.2, size: 1.7 },
    { price: 61256.0, size: 2.1 },
    { price: 61257.5, size: 1.3 },
    { price: 61258.2, size: 2.8 },
  ],
  timestamp: Date.now(),
};

export const WEBSOCKET_URL = "wss://ws.gomarket-cpp.goquant.io/ws/l2-orderbook/okx/BTC-USDT-SWAP";

export const INITIAL_METRICS: MetricsData = {
  slippage: 0.0,
  fees: 0.0,
  marketImpact: 0.0,
  netCost: 0.0,
  makerPercentage: 50,
  takerPercentage: 50,
  latency: 0,
};