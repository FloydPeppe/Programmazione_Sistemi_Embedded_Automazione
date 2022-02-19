/*
 * utils.c
 *
 *  Created on: Sep 27, 2021
 *      Author: peppe
 */

#include <my_lib/datastream_utils.h>


void reverse(char* str, int len){
    int i = 0, j = len - 1, temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

int itostr(int x, char str[], int d){
    int i = 0;
    int neg= 0;

    if( x<0 ){
    	x= -x;
    	neg= 1;
    }

    while (x) {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }

    while (i < d)
        str[i++] = '0';

    if(neg)
    	str[i++]= '-';

    reverse(str, i);

    str[i] = '\0';

    return i;
}

void ftoa(float n, char* res, int afterpoint){
    // Extract integer part
    int ipart = (int)n;
    float fpart;

    if( n < 0 )
    	// Extract floating part
    	fpart = -(n - (float)ipart);
    else
    	// Extract floating part
    	fpart = n - (float)ipart;

    // convert integer part to string
    int i = itostr(ipart, res, 0);

    // check for display option after point
    if (afterpoint != 0) {
        res[i] = '.';
        fpart = fpart * pow(10, afterpoint);
        itostr((int)fpart, res + i + 1, afterpoint);
    }
}
