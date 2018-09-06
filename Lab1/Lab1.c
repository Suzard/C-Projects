//Edward Chen ID #: 88277651
#include<stdio.h>
#include <stdlib.h>
#include <string.h>

//#define MAXCOURSES TOTAL_CLASSES   //max number of courses
#define CHAR_LENGTH 255 //max char per name
#define TOTAL_CLASSES 255 //max char read per line in input file
#define NUMOFQUARTERS 3 //number of quarters per year
#define MAXIMUM_PREREQS 40 //maximum pre_reqs a class can hav changed this to 40 from 10
char *offering_check = NULL;
struct prereq_class{
	char prerequisites[MAXIMUM_PREREQS][CHAR_LENGTH]; //length of 0 means no pre_reqs
	char name[CHAR_LENGTH];
	char quarter[CHAR_LENGTH];
	int recursion_counter;
	int validated;
};
struct required_class{	
	char name[CHAR_LENGTH];
	int graduating;//0 means not taken
};
int check_inside( struct prereq_class *class_data, char offering_word[], int class_counter){//currently not even using
	int iterating = 0;
	int inside = 0;
	for(iterating; iterating < class_counter; iterating++){
		//printf("check inside %s\n", class_data[iterating].name);
		if(strcmp(class_data[iterating].name, offering_word) == 0){
			inside = 1;
			break;
		}
	}
	return inside;
}
void initializing_class(struct prereq_class *class_data, int*counter, char offering_word[], int expected_quarter, char actual_quarter[]){
	int real_quarter = atoi( actual_quarter);
	if(expected_quarter== real_quarter){
		strcpy(class_data[*counter].name,offering_word);
		strcpy(class_data[*counter].quarter,offering_word);
		(*counter)++;
	}
}
void initializing_pre_reqs(struct prereq_class *class_data, char* offering_word, int class_counter){
	int iterating = 0;
	for(iterating = 0; iterating < class_counter; iterating++){
		if(strcmp(class_data[iterating].name, offering_word) == 0){
			int recursion_counter = 0;
			while(1){
				offering_word = strtok(NULL, " \n");
				if(offering_word ==NULL )
					break;
				strcpy(class_data[iterating].prerequisites[recursion_counter],offering_word);
				recursion_counter++;
			}		
			class_data[iterating].recursion_counter = recursion_counter;	
			break;
		}	
	}
}
void recursive_files(struct prereq_class *fall_data,struct prereq_class *winter_data, struct prereq_class *spring_data, int fall_counter, int 
winter_counter, int spring_counter,int *req_class_counter, struct required_class *graduation_status,  int recursion_counter, char initial_recursion[MAXIMUM_PREREQS][CHAR_LENGTH]){
	int end_function = 0; //goal update graduation_status to contain all pre_reqs and pre_reqs.
	int iteration = 0;
	for(iteration; iteration < recursion_counter ; iteration++){
		int valid = 1;
		int inner_loop = 0;
		if(strlen(initial_recursion[iteration]) == 0) //makes sure empty arguments aren't added to pre_reqs
			break;
		for(inner_loop; inner_loop < *req_class_counter; inner_loop++){
			if(strcmp(graduation_status[inner_loop].name, initial_recursion[iteration]) == 0){
				valid = 0;
				break;
			}
		}
		if(valid){
		strcpy(graduation_status[*req_class_counter].name, initial_recursion[iteration]);
		(*req_class_counter)++;
		}
		int prereq_iterating = 0;
		int recursion_counter = 0;
		struct prereq_class *recursive_class;
		int proceed = 1;	
		if(check_inside(fall_data,initial_recursion[iteration], fall_counter)){
			recursive_class = fall_data;
			recursion_counter = fall_counter;
			proceed = 0;
		}
		if(proceed && check_inside(winter_data,initial_recursion[iteration], winter_counter)){
			recursive_class = winter_data;
			recursion_counter = winter_counter;
			proceed = 0;
		}
		if(proceed && check_inside(spring_data,initial_recursion[iteration], spring_counter)){
			recursive_class = spring_data;
			recursion_counter = spring_counter;
		}
		for(prereq_iterating= 0; prereq_iterating<recursion_counter;prereq_iterating++){
			if(strcmp(initial_recursion[iteration], recursive_class[prereq_iterating].name) == 0){
				if(recursive_class[prereq_iterating].validated){
					break;
				}
				recursive_class[prereq_iterating].validated = 1;
				recursive_files(fall_data,winter_data,spring_data, fall_counter,winter_counter,spring_counter,
				 req_class_counter, graduation_status, recursion_counter, recursive_class[prereq_iterating].prerequisites);
			}
		}
	}
}
main(int argc, char *argv[])
{
	if(argc != 4){//makes sure only 3 file arguments
		printf("You should only do 3 files\n");
		return;
	}
	struct prereq_class fall_data[TOTAL_CLASSES]; //fall array struct that contains all the data
	struct prereq_class winter_data[TOTAL_CLASSES]; //winter array struct that contains all the data
	struct prereq_class spring_data[TOTAL_CLASSES]; //spring array struct that contains all the data
	struct required_class graduation_status[TOTAL_CLASSES]; //array struct that contains data on whether student graduates
	int year_number = 1; //if quarter hits 4 increment by 1
	int quarter_number = 1; //if quarter hits 4 reset to 1
	FILE * offering_file;	//storing course file data from offering.txt
	offering_file = fopen(argv[1],"r");
	int fall_counter= 0;
	int winter_counter= 0;
	int spring_counter= 0;
	while(!feof(offering_file)){
		char offering_line[CHAR_LENGTH] = "";
		fgets(offering_line,CHAR_LENGTH, offering_file);
		char *offering_word  = strtok(offering_line, " \n");
		if(offering_word == NULL)
			continue;
		char offering_name[CHAR_LENGTH] = "";
		strcpy(offering_name,offering_word);
		offering_word = strtok(NULL, " \n");
		initializing_class(fall_data,&fall_counter,offering_name,1, offering_word);
		initializing_class(winter_data,&winter_counter,offering_name,2, offering_word);
		initializing_class(spring_data,&spring_counter,offering_name,3, offering_word);
	}
	fclose(offering_file);
	FILE *prereq_file;//storing information about pre_reqs in course data , might have to help store info of pre_reqs as requirement if needed
	prereq_file = fopen(argv[2],"r");
	while(!feof(prereq_file)){
		char prereq_line[CHAR_LENGTH] = "";
		fgets(prereq_line,CHAR_LENGTH, prereq_file);
		char *prereq_word  = strtok(prereq_line, " \n");
		initializing_pre_reqs(fall_data,prereq_line,fall_counter);	
		initializing_pre_reqs(winter_data,prereq_line,winter_counter);
		initializing_pre_reqs(spring_data,prereq_line,spring_counter);
	}
	fclose(prereq_file);
	FILE *req_file;//storing required classes from a requirements file 
	req_file = fopen(argv[3],"r");
	char line[CHAR_LENGTH] = "";
	char initial_recursion[MAXIMUM_PREREQS][CHAR_LENGTH];
	int recursion_counter = 0;
	fgets(line,CHAR_LENGTH, req_file);
	char *word  = strtok(line, " \n");
	if(word == NULL) 
		return;
	int req_class_counter =0;
	while(word != NULL){
		int length = strlen(word)-1;
		if(word[length]== '\n'){
			word[length] = '\0';
		}
		strcpy(graduation_status[req_class_counter].name,word);
		strcpy(initial_recursion[recursion_counter],word);
		word = strtok(NULL, " ");
		recursion_counter++;
		req_class_counter++;
	}
	fclose(req_file);
	recursive_files(fall_data,winter_data,spring_data, fall_counter,winter_counter, spring_counter, &req_class_counter, graduation_status, recursion_counter, initial_recursion);
	char current_quarter[TOTAL_CLASSES][CHAR_LENGTH];
	while (1){//creating schedule
		int checking_graduation = 0;
		int required_classes = 0;
		int graduating = 0;
		int courses_taken = 0;
		for(checking_graduation = 0; checking_graduation < req_class_counter; checking_graduation++){
			if(graduation_status[checking_graduation].graduating == 1){
				graduating++;
			}
		}
		if(graduating == req_class_counter)
			break;
		memset(current_quarter,0,sizeof(current_quarter));
		int class_counter = 0;
		struct prereq_class *class_data;
		if(quarter_number == 1){
			class_data = fall_data;
			class_counter = fall_counter;
		}
		if(quarter_number == 2){
			class_data = winter_data;
			class_counter = winter_counter;
		}
		if(quarter_number == 3){
			class_data = spring_data;
			class_counter = spring_counter;
		}
		for(required_classes = 0; required_classes < class_counter; required_classes++){//searching for required courses for graduation
			int required = 0;
			for(required; required < req_class_counter; required++){
				if(strcmp(class_data[required_classes].name, graduation_status[required].name) == 0){
					if(graduation_status[required].graduating == 0){ //check if not taken
						int takeable = 0;
						int taken = 0;	
						for(takeable; takeable < class_data[required_classes].recursion_counter; takeable++){//iterate throug prereq
							int checking_c = 0;
							for(checking_c; checking_c < req_class_counter; checking_c++){ //check if pre_reqs taken	
								if(strcmp(class_data[required_classes].prerequisites[takeable],
								 graduation_status[checking_c].name) == 0 ){
									if(graduation_status[checking_c].graduating == 1){
										taken++;
									}
								}
							}
						}
						if(taken == class_data[required_classes].recursion_counter){
							strcpy(current_quarter[courses_taken], graduation_status[required].name);
							courses_taken++;
						}
					}
				}
			}
		}
		if(courses_taken != 0){
			printf("%d %d ", year_number, quarter_number);
			int current_class = 0;
			for(current_class = 0; current_class < courses_taken; current_class++){
				int prereq_checking = 0;// must do comparisons here in order to avoid take concurrent classes with pre_reqs
				for(prereq_checking; prereq_checking < req_class_counter; prereq_checking++){
					if(strcmp(current_quarter[current_class], graduation_status[prereq_checking].name) == 0){
						graduation_status[prereq_checking].graduating = 1;
					}
				}
				printf("%s ",current_quarter[current_class]);
			}
			printf("\n");
		}
		quarter_number++;
		if(quarter_number == 4){
			quarter_number =1;
			year_number++;
		}
	}
}

