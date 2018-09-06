//Edward Chen ID #: 88277651
#include <string.h>
#include<stdio.h>
#include<time.h>
#include<unistd.h>
#include<stdlib.h>
#include <sys/mman.h>
#include<sys/types.h>
#define TOTAL 10
#define CHAR_LENGTH 255
#define MAX_LENGTH 10
int bin_val(int a[], int end, int start, int search,int *count, int *count1, int *count2){
  if(abs(start -end) >= 10){
	printf("-1\n");
	return -1;
  }
  if(start != end){
    pid_t child = fork();
    wait();
    if(child != 0){
    	return bin_val(a, end, ((start + end)/2)+1,search,count, count1,count2);
    }
    else{
	return bin_val(a,(start+end)/2, start, search,count,count1,count2);
    }
  }
  else{//means only one value i think
    if(a[end] == search){
	*count1 = end;
	printf("pid %d, value: %d\n",getpid(),a[start]);
	(*count)++;
        if(*count == *count2){
	       printf("Search output: %d\n",*count1);
        }
        return end;
    }
    else{
        printf("pid %d, value: %d\n",getpid(),a[start]);
        (*count)++;
        if(*count == *count2){
	       printf("Search output: %d\n",*count1);
        }
  	exit(0);
	return -1;
    }
  } 
  exit(0);
  return -1;
 
}
int main(int argc, char *argv[]){
	int val = atoi(argv[2]);
	int prereq[MAX_LENGTH];
	FILE * offering_file;
	offering_file = fopen(argv[1],"r");
	int counter = 0;
	while(!feof(offering_file)){
		char offering_line[CHAR_LENGTH] = "";
		fgets(offering_line,CHAR_LENGTH, offering_file);
		char *offering_word  = strtok(offering_line, " \n");
		while(offering_word != NULL || offering_word != '\0'){
		prereq[counter] = atoi(offering_word);
		counter++;
		offering_word = strtok(NULL, " \n");
		}
	}
	int counting = 0;
        int * shared = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
        int * shared1 = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	*shared1 = -1;
        int * shared3 = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	*shared3 = counter;
	if(counter == 0){
		printf("-1\n");
		return 0;
	}
	bin_val(prereq,counter-1, 0,val,shared, shared1,shared3);
	return 0;
}
