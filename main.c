#include <stdio.h>

int main()
{
	int choice;

	printf("=====FOOD ORDER MANAGEMENT SYSTEM===== \n");
	printf("1. View Menu \n");
	printf("2. Manage Menu \n");
	printf("3. Create Order \n");
	printf("4. Update Order Status \n");
	printf("5. Record Payment \n");
	printf("6. Daily Order/Sales Report \n");
	printf("7. Exit \n");

	scanf(" %d", &choice);

	switch (choice)
	{
	case 1: // algorithm nang view menu
		printf("Move to View Menu");
		break;
	case 2: // algorithm ng manageme menu
		printf("Move to Manage Menu");
		break;
	case 3: // algorithm nang create order
		printf("Move to Create Order Section");
		break;
	case 4: // algorithm nang update order status
		printf("Move to Update Order Status");
		break;
	case 5: // algorithm nang record payment;
		printf("Move to Record Payment");
		break;
	case 6: // algorithm of daily order/sales report
		printf("Move to Daily Order/Sales Report");
	case 7: // exit to main program;
		return 1;
	}

	return 0;
}