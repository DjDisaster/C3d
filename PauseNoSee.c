// Instead of reseting the canvas each time 
// The pixels are swapped instead of drawn over.
// This means if you get a screenshot of the cube 
// You will not be able to see it. 

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include<unistd.h>

#define WIDTH 100
#define HEIGHT 100

const char* getCharRed(char chr) {
    static char buffer[16];
    sprintf(buffer, "\033[31m%c\033[0m", chr);
    return buffer;
}

const char* getCharBlue(char chr) {
    static char buffer[16];
    sprintf(buffer, "\033[39m%c\033[0m", chr);
    return buffer;
}

struct vector {
    float x;
    float y;
    float z;
};


const char* character1;
const char* character2;
const char* pixels[WIDTH][HEIGHT];
float offsetX = 0;
float offsetY = 0;
float offsetZ = 0;
float cameraAngleX = 0;
float cameraAngleY = 0;
float cameraAngleZ = 0;
float cameraDistance = 100;
float scale = 75;


void swap(int x, int y) {
    const char* current = pixels[x][y];
    if (current == character1) {
        pixels[x][y] = character2;
    } else {
        pixels[x][y] = character1;
    }
}

void drawLine(int x0, int y0, int x1, int y1)
{
    int steep = (abs(y1 - y0) > abs(x1 - x0));
    if (steep) {
        int t = x0;  x0 = y0;  y0 = t; t = x1;      x1 = y1;  y1 = t;
    }
    if (x0 > x1) {
        int t = x0;  x0 = x1;  x1 = t; t = y0;      y0 = y1;  y1 = t;
    }

    int dx = x1 - x0;
    int dy = abs(y1 - y0);
    int err = dx / 2;
    int ystep = (y0 < y1) ? 1 : -1;
    int y = y0;

    for (int x = x0; x <= x1; x++) {
        if (steep) {
            if (y >= 0 && y < WIDTH && x >= 0 && x < HEIGHT) {
                //pixels[y][x] = character2;
                swap(y,x);
            }
        } else {
            if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
                //pixels[x][y] = character2;
                swap(x,y);
            }
        }
        err -= dy;
        if (err < 0) {
            y += ystep;
            err += dx;
        }
    }
}


void draw() {
    
    struct vector vertices[] = {
        {offsetX - 0.5, offsetY - 0.5, offsetZ - 0.5}, {offsetX - 0.5, offsetY - 0.5, offsetZ + 0.5},
        {offsetX - 0.5, offsetY + 0.5, offsetZ - 0.5}, {offsetX - 0.5, offsetY + 0.5, offsetZ + 0.5},
        {offsetX + 0.5, offsetY - 0.5, offsetZ - 0.5}, {offsetX + 0.5, offsetY - 0.5, offsetZ + 0.5},
        {offsetX + 0.5, offsetY + 0.5, offsetZ - 0.5}, {offsetX + 0.5, offsetY + 0.5, offsetZ + 0.5}
    };
    size_t numElements = sizeof(vertices) / sizeof(vertices[0]);

    struct vector projectedVertices[8];
    float offsetCanvasX = WIDTH / 2;
    float offsetCanvasY = HEIGHT / 2;
    int i = 0;
    while (i < numElements) {
        float x = vertices[i].x;
        float y = vertices[i].y;
        float z = vertices[i].z;


        float tmpY = y * cos(cameraAngleX) - z * sin(cameraAngleX);
        float tmpZ = y * sin(cameraAngleX) + z * cos(cameraAngleX);

        float tmpX = x * cos(cameraAngleY) + tmpZ * sin(cameraAngleY);
        tmpZ = -x * sin(cameraAngleY) + tmpZ * cos(cameraAngleY);

        x = tmpX * cos(cameraAngleZ) - tmpY * sin(cameraAngleZ);
        y = tmpX * sin(cameraAngleZ) + tmpY * cos(cameraAngleZ);

        float perspective = cameraDistance / (tmpZ + cameraDistance);
        projectedVertices[i].x = x * perspective * scale + offsetCanvasX;
        projectedVertices[i].y = y * perspective * scale + offsetCanvasY;
        
        i++;
    }
    
    int edges[][2] = {
        {0, 1}, {1, 3}, {3, 2}, {2, 0},
        {4, 5}, {5, 7}, {7, 6}, {6, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };
    int marker = 0;
    numElements = sizeof(edges) / sizeof(edges[0]);
    while (marker < numElements) {
        int x1 = projectedVertices[edges[marker][0]].x;
        int y1 = projectedVertices[edges[marker][0]].y;
        int x2 = projectedVertices[edges[marker][1]].x;
        int y2 = projectedVertices[edges[marker][1]].y;
        drawLine(x1, y1, x2, y2);        
        marker++;
    }
}

void resetCanvas() {
    for (int x = 0; x < HEIGHT; x++) {
        for (int y = 0; y < WIDTH; y++) {
            pixels[y][x] = character1;
        }
    }
}

void printPixels() {
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            printf(pixels[x][y]);
        }
        printf("\n");
    }
}

void deleteLastLines(int numLines) {
    for (int i = 0; i < numLines; i++) {
        printf("\033[A");  
        printf("\033[2K");  

    }
}




int main() {
    character1 = getCharRed('a');
    character2 = getCharBlue('b');
    resetCanvas();
    while (1 > 0) {
        //resetCanvas();
        cameraAngleX+=0.03; 
        cameraAngleY+=0.03;
        draw();
        printPixels();
        usleep(1000);
        deleteLastLines(HEIGHT);
        usleep(10000);
    }
    return 0;
}
