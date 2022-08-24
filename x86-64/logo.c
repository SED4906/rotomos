#include <x86-64/cpu.h>
#include <x86-64/fb.h>
#include <stdint.h>

void plotlinelow(uint64_t x0, uint64_t y0, uint64_t x1, uint64_t y1, uint32_t rgba) {
    int dx=x1-x0;
    int dy=y1-y0;
    int yi=1;
    if(dy<0) {
        yi=-1;
        dy=-dy;
    }
    int D=2*dy-dx;
    int y=y0;
    int xd = (x1-x0 > 0 ? 1 : -1);
    for(uint64_t x=x0;x!=x1;x+=xd) {
        fb_plot(x,y,rgba);
        if(D > 0) {
            y+=yi;
            D+=2*(dy-dx);
        }
        else {
            D+=2*dy;
        }
    }
}

void plotlinehigh(uint64_t x0, uint64_t y0, uint64_t x1, uint64_t y1, uint32_t rgba) {
    int dx=x1-x0;
    int dy=y1-y0;
    int xi=1;
    if(dx<0) {
        xi=-1;
        dx=-dx;
    }
    int D=2*dx-dy;
    int x=x0;
    int yd = (y1-y0 > 0 ? 1 : -1);
    for(uint64_t y=y0;y!=y1;y+=yd) {
        fb_plot(x,y,rgba);
        if(D > 0) {
            x+=xi;
            D+=2*(dx-dy);
        }
        else {
            D+=2*dx;
        }
    }
}

void fb_plot_line(uint64_t x0, uint64_t y0, uint64_t x1, uint64_t y1, uint32_t rgba) {
    int y=y1-y0;
    if(y<0) y=-y;
    int x=x1-x0;
    if(x<0) x=-x;
    if(y<x) {
        if(x0>x1) plotlinelow(x1,y1,x0,y0,rgba);
        else plotlinelow(x0,y0,x1,y1,rgba);
    } else {
        if(y0>y1) plotlinehigh(x1,y1,x0,y0,rgba);
        else plotlinehigh(x0,y0,x1,y1,rgba);
    }
}

uint8_t data_rotom_logo[] = {
    //Tail
    55,37,53,32, 53,32,54,29, 54,29,64,25, 64,25,72,22, 72,22,74,19,
    74,19,74,16, 74,16,70,13, 64,10,74,16, 64,10,52,9,  52,9,48,11,
    45,15,48,11, 45,15,42,21, 40,25,42,21, 40,25,37,26, 32,26,37,26,
    32,26,22,27, 20,25,22,27, 20,25,20,20, 18,19,20,20, 18,19,17,20,
    10,30,17,20, 10,30,8,36,  8,46,8,36,   8,46,10,49,  14,52,10,49,
    14,52,18,53, 35,52,18,53, 35,52,40,56, 40,60,40,56, 40,60,36,64,
    25,72,36,64, 25,72,21,76, 22,84,21,76, 22,84,26,90, 30,91,26,90,
    30,91,44,88, 50,81,44,88, 50,81,53,78, 57,77,53,78, 57,77,71,82,
    77,81,71,82, 77,81,81,77, 83,72,81,77, 83,72,81,66, 80,62,81,66,
    80,62,84,64, 90,58,84,64, 90,58,90,45, 86,40,90,45, 86,40,80,38,
    76,36,80,38, 76,36,75,32, 78,38,75,32, 78,38,80,27, 85,26,80,27,
    //Right Arm
    52,40,49,34, 46,37,49,34, 46,37,40,30, 44,42,40,30, 44,42,49,38,
    52,40,49,38,
    // Left Arm
    61,36,61,30, 64,31,61,30, 64,31,69,25, 69,36,69,25, 69,36,63,33,
    61,36,63,33,
    //Body
    55,37,60,38, 65,39,60,38, 65,39,70,42, 71,46,70,42, 71,46,70,50,
    67,52,70,50, 67,52,67,62, 66,64,67,62, 66,64,59,55, 52,52,59,55,
    52,52,50,49, 52,44,50,49, 52,44,53,41, 53,38,53,41, 53,38,55,37,
    //Mouth
    59,49,60,45, 62,48,60,45
};
uint64_t data_rotom_logo_len = 336;

uint8_t data_rotom_logo_text[] = {
    //R
    8,59,14,13,  29,17,14,13, 29,17,30,23, 25,31,30,23, 25,31,13,34,
    29,57,13,34,
    //o
    33,42,45,38, 50,39,45,38, 50,39,53,49, 43,61,53,49, 43,61,35,56,
    33,42,35,56,
    //t
    59,20,60,52, 64,57,60,52, 64,57,75,48, 53,26,71,28,
    //o
    79,44,88,39, 93,42,88,39, 93,42,92,55, 87,59,92,55, 87,59,78,52,
    79,44,78,52,
    //m
    97,56,97,38, 101,41,97,38, 101,41,103,53, 106,38,103,53, 106,38,112,54,
    //O
    158,14,177,26, 173,45,177,26, 173,45,164,54, 145,50,164,54, 145,50,140,35,
    148,20,140,35, 148,20,158,14,
    //S
    215,15,196,12, 187,20,196,12, 187,20,200,32, 210,42,200,32, 210,42,205,54,
    186,58,205,54
};
uint64_t data_rotom_logo_text_len = 160;

void fb_draw_rotom_logo(uint64_t offset_x,uint64_t offset_y) {
    for(uint64_t i=0; i<data_rotom_logo_len; i+=4) {
        fb_plot_line(offset_x+data_rotom_logo[i],offset_y+data_rotom_logo[i+1],offset_x+data_rotom_logo[i+2],offset_y+data_rotom_logo[i+3],0x00C1FFFF);
        idle();
    }
    fb_plot(offset_x+64,offset_y+48,0x00C1FFFF);idle();
    fb_plot(offset_x+58,offset_y+52,0x00C1FFFF);idle();
}

void fb_draw_rotom_text(uint64_t offset_x,uint64_t offset_y) {
    for(uint64_t i=0; i<data_rotom_logo_text_len; i+=4) {
        fb_plot_line(offset_x+data_rotom_logo_text[i],offset_y+data_rotom_logo_text[i+1],offset_x+data_rotom_logo_text[i+2],offset_y+data_rotom_logo_text[i+3],0xFF8A00FF);
        idle();
    }
}