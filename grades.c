#include <stdlib.h>
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
struct node *id_exists(struct list*,int);


int clone_student(void *student, void **output){	
	struct student *current_student = (struct student*)student;	
	struct student **new_output = (struct student**)output;
	*new_output = current_student;
	printf("finished clone student\n");
	return 0;
}

void destroy_student(void *student){
	if(!student){
		return 0;
	}
	struct student *current_student = (struct student*)student;
	free(current_student);
	return student;
}


int clone_course(void *course, void **output) {
	struct course *cur_course = (struct course*)course;	
	struct course **new_output = (struct course**)output;
	*new_output = cur_course;
	printf("finished clone course\n");
	return 0;
}


void destroy_course(void *course) {
	if(!course){
		return 0;
	}
	struct course *current_course = (struct course*)course;
	free(current_course);
	return course;
}



struct node *id_exists(struct list* student_list,int id_to_check) {
	//printf("entered id_exists\n");
	//I think we have to do malloc here
	struct node *cur_node = list_begin(student_list);
	//printf("stepped in id_exists\n");
	//printf("first cur_node = %lx\n", cur_node);
	if(!cur_node) {
		printf("student list does not work\n");
		return NULL;  // we have to think what happens if the list have not been initiated.
	}
	while(cur_node) {
		//struct student *our_student = list_get(cur_node);
		//void *temp = list_get(cur_node);
		//printf("temp = %lx\n",temp);
		struct student *our_student = list_get(cur_node);
		printf("our_student name in id_exists is %s\n",our_student->name);
		if(our_student->id == id_to_check) {
			return cur_node;
		}
		cur_node = list_next(cur_node);
		printf("cur_node = %lx\n",cur_node);
	}
	return NULL;
}


struct grades *grades_init() {
	struct grades *out = (struct grades*)malloc(sizeof(struct grades));
	printf("%lx\n",out);
	if(!out) {
		return NULL;
	}
	/*
	we are not sure of this malloc or the ** and we done this late at night!
	*/
	/*struct list **new_list = (struct list**)malloc(sizeof(struct list*));
	if(!new_list) {
		return NULL;
	}
	*new_list = list_init(clone_student,destroy_student);*/
	/*out->student_list = (struct list*)malloc(sizeof(struct list));
	if(!out->student_list) {
		return NULL;
	}*/
	//*new_list = list_init(clone_student,destroy_student);
	out->student_list = list_init(clone_student,destroy_student);
	printf("***** %lx\n",out->student_list);
	if(!out->student_list) {
		return NULL;
	}
	//printf("init!!!!\n");
	return out;
}

void grades_destroy(struct grades *grades) {
	// do we have to destroy every students' list itself?
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
	//printf("entered to add student\n");
	if(id_exists(grades->student_list,id)) {
		printf("id_exists!=0\n");
		return 1;
	}
	//printf("after id_exists\n");
	struct student *new_student = (struct student*)malloc(sizeof(struct student));
	printf("%lx\n",new_student);
	if(!new_student) {
        exit(1);
    }
    printf("the id we entered is %d\n",id);
	new_student->id = id;
	printf("and the new student id is %d\n",new_student->id);
	new_student->name = name;
	printf("finally, the student name is %s\n",new_student->name);
	new_student->course_list = list_init(clone_course,destroy_course);
	if(!(new_student->course_list)) {
		printf("course_list=0\n");
		return 1; //list initiation failed
	}
	int x = list_push_back(grades->student_list, new_student);
	printf("%d\n",x);
	printf("***** %lx\n",grades->student_list);
	return 0;
}


int grades_add_grade(struct grades *grades,
                     const char *name,
                     int id,
                     int grade) {
	struct node *current_student = id_exists(grades->student_list,id);
	if(current_student == 0 || grade<0 || grade >100) {
		return 1;
	} // we have also to check the course name
	struct student *our_student = list_get(current_student);

	struct node *cur_course = list_begin(our_student->course_list);
	while(cur_course) {
		struct course *our_course = list_get(cur_course);
		if(our_course->course_name == name){
			return 1;
		}
		cur_course = list_next(cur_course);
	}
	struct course *new_course = (struct course*)malloc(sizeof(struct course));
	if(!new_course) {
        exit(1);
    }
    new_course->course_name = name;
    new_course->grade = grade;
    int x = list_push_back(our_student->course_list, new_course);
	printf("course add returns %d\n",x);
	return 0;
}

float grades_calc_avg(struct grades *grades, int id, char **out) {
	struct node *current_student = id_exists(grades->student_list,id);
	if(!grades || !current_student) {
		out = NULL;
		return -1;
	}
	struct student *our_student = list_get(current_student);

	out = (char*)malloc(sizeof(char)*(strlen(our_student->name)+1));
	*out = our_student->name;  //maybe not with*
	struct node *cur_course = list_begin(our_student->course_list);
	
	if(!cur_course) {
		return 0;
	}
	int number_of_courses = 0;
	int sum = 0;
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

	printf("%s %d: ", our_student->name, our_student->id);
	struct node *cur_course = list_begin(our_student->course_list);
	while(cur_course) {
		struct course *our_course = list_get(cur_course);
		printf("%s %d, ", our_course->course_name, our_course->grade);
		cur_course = list_next(cur_course);
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
		printf("%lx,  first student name is %s, and his id is %d\n",our_student,our_student->name,our_student->id);
		grades_print_student(grades,our_student->id);
		current_student = list_next(current_student);
	}
	return 0;
}

