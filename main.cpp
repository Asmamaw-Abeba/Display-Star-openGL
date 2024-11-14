 #include<GL/glut.h>
void myInit() {
    glClearColor(0.5,0.5,0.5,0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0,680,680,0);
}
void myDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0,1.0,0.0);
    glLineWidth(2);

     glBegin(GL_LINES);
     {   // THE HEIGHT OF THE FLAG
         glVertex2i(150,1);
         glVertex2i(150,615);

         //UPPER ATTACHER
         glVertex2i(150,1);
         glVertex2i(160,10);

          // LOWER ATTACHER
         glVertex2i(150,250);
         glVertex2i(160,240);

         // THE HEIGHT OF ROPE ENDED
         glVertex2i(152,1);
         glVertex2i(152,600);
     }
      glEnd();

      // HERE FOR BORDERS OF FLAG
    glBegin(GL_LINES);
     {

         glVertex2i(160,10);
         glVertex2i(160,240);

         glVertex2i(480,10);
         glVertex2i(480,240);

         glVertex2i(160,10);
         glVertex2i(480,10);

         glVertex2i(160,80);
         glVertex2i(480,80);

         glVertex2i(160,160);
         glVertex2i(480,160);

         glVertex2i(160,240);
         glVertex2i(480,240);


     }
      glEnd();


   // HERE FOR STAR
    glBegin(GL_LINE_LOOP);
    {
       glVertex2i(320,80);
       glVertex2i(275,160);
       glVertex2i(380,110);
       glVertex2i(260,110);
       glVertex2i(355,160);

     }
    glEnd();


    // HERE FOR STEPS
    glBegin(GL_LINE_STRIP);
     { // UPPER STEP
        glVertex2i(100,615);
        glVertex2i(200,615);
        glVertex2i(200,640);
        glVertex2i(100,640);
        glVertex2i(100,615);
     }
    glEnd();



    glBegin(GL_LINE_STRIP);
     { // LOWER STEP
        glVertex2i(50,640);
        glVertex2i(250,640);
        glVertex2i(250,680);
        glVertex2i(50,680);
        glVertex2i(50,640);
     }
      glEnd();

    glFlush();
}

int main(int argc, char *argv[]){
    glutInit(&argc,argv);
    glutInitWindowSize(640,640);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutCreateWindow("Draw a star on FLAG");
    glutDisplayFunc(myDisplay);
    myInit();
    glutMainLoop();

    return 0;
}
