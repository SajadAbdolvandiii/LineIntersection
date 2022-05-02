/*
----------
STUDENT : Sajad Abdolvandi
SECTION : 01
HOMEWORK: homework 2
----------
PROBLEMS: NO PROBLEMS!  AS YOU REMEMBER YOU SHOULD ADD YOUR GLUT LIBRARY AND FIX "Vprint_s" functions!
 
 
If you found a problem or bug i would be happy to see and fix it, But if you find :)
----------
ADDITIONAL FEATURES:
It has a loading animation in the board with a cool algorithm and structure! (The most complicated);
It has  lots of fade out and fade in animations!
it has X and Y limitations (end point or start point do not overlap each other or overflow the window)
You can reset the app by pressing "r" key
You can't change the mode while the first animation is running!
You can click the exit ("esc") button and see the closing animation!
You can't close the app while first animation is running
It has Chart Openning animation
I used Antialiasing with GL_SMOOTH for lines + i used "GL_POINTS" to have smooth circles instead of "circle function"
I wanted to use a modern user interface so i did NOT used two color for grid BUT i used gray and black as two colors.
It has a lot of "Error warning" and "status" messgaes for the user and they DON'T OVERLAP EACH OTHER (they change for each mode and they fade out)
You can see a transparent "line" and "set point" before setting the point !
 line between point would not have color before the START and END point set!
*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GLUT/GLUT.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define WINDOW_WIDTH  600
#define WINDOW_HEIGHT 660
#define TIMER_PERIOD   1// Period for the timer.
#define TIMER_ON       1 // 0:disable timer, 1:enable timer
#define D2R 0.0174532



#define LOAD 0
#define START 1
#define END 2
#define LINES 3
#define NONE 4
#define MovingStart 5
#define MovingEnd 6
#define StillStart 7
#define StillEnd 8
int mode = LOAD;
int Smode = NONE;
int Emode = NONE;
int MovingX,MovingY;
int chartX = 50;
int chartY = 50;
int myflag = 0;
typedef struct {
    int chXL;
    int chYU;
    int chXR;
    int chYD;
}Chart;
typedef struct{
    int x,y;
} point;
point startPos;
point endPos;
point Mouse;
int Mycounter = 0;
int krow = 0,kcol = 0;
Chart myChart[14][20];
int flag = 0;
int flag2 = 0;
int mytime = 0;
int myrow = 0, mycol = 0;
int keyflag[3] = {0};
float myred,myblue,mygreen;
float ChartOpac = 0;
double myOpac = 100;
double oppp;
typedef struct{
    int firstPointX;
    int firstPointY;
    int secondPointX;
    int secondPointY;
} myLines;
enum LineMoode{
    NOPOINT = 0,
    FIRSTPOINT =1,
    SECONDPOINT = 2
};
int lineMode = NOPOINT;
int lineCounter = 0;
int DotCounter = 0;
point lineArray[200];
bool reset = false;
double opInvert =0;
double resetOp =-1;
bool exitBool = false;
double rectUpY =330;
double rectDownY = -330;
double chartRectU = 0,chartRectD = 0;
bool MyWeirdBool2 = false;
/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height
bool MyWeirdBool = false;
double weird =1;
double ClickOp  =1;


//
// to draw circle, center at (x,y)
// radius r
//
void circle( int x, int y, int r )
{
#define PI 3.1415
   float angle ;
   glBegin( GL_POLYGON ) ;
   for ( int i = 0 ; i < 100 ; i++ )
   {
      angle = 2*PI*i/100;
      glVertex2f( x+r*cos(angle), y+r*sin(angle)) ;
   }
   glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
   float angle;

   glBegin(GL_LINE_LOOP);
   for (int i = 0; i < 100; i++)
   {
      angle = 2 * PI*i / 100;
      glVertex2f(x + r*cos(angle), y + r*sin(angle));
   }
   glEnd();
}

void print(int x, int y, const char *string, void *font )
{
   int len, i ;

   glRasterPos2f( x, y );
   len = (int) strlen( string );
   for ( i =0; i<len; i++ )
   {
      glutBitmapCharacter( font, string[i]);
   }
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void *font, const char *string , ...)
{
   va_list ap;
   va_start ( ap, string );
   char str[1024] ;
   vsprintf( str, string, ap ) ;
   va_end(ap) ;
   
   int len, i ;
   glRasterPos2f( x, y );
   len = (int) strlen( str );
   for ( i =0; i<len; i++ )
   {
      glutBitmapCharacter( font, str[i]);
   }
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char *string, ...) {
   va_list ap;
   va_start(ap, string);
   char str[1024];
   vsprintf(str, string, ap);
   va_end(ap);
   glPushMatrix();
      glTranslatef(x, y, 0);
      glScalef(size, size, 1);
      
      int len, i;
      len = (int)strlen(str);
      for (i = 0; i<len; i++)
      {
         glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
      }
   glPopMatrix();
}

//
// To display onto window using OpenGL commands
//
bool onSegment(point p, point q, point r)
{
    if (q.x <= MAX(p.x, r.x) && q.x >= MIN(p.x, r.x) && q.y <= MAX(p.y, r.y) && q.y >= MIN(p.y, r.y)){
       return true;
    }
    return false;
}
int orientation(point p, point q, point r)
{
    int val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
    if (val == 0) return 0;
    
    return (val > 0)? 1: 2;
}
// p1 = start
// p2 end
// p1 lineArray[i]
// p2 = linearray[i+1]

bool doIntersect(point p1, point q1, point p2, point q2)

{
   
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);
    if (o1 != o2 && o3 != o4)
        return true;
    if (o1 == 0 && onSegment(p1, p2, q1)) return true;
    if (o2 == 0 && onSegment(p1, q2, q1)) return true;
    if (o3 == 0 && onSegment(p2, p1, q2)) return true;
    if (o4 == 0 && onSegment(p2, q1, q2)) return true;
    return false;
}

void dispUpBar(){
    glColor4f(0, 0, 0,opInvert);
    glRectd(-300, 330, 300, 300);
    glColor4f(1, 1, 1,opInvert);
    vprint(-80, 310, GLUT_BITMAP_HELVETICA_12, "HW #2 – 2020 – 2021 Spring");
    vprint(-290, 310, GLUT_BITMAP_HELVETICA_12, "Sajad-22001504");

}
void dispDownBar(){
    glColor4f(0, 0, 0,opInvert);
    glRectd(-300, -300, 300, -330);
    glColor4f(1, 1, 1,opInvert);
    vprint(-270, -318, GLUT_BITMAP_HELVETICA_10, "F1:  SET START POSITION");
    vprint(-120, -318, GLUT_BITMAP_HELVETICA_10, "F2:  SET END POSITION"); //-40  -100
    vprint(20, -318, GLUT_BITMAP_HELVETICA_10, "F3:  SET LINES");// -220 // 0
    vprint(120, -318, GLUT_BITMAP_HELVETICA_10, "R-r: RESET");
    vprint(210, -318, GLUT_BITMAP_HELVETICA_10, "ESC: CLOSE");
    switch (mode) {
            
        case LOAD:
           
            break;
            case START:
            glColor4f(1, 1, 1,0.6);
            glPointSize(20.0);
            glBegin(GL_POINTS);
            glVertex2f(-264, -314);
            glEnd();
            
               
            break;
        case END:
            glColor4f(1, 1, 1,0.6);
            glPointSize(20.0);
            glBegin(GL_POINTS);
            glVertex2f(-113, -314);
            glEnd();
            
            break;
        case LINES:
            glColor4f(1, 1, 1,0.6);
            glPointSize(20.0);
            glBegin(GL_POINTS);
            glVertex2f(27, -314);
            glEnd();
            
            break;
        default:
            break;
    }


}



void dispChart(){
   
  
    for (int i = 0; i<=12; i++) {
        glLineWidth(1);
        glBegin(GL_LINES);
        if (i%2 ==0) {
        glColor4f(0, 0, 0, 0.5);

        }else
        glColor4f(0, 0, 0, 1);
        glVertex2f(300-(i*50), 300);
        glVertex2f(300-(i*50), -300);
        glEnd();
    }

    for (int j = 0; j<=12; j++) {
        glLineWidth(1);
        glBegin(GL_LINES);
        if (j%2 ==0) {
        glColor4f(0, 0, 0, 0.5);

        }else
        glColor4f(0, 0, 0, 1);        glVertex2f(-300, -300+(j*50));
        glVertex2f(300, -300+(j*50));
        glEnd();
    }
    
  glColor3f(1, 1, 1);
   glRectd(-300, 330, 300, chartRectU);
   glRectd(-300, -330, 300, chartRectD);

}
void dispLoadingAnim(){
    if ((myrow <13 || mycol <13) && flag2 != 1) {
        
        glRectd(myChart[myrow][mycol].chXL+10, myChart[myrow][mycol].chYU-10, myChart[myrow][mycol].chXR-10, myChart[myrow][mycol].chYD+10);// center
        for (int i = 0; i<12; i++) {

            glRectd(myChart[i][mycol].chXL+10, myChart[i][mycol].chYU-10, myChart[i][mycol].chXR-10, myChart[i][mycol].chYD+10);
            glRectd(myChart[myrow][i].chXL+10, myChart[myrow][i].chYU-10, myChart[myrow][i].chXR-10, myChart[myrow][i].chYD+10);//row
        }
    
    }
}
void CloseAnim(){
    if (exitBool == true) {
        glColor3f(0, 0, 0);
        glRectd(-300, 300, 300, rectUpY);
        glRectd(-300, -300, 300, rectDownY);
    }
}
void StartPointFunc(){
    glColor3f(0.2, 0.2, 0.2);
    glPointSize(100.0);
    glBegin (GL_POINTS);
    glVertex2f (startPos.x, startPos.y);
    glEnd ();
    glColor3f(1, 1, 1);
    vprint(startPos.x-6, startPos.y-8, GLUT_BITMAP_TIMES_ROMAN_24, "S");
    
   
}
void EndPointFunc(){
    glColor3f(0.2, 0.2, 0.2);
    glPointSize(10.0);
    glPointSize(100.0);
    glBegin (GL_POINTS);
    glVertex2f (endPos.x, endPos.y);
    glEnd ();
    glColor3f(1, 1, 1);
    vprint(endPos.x-6, endPos.y-8, GLUT_BITMAP_TIMES_ROMAN_24, "E");
}
void DispLineBetween(){
    if (endPos.x!= -999) {
        glLineWidth(5);
        glBegin(GL_LINES);
        glColor4f(0, 0, 0,0.7);
        glVertex2f(startPos.x, startPos.y);
        glVertex2f(endPos.x, endPos.y);
        glEnd();
        if (mode == START) {
            EndPointFunc();
        }
    }
}
void DispMyLines(){
    if (DotCounter%2 == 0) {
        for (int i = 0; i<DotCounter; i+=2) {
            glLineWidth(3);
            if(startPos.x != -999 && endPos.x != -999){
                
                   if (doIntersect(startPos, endPos, lineArray[i], lineArray[i+1]))
                       glColor4f(1, 0, 0,0.7);
                   else
                       glColor4f(0, 0.8, 0,0.7);
              
            }else
                glColor4f(0, 0, 0, 1);
             
           
                glBegin(GL_LINES);
                glVertex2f(lineArray[i].x, lineArray[i].y);
                glVertex2f(lineArray[i+1].x, lineArray[i+1].y);
                glEnd();
            
        }
    }else{
        for (int i = 0; i<DotCounter-2; i+=2) {
            glLineWidth(3);
            if(startPos.x != -999 && endPos.x != -999){
                
                   if (doIntersect(startPos, endPos, lineArray[i], lineArray[i+1]))
                       glColor4f(1, 0, 0,0.7);
                   else
                       glColor4f(0, 0.8, 0,0.7);
              
            }else
                glColor4f(0, 0, 0, 1);
             
           
            
                glBegin(GL_LINES);
                glVertex2f(lineArray[i].x, lineArray[i].y);
                glVertex2f(lineArray[i+1].x, lineArray[i+1].y);
                glEnd();
            
        }
    }
   

}
void DispMyDots(){
    for (int i = 0; i<DotCounter; i++) {
        glPointSize(8.0);
        glColor3f(0, 0, 0);
        glBegin(GL_POINTS);
        glVertex2f(lineArray[i].x, lineArray[i].y);
        glEnd();
    }
}
void ModeLines(){
    if (lineArray[0].x == -999 && lineArray[1].x == -999) {
        DispMyDots();
    }else{
        DispMyLines();
        DispMyDots();

    }
}
void MovingS(){
   
    if (endPos.x!= -999) {
        glLineWidth(5);
        glBegin(GL_LINES);
        glColor4f(0, 0, 0,0.2);
        glVertex2f(MovingX, MovingY);
        glVertex2f(endPos.x, endPos.y);
        glEnd();
        if (mode == START) {
            EndPointFunc();
        }
    }
    glColor4f(0.2, 0.2, 0.2,0.4);
    glPointSize(100.0);
    glBegin (GL_POINTS);
    glVertex2f (MovingX, MovingY);
    glEnd ();
    glColor3f(1, 1, 1);
    vprint(MovingX-6, MovingY-8, GLUT_BITMAP_TIMES_ROMAN_24, "S");

    
    
}
void MovingE(){
   
        glLineWidth(5);
        glBegin(GL_LINES);
        glColor4f(0, 0, 0,0.2);
        glVertex2f(startPos.x, startPos.y);
        glVertex2f(MovingX, MovingY);
        glEnd();
        if (mode == START) {
            EndPointFunc();
        }
    
    glColor4f(0.2, 0.2, 0.2,0.4);
    glPointSize(100.0);
    glBegin (GL_POINTS);
    glVertex2f (MovingX, MovingY);
    glEnd ();
    glColor3f(1, 1, 1);
    vprint(MovingX-6, MovingY-8, GLUT_BITMAP_TIMES_ROMAN_24, "E");
  
    
}
void display() {
   //
   // clear window to black
   //
   glClearColor(1, 1, 1, 1);
   glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0, 0, 0);
    glEnable( GL_LINE_SMOOTH );
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);
    glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
    Mouse.x =MovingX;
    Mouse.y = MovingY;
    if (flag2 != 1) {
     
        vprint(-20, -320, GLUT_BITMAP_HELVETICA_12, "Loading...");
    }
    dispLoadingAnim();

    if (flag2 ==1) {

        dispChart();
        dispUpBar();
        dispDownBar();

    }
    if (rectUpY <=-200) {
        exit(0);
    }
    oppp = myOpac/100;
    if (flag2 == 1) {
        switch (mode) {
                
            case LOAD:
                if (endPos.x == -999 && opInvert>=1 ) {
                    glColor4f(0, 0, 0,oppp);
                    vprint(-210, 270, GLUT_BITMAP_HELVETICA_18, "You CAN'T add end point without any start points!");
                    
                }
                if(MyWeirdBool2 == true && opInvert >= 1){
                    glColor4f(0, 0, 0,ClickOp);
                    vprint(-60, 270, GLUT_BITMAP_HELVETICA_18, "Select Mode!");
                }
                break;
                case START:
                if (endPos.x != -999) {
                    EndPointFunc();
                }
                
                if (endPos.x == -999 && opInvert>=1 && MyWeirdBool == true) {
                    glColor4f(0, 0, 0,oppp);
                    vprint(-210, 240, GLUT_BITMAP_HELVETICA_18, "You CAN'T add end point without any start points!");
                    
                }
                    ModeLines();
                    glColor4f(0, 0, 0,oppp);
                    vprint(-105, 270, GLUT_BITMAP_HELVETICA_18, "Click to set START point!");
                    DispLineBetween();
                    StartPointFunc();
                MovingS();
                break;
            case END:

                if (startPos.x == -999) {
                    glColor4f(0, 0, 0,oppp);
                    vprint(-210, 270, GLUT_BITMAP_HELVETICA_18, "You CAN'T add end point without any start points!");

                }else{
                    ModeLines();
                    MovingE();

                glColor4f(0, 0, 0,oppp);
                vprint(-95, 270, GLUT_BITMAP_HELVETICA_18, "Click to set END point!");
                DispLineBetween();
                StartPointFunc();
                EndPointFunc();
                }
                break;
            case LINES:
                ModeLines();
                DispLineBetween();
                StartPointFunc();
                EndPointFunc();
                glColor4f(0, 0, 0,oppp);
                vprint(-95, 270, GLUT_BITMAP_HELVETICA_18, "Click to set LINE point!");
                break;
            default:
                break;
        }
    }
    if (resetOp >=0 && opInvert >=1) {
        glColor4f(0, 0, 0,resetOp);
        vprint(-40, 270, GLUT_BITMAP_HELVETICA_18, "Reseting...");
    }
    if (flag2 == 1) {
        CloseAnim();

    }
    glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y )
{
   // exit when ESC is pressed.
    if ( key == 27  && flag2 == 1){
        exitBool = true;
    }
    if (key == 114 || key == 82) {
        mode = LOAD;
        for (int i= 0; i<2; i++) {
            endPos.x = -999;
            endPos.y = -999;
            startPos.y = -999;
            startPos.x = -999;
        }
        for (int i = 0; i<lineCounter; i++) {
            lineArray[i].x = -999;
            lineArray[i].y = -999;

        }
        lineCounter = 0;
        DotCounter = 0;
        resetOp = 1;
        oppp = 0;
        ClickOp = 0;
        myOpac = 0;
        reset = true;
    }
    
   // to refresh the window it calls display() function
}

void onKeyUp(unsigned char key, int x, int y )
{
   // exit when ESC is pressed.
   
    
   // to refresh the window it calls display() function
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//

void onSpecialKeyDown( int key, int x, int y )
{
   // Write your codes here.
   switch (key) {
   case GLUT_KEY_UP: up = true; break;
   case GLUT_KEY_DOWN: down = true; break;
   case GLUT_KEY_LEFT: left = true; break;
   case GLUT_KEY_RIGHT: right = true; break;
       
   }
    if (key == GLUT_KEY_F1  && mode != START && flag2 == 1 && opInvert >=1) {
        mode = START;
        Smode = MovingStart;
        myOpac = 100;

    }else if (key == GLUT_KEY_F1  && mode == START && opInvert >=1){
        //mode = LOAD;
        /*
        for (int i= 0; i<2; i++) {
            endPos[i] = -999;
            startPos[i] = -999;
        }
*/
        myOpac = 100;
        oppp = 0;
    }
    /*
    if (key == GLUT_KEY_F2 && mode != END && mode !=LOAD && flag2 == 1&& opInvert >=1 &) {
        mode= END;
        myOpac = 100;


    }
     */
    else if(key == GLUT_KEY_F2 && mode == END&& opInvert >=1){
        //mode= LOAD;
        /*
        for (int i= 0; i<2; i++) {
            endPos[i] = -999;
            startPos[i] = -999;
        }
         */
        myOpac = 100;
        oppp = 0;


    }
    if (key == GLUT_KEY_F3 && mode != LINES && flag2 == 1&& opInvert >=1) {
        mode= LINES;
        myOpac = 100;


    }else if(key == GLUT_KEY_F3 && mode == LINES&& opInvert >=1){
        //mode = LOAD;
        /*
        for (int i= 0; i<2; i++) {
            endPos[i] = -999;
            startPos[i] = -999;
        }
         */
     
        myOpac = 100;
        oppp = 0;

    }
    
    if(key == GLUT_KEY_F2 && mode == LOAD && opInvert >=1 && flag2 == 1){
        myOpac = 100;
        oppp = 0;
    }
    if(key == GLUT_KEY_F2 && mode == LINES && opInvert >=1 && flag2 == 1&& endPos.x == -999){
        myOpac = 100;
        oppp = 0;
    }
    if(key == GLUT_KEY_F2 && mode == LINES && opInvert >=1 && flag2 == 1 && endPos.x != -999 ){
        mode  = END;
        Emode = MovingEnd;
        myOpac = 100;
        oppp = 0;
    }
    if (mode == START && startPos.x == -999 && key == GLUT_KEY_F2 &&  flag2 == 1&& opInvert >=1) {
        myOpac = 100;
        oppp = 0;
        MyWeirdBool = true;
        Emode = MovingEnd;

    }
    if (mode == START && startPos.x != -999 && key == GLUT_KEY_F2 &&  flag2 == 1&& opInvert >=1) {
        mode = END;
        Emode = MovingEnd;

        myOpac = 100;
    }
   
    
    if (mode == START && key == GLUT_KEY_F1 &&  flag2 == 1&& opInvert >=1) {
        MyWeirdBool = false;
        ClickOp = 1;
        Smode = MovingStart;

    }


   // to refresh the window it calls display() function
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp( int key, int x, int y )
{
   // Write your codes here.
   switch (key) {
   case GLUT_KEY_UP: up = false; break;
   case GLUT_KEY_DOWN: down = false; break;
   case GLUT_KEY_LEFT: left = false; break;
   case GLUT_KEY_RIGHT: right = false; break;
   }
   
   // to refresh the window it calls display() function
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//        8 -265
double DistFromStart(){
    double cx =abs(MovingX-startPos.x) ;
    double cy = abs(MovingY-startPos.y) ;
    double res = pow(cx, 2) + pow(cy, 2);
    return (sqrt(res));
}
double DistFromEnd(){
    double cx =abs(MovingX-endPos.x) ;
    double cy = abs(MovingY-endPos.y) ;
    double res = pow(cx, 2) + pow(cy, 2);
    return (sqrt(res));
}

void onClick( int button, int stat, int x, int y )
{
   // Write your codes here.

    if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN &&mode == START && MovingY >= -270 && MovingY <= 235 && DistFromEnd() >75 && MovingX > -270 && MovingX < 270 ) {
       
  
                Smode = StillStart;
                startPos.x= MovingX;
                startPos.y =  MovingY;
                
            
    }
    
    if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN && mode == END && MovingY >= -270  && DistFromStart() >65 && MovingX > -270 && MovingY < 235 && MovingX < 270 && startPos.x != -999) {
        Emode = StillEnd;
        endPos.x= MovingX;
        endPos.y =  MovingY;
    }
   
    if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN && mode == LINES && MovingY >= -270 && MovingY < 265 && MovingX > -270 && MovingX < 270 && lineMode == FIRSTPOINT && DistFromStart() >35 && DistFromEnd() >35) {
        lineArray[DotCounter].x = MovingX;
        lineArray[DotCounter].y = MovingY;
        lineMode = SECONDPOINT;
        DotCounter++;
        lineCounter++;
    }
    if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN && mode == LINES && MovingY >= -270 && MovingY < 265  && MovingX > -270 && MovingX < 270 && lineMode == NOPOINT && DistFromStart() >35 && DistFromEnd() >35 ) {
        lineArray[DotCounter].x = MovingX;
        lineArray[DotCounter].y = MovingY;
        DotCounter++;
        lineMode = FIRSTPOINT;
    }
    if (lineMode == SECONDPOINT) {
        lineMode = NOPOINT;
    }
    if(mode == LOAD && button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN){
        ClickOp = 1;
        MyWeirdBool2 = true;
    }

   
   // to refresh the window it calls display() function
}


