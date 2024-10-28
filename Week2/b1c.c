#include <stdio.h>

typedef struct student {
    char name[20];
    int eng;
    int math;
    int phys;
} STUDENT;

STUDENT data[] = {
    {"Tuan", 82, 72, 58},
    {"Nam", 77, 82, 79},
    {"Khanh", 52, 62, 39},
    {"Phuong", 61, 82, 88}
};

int main() {
    STUDENT *p;
    int size = sizeof(data) / sizeof(data[0]);
    for (p = data; p < data + size; p++) {
        printf("Name: %s\n", p->name);
        printf("  ENG: %d\n", p->eng);
        printf("  MATH: %d\n", p->math);
        printf("  PHYS: %d\n\n", p->phys);
    }
    return 0;
}
