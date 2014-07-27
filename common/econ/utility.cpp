

#include "utility.h"

// food/housing/physical res utility evaluation
int PhUtil(int price, int cmdist)
{
	if(price <= 0)
		return MAX_UTIL;
	
	if(cmdist <= 0)
		return MAX_UTIL;

	return MAX_UTIL / price / cmdist;
}

// electricity utility evaluation
int GlUtil(int price)
{
	if(price <= 0)
		return MAX_UTIL;

	return MAX_UTIL / price;
}

// inverse phys utility - solve for distance based on utility and price
int InvPhUtilD(int util, int price)
{
	//util = 100000000 / price / cmdist;
	//util / 100000000 = 1 / price / cmdist;
	//util * price / 100000000 = 1 / cmdist;
	//100000000 / (util * price) = cmdist;
	
	if(util <= 0)
		return MAX_UTIL;
	
	if(price <= 0)
		return MAX_UTIL;

	return MAX_UTIL / (util * price);
}

// inverse phys utility - solve for price based on utility and distance
int InvPhUtilP(int util, int cmdist)
{
	if(util <= 0)
		return MAX_UTIL;
	
	if(cmdist <= 0)
		return MAX_UTIL;

	return MAX_UTIL / (util * cmdist);
}

int InvGlUtilP(int util)
{
	if(util <= 0)
		return MAX_UTIL;

	return MAX_UTIL / util;
}