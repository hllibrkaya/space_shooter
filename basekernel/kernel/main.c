/*
Copyright (C) 2015-2019 The University of Notre Dame
This software is distributed under the GNU General Public License.
See the file LICENSE for details.
*/

#include "console.h"
#include "page.h"
#include "process.h"
#include "keyboard.h"
#include "mouse.h"
#include "interrupt.h"
#include "clock.h"
#include "ata.h"
#include "device.h"
#include "cdromfs.h"
#include "string.h"
#include "graphics.h"
#include "kernel/ascii.h"
#include "kernel/syscall.h"
#include "rtc.h"
#include "kernelcore.h"
#include "kmalloc.h"
#include "memorylayout.h"
#include "kshell.h"
#include "cdromfs.h"
#include "diskfs.h"
#include "serial.h"

#define MAX_ASTEROIDS 20
#define MAX_BULLETS 10
#define SCREEN_WIDTH  120
#define SCREEN_HEIGHT 60
#define ASTEROID_SPEED_DIVISOR 5 
struct asteroid {
    int x, y; // Asteroid konumu
    int dx, dy; // Hareket yönü
  int move_timer;
};

struct spaceship {
    int x, y; 
    int dx, dy; 
    char * direction;
};

struct bullet {
    int x, y; 
    int dx, dy; 
    int active; // Mermi aktif mi?
};
int game=1,score=0;life=3;
struct spaceship player;
struct asteroid asteroids[MAX_ASTEROIDS];
struct bullet bullets[MAX_ASTEROIDS]; 
void gotoxy(struct graphics *g,int x,int y,char *str){
	x = x * 8;
	y = y * 8;
	int value=0;
	int bit = 8;
	while(strlen(str)>value){
	graphics_char(g,x+(bit*value),y,str[value]);
	value++;
	}
	}
void border(struct graphics *g){
for(int i=9;i<61;i++){
gotoxy(g,9,i,"#");
}
for(int i=9;i<111;i++){
gotoxy(g,i,9,"#");
}
for(int i=9;i<111;i++){
gotoxy(g,i,61,"#");
}
for(int i=9;i<61;i++){
gotoxy(g,111,i,"#");
}
}
void delay(int delays){
for(int i = 0; i < delays * 5000000;i++){ 
	
}
}
unsigned long int next = 1;  // for random numbers

int rand(void) {
    const unsigned long int a = 1103515245;
    const unsigned long int c = 12345;
    const unsigned long int m = 2147483648;
    next = (a * next + c) % m;
    return (int)(next & 0x7FFFFFFF);
}


void srand(unsigned int seed) {
    next = seed;
}
void wrap_position(int *x, int *y) {
    // Eğer obje ekrandan çıkarsa diğer taraftan devam ettir
    if (*x < 10) {
        *x = SCREEN_WIDTH -11;
    } else if (*x >= SCREEN_WIDTH-10) {
        *x = 10;
    }
    if (*y < 10) {
        *y = SCREEN_HEIGHT - 11;
    } else if (*y >= SCREEN_HEIGHT) {
        *y = 10;
    }
}


void initialize_game(struct graphics *g) {
srand(120);
    gotoxy(g,10,5,"Score:");
    char scores;
    uint_to_string(score,scores);
    gotoxy(g,16,5,scores);
    gotoxy(g,105,5,"Life:");
    char lifes;
    uint_to_string(life,lifes);
    gotoxy(g,110,5,lifes);
    border(g);
    // Uzay gemisi, asteroidler ve mermiler için başlangıç değerleri
    player.x = SCREEN_WIDTH / 2;
    player.y = SCREEN_HEIGHT / 2+5;
    player.dx = 0;
    player.dy = -1;

     for (int i = 0; i < MAX_ASTEROIDS; i++) {
        asteroids[i].x = (rand() % SCREEN_WIDTH);
        asteroids[i].y = (rand() % SCREEN_HEIGHT);
        asteroids[i].dx = (rand() % 3==0) ? 1 : -1;
        asteroids[i].dy = (rand() % 3==0) ? 1 : -1;
        asteroids[i].move_timer = ASTEROID_SPEED_DIVISOR; // Asteroid hareket sayacını
    }
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        bullets[i].active = 0; // Başlangıçta tüm mermiler pasif
    }
}

