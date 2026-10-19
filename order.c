#include <stdio.h>
#include <string.h>
#include "food_system.h"

const char *status_text(int status)
{
	static const char *names[STATUS_COUNT] = {
			"Pending", "Preparing", "Ready", "Completed", "Cancelled"};

	if (status < 0 || status >= STATUS_COUNT)
		return "Unknown";
	return names[status];
}

/* ---- order file helpers ----
	 Orders are stored one after another in a binary file. Order number N is
	 the Nth record, so we can jump straight to it with fseek.
	 NOTE: if you ever change the Order struct, delete orders.dat. */

int count_orders(void)
{
	FILE *fp = fopen(ORDER_FILE, "rb");
	long size;

	if (fp == NULL)
		return 0;
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fclose(fp);
	return (int)(size / (long)sizeof(Order));
}

int read_order(int orderNo, Order *o)
{
	FILE *fp = fopen(ORDER_FILE, "rb");
	int ok;

	if (fp == NULL || orderNo < 1)
	{
		if (fp)
			fclose(fp);
		return 0;
	}
	ok = fseek(fp, (long)(orderNo - 1) * (long)sizeof(Order), SEEK_SET) == 0 &&
			 fread(o, sizeof(Order), 1, fp) == 1;
	fclose(fp);
	return ok;
}

int write_order(int orderNo, const Order *o)
{
	FILE *fp = fopen(ORDER_FILE, "r+b");
	int ok;

	if (fp == NULL || orderNo < 1)
	{
		if (fp)
			fclose(fp);
		return 0;
	}
	ok = fseek(fp, (long)(orderNo - 1) * (long)sizeof(Order), SEEK_SET) == 0 &&
			 fwrite(o, sizeof(Order), 1, fp) == 1;
	fclose(fp);
	return ok;
}

int append_order(const Order *o)
{
	FILE *fp = fopen(ORDER_FILE, "ab");
	int ok;

	if (fp == NULL)
		return 0;
	ok = fwrite(o, sizeof(Order), 1, fp) == 1;
	fclose(fp);
	return ok;
}

void print_order_summary(const Order *o)
{
	int i;

	printf("\n========== ORDER #%d ==========\n", o->orderNo);
	printf("Customer: %s\n", o->customer);
	printf("Date:     %s\n\n", o->date);
	for (i = 0; i < o->itemCount; i++)
		printf("%-26s x%-3d P%9.2f\n", o->items[i].name, o->items[i].qty,
					 o->items[i].subtotal);
	printf("--------------------------------------\n");
	printf("%-31s P%9.2f\n", "TOTAL", o->total);
	printf("Status:   %s\n", status_text(o->status));
	if (o->paid)
		printf("Payment:  Paid (%s)\n", o->method);
	else
		printf("Payment:  Unpaid\n");
}

/* 3. CREATE ORDER */
void create_order(const Food foods[], int count)
{
	Order o;
	OrderItem *it;
	int idx, id, qty, i, found;

	printf("\n--- CREATE ORDER ---\n");

	if (count == 0)
	{
		printf("The menu is empty. Add food items first.\n");
		press_any_key();
		return;
	}

	memset(&o, 0, sizeof o);
	read_nonempty("Customer name: ", o.customer, sizeof o.customer);

	do
	{
		/* select food item (repeat until available) */
		for (;;)
		{
			display_menu_items(foods, count);
			id = read_int("\nEnter food ID (0 to cancel order): ", 0, 1000000);
			if (id == 0)
			{
				printf("Order cancelled.\n");
				press_any_key();
				return;
			}

			idx = find_food_index(foods, count, id);
			if (idx < 0)
			{
				printf("Food ID not found.\n");
				continue;
			}
			if (!foods[idx].available)
			{
				printf("\"%s\" is SOLD OUT. Please choose another item.\n",
							 foods[idx].name);
				continue;
			}
			break;
		}

		/* quantity (read_int repeats with an error message until valid) */
		qty = read_int("Enter quantity (1-99): ", 1, 99);

		/* same item ordered twice? merge into one line */
		found = -1;
		for (i = 0; i < o.itemCount; i++)
			if (o.items[i].id == foods[idx].id)
				found = i;

		if (found >= 0)
		{
			it = &o.items[found];
			it->qty += qty;
		}
		else
		{
			it = &o.items[o.itemCount++];
			it->id = foods[idx].id;
			snprintf(it->name, sizeof it->name, "%s", foods[idx].name);
			it->price = foods[idx].price;
			it->qty = qty;
		}
		it->subtotal = it->price * it->qty;
		printf("Added: %s x%d (subtotal P%.2f)\n", it->name, it->qty,
					 it->subtotal);

		if (o.itemCount >= MAX_ITEMS)
		{
			printf("Maximum number of items per order reached.\n");
			break;
		}
	} while (ask_yes_no("Add another item? (Y/N): "));

	/* calculate total */
	o.total = 0;
	for (i = 0; i < o.itemCount; i++)
		o.total += o.items[i].subtotal;

	/* save order (status: pending) */
	o.orderNo = count_orders() + 1;
	get_today(o.date);
	o.status = STATUS_PENDING;
	o.paid = 0;
	snprintf(o.method, sizeof o.method, "None");

	if (!append_order(&o))
		printf("Error: could not save the order.\n");
	else
	{
		printf("\nOrder saved.\n");
		print_order_summary(&o);
	}

	press_any_key();
}

/* 4. UPDATE ORDER STATUS */
void update_order_status(void)
{
	int total = count_orders();
	int no, s, i;
	Order o;

	printf("\n--- UPDATE ORDER STATUS ---\n");

	if (total == 0)
	{
		printf("There are no orders yet.\n");
		press_any_key();
		return;
	}

	no = read_int("Enter order no. (0 to go back): ", 0, total);
	if (no == 0)
		return;

	if (!read_order(no, &o))
	{
		printf("Could not read order #%d.\n", no);
		press_any_key();
		return;
	}

	print_order_summary(&o);

	printf("\nChoose status:\n");
	for (i = 0; i < STATUS_COUNT; i++)
		printf("%d. %s\n", i + 1, status_text(i));
	s = read_int("Enter choice: ", 1, STATUS_COUNT);

	o.status = s - 1;
	if (write_order(no, &o))
		printf("\nOrder status updated successfully.\n");
	else
		printf("\nError: could not save the change.\n");

	press_any_key();
}
