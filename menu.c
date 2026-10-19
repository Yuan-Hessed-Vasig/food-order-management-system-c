#include <stdio.h>
#include <string.h>
#include "food_system.h"

/* commas would break the CSV format, so swap them for spaces */
static void sanitize(char *s)
{
	for (; *s; s++)
		if (*s == ',')
			*s = ' ';
}

static int default_menu(Food foods[])
{
	static const char *names[] = {
			"Fried Chicken/Rice", "Plain Rice", "Fried Rice/3pcs Siomai",
			"Shawarma Rice", "Burger Siomaitake/Rice", "Burger", "Fries",
			"Pizza", "Ice cream", "Soft Drinks"};
	static const char *cats[] = {
			"Rice Meals", "Extras", "Rice Meals", "Rice Meals", "Rice Meals",
			"Snacks", "Snacks", "Snacks", "Desserts", "Drinks"};
	static const double prices[] = {99, 25, 80, 98, 89, 50, 45, 99, 40, 30};
	int i;

	for (i = 0; i < 10; i++)
	{
		foods[i].id = i + 1;
		snprintf(foods[i].name, sizeof foods[i].name, "%s", names[i]);
		snprintf(foods[i].category, sizeof foods[i].category, "%s", cats[i]);
		foods[i].price = prices[i];
		foods[i].available = 1;
	}
	return 10;
}

void save_menu(const Food foods[], int count)
{
	FILE *fp = fopen(MENU_FILE, "w");
	int i;

	if (fp == NULL)
	{
		printf("Error: could not save %s (is it open in Excel?)\n", MENU_FILE);
		return;
	}

	fprintf(fp, "id,name,category,price,available\n");
	for (i = 0; i < count; i++)
		fprintf(fp, "%d,%s,%s,%.2f,%d\n", foods[i].id, foods[i].name,
						foods[i].category, foods[i].price, foods[i].available);

	fclose(fp);
}

int load_menu(Food foods[])
{
	FILE *fp = fopen(MENU_FILE, "r");
	char line[256];
	int n = 0;

	if (fp == NULL)
	{
		/* first run: create the file with the default menu */
		n = default_menu(foods);
		save_menu(foods, n);
		return n;
	}

	fgets(line, sizeof line, fp); /* skip header */
	while (n < MAX_FOODS && fgets(line, sizeof line, fp) != NULL)
	{
		Food f;
		if (sscanf(line, "%d,%49[^,],%49[^,],%lf,%d", &f.id, f.name,
							 f.category, &f.price, &f.available) == 5)
			foods[n++] = f;
	}

	fclose(fp);
	return n;
}

int find_food_index(const Food foods[], int count, int id)
{
	int i;
	for (i = 0; i < count; i++)
		if (foods[i].id == id)
			return i;
	return -1;
}

/* prints the menu grouped by category */
void display_menu_items(const Food foods[], int count)
{
	int i, j, seen;

	if (count == 0)
	{
		printf("\n(Menu is empty)\n");
		return;
	}

	printf("\n%-4s %-28s %10s  %s\n", "ID", "Name", "Price", "Status");
	for (i = 0; i < count; i++)
	{
		seen = 0;
		for (j = 0; j < i; j++)
			if (strcmp(foods[j].category, foods[i].category) == 0)
			{
				seen = 1;
				break;
			}
		if (seen)
			continue;

		printf("\n[%s]\n", foods[i].category);
		for (j = i; j < count; j++)
			if (strcmp(foods[j].category, foods[i].category) == 0)
				printf("%-4d %-28s P%9.2f  %s\n", foods[j].id, foods[j].name,
							 foods[j].price,
							 foods[j].available ? "Available" : "SOLD OUT");
	}
}

/* 1. VIEW MENU */
void view_menu(const Food foods[], int count)
{
	printf("\n=========== FOOD MENU ===========");
	display_menu_items(foods, count);
	press_any_key();
}

/* ---- helpers for MANAGE MENU ---- */

static int select_food(const Food foods[], int count)
{
	int id, idx;

	display_menu_items(foods, count);
	id = read_int("\nEnter food ID (0 to cancel): ", 0, 1000000);
	if (id == 0)
		return -1;

	idx = find_food_index(foods, count, id);
	if (idx < 0)
		printf("Food ID not found.\n");
	return idx;
}

static int add_food(Food foods[], int *count)
{
	Food f;
	int i, maxId = 0;

	if (*count >= MAX_FOODS)
	{
		printf("The menu is full.\n");
		return 0;
	}

	for (i = 0; i < *count; i++)
		if (foods[i].id > maxId)
			maxId = foods[i].id;

	f.id = maxId + 1;
	read_nonempty("Food name: ", f.name, sizeof f.name);
	read_nonempty("Category: ", f.category, sizeof f.category);
	f.price = read_double("Price: ", 0);
	f.available = 1;
	sanitize(f.name);
	sanitize(f.category);

	foods[(*count)++] = f;
	return 1;
}

static int edit_food(Food foods[], int count)
{
	int idx = select_food(foods, count);
	char name[50], category[50], pbuf[32];
	double p;

	if (idx < 0)
		return 0;

	printf("Leave a field blank to keep its current value.\n");

	read_line("New name: ", name, sizeof name);
	if (name[0] != '\0')
	{
		sanitize(name);
		strcpy(foods[idx].name, name);
	}

	read_line("New category: ", category, sizeof category);
	if (category[0] != '\0')
	{
		sanitize(category);
		strcpy(foods[idx].category, category);
	}

	read_line("New price: ", pbuf, sizeof pbuf);
	if (pbuf[0] != '\0')
	{
		if (parse_double(pbuf, &p) && p >= 0)
			foods[idx].price = p;
		else
			printf("Invalid price, the old price was kept.\n");
	}
	return 1;
}

static int remove_food(Food foods[], int *count)
{
	int idx = select_food(foods, *count);
	int i;

	if (idx < 0)
		return 0;

	if (!ask_yes_no("Remove this item? (Y/N): "))
		return 0;

	for (i = idx; i < *count - 1; i++)
		foods[i] = foods[i + 1];
	(*count)--;
	return 1;
}

static int toggle_availability(Food foods[], int count)
{
	int idx = select_food(foods, count);

	if (idx < 0)
		return 0;

	foods[idx].available = !foods[idx].available;
	printf("\"%s\" is now %s.\n", foods[idx].name,
				 foods[idx].available ? "Available" : "SOLD OUT");
	return 1;
}

/* 2. MANAGE MENU */
void manage_menu(Food foods[], int *count)
{
	int action, changed = 0;

	printf("\n--- MANAGE MENU ---\n");
	printf("1. Add\n2. Edit\n3. Remove\n4. Update Availability\n5. Back\n");
	action = read_int("Choose action: ", 1, 5);

	switch (action)
	{
	case 1:
		changed = add_food(foods, count);
		break;
	case 2:
		changed = edit_food(foods, *count);
		break;
	case 3:
		changed = remove_food(foods, count);
		break;
	case 4:
		changed = toggle_availability(foods, *count);
		break;
	default:
		return;
	}

	if (changed)
	{
		save_menu(foods, *count);
		printf("\nMenu updated successfully.\n");
	}
	press_any_key();
}