void draw_game(struct graphics *g) {

if(player.direction=="U")
    gotoxy(g, player.x, player.y, "^");
if(player.direction=="L")
    gotoxy(g, player.x, player.y, "<");
if(player.direction=="R")
    gotoxy(g, player.x, player.y, ">");
if(player.direction=="D")
    gotoxy(g, player.x, player.y, "v");
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
	
        gotoxy(g, asteroids[i].x, asteroids[i].y, "O");
    }

    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (bullets[i].active) {
            gotoxy(g, bullets[i].x, bullets[i].y, "*");
        }
    }
}
void update_asteroids_position(struct graphics *g) {

    for (int i = 0; i < MAX_ASTEROIDS; i++) {

        if (--asteroids[i].move_timer <= 0) {
	gotoxy(g,  asteroids[i].x,asteroids[i].y, " ");
            // Asteroidi hareket ettir
            asteroids[i].x += asteroids[i].dx;
            asteroids[i].y += asteroids[i].dy;
            wrap_position(&asteroids[i].x, &asteroids[i].y);

            // Sayacı sıfırla
            asteroids[i].move_timer = ASTEROID_SPEED_DIVISOR;
        }
    }
}
void explosion_ship(struct graphics *g){
 for (int j = 0; j < MAX_ASTEROIDS; j++) {
	if(asteroids[j].dx==1&&asteroids[j].dy==1){
               	if (asteroids[j].x+1 == player.x && asteroids[j].y+1 == player.y) {
		asteroids[j].x = rand() % SCREEN_WIDTH;
	    	asteroids[j].y = rand() % SCREEN_HEIGHT;
		life=life-1;
		char lifes;
    		uint_to_string(life,lifes);
   		gotoxy(g,110,5,lifes);
		}
	}
	else if(asteroids[j].dx==-1&&asteroids[j].dy==-1){
               	if (asteroids[j].x-1 == player.x && asteroids[j].y-1 == player.y) {
		asteroids[j].x = rand() % SCREEN_WIDTH;
	    	asteroids[j].y = rand() % SCREEN_HEIGHT;
		life=life-1;
		char lifes;
    		uint_to_string(life,lifes);
   		gotoxy(g,110,5,lifes);
		}
	}
	else if(asteroids[j].dx==1&&asteroids[j].dy==-1){
               	if (asteroids[j].x+1 == player.x && asteroids[j].y-1 == player.y) {
		asteroids[j].x = rand() % SCREEN_WIDTH;
	    	asteroids[j].y = rand() % SCREEN_HEIGHT;
		life=life-1;
		char lifes;
    		uint_to_string(life,lifes);
   		gotoxy(g,110,5,lifes);
		}
	}
	else if(asteroids[j].dx==-1&&asteroids[j].dy==1){
               	if (asteroids[j].x-1 == player.x && asteroids[j].y+1 == player.y) {
		asteroids[j].x = rand() % SCREEN_WIDTH;
	    	asteroids[j].y = rand() % SCREEN_HEIGHT;
		life=life-1;
		char lifes;
    		uint_to_string(life,lifes);
   		gotoxy(g,110,5,lifes);
		}
	}
 }
}
void update_game(struct graphics *g) {
  update_asteroids_position(g);
  explosion_ship(g);
    // Mermileri güncelleme
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
		gotoxy(g,bullets[i].x,bullets[i].y," ");
            bullets[i].x += bullets[i].dx;
            bullets[i].y += bullets[i].dy;

            // Eğer mermi ekrandan çıkarsa, onu pasifleştir
            if (bullets[i].x < 10 || bullets[i].x >= SCREEN_WIDTH-10 ||
                bullets[i].y < 10 || bullets[i].y >= SCREEN_HEIGHT) {
                bullets[i].active = 0;
            } else {
                // Mermi ekranda ise çarpışma kontrolü yap
                for (int j = 0; j < MAX_ASTEROIDS; j++) {
		   if (asteroids[j].x== bullets[i].x && asteroids[j].y== bullets[i].y) {
			score=score+10;
			char scores;
 			uint_to_string(score,scores);
    			gotoxy(g,16,5,scores);
			gotoxy(g,asteroids[j].x,asteroids[j].y," ");
			asteroids[j].x = rand() % SCREEN_WIDTH;
	    		asteroids[j].y = rand() % SCREEN_HEIGHT;
			bullets[i].active = 0;
		        asteroids[j].dx =(rand() % 3) ? 1 : -1;
	      	        asteroids[j].dy =(rand() % 3) ? 1 : -1;

		   }
		   if(asteroids[j].dx==1&&asteroids[j].dy==1){
                    	if (asteroids[j].x+1 == bullets[i].x && asteroids[j].y+1 == bullets[i].y) {
			    score=score+10;
		            char scores;
 			    uint_to_string(score,scores);
    			    gotoxy(g,16,5,scores);
			    gotoxy(g,asteroids[j].x,asteroids[j].y," ");
		            asteroids[j].x = rand() % SCREEN_WIDTH;
	     		    asteroids[j].y = rand() % SCREEN_HEIGHT;
			    bullets[i].active = 0;
	  		    asteroids[j].dx =(rand() % 3) ? 1 : -1;
		            asteroids[j].dy =(rand() % 3) ? 1 : -1;
		       }
		    }
 	    	    if(asteroids[j].dx==-1&&asteroids[j].dy==-1){
                    	if (asteroids[j].x-1 == bullets[i].x && asteroids[j].y-1 == bullets[i].y) {
			    score=score+10;
		            char scores;
 			    uint_to_string(score,scores);
    			    gotoxy(g,16,5,scores);
			    gotoxy(g,asteroids[j].x,asteroids[j].y," ");
		            asteroids[j].x = rand() % SCREEN_WIDTH;
	     		    asteroids[j].y = rand() % SCREEN_HEIGHT;
			    bullets[i].active = 0;
	  		    asteroids[j].dx =(rand() % 3) ? 1 : -1;
		            asteroids[j].dy =(rand() % 3) ? 1 : -1;
		       }
		    }
  		    if(asteroids[j].dx==1&&asteroids[j].dy==-1){
                    	if (asteroids[j].x+1 == bullets[i].x && asteroids[j].y-1 == bullets[i].y) {
			    score=score+10;
		            char scores;
 			    uint_to_string(score,scores);
    			    gotoxy(g,16,5,scores);
			    gotoxy(g,asteroids[j].x,asteroids[j].y," ");
		            asteroids[j].x = rand() % SCREEN_WIDTH;
	     		    asteroids[j].y = rand() % SCREEN_HEIGHT;
			    bullets[i].active = 0;
	  		    asteroids[j].dx =(rand() % 3) ? 1 : -1;
		            asteroids[j].dy =(rand() % 3) ? 1 : -1;
		       }
		    }
 	    	    if(asteroids[j].dx==-1&&asteroids[j].dy==1){
                    	if (asteroids[j].x-1 == bullets[i].x && asteroids[j].y+1 == bullets[i].y) {
			    score=score+10;
		            char scores;
 			    uint_to_string(score,scores);
    			    gotoxy(g,16,5,scores);
			    gotoxy(g,asteroids[j].x,asteroids[j].y," ");
		            asteroids[j].x = rand() % SCREEN_WIDTH;
	     		    asteroids[j].y = rand() % SCREEN_HEIGHT;
			    bullets[i].active = 0;
	  		    asteroids[j].dx =(rand() % 3) ? 1 : -1;
		            asteroids[j].dy =(rand() % 3) ? 1 : -1;
		       }
		    }
                }
            }
        }
    }
}

