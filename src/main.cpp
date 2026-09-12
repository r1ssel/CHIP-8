#include <iostream>
#include "chip8.h"
#include <GL/glut.h>

chip8 myChip;

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
}

int main(int argc, char** argv) {
    static bool init = true;
    if (init){
        init = false;
        printf("Init...\r\n");
        printf("Init completed\r\n");
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(640, 480);
    glutCreateWindow("Chip8");
    glutDisplayFunc(display);
    glutMainLoop();

    myChip.emulateCycle();
    
    return 0;
}