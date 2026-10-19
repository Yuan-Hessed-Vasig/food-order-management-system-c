#include <stdio.h>
#include <string.h>
#include "food_system.h"

#define MAX_TALLY 200

typedef struct
{
	char name[50];
	int qty;
} Tally;

static int valid_date(const char *s)
{
	int y, m, d;
	return strlen(s) == 10 && sscanf(s, "%4d-%2d-%2d", &y, &m, &d) == 3 &&
				 s[4] == '-' && s[7] == '-' && m >= 1 && m <= 12 && d >= 1 && d <= 31;
}

/* 6. DAILY ORDER/SALES REPORT */
void daily_sales_order_report(void)
{
	char date[32], today[11];
	int total = count_orders();
	int n, i, j, k;
	Order o;
	Tally tally[MAX_TALLY];
	int nt, found, orders, cancelled, itemsSold;
	double sales, collected;

	printf("\n--- DAILY ORDER/SALES REPORT ---\n");
	get_today(today);

	do
	{
		/* ask which day to report on */
		for (;;)
		{
			printf("Enter date (YYYY-MM-DD), or press Enter for today (%s): ", today);
			read_line(NULL, date, sizeof date);
			if (date[0] == '\0')
			{
				strcpy(date, today);
				break;
			}
			if (valid_date(date))
				break;
			printf("Invalid date format.\n");
		}

		/* load records and calculate */
		found = orders = cancelled = itemsSold = nt = 0;
		sales = collected = 0;

		for (n = 1; n <= total; n++)
		{
			if (!read_order(n, &o) || strcmp(o.date, date) != 0)
				continue;

			found++;
			if (o.status == STATUS_CANCELLED)
			{
				cancelled++;
				continue;
			}

			orders++;
			sales += o.total;
			if (o.paid)
				collected += o.total;

			for (i = 0; i < o.itemCount; i++)
			{
				itemsSold += o.items[i].qty;

				k = -1;
				for (j = 0; j < nt; j++)
					if (strcmp(tally[j].name, o.items[i].name) == 0)
						k = j;
				if (k < 0 && nt < MAX_TALLY)
				{
					k = nt++;
					snprintf(tally[k].name, sizeof tally[k].name, "%s", o.items[i].name);
					tally[k].qty = 0;
				}
				if (k >= 0)
					tally[k].qty += o.items[i].qty;
			}
		}

		/* display report */
		printf("\n========== REPORT FOR %s ==========\n", date);
		if (found == 0)
			printf("No orders recorded on this date.\n");
		else
		{
			printf("Total orders:     %d\n", orders);
			printf("Cancelled orders: %d\n", cancelled);
			printf("Total sales:      P%.2f\n", sales);
			printf("  Paid:           P%.2f\n", collected);
			printf("  Unpaid:         P%.2f\n", sales - collected);
			printf("Items sold:       %d\n", itemsSold);

			if (nt > 0)
			{
				printf("\nBreakdown:\n");
				for (j = 0; j < nt; j++)
					printf("  %-28s x%d\n", tally[j].name, tally[j].qty);
			}
		}
	} while (ask_yes_no("\nView another report? (Y/N): "));

	press_any_key();
}
