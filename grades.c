#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "grades.h"
#include "linked-list.h"

struct grades {
    struct list *student_list;
};

struct student {
	char *name;
	int id;
	struct list *course_list;
};

struct course {
	char *course_name;
	int grade;
};

int clone_student(void*, void**);

void destroy_student(void*);

int clone_course(void*, void**);

void destroy_course(void*);

char *clone_string(char*);

struct node *id_exists(struct list*,int);

/**
 * @brief user-function that clones the student struct for the linked-list to * "output"
 * @param[student] a variable points to the student element we want to clone. 
 * @param[output] address in memory in which we point to the pointer to the   * element we clone
 * @returns always return 0
 */
int clone_student(void *student, void **output){	
	struct student *current_student = (struct student*)student;	
	struct student **new_output = (struct student**)output;
	*new_output = current_student;
	return 0;
}
/**
 * @brief user-function that destroyes a student struct
 * @param[student] a variable points to the student element we want to destroy. 
 */
void destroy_student(void *student){
	if(!student){
		return;
	}
	struct student *current_student = (struct student*)student;
	free(current_student->name);
	free(current_student);
	return;
}

/**
 * @brief user-function that clones the course struct for the linked-list to * "output"
 * @param[course] a variable points to the course element we want to clone. 
 * @param[output] address in memory in which we point to the pointer to the   * element we clone
 * @returns always return 0
 */
int clone_course(void *course, void **output) {
	struct course *cur_course = (struct course*)course;	
	struct course **new_output = (struct course**)output;
	*new_output = cur_course;
	return 0;
}

/**
 * @brief user-function that destroyes a course struct
 * @param[student] a variable points to the course element we want to destroy. 
 */
void destroy_course(void *course) {
	if(!course){
		return;
	}
	struct course *current_course = (struct course*)course;
	free(current_course->course_name);
	free(current_course);
	return;
}

/**
 * @brief allocates a string we want to add to an element in our list.
 * @param[name] a variable points to the name (beggining of string) we want to 
 * add to our element.
 * @returns return a pointer to the allocated block of memory, or NULL on error
 */
char *clone_string(char* name) {
	char *str = (char*)name;
	char *out = (char*)malloc(sizeof(char)*(strlen(str)+1));
	if(!out) {
		return NULL;
	}
	strcpy(out,str);
	return out;
}

/**
 * @brief checks if the new student we want to add to the list, have already added, by comparing the new id with every id that already exists on the list.
 * @param[list] a pointer to beggining of the list.
 * @param[id] the id of the student we want to add to the list.
 * @returns NULL if id does not exists, and a pointer to the student node who * has the same id as the new id, if exists.
 */
struct node *id_exists(struct list* student_list,int id_to_check) {
	struct node *cur_node = list_begin(student_list);
	if(!cur_node) {
		return NULL;  // we have to think what happens if the list have not been initiated.
	}
	while(cur_node) {
		struct student *our_student = list_get(cur_node);
		if(our_student->id == id_to_check) {
			return cur_node;
		}
		cur_node = list_next(cur_node);
	}
	return NULL;
}


struct grades *grades_init() {
	struct grades *out = (struct grades*)malloc(sizeof(struct grades));
	if(!out) {
		return NULL;
	}

	out->student_list = list_init(clone_student,destroy_student);
	if(!out->student_list) {
		free(out);
		return NULL;
	}
	return out;
}

void grades_destroy(struct grades *grades) {
	struct node *cur_node = list_begin(grades->student_list);
	while(cur_node) {
		struct student *our_student = list_get(cur_node);
		list_destroy(our_student->course_list);
		cur_node = list_next(cur_node);
	}
	list_destroy(grades->student_list);
	free(grades);
}


int grades_add_student(struct grades *grades, const char *name, int id){
	if(id_exists(grades->student_list,id)) {
		return 1;
	}
	struct student *new_student = (struct student*)malloc(sizeof(struct student));
	if(!new_student) {
        exit(1);
    }
	new_student->id = id;
	
	char *temp_name = clone_string(name);
	new_student->name = temp_name;
	new_student->course_list = list_init(clone_course,destroy_course);
	if(!(new_student->course_list)) {
		return 1;
	}
	return list_push_back(grades->student_list, new_student);
}


int grades_add_grade(struct grades *grades,
                     const char *name,
                     int id,
                     int grade) {
	struct node *current_student = id_exists(grades->student_list,id);
	if(current_student == 0 || grade<0 || grade >100) {
		return 1;
	} 
	// we have also to check the course name
	struct student *our_student = list_get(current_student);

	struct node *cur_course = list_begin(our_student->course_list);
	while(cur_course) {
		struct course *our_course = list_get(cur_course);
		if(!strcmp(our_course->course_name,name)) {;
			return 1;
		}
		cur_course = list_next(cur_course);
	}
	struct course *new_course = (struct course*)malloc(sizeof(struct course));
	if(!new_course) {
        exit(1);
    }
    char *temp_name = clone_string(name);
    new_course->course_name = temp_name;
    new_course->grade = grade;
	return list_push_back(our_student->course_list, new_course);;
}

float grades_calc_avg(struct grades *grades, int id, char **out) {
	struct node *current_student = id_exists(grades->student_list,id);
	if(!grades || !current_student) {
		out = NULL;
		return -1;
	}
	struct student *our_student = list_get(current_student);
	char **new_out = out;
	char *temp_name = clone_string(our_student->name);
	*new_out = temp_name;  //maybe not with*
	struct node *cur_course = list_begin(our_student->course_list);
	
	if(!cur_course) {
		return 0;
	}
	int number_of_courses = 0;
	float sum = 0;
	while(cur_course) {
		struct course *our_course = list_get(cur_course);
		number_of_courses++;
		sum += our_course->grade;
		cur_course = list_next(cur_course);
	}
	float avg = sum/number_of_courses;
	return avg;
}


int grades_print_student(struct grades *grades, int id) {
	struct node *current_student = id_exists(grades->student_list,id);
	if(!grades || !current_student) {
		return 1;
	}
	struct student *our_student = list_get(current_student);

	printf("%s %d:", our_student->name, our_student->id);
	struct node *cur_course = list_begin(our_student->course_list);
	while(cur_course) {
		struct course *our_course = list_get(cur_course);
		printf(" %s %d", our_course->course_name, our_course->grade);
		cur_course = list_next(cur_course);
		if(cur_course) {
			printf(",");
		}
	}
	printf("\n");
	return 0;
}

int grades_print_all(struct grades *grades) {
	if(!grades) {
		return 1;
	}
	struct node *current_student = list_begin(grades->student_list);
	if(!current_student) {
		return 0;
	}

	while(current_student) {
		struct student *our_student = list_get(current_student);
		grades_print_student(grades,our_student->id);
		current_student = list_next(current_student);
	}
	return 0;
}