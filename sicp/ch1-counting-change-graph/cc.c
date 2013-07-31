#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

static const int firstDenomination[] = {0, 1, 5, 10, 25, 50};
static const char *denomSet[] = {
	"{}",
	"{1¢}",
	"{5¢, 1¢}",
	"{10¢, 5¢, 1¢}",
	"{25¢, 10¢, 5¢, 1¢}",
	"{50¢, 25¢, 10¢, 5¢, 1¢}"
};
static int nodeIdent = 0;

int cc(int amount, int kindsOfCoins) {
	if ( amount == 0 ) {
		printf(
			"  n%d[label=\"cc(0¢, %s) = 1\", fillcolor=\"#ffff44\", style=\"filled\"]\n",
			nodeIdent++, denomSet[kindsOfCoins]);
		return 1;
	} else if ( amount < 0 || kindsOfCoins == 0 ) {
		if ( amount < 0 ) {
			assert(kindsOfCoins > 0);
			printf(
				"  n%d[label=\"cc(%d¢, %s) = 0\", fillcolor=\"#ff4444\", style=\"filled\"]\n",
				nodeIdent++, amount, denomSet[kindsOfCoins]);
		} else {
			printf(
				"  n%d[label=\"cc(%d¢, %s) = 0\", fillcolor=\"#ff44ff\", style=\"filled\"]\n",
				nodeIdent++, amount, denomSet[kindsOfCoins]);
		}
		return 0;
	} else {
		int result, thisNode = nodeIdent++;
		printf("  n%d -> n%d\n", thisNode, nodeIdent);
		result = cc(amount, kindsOfCoins - 1);
		printf("  n%d -> n%d\n", thisNode, nodeIdent);
		result += cc(amount - firstDenomination[kindsOfCoins], kindsOfCoins);
		printf(
			"  n%d[label=\"cc(%d¢, %s) = %d\", fillcolor=\"#eeeeee\", style=\"filled\"]\n",
			thisNode, amount, denomSet[kindsOfCoins], result);
		return result;
	}
}

int main(int argc, const char *argv[]) {
	if ( argc != 3 ) {
		fprintf(stderr, "Synopsis: %s <amount> <numTypes>\n", argv[0]);
		exit(1);
	}
	printf("digraph G {\n");
	cc(atoi(argv[1]), atoi(argv[2]));
	printf("}\n");
	return 0;
}
