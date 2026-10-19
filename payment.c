#include <stdio.h>
#include <string.h>
#include "food_system.h"

/* 5. RECORD PAYMENT */
void record_payment(void)
{
	static const char *methods[] = {"Cash", "Card", "Bank Transfer", "Other"};
	int total = count_orders();
	int no, m, s, i;
	Order o;

	printf("\n--- RECORD PAYMENT ---\n");

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

	if (o.status == STATUS_CANCELLED)
	{
		printf("Order #%d is cancelled. No payment can be recorded.\n", no);
		press_any_key();
		return;
	}

	printf("\nOrder #%d - %s\n", o.orderNo, o.customer);
	printf("Total amount: P%.2f\n", o.total);
	if (o.paid)
		printf("(Already marked as paid via %s)\n", o.method);

	printf("\nPayment method:\n");
	for (i = 0; i < 4; i++)
		printf("%d. %s\n", i + 1, methods[i]);
	m = read_int("Enter choice: ", 1, 4);

	printf("\n1. Paid\n2. Unpaid\n");
	s = read_int("Payment status: ", 1, 2);

	o.paid = (s == 1);
	snprintf(o.method, sizeof o.method, "%s", methods[m - 1]);

	if (write_order(no, &o))
		printf("\nPayment record saved successfully.\n");
	else
		printf("\nError: could not save the payment.\n");

	press_any_key();
}
