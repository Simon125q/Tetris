#include "primlib.h"
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#define HEIGHT 22
#define WIDTH 10
#define SMALL_HEIGHT 4
#define SMALL_WIDTH 4
#define TRUE 1
#define FALSE 0
#define DELAY 16
#define RIGHT 1
#define LEFT -1
#define SCREEN_WIDTH gfx_screenWidth()
#define SCREEN_HEIGHT gfx_screenHeight()
#define PIXEL (SCREEN_HEIGHT/(HEIGHT+1))
#define FALLING_SPEED 20

int block_position[WIDTH][HEIGHT] = {0};
int moving_block_position[WIDTH][HEIGHT] = {0};
int next_block[4][4] = {0};
int falling = TRUE;
int play = TRUE;
int movement = 0;
int piece_shape = 0;
int piece_rotation = 0;
int next_piece_shape = 0;
int next_piece_rotation = 0;

int pieces[7 /*kind */ ][4 /* rotation */ ][4][4] =
{
/* square */
  {
   {
    {2, 1, 0, 0},
    {1, 1, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
    },
   {
    {2, 1, 0, 0},
    {1, 1, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
    },
   {
    {2, 1, 0, 0},
    {1, 1, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
    },
   {
    {2, 1, 0, 0},
    {1, 1, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
    }
   },
/* I */

  {
   {
    {1, 2, 1, 1},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
    },
   {
    {1, 0, 0, 0},
    {2, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0}
    },
   {
    {1, 1, 2, 1},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
    },
   {
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {2, 0, 0, 0},
    {1, 0, 0, 0}
    }
   }
  ,
/* L */
  {
   {
    {1, 0, 0, 0},
    {2, 0, 0, 0},
    {1, 1, 0, 0},
    {0, 0, 0, 0}
    },
   {
    {1, 2, 1, 0},
    {1, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
    },
   {
    {1, 1, 0, 0},
    {0, 2, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 0, 0}
    },
   {
    {0, 0, 1, 0},
    {1, 2, 1, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
    }
   },
/* L mirrored */
  {
   {
    {0, 1, 0, 0},
    {0, 2, 0, 0},
    {1, 1, 0, 0},
    {0, 0, 0, 0}
    },
   {
    {1, 0, 0, 0},
    {1, 2, 1, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
    },
   {
    {1, 1, 0, 0},
    {2, 0, 0, 0},
    {1, 0, 0, 0},
    {0, 0, 0, 0}
    },
   {
    {1, 2, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
    }
   },
/* N */
  {
   {
    {0, 1, 0, 0},
    {2, 1, 0, 0},
    {1, 0, 0, 0},
    {0, 0, 0, 0}
    },
   {
    {1, 2, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
    },
   {
    {0, 1, 0, 0},
    {1, 2, 0, 0},
    {1, 0, 0, 0},
    {0, 0, 0, 0}
    },
   {
    {1, 1, 0, 0},
    {0, 2, 1, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
    }
   },
/* N mirrored */
  {
   {
    {1, 0, 0, 0},
    {2, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 0, 0}
    },
   {
    {0, 2, 1, 0},
    {1, 1, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
    },
   {
    {1, 0, 0, 0},
    {1, 2, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 0, 0}
    },
   {
    {0, 1, 1, 0},
    {1, 2, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
    }
   },
/* T */
  {
   {
    {1, 0, 0, 0},
    {2, 1, 0, 0},
    {1, 0, 0, 0},
    {0, 0, 0, 0}
    },
   {
    {1, 2, 1, 0},
    {0, 1, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
    },
   {
    {0, 1, 0, 0},
    {1, 2, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 0, 0}
    },
   {
    {0, 1, 0, 0},
    {1, 2, 1, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
    }
   }
};

void create_grid();
void draw_blocks();
void draw_moving_block();
void generate_starting_blocks();
void create_next_block();
void falling_blocks();
void controlls();
void rotate_block();
void calculate_block_origin(int rotation_x, int rotation_y);
int check_rotation_posibility(int block_origin_x, int block_origin_y, int next_rotation);
void move_to_side(int side);
void move_down();
void add_to_fallen();
void loosing();
void check_full_line();
void remove_full_line(int row_to_remove);

int main(int argc, char* argv[])
{
	srand(time(NULL));

	if (gfx_init()) {
		exit(3);
	}
	
  /*Generating the next block to fall*/
  create_next_block();

	generate_starting_blocks();

	while (play) {

		gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1, BLACK);

		draw_blocks();
		
		create_grid();

		if(falling == FALSE)
		{
			create_next_block();
		}

    controlls();

		if(movement % FALLING_SPEED == 0)
    {
        while(movement > FALLING_SPEED)
        {
          movement -= FALLING_SPEED;
        }
        falling_blocks();
    }
		
		draw_moving_block();

    check_full_line();

    loosing();

		gfx_updateScreen();

		SDL_Delay(DELAY);

    movement++;
	}

	gfx_getkey();

	return 0;
}

void controlls()
{
	int left = FALSE;
	int right = TRUE;
  
	if(gfx_isKeyDown(SDLK_SPACE))
	{
		rotate_block();
	}
	if(gfx_isKeyDown(SDLK_RIGHT))
	{
		move_to_side(right);
	}
	if(gfx_isKeyDown(SDLK_LEFT))
	{
		move_to_side(left);
	}
	if(gfx_isKeyDown(SDLK_DOWN))
	{
		move_down();
	}
	if (gfx_isKeyDown(SDLK_ESCAPE))
	{
		play= FALSE;
	}

}

void create_grid()
{
  /*Drawing the main grid*/
	for(int column = 0; column < WIDTH + 1; column++)
		{
			gfx_line(SCREEN_WIDTH/3 + column * PIXEL, PIXEL, SCREEN_WIDTH/3 + column * PIXEL,
       (HEIGHT+1) * PIXEL, WHITE);
		}
	for(int row = 1; row <= HEIGHT + 1; row++)
		{
			gfx_line(SCREEN_WIDTH/3, row * PIXEL, SCREEN_WIDTH/3 + WIDTH * PIXEL,
       row * PIXEL, WHITE);
		}
  // /*Drawing the small grid for the next block*/
  // for(int small_column = 0; small_column < SMALL_WIDTH + 1; small_column++)
	// 	{
	// 		gfx_line(SCREEN_WIDTH/3 + (WIDTH + 1) * PIXEL + small_column * PIXEL, PIXEL,
  //      SCREEN_WIDTH/3 + (WIDTH + 1) * PIXEL + small_column * PIXEL, (SMALL_HEIGHT+1) * PIXEL, RED);
	// 	}
	// for(int small_row = 1; small_row <= SMALL_HEIGHT + 1; small_row++)
	// 	{
	// 		gfx_line(SCREEN_WIDTH/3 + (WIDTH + 1) * PIXEL, small_row * PIXEL,
  //      SCREEN_WIDTH/3 + (WIDTH + 1) * PIXEL + SMALL_WIDTH * PIXEL, small_row * PIXEL, RED);
	// 	}
	}

void draw_blocks()
{
/*Drawing the fallen blocks*/
	for(int column = 0; column < WIDTH; column++)
		{
			for(int row = 0; row < HEIGHT; row++)
			{
				
				if(block_position[column][row] == 1)
				{
					gfx_filledRect(SCREEN_WIDTH/3 + column * PIXEL, row * PIXEL + PIXEL,
           SCREEN_WIDTH/3 + (column  + 1) * PIXEL, (row + 1) * PIXEL + PIXEL, RED);
				}
			}
		}
/*Drawing the next block to be falling*/
    for(int column = 0; column < 4; column++)
		{
			for(int row = 0; row < 4; row++)
			{
				
				if(next_block[column][row] == 1)
				{
					gfx_filledRect(SCREEN_WIDTH/3 + (WIDTH + 1) * PIXEL + column * PIXEL, row * PIXEL + PIXEL,
           SCREEN_WIDTH/3 + (WIDTH + 1) * PIXEL + (column  + 1) * PIXEL, (row + 1) * PIXEL + PIXEL, GREEN);
				}
        else if(next_block[column][row] == 2)
				{
					gfx_filledRect(SCREEN_WIDTH/3 + (WIDTH + 1) * PIXEL + column * PIXEL, row * PIXEL + PIXEL,
           SCREEN_WIDTH/3 + (WIDTH + 1) * PIXEL + (column  + 1) * PIXEL, (row + 1) * PIXEL + PIXEL, YELLOW);
				}
			}
		}
}

void draw_moving_block()
{
  /*Drawing the currently falling blocks*/
	for(int column = 0; column < WIDTH; column++)
		{
			for(int row = 0; row < HEIGHT; row++)
			{
				
				if(moving_block_position[column][row] == 1)
				{
					gfx_filledRect(SCREEN_WIDTH/3 + column * PIXEL, row * PIXEL + PIXEL,
           SCREEN_WIDTH/3 + (column  + 1) * PIXEL, (row + 1) * PIXEL + PIXEL, GREEN);
				}
				else if(moving_block_position[column][row] == 2)
				{
					gfx_filledRect(SCREEN_WIDTH/3 + column * PIXEL, row * PIXEL + PIXEL,
           SCREEN_WIDTH/3 + (column  + 1) * PIXEL, (row + 1) * PIXEL + PIXEL, YELLOW);
				}
			}
		}
}

void generate_starting_blocks()
{
  /*Generating the current falling block*/
	piece_shape = rand() % 7;
	piece_rotation = rand() % 4;
	for(int row = 0; row<4; row++)
		{
			for(int column = 0; column<4; column++)
			{
				moving_block_position[WIDTH/2+column][row]=pieces[piece_shape][piece_rotation][row][column];
			}
		}
}

void create_next_block()
{
  piece_shape = next_piece_shape;
  piece_rotation = next_piece_rotation;
	next_piece_shape = rand() % 7;
	next_piece_rotation = rand() % 4;
  /*Make the next block to fall currently falling block*/
  for(int row = 0; row<4; row++)
		{
			for(int column = 0; column<4; column++)
			{
				moving_block_position[WIDTH/2+column][row]=next_block[column][row];
			}
		}
  /*Generate next block to fall*/
  for(int row = 0; row<4; row++)
		{
			for(int column = 0; column<4; column++)
			{
				next_block[column][row]=pieces[next_piece_shape][next_piece_rotation][row][column];
			}
		}
    falling = TRUE;
}

void move_to_side(int side)
{
  int direction;
  int move = TRUE;
	if(side == TRUE)
	{
    direction = RIGHT;
	}
	if(side == FALSE)
	{
    direction = LEFT;
	}

  /*Check if there is something right next to falling block
  if there is something or it is wall make move = FALSE*/
	for(int row = 0; row < HEIGHT; row++)
		{
			for(int column = 0; column < WIDTH; column++)
			{
				if(moving_block_position[column][row] !=0)
				{
          if(column == WIDTH - 1 && direction == RIGHT)
          {
            move = FALSE;
          }
          else if(column == 0 && direction == LEFT)
          {
            move = FALSE;
          }
					else if(block_position[column + direction][row] != 0)
					{
						move = FALSE;
					}
				}
			}
		}
   
  /*If move == TRUE move the falling block 1 SQUERE to the side*/
	if (move == TRUE && direction == LEFT)
	{
		for(int row = HEIGHT-1; row >= 0; row--)
		{
			for(int column = 0; column < WIDTH; column++)
			{
				if (moving_block_position[column][row] != 0)
				{
					moving_block_position[column+direction][row] = moving_block_position[column][row];
					moving_block_position[column][row] = 0;
				}
			}
		}
    SDL_Delay(DELAY);
	}
  else if (move == TRUE && direction == RIGHT)
  {
    for(int row = HEIGHT-1; row >= 0; row--)
		{
			for(int column = WIDTH - 1; column >= 0; column--)
			{
				if (moving_block_position[column][row] != 0)
				{
					moving_block_position[column+direction][row] = moving_block_position[column][row];
					moving_block_position[column][row] = 0;
				}
			}
		}
    SDL_Delay(DELAY);
  }
}

void rotate_block()
{
  /*Find the center of rotation of falling block*/
  int rotation_y, rotation_x;
  for(int column = 0; column < WIDTH; column++)
  {
    for(int row = 0; row < HEIGHT; row++)
    {
      if(moving_block_position[column][row] == 2)
      {
        rotation_x = column;
        rotation_y = row;
      }
    }
  }
  calculate_block_origin(rotation_x, rotation_y);
	
}

void calculate_block_origin(int rotation_x, int rotation_y)
{
  int next_rotation_center_x, next_rotation_center_y;
  int block_origin_x, block_origin_y;
  int next_rotation;
  next_rotation = (piece_rotation + 1)%4;
  piece_rotation = next_rotation;
  /*find the center of rotation of the next rotation of the block*/
  for(int column = 0; column < 4; column++)
  {
    for(int row = 0; row < 4; row++)
    {
      if(pieces[piece_shape][next_rotation][column][row] == 2)
      {
        next_rotation_center_x = column;
        next_rotation_center_y = row;
        break;
      }
    }
  }
  /*calculate the origin of next rotation*/
  block_origin_x = rotation_x - next_rotation_center_x;
  block_origin_y = rotation_y - next_rotation_center_y;
  /*check if rotation is posible and rotate*/
  if(block_origin_x > 0 && block_origin_x + 4 <= WIDTH && block_origin_y > 0 && block_origin_y + 4 < HEIGHT)
  {
    if(check_rotation_posibility(block_origin_x, block_origin_y, next_rotation))
    {
      for(int column = 0; column < WIDTH; column++)
      {
        for(int row = 0; row < HEIGHT; row++)
        {
          moving_block_position[column][row] = 0;
        }
      }
      for(int column = 0; column < 4; column++)
      {
        for(int row = 0; row < 4; row++)
        {
          
          moving_block_position[block_origin_x + column][block_origin_y + row] = pieces[piece_shape][next_rotation][column][row];
        }
      }
    }
  }
  SDL_Delay(100);
}

int check_rotation_posibility(int block_origin_x, int block_origin_y, int next_rotation)
{
  /*check if rotation is posible according to fallen blocks*/
  for(int column = 0; column < 4; column++)
      {
        for(int row = 0; row < 4; row++)
        {
          if(block_position[block_origin_x + column][block_origin_y + row] == 1 &&
          pieces[piece_shape][next_rotation][column][row])
          {
            return FALSE;
          }
        }
      }
  return TRUE;
}

void move_down()
{
  falling_blocks();
}

void falling_blocks()
{
  /*Check if there is something right under falling block
  if there is something or it is bottom make falling = FALSE*/
	for(int row = 0; row < HEIGHT; row++)
		{
			for(int column = 0; column<WIDTH; column++)
			{
				if(moving_block_position[column][row] !=0)
				{
          if(row == HEIGHT - 1)
          {
            falling = FALSE;
          }
					else if(block_position[column][row + 1] != 0)
					{
						falling = FALSE;
					}
				}
			}
		}
  /*If falling == TRUE move the falling block 1 SQUERE down
  If falling == FALSE add the falling block to array with fallen blocks*/
	if (falling == TRUE)
	{
		for(int row = HEIGHT-1; row >= 0; row--)
		{
			for(int column = 0; column < WIDTH; column++)
			{
				if (moving_block_position[column][row] != 0)
				{
					moving_block_position[column][row+1] = moving_block_position[column][row];
					moving_block_position[column][row] = 0;
				}
			}
		}
	}
  else{
    add_to_fallen();
  }
		
}

void add_to_fallen()
{
  /*Add the falling block to array with fallen blocks*/
  for(int row = 0; row < HEIGHT; row++)
		{
			for(int column = 0; column<WIDTH; column++)
			{
				if(moving_block_position[column][row] !=0)
				{
					block_position[column][row] = 1;
          moving_block_position[column][row] = 0;
				}
			}
		}
}

void loosing()
{
  for(int column = 0; column < WIDTH; column++)
  {
    if(block_position[column][0] == 1)
    {
      gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1, BLACK);
      gfx_textout(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, "YOU LOOSE!!", RED);
      gfx_updateScreen();
      gfx_getkey();
      exit(0);
    }
  }
}

void check_full_line()
{
  /*Check if line is full and if it is call function to remove it*/
  int full_line = TRUE;
  for(int row = HEIGHT - 1; row >= 0; row--)
		{
			for(int column = 0; column < WIDTH; column++)
			{
				
				if(block_position[column][row] == 0)
				{
					full_line = FALSE;
          break;
				}
        else{
          full_line = TRUE;
        }
			}
      if(full_line == TRUE)
      {
        remove_full_line(row);
      }
		}
}

void remove_full_line(int row_to_remove)
{
  /*Removing line*/
  for(int column = 0; column < WIDTH; column++)
  {
    block_position[column][row_to_remove] = 0;
  }
  /*Moving blocks after line removal*/
  for(int column = 0; column < WIDTH; column++)
  {
    for(int row = row_to_remove; row >= 0; row--)
    {
      if(row != 0)
      {
        if(block_position[column][row-1] == 1)
        {
          block_position[column][row] = 1;
          block_position[column][row-1] = 0;
        }
      }
    }
  }
}