#pragma once

//BEAR RUN

#include "include/Engine_core.h"
#include "include/Light_core.h"
#include "include/Actor_core.h"
#include "include/Particle_core.h"
#include "Stack.h"
#include <time.h>

namespace My_Game
{
	//create this function
	void init_Maze(Grid::Grid *g);
	//create this function
	int next_Maze_Step(Grid::Point *current_pos, Grid::Grid *g, int &done);

	namespace Command
	{

	}

	namespace Assets
	{
		namespace Animation
		{

		}
	}

	namespace World
	{

		namespace Parameters
		{
			const int maze_n_rows = 80;
			const int maze_n_cols = 128;
			int step_frequency = 5;

		}

		Tileset::Tileset tileset;
		Font::Font text;

		Particle::Emitter emitter;

		Grid::Grid maze;

		Grid::Point current_maze_position = { 1, 1 };

		Grid_Camera::Grid_Camera camera;

		Stacking::Stacking stack;

		unsigned int last_step_time = 0;

	}

	void init(int screen_w, int screen_h)
	{
		//initialize all systems and open game window
		Engine::init("hello", screen_w, screen_h);

		Tileset::init(&World::tileset, "road_tileset.txt", Engine::renderer);
		Particle::init(&World::emitter, "dirt.txt", 512, Engine::renderer);

		Grid::init(&World::maze, World::Parameters::maze_n_rows, World::Parameters::maze_n_cols);

		Grid_Camera::init(&World::camera, Engine::screen_width, Engine::screen_height);
		World::camera.world_coord.x = 0;
		World::camera.world_coord.y = 0;
		World::camera.world_coord.w = World::Parameters::maze_n_cols;
		World::camera.world_coord.h = World::Parameters::maze_n_rows;

		Stacking::init(&World::stack, World::Parameters::maze_n_cols * World::Parameters::maze_n_rows);

	}

	void begin_Play(unsigned int current_time)
	{
		Font::set_Screen_Size(&World::text, 16, 16);
		srand(time(0));
		init_Maze(&World::maze);
		World::maze.data[World::current_maze_position.y * World::Parameters::maze_n_cols + World::current_maze_position.x] = 4;


	}

	void update(unsigned int current_time, float dt)
	{

		Engine::event_Loop();



		if (current_time - World::last_step_time >= World::Parameters::step_frequency)
		{
			World::last_step_time = current_time;
			int done = 0;
			int dir = next_Maze_Step(&World::current_maze_position, &World::maze, done);


		}


	}

	void draw(unsigned int current_time)
	{

		SDL_RenderClear(Engine::renderer);

		Grid_Camera::calibrate(&World::camera);

		Tileset::draw_Grid(&World::tileset, &World::camera, &World::maze, Engine::renderer);

		Particle::draw(&World::emitter, &World::camera, current_time, Engine::renderer);

		//flip buffers
		SDL_RenderPresent(Engine::renderer);
	}

	void init_Maze(Grid::Grid *g)
	{
		for (int i = 0; i < World::Parameters::maze_n_rows; i++)
		{
			for (int j = 0; j < World::Parameters::maze_n_cols; j++)
			{
				g->data[i * g->n_cols + j] = 2;
			}
		}

	}

	int next_Maze_Step(Grid::Point *current_pos, Grid::Grid *g, int &done)
	{
		int count = 0;
		int *direction = new int[4];

		//current_pos->y * g->n_cols + current_pos->x current position in array
		//destroy tiles you pass over, i.e. x + 1 and x + 2

		if (current_pos->x + 2 < g->n_cols && g->data[current_pos->y * g->n_cols + current_pos->x + 2] == 2)
		{
			direction[count] = 0; //right
			count++;
		}

		if (current_pos->x - 2 >= 0 && g->data[current_pos->y * g->n_cols + current_pos->x - 2] == 2)
		{
			direction[count] = 1; //left
			count++;
		}

		if (current_pos->y + 2 < g->n_rows && g->data[(current_pos->y + 2) * g->n_cols + current_pos->x] == 2)
		{
			direction[count] = 2; //down
			count++;
		}

		if (current_pos->y - 2 >= 0 && g->data[(current_pos->y - 2) * g->n_cols + current_pos->x] == 2)
		{
			direction[count] = 3; //up
			count++;
		}

		if (count == 0)
		{
			g->data[current_pos->y * g->n_cols + current_pos->x] = 1;

			if (World::stack.n_data == 0)
			{
				done = 1;
				return 0;
			}

			int prev_pos = Stacking::Pop(&World::stack);

			int tmpX = current_pos->x; //x position before popped
			int tmpY = current_pos->y;

			World::current_maze_position.x = prev_pos % g->n_cols; //updates x coordinate from original position to popped position
			World::current_maze_position.y = (prev_pos - World::current_maze_position.x) / g->n_cols;

			tmpX = (tmpX - World::current_maze_position.x) / 2; //updating x position for coordinates after pop
			tmpY = (tmpY - World::current_maze_position.y) / 2;

			g->data[(World::current_maze_position.y + tmpY) * g->n_cols + (World::current_maze_position.x + tmpX)] = 1;

			return 0;

		}

		int random_num = rand() % count;

		if (direction[random_num] == 0)
		{
			g->data[current_pos->y * g->n_cols + current_pos->x + 2] = 4;
			g->data[current_pos->y * g->n_cols + current_pos->x + 1] = 4;

			Stacking::Push(&World::stack, current_pos->y * g->n_cols + current_pos->x);

			current_pos->x += 2;

		}

		if (direction[random_num] == 1)
		{
			g->data[current_pos->y * g->n_cols + current_pos->x - 2] = 4;
			g->data[current_pos->y * g->n_cols + current_pos->x - 1] = 4;

			Stacking::Push(&World::stack, current_pos->y * g->n_cols + current_pos->x);

			current_pos->x -= 2;

		}

		if (direction[random_num] == 2)
		{
			g->data[(current_pos->y + 2) * g->n_cols + current_pos->x] = 4;
			g->data[(current_pos->y + 1) * g->n_cols + current_pos->x] = 4;

			Stacking::Push(&World::stack, current_pos->y * g->n_cols + current_pos->x);

			current_pos->y += 2;

		}

		if (direction[random_num] == 3)
		{
			g->data[(current_pos->y - 2) * g->n_cols + current_pos->x] = 4;
			g->data[(current_pos->y - 1) * g->n_cols + current_pos->x] = 4;

			Stacking::Push(&World::stack, current_pos->y * g->n_cols + current_pos->x);

			current_pos->y -= 2;

		}

		return 0;

	}
}