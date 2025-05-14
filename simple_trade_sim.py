from dataclasses import dataclass
from typing import List
from datetime import datetime

@dataclass
class OrderBookLevel:
    price: float
    quantity: float

@dataclass
class OrderBook:
    asks: List[OrderBookLevel]
    bids: List[OrderBookLevel]
    timestamp: str
    exchange: str
    symbol: str

class OrderBookProcessor:
    def __init__(self):
        self.current_order_book = None

    def process_order_book(self, data: OrderBook):
        self.current_order_book = data
        print(f"Order book updated for {data.symbol}")

    def get_latest_order_book(self) -> OrderBook:
        return self.current_order_book

    def calculate_market_impact(self, quantity: float, is_buy: bool) -> float:
        levels = self.current_order_book.asks if is_buy else self.current_order_book.bids
        if not levels:
            return 0.0

        remaining_quantity = quantity
        weighted_price = 0.0
        total_quantity = 0.0

        for level in levels:
            if remaining_quantity <= 0.0:
                break
            executed_quantity = min(remaining_quantity, level.quantity)
            weighted_price += level.price * executed_quantity
            total_quantity += executed_quantity
            remaining_quantity -= executed_quantity

        if total_quantity == 0.0:
            return 0.0
        
        average_price = weighted_price / total_quantity
        mid_price = (levels[0].price + levels[0].price) / 2.0
        return abs(average_price - mid_price) / mid_price

class FeeCalculator:
    def calculate_fees(self, order_size: float, is_maker: bool) -> float:
        fee_rate = 0.0008 if is_maker else 0.0010  # 0.08% maker fee, 0.10% taker fee
        return order_size * fee_rate

def main():
    print("Starting GoQuant Trade Simulator (Simple Version)")
    print("------------------------------------------------")

    order_book_processor = OrderBookProcessor()
    fee_calculator = FeeCalculator()

    # Create sample order book
    order_book = OrderBook(
        symbol="BTC-USDT",
        exchange="OKX",
        timestamp=datetime.now().isoformat(),
        asks=[
            OrderBookLevel(50000.0, 1.5),
            OrderBookLevel(50001.0, 2.0),
            OrderBookLevel(50002.0, 1.0)
        ],
        bids=[
            OrderBookLevel(49999.0, 2.0),
            OrderBookLevel(49998.0, 1.5),
            OrderBookLevel(49997.0, 1.0)
        ]
    )

    # Process order book
    order_book_processor.process_order_book(order_book)

    # Calculate and display market impact
    market_impact = order_book_processor.calculate_market_impact(1.0, True)
    print(f"Market impact for 1 BTC: {market_impact * 100:.2f}%")

    # Calculate and display fees
    order_size = 1.0  # 1 BTC
    maker_fee = fee_calculator.calculate_fees(order_size, True)
    taker_fee = fee_calculator.calculate_fees(order_size, False)
    print(f"Fees for {order_size} BTC:")
    print(f"  Maker fee: {maker_fee:.8f} BTC")
    print(f"  Taker fee: {taker_fee:.8f} BTC")

    print("\nSimulation complete!")

if __name__ == "__main__":
    main() 