void rotate_spaceship(int direction) {
    
    if (direction == 'L') {
        if (player.dx == 0 && player.dy == -1) {
            player.dx = -1;
            player.dy = 0;
	   player.direction="L";
        } else if (player.dx == -1 && player.dy == 0) {
            player.dx = 0;
            player.dy = 1;
player.direction="D";
        } else if (player.dx == 0 && player.dy == 1) {
            player.dx = 1;
            player.dy = 0;
player.direction="R";
        } else if (player.dx == 1 && player.dy == 0) {
            player.dx = 0;
            player.dy = -1;
player.direction="U";
        }
    } else if (direction == 'R') { 
        if (player.dx == 0 && player.dy == -1) {
            player.dx = 1;
            player.dy = 0;
player.direction="R";
        } else if (player.dx == 1 && player.dy == 0) {
            player.dx = 0;
            player.dy = 1;
player.direction="D";
        } else if (player.dx == 0 && player.dy == 1) {
            player.dx = -1;
            player.dy = 0;
player.direction="L";
        } else if (player.dx == -1 && player.dy == 0) {
            player.dx = 0;
            player.dy = -1;
player.direction="U";
        }
    }
}

void move_spaceship(struct graphics *g) {
	gotoxy(g, player.x, player.y, " ");
    
    player.x += player.dx;
    player.y += player.dy;

    wrap_position(&player.x, &player.y);
}
void fire_bullet() {
    
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (!bullets[i].active) {
            bullets[i].x = player.x;
            bullets[i].y = player.y;
            bullets[i].dx = player.dx; 
            bullets[i].dy = player.dy;
            bullets[i].active = 1;
            break;
        }
    }
}
void handle_input(struct graphics *g) {
    
    char key = keyboard_read(1);
    switch (key) {
        case 'a':
        case 'A':
            rotate_spaceship('L');
            break;
        case 'd':
        case 'D':
            rotate_spaceship('R');
            break;
        case 'w':
        case 'W':
            move_spaceship(g);
            break;
        case 's':
        case 'S':
            
            break;
        case ' ':
            fire_bullet();
            break;
    }
}


void gameLoop(struct graphics *g){
while (game) {
        if(life==0){
	gotoxy(g, SCREEN_WIDTH/2,SCREEN_HEIGHT/2, "Game Over");
	game=0;
	}
	else{
	handle_input(g); 
	update_game(g); 
	draw_game(g); 
	delay(1); 
	}
}
}

int kernel_main() {
    struct graphics *g = graphics_create_root();
console_init(g);
	console_addref(&console_root);

	page_init();
	kmalloc_init((char *) KMALLOC_START, KMALLOC_LENGTH);
	interrupt_init();
	rtc_init();
	keyboard_init();
	process_init();
    initialize_game(g);
    gameLoop(g);
    return 0;
}
