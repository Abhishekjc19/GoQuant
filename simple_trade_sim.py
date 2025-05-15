"""
GoQuant Trade Simulator (Simple Version)

This module provides a simplified implementation of the GoQuant trading system,
including order book processing, market impact calculation, and fee computation.
It serves as a demonstration and testing tool for the core trading functionality.

Author: GoQuant Team
Version: 1.0
Date: 2024
"""

from dataclasses import dataclass
from typing import List
from datetime import datetime

@dataclass
class OrderBookLevel:
    """
    Represents a single price level in the order book.
    
    Attributes:
        price (float): The price at this level
        quantity (float): The available quantity at this price
    """
    price: float
    quantity: float

@dataclass
class OrderBook:
    """
    Represents a complete order book snapshot.
    
    Attributes:
        asks (List[OrderBookLevel]): List of ask (sell) orders
        bids (List[OrderBookLevel]): List of bid (buy) orders
        timestamp (str): ISO format timestamp of the snapshot
        exchange (str): Exchange identifier
        symbol (str): Trading pair symbol
    """
    asks: List[OrderBookLevel]
    bids: List[OrderBookLevel]
    timestamp: str
    exchange: str
    symbol: str

class OrderBookProcessor:
    """
    Processes and analyzes order book data.
    
    This class handles order book updates and provides methods for calculating
    market impact and other order book metrics.
    """
    
    def __init__(self):
        """Initialize the order book processor with an empty order book."""
        self.current_order_book = None

    def process_order_book(self, data: OrderBook):
        """
        Update the current order book with new data.
        
        Args:
            data (OrderBook): New order book snapshot
        """
        self.current_order_book = data
        print(f"Order book updated for {data.symbol}")

    def get_latest_order_book(self) -> OrderBook:
        """
        Get the most recent order book snapshot.
        
        Returns:
            OrderBook: Current order book state
        """
        return self.current_order_book

    def calculate_market_impact(self, quantity: float, is_buy: bool) -> float:
        """
        Calculate the market impact of an order.
        
        Args:
            quantity (float): Order size in base currency
            is_buy (bool): True for buy orders, False for sell orders
            
        Returns:
            float: Market impact as a percentage of mid price
        """
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
    """
    Calculates trading fees based on order type and size.
    
    This class implements a simple fee structure with different rates
    for maker and taker orders.
    """
    
    def calculate_fees(self, order_size: float, is_maker: bool) -> float:
        """
        Calculate trading fees for an order.
        
        Args:
            order_size (float): Size of the order in base currency
            is_maker (bool): True for maker orders, False for taker orders
            
        Returns:
            float: Fee amount in base currency
        """
        fee_rate = 0.0008 if is_maker else 0.0010  # 0.08% maker fee, 0.10% taker fee
        return order_size * fee_rate

def main():
    """
    Main entry point for the trade simulator.
    
    Creates sample order book data and demonstrates the functionality
    of the order book processor and fee calculator.
    """
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