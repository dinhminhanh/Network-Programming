#include <stdio.h>

struct student {
    char name[20];
    int eng;
    int math;
    int phys;
    double mean;
    char grade;
};

static struct student data[] = {
    {"Tuan", 82, 72, 58, 0.0},
    {"Nam", 77, 82, 79, 0.0},
    {"Khanh", 52, 62, 39, 0.0},
    {"Phuong", 61, 82, 88, 0.0}
};

char grade(double mean){
    if (mean >= 90) return 'S';
    if (mean >= 80) return 'A';
    if (mean >= 70) return 'B';
    if (mean >= 60) return 'C';
    return 'D';
}

void calculate_means(struct student *students, int size) {
    for (int i = 0; i < size; i++) {
        students[i].mean = (students[i].eng + students[i].math + students[i].phys) / 3.0;
        students[i].grade = grade(students[i].mean); 
    }
}

void print(struct student *students, int size){
    for(int i = 0; i < size; i++) {
        printf("Name: %s\n", data[i].name);
        printf("  ENG: %d\n", data[i].eng);
        printf("  MATH: %d\n", data[i].math);
        printf("  PHYS: %d\n", data[i].phys);
        printf("  MEANS: %.2f\n", data[i].mean);
        printf("  GRADE: %c\n\n", students[i].grade);
      }
}


int main(void) {

    int size = sizeof(data) / sizeof(data[0]); 
    calculate_means(data, size); 
    print(data, size);
    return 0;
}