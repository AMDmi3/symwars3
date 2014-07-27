

#ifndef UTILITY_H
#define UTILITY_H

#define MAX_UTIL	100000000

int PhUtil(int price, int cmdist);
int GlUtil(int price);
int InvPhUtilD(int util, int price);
int InvPhUtilP(int util, int cmdist);
int InvGlUtilP(int util);

#endif