//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize( int w, int h )
{
   winWidth = w;
   winHeight = h;
   glViewport( 0, 0, w, h ) ;
   glMatrixMode( GL_PROJECTION );
   glLoadIdentity();
   glOrtho( -w/2, w/2, -h/2, h/2, -1, 1);
   glMatrixMode( GL_MODELVIEW);
   glLoadIdentity();
   display(); // refresh window.
}

void onMoveDown( int x, int y ) {
   // Write your codes here.


   
   // to refresh the window it calls display() function
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove( int x, int y ) {
   // Write your codes here.
    MovingX = x - winWidth / 2;
    MovingY = winHeight / 2 - y;
    
   
    
    glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer( int v ) {
    glutTimerFunc( TIMER_PERIOD, onTimer, 0 ) ;

   // Write your codes here.
    if (myrow<12 && flag  == 0 && mytime%3 == 0) {
        myrow++;
        mycol++;
       
    }if (myrow >=12) {
        flag  = 1;
    }
    if (flag == 1 && myrow<=12 && mytime%3 == 0) {
        myrow--;
        mycol--;
    }if(myrow == -1 && flag == 1){
        flag = 0;
        flag2 = 1;
    }
    mytime++;
    if (mode !=LOAD ) {
        myOpac-=1;
    }
    if (flag2 == 1) {
        opInvert+=.01;
        ClickOp-= 0.01;
        if(chartRectD > -300){
            chartRectD-=5;
            chartRectU+=5;
        }
    }
    if (reset == true) {
        resetOp-=.05;

    }
    if (exitBool == true) {
        rectUpY -= 10;
        rectDownY+=10;
    }
    if (MyWeirdBool == true) {
        weird-=0.01;
    }
    if (mode == LOAD) {
        myOpac-=1;
    }
    if(flag2 == 0)
        ChartOpac+=0.01;
   // to refresh the window it calls display() function
   glutPostRedisplay() ; // display()

}
#endif

void Init() {
   
   // Smoothing shapes
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
}

int main( int argc, char *argv[] ) {
   glutInit(&argc, argv );
   glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );
   glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
   glutInitWindowPosition(400, 100);
   glutCreateWindow("HW2 - Sajad Abdolvandi");
    
    for (int row = 0; row <12; row++) {
        for (int col = 0; col <12; col++) {
            if (col == 0 && row != 0){
                myChart[row][col].chXL = -300;
                myChart[row][col].chXR = -300+((chartX));
                myChart[row][col].chYU =300-(row*chartY);
                myChart[row][col].chYD =300-(row*(chartY))-chartY;
            }else if (row == 0 && col != 0){
                myChart[row][col].chXL = -300+(chartX*col);
                myChart[row][col].chXR = -300+((chartX)*(col))+chartX;
                myChart[row][col].chYU =300;
                myChart[row][col].chYD =300-(chartY);
            }else if(row == 0 && col == 0){
                myChart[row][col].chXL = -300;
                myChart[row][col].chXR = -300+((chartX));
                myChart[row][col].chYU =300;
                myChart[row][col].chYD =300-((chartY));
            }else{
            myChart[row][col].chXL = -300+(chartX*col);
            myChart[row][col].chXR = -300+((chartX*col))+chartX;
            myChart[row][col].chYU =300-(row*chartY);
            myChart[row][col].chYD =300-(row*(chartY))-chartY;
            }
        }
    }
 
        startPos.x = -999;
        startPos.y = -999;
        endPos.y = -999;
        endPos.x = -999;
    
    for (int i = 0; i<100; i++) {
        lineArray[i].x = -999;
        lineArray[i].y = -999;
    }
  
   glutDisplayFunc(display);
   glutReshapeFunc(onResize);

   //
   // keyboard registration
   //
   glutKeyboardFunc(onKeyDown);
   glutSpecialFunc(onSpecialKeyDown);

   glutKeyboardUpFunc(onKeyUp);
   glutSpecialUpFunc(onSpecialKeyUp);

   //
   // mouse registration
   //
   glutMouseFunc(onClick);
   glutMotionFunc(onMoveDown);
   glutPassiveMotionFunc(onMove);
   
   #if  TIMER_ON == 1
   // timer event
   glutTimerFunc( TIMER_PERIOD, onTimer, 0 ) ;
   #endif

   Init();
    glutMainLoop();
}
