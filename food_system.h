#ifndef FOOD_SYSTEM_H
#define FOOD_SYSTEM_H

#define MAX_FOODS 100
#define MAX_ITEMS 20
#define MENU_FILE "menu.csv"
#define ORDER_FILE "orders.dat"

/* ---------- order status codes ---------- */
enum
{
	STATUS_PENDING = 0,
	STATUS_PREPARING,
	STATUS_READY,
	STATUS_COMPLETED,
	STATUS_CANCELLED,
	STATUS_COUNT
};

/* ---------- data structures ---------- */
typedef struct
{
	int id;
	char name[50];
	char category[50];
	double price;
	int available; /* 1 = available, 0 = sold out */
} Food;

typedef struct
{
	int id;
	char name[50];
	double price;
	int qty;
	double subtotal;
} OrderItem;

typedef struct
{
	int orderNo;
	char customer[50];
	char date[11]; /* YYYY-MM-DD */
	OrderItem items[MAX_ITEMS];
	int itemCount;
	double total;
	int status;			 /* STATUS_* */
	int paid;				 /* 1 = paid, 0 = unpaid */
	char method[20]; /* payment method */
} Order;

/* ---------- utils.c ---------- */
void read_line(const char *prompt, char *buf, int size);
int parse_int(const char *s, int *out);
int parse_double(const char *s, double *out);
int read_int(const char *prompt, int min, int max);
double read_double(const char *prompt, double min);
void read_nonempty(const char *prompt, char *buf, int size);
int ask_yes_no(const char *prompt);
void press_any_key(void);
void get_today(char out[11]);

/* ---------- menu.c ---------- */
int load_menu(Food foods[]);
void save_menu(const Food foods[], int count);
int find_food_index(const Food foods[], int count, int id);
void display_menu_items(const Food foods[], int count);
void view_menu(const Food foods[], int count);
void manage_menu(Food foods[], int *count);

/* ---------- order.c ---------- */
const char *status_text(int status);
int count_orders(void);
int read_order(int orderNo, Order *o);
int write_order(int orderNo, const Order *o);
int append_order(const Order *o);
void print_order_summary(const Order *o);
void create_order(const Food foods[], int count);
void update_order_status(void);

/* ---------- payment.c ---------- */
void record_payment(void);

/* ---------- report.c ---------- */
void daily_sales_order_report(void);

#endif
