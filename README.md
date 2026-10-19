# Food Order Management System

A console-based food ordering system written in C. It manages a food menu, takes customer orders, tracks order status and payments, and produces a daily sales report. All data is saved in files, so nothing is lost when the program closes.

**Course:** Computer Programming (Basic C)
**Group members:** 
- _Yuan Hessed Vasig_
- _Jhermaine Xyrra Manangan_
- _Jireh Paul Jarabe_
- _Ailyn Ebrao_


---

## Features

| # | Option | What it does |
|---|--------|--------------|
| 1 | View Menu | Shows all food items grouped by category, with prices and availability (Available / SOLD OUT). |
| 2 | Manage Menu | Add, edit, remove, or update the availability of food items. Changes are saved to `menu.csv`. |
| 3 | Create Order | Takes a customer name and one or more items with quantities, checks availability, computes the total, and saves the order with status *Pending*. |
| 4 | Update Order Status | Changes an order's status: Pending, Preparing, Ready, Completed, or Cancelled. |
| 5 | Record Payment | Records the payment method (Cash, Card, Bank Transfer, Other) and whether the order is Paid or Unpaid. |
| 6 | Daily Order/Sales Report | Shows total orders, total sales, paid and unpaid amounts, and items sold for a chosen date (default: today). |
| 7 | Exit | Asks for confirmation, then closes the program. |

All inputs are validated. Entering letters, out-of-range numbers, or empty values shows an error and asks again instead of crashing.

---

## Project files

| File | Purpose |
|------|---------|
| `main.c` | Program entry point: loads the menu, shows the main menu, and calls the correct function for each option. |
| `food_system.h` | Shared header: constants, the `Food`, `OrderItem` and `Order` structs, and all function declarations. |
| `menu.c` | Options 1 and 2, plus loading and saving `menu.csv`. |
| `order.c` | Options 3 and 4, plus the functions that read and write `orders.dat`. |
| `payment.c` | Option 5. |
| `report.c` | Option 6. |
| `utils.c` | Shared input helpers (safe number input, yes/no prompts, current date). |

### Data files (created when the program runs)

| File | Format | Contents |
|------|--------|----------|
| `menu.csv` | Text (CSV) | The food menu. Can be opened and edited in Excel. |
| `orders.dat` | Binary | All saved orders. Only the program can read it. |

---

## How to compile

Put all `.c` files and `food_system.h` in the same folder, then run:

```
gcc main.c menu.c order.c payment.c report.c utils.c -o food_system
```

If you use an IDE (Code::Blocks, Dev-C++), create a C console project and add **all six `.c` files** and the header to it.

## How to run

- Windows: `food_system.exe`
- Linux / macOS: `./food_system`

Run it from the folder where you want `menu.csv` and `orders.dat` to be stored.

---

## menu.csv format

The first line is a header. Each following line is one food item:

```
id,name,category,price,available
1,Fried Chicken/Rice,Rice Meals,99.00,1
2,Plain Rice,Extra,25.00,1
```

- `available` is `1` for available and `0` for sold out.
- Do **not** use commas inside names or categories.
- Close the file in Excel before running the program, otherwise saving changes may fail.
- If `menu.csv` is missing on first run, the program creates one with a default menu.

## orders.dat

Orders are stored one after another, and order number N is the Nth record. Each order contains the customer name, date, items (name, price, quantity, subtotal), total, status, and payment details.

To reset all orders (for example after testing), delete `orders.dat`. If the `Order` struct is ever changed, delete this file too, because old records will no longer match.

---

## Sample usage

1. Choose **3. Create Order**, enter a customer name, pick food IDs and quantities, and answer `Y` or `N` to "Add another item?".
2. Choose **4. Update Order Status** to move the order from Pending to Preparing, Ready, and Completed.
3. Choose **5. Record Payment** to mark it Paid.
4. Choose **6. Daily Order/Sales Report** and press Enter to see today's totals.

---

## Limitations

1. No online customer ordering.
2. No online payment integration; payment is recorded manually.
3. Limited real-time inventory (availability is a simple available / sold out flag).
4. Standalone program (single user, not multi-user).
5. No automatic food preparation monitoring.
6. Payment verification is manual.
7. Records are file-based and offline.

Other limits: up to 100 menu items, up to 20 different items per order, and quantities from 1 to 99 per item.
