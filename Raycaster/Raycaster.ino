#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware graphics library

#include "nunchuk.h"
//Basic Colors
#define RED    0xf800
#define GREEN 0x07e0
#define BLUE  0x001f
#define BLACK 0x0000
#define YELLOW  0xffe0
#define WHITE 0xffff

//Other Colors
#define CYAN    0x07ff  
#define BRIGHT_RED  0xf810  
#define GRAY1   0x8410  
#define GRAY2   0x4208  
#define MAGENTA     0xf81f

// Defs for stick
#define STICK_X     A5
#define STIee CK_Y     A4
#define EXTRA_GND_1 6
//#define EXTRA_GND_2 7

// Defs for led panel
#define CLK 11
#define LAT A3
#define OE  9
#define A   A0
#define B   A1
#define C   A2
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);

struct Player {
public:
  double xPos = 16, yPos = 9;
  double xDir = 1, yDir = 0, xPlane = 0, yPlane = 0.66;
  bool forward = false, backward = false, left = false, right = false;

  const double moveSpeed = .16;
  const double rotSpeed = .045;
};

byte level_map[24][32] = {
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,2,2,2,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,0,0,1},
  {1,0,0,2,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,2,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,4,4,4,4,4,4,1},
  {1,0,0,2,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,2,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,4,4,4,4,4,4,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,4,4,4,4,4,4,1},
  {1,0,0,0,0,0,0,0,0,0,3,3,3,0,0,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,3,0,3,0,0,3,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,3,0,3,0,0,3,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,3,0,3,0,0,3,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,3,0,0,0,3,3,0,3,3,3,3,3,3,3,3,0,3,3,0,3,0,0,0,0,0,0,0,0,1},
  {1,0,0,3,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,3,0,3,0,0,0,0,0,0,0,0,1},
  {1,0,0,3,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,3,0,3,0,0,0,0,0,0,0,0,1},
  {1,0,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

//// a 32x24 block map
//byte level_map[32][20] = {
//  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
//  {1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1},
//  {1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1},
//  {1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1},
//  {1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
//  {1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
//  {1,1,1,0,0,0,0,0,4,4,4,4,0,0,0,0,0,0,1,1},
//  {1,1,0,0,0,0,0,4,4,4,4,4,4,0,0,0,0,0,0,1},
//  {1,1,0,0,0,0,4,4,4,4,4,4,4,4,0,0,0,0,0,1},
//  {1,1,0,0,0,0,4,4,4,4,4,4,4,4,0,0,0,0,0,1},
//  {1,0,0,0,0,0,4,4,4,4,4,4,4,4,0,0,0,0,0,1},
//  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
//  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
//  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
//  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
//  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
//  {3,0,0,0,0,0,4,4,4,4,4,4,4,4,0,0,0,0,0,2},
//  {3,0,0,0,0,0,4,4,4,4,4,4,4,4,0,0,0,0,0,2},
//  {3,0,0,0,0,0,4,4,4,4,4,4,4,4,0,0,0,0,0,2},
//  {3,0,0,0,0,0,0,4,4,4,4,4,4,0,0,0,0,0,0,2},
//  {3,0,0,0,0,0,0,4,4,4,4,4,4,0,0,0,0,0,0,2},
//  {3,0,0,0,0,0,0,0,4,4,4,4,0,0,0,0,0,0,0,2},
//  {3,3,0,0,0,0,0,0,4,4,4,4,0,0,0,0,0,0,2,2},
//  {3,3,0,0,0,0,0,0,0,4,4,0,0,0,0,0,0,0,2,2},
//  {3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2},
//  {3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2},
//  {3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2},
//  {3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2},
//  {3,3,3,3,3,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2},
//  {3,3,3,3,3,3,3,0,0,0,0,0,0,2,2,2,2,2,2,2},
//  {3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2}
//};

int mapWidth = 0;
int mapHeight = 0;

const int SCREEN_WIDTH = 32;
const int SCREEN_HEIGHT = 16;

Player player;
Nunchuk controller;

void cast_rays(int stepNum) {
  double cameraX, rayDirX, rayDirY, sideDistX, sideDistY, deltaDistX, deltaDistY, perpWallDist;
  int mapX, mapY, stepX, stepY, side;
  bool hit;
  
  for(int x = 0; x < SCREEN_WIDTH; x++) {
    cameraX = 2 * x / (double)(SCREEN_WIDTH) - 1;
    rayDirX = player.xDir + player.xPlane * cameraX;
    rayDirY = player.yDir + player.yPlane * cameraX;

    mapX = (int)player.xPos;
    mapY = (int)player.yPos;

    deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX));
    deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY));

    side = 0;
    hit = false;

    if(rayDirX < 0) {
      stepX = -1;
      sideDistX = (player.xPos - mapX) * deltaDistX;
    } else {
      stepX = 1;
      sideDistX = (mapX + 1.0 - player.xPos) * deltaDistX;
    }

    if(rayDirY < 0) {
      stepY = -1;
      sideDistY = (player.yPos - mapY) * deltaDistY;
    } else {
      stepY = 1;
      sideDistY = (mapY + 1 - player.yPos) * deltaDistY;
    }

    while(!hit) {
      if(sideDistX < sideDistY) {
        sideDistX += deltaDistX;
        mapX += stepX;
        side = 0;
      } else {
        sideDistY += deltaDistY;
        mapY += stepY;
        side = 1;
      }

      // Check if ray has hit a wall
      if(level_map[mapY][mapX] != 0) {
        hit = true;
//        Serial.print(mapY);
//        Serial.print(", ");
//        Serial.print(mapX);
//        Serial.print(" - ");
//        Serial.println(level_map[mapY][mapX]);
      }
    }

    // Calculate distance to point of impact
    if(side == 0)
      perpWallDist = sqrt(((mapX - player.xPos + (1 - stepX) / 2) / rayDirX) * ((mapX - player.xPos + (1 - stepX) / 2) / rayDirX));
    else
      perpWallDist = sqrt(((mapY - player.yPos + (1 - stepY) / 2) / rayDirY) * ((mapY - player.yPos + (1 - stepY) / 2) / rayDirY));

    // Now calculate height of the wal based on the distance from the camera
    int lineHeight;
    if(perpWallDist > 0)
      lineHeight = abs((int)(SCREEN_HEIGHT / perpWallDist));
    else
      lineHeight = SCREEN_HEIGHT;

    // Calculate lowest and heighest pixel to fill in current stripe
    int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;

    if(drawStart < 0)
      drawStart = 0;

    int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;
    if(drawEnd >= SCREEN_HEIGHT)
      drawEnd = SCREEN_HEIGHT - 1;

    int texNum = level_map[mapY][mapX];
    
