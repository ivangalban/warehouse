#include "product.h"

typedef struct {
	char *type;
	int amount;
	int limit;
} product_box;

void init(int argc, char **argv);