//
// Created by fuhchang on 10/7/15.
//

#include "date.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct date{
    int day;
    int month;
    int year;
    char *dateArray;
}Date;
/*
 * date_create creates a Date structure from `datestr`
 * `datestr' is expected to be of the form "dd/mm/yyyy"
 * returns pointer to Date structure if successful,
 *         NULL if not (syntax error)
 */
Date *date_create(char *datestr){
  	int len = strlen(datestr);

    Date *datePtr = (Date *)malloc(sizeof(Date));
    
    int i;
    if(datestr != NULL && datestr[2] == '/' && datestr[5] == '/' && datestr[len] == '\0'){
    	for(i=0; i<len;i++){
    		char checkstr[2] = {datestr[i]};
    		int result = strcmp(checkstr ,"/");
    		if(result == 0){
    			char str[2] = {datestr[i]};
    			int num = atoi(str);
    			if(i == 0){
    				if(num <0 || num > 3){
    					date_destroy(datePtr);
    					return NULL;
    				}
    			}else if(i ==3){
    				if(num <0 || num > 1){
    					date_destroy(datePtr);
    					return NULL;
    				}
    				if(num ==1){
    					char str[2] = {datestr[i+1]};
    					int num = atoi(str);
    					if(num <0 && num >2){
    						date_destroy(datePtr);
    						return NULL;
    					}
    				}
    			}else{
    				if(num < 0 || num >9){
    					date_destroy(datePtr);
    					return NULL;
    			}
    			}
    		}
    	}
    	datePtr->dateArray = datestr;
    	char strYear[5] = {datestr[6],datestr[7],datestr[8],datestr[9]};
		char strMonth[3] = {datestr[3],datestr[4]};
		char strDay[3] = {datestr[0],datestr[1]};
    	datePtr->day = atoi(strDay);
    	datePtr->month = atoi(strMonth);
    	datePtr->year = atoi(strYear);
	    return datePtr;
	}else{
		date_destroy(datePtr);
		return NULL;
    }
    	
    
	}

/*
 * date_duplicate creates a duplicate of `d'
 * returns pointer to new Date structure if successful,
 *         NULL if not (memory allocation failure)
 */
Date *date_duplicate(Date *d){

	Date *datePtr = (Date *)malloc(sizeof(*d));
	
	if(datePtr  != NULL){
		return memcpy(datePtr,d,sizeof(*d));
	}else{
		date_destroy(datePtr);
		return NULL;
	}
}

/*
 * date_compare compares two dates, returning <0, 0, >0 if
 * date1<date2, date1==date2, date1>date2, respectively
 */
int date_compare(Date *date1, Date *date2){
		if(date1->year > date2->year){
			return 1;
		}else if(date1->year < date2->year){
			return -1;
		}else{
			if(date1->month > date2->month){
				return 1;
			}else if(date1->month < date2->month){
				return -1;
			}else{
				if(date1->day > date2->day){
					return 1;
				}else if(date1->day < date2->day){
					return -1;
				}else{
					return 0;
				}
			}
		}
}

/*
 * date_destroy returns any storage associated with `d' to the system
 */
void date_destroy(Date *d){
	free(d);
}