//    Serial.print("Texture: ");
//    Serial.print(texNum);
//    Serial.print(" from pos: (");
//    Serial.print(mapX);
//    Serial.print(", ");
//    Serial.print(mapY);
//    Serial.print(") - Line down from (");
//    Serial.print(x);
//    Serial.print(", ");
//    Serial.print(drawStart);
//    Serial.print(") to (");
//    Serial.print(x);
//    Serial.print(", ");
//    Serial.print(drawEnd);
//    Serial.println(")");
    
    //Serial.println(texNum);
    if(stepNum >= 3 && (player.forward || player.backward)) {
      drawStart --;
      drawEnd --;
    }    
    
    if(drawEnd >= SCREEN_HEIGHT)
      drawEnd = SCREEN_HEIGHT - 1;
    if(drawStart < 0)
      drawStart = 0;
    
    if(texNum != 0) {
      matrix.drawLine(x, 0, x, drawStart, BLACK);
      matrix.drawLine(x, drawEnd, x, SCREEN_HEIGHT, BLACK);
      if(side == 1) {
        if(texNum == 1) {
          matrix.drawLine(x, drawStart, x, drawEnd, BLUE);
        }
        if(texNum == 2) {
          matrix.drawLine(x, drawStart, x, drawEnd, WHITE);
        }
        if(texNum == 3) {
          matrix.drawLine(x, drawStart, x, drawEnd, GREEN);
        }
        if(texNum == 4) {
          matrix.drawLine(x, drawStart, x, drawEnd, RED);
        }
      } else {
        if(texNum == 1)
          matrix.drawLine(x, drawStart, x, drawEnd, 0x0007);
        if(texNum == 2)
          matrix.drawLine(x, drawStart, x, drawEnd, GRAY2);
        if(texNum == 3)
          matrix.drawLine(x, drawStart, x, drawEnd, 0x03e0);
        if(texNum == 4)
          matrix.drawLine(x, drawStart, x, drawEnd, matrix.Color333(2, 0, 0));
      }
    }
  }
}

bool player_move() {
  bool success = false;
  
  if(player.forward) {
    if(level_map[int(player.yPos)][int(player.xPos + player.xDir * player.moveSpeed)] == 0
        && int(player.xPos + player.xDir * player.moveSpeed) < mapWidth
        && int(player.xPos + player.xDir * player.moveSpeed) > 0) {
      player.xPos += player.xDir * player.moveSpeed;
      success = true;
    }
    if(level_map[int(player.yPos + player.yDir * player.moveSpeed)][int(player.xPos)] == 0
        && int(player.yPos + player.yDir * player.moveSpeed) < mapWidth
        && int(player.yPos + player.yDir * player.moveSpeed) > 0) {
      player.yPos += player.yDir * player.moveSpeed;
      success = true;
    }
  } else if(player.backward) {
    if(level_map[int(player.yPos)][int(player.xPos - player.xDir * player.moveSpeed)] == 0
        && int(player.xPos - player.xDir * player.moveSpeed) < mapWidth
        && int(player.xPos - player.xDir * player.moveSpeed) > 0) {
      player.xPos -= player.xDir * player.moveSpeed;
      success = true;
    }
    if(level_map[int(player.yPos - player.yDir * player.moveSpeed)][int(player.xPos)] == 0
        && int(player.yPos - player.yDir * player.moveSpeed) < mapWidth
        && int(player.yPos - player.yDir * player.moveSpeed) > 0) {
      player.yPos -= player.yDir * player.moveSpeed;
      success = true;
    }
  }

  if(player.left) {
    double oldXDir = player.xDir;
    player.xDir = player.xDir*cos(-player.rotSpeed) - player.yDir*sin(-player.rotSpeed);
    player.yDir = oldXDir*sin(-player.rotSpeed) + player.yDir*cos(-player.rotSpeed);

    double oldXPlane = player.xPlane;
    player.xPlane = player.xPlane*cos(-player.rotSpeed) - player.yPlane*sin(-player.rotSpeed);
    player.yPlane = oldXPlane*sin(-player.rotSpeed) + player.yPlane*cos(-player.rotSpeed);
    
    success = true;
  } else if(player.right) {
    double oldXDir = player.xDir;
    player.xDir = player.xDir*cos(player.rotSpeed) - player.yDir*sin(player.rotSpeed);
    player.yDir = oldXDir*sin(player.rotSpeed) + player.yDir*cos(player.rotSpeed);

    double oldXPlane = player.xPlane;
    player.xPlane = player.xPlane*cos(player.rotSpeed) - player.yPlane*sin(player.rotSpeed);
    player.yPlane = oldXPlane*sin(player.rotSpeed) + player.yPlane*cos(player.rotSpeed);

    success = true;
  }

  return success;
}

void check_controls() {
  controller.update();
  
  if(controller.left()) {
    player.left = true;
    player.right = false;
  } else if(controller.right()) {
    player.left = false;
    player.right = true;
  } else {
    if(player.left)
      player.left = false;
    if(player.right)
      player.right = false;
  }

  if(controller.z_button()) {
    player.forward = true;
    player.backward = false;
  } else if(controller.c_button()) {
    player.forward = false;
    player.backward = true;
  } else {
    if(player.forward)
      player.forward = false;
    if(player.backward)
      player.backward = false;
  }

//  if(controller.z_button()) {
//    player.forward = true;
//  } else {
//    if(player.forward)
//      player.forward = false;
//  }
}

void setup() {
  matrix.begin();
  Serial.begin(9600);

  // Add extra ground connections for stick
  pinMode(EXTRA_GND_1, OUTPUT);
//  pinMode(EXTRA_GND_2, OUTPUT);
  digitalWrite(EXTRA_GND_1, LOW);
//  digitalWrite(EXTRA_GND_2, LOW);
  
  mapWidth = sizeof(level_map[0])/sizeof(byte);
  mapHeight = sizeof(level_map)/mapWidth/sizeof(byte);

  Serial.print(mapWidth);
  Serial.print(" x ");
  Serial.println(mapHeight);

  controller.initialize();
  
//  Serial.println(numRays);
  
  cast_rays(0);
}

int stepNum = 0;
void loop() {
  check_controls();
  if(player_move()) {
    cast_rays(stepNum++);

    if(stepNum >= 7)
      stepNum = 0;
  }
}

