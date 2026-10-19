#include <stdio.h>
#include "food_system.h"

void show_main_menu(void)
{
	printf("\n=====FOOD ORDER MANAGEMENT SYSTEM=====\n");
	printf("1. View Menu\n");
	printf("2. Manage Menu\n");
	printf("3. Create Order\n");
	printf("4. Update Order Status\n");
	printf("5. Record Payment\n");
	printf("6. Daily Order/Sales Report\n");
	printf("7. Exit\n");
}

int confirm_exit(void)
{
	return ask_yes_no("Confirm exit? (Y/N): ");
}

int main(void)
{
	Food foods[MAX_FOODS];
	int count = load_menu(foods); /* load once at startup */
	int choice;
	int running = 1;
	char buf[64];

	while (running)
	{
		show_main_menu();
		read_line("Enter choice: ", buf, sizeof buf);

		/* Valid choice (1-7)? No -> show the menu again */
		if (!parse_int(buf, &choice) || choice < 1 || choice > 7)
		{
			printf("Invalid choice, enter 1-7.\n");
			continue;
		}

		switch (choice)
		{
		case 1:
			view_menu(foods, count);
			break;
		case 2:
			manage_menu(foods, &count);
			break;
		case 3:
			create_order(foods, count);
			break;
		case 4:
			update_order_status();
			break;
		case 5:
			record_payment();
			break;
		case 6:
			daily_sales_order_report();
			break;
		case 7:
			if (confirm_exit())
				running = 0;
			break;
		}
	}

	printf("Goodbye!\n");
	return 0;
}
