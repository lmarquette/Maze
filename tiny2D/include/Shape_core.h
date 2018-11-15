#pragma once
#include "Shape_data.h"
#include "Spawn_Stack_core.h"

namespace Shape
{
	namespace Rect
	{
		void init(Factory *f, int array_size);

		int make_Instance(Factory *f);

		int collision(const Data *pa, const Data *pb);

		//minkowski sum
		//if collision direction is important, use this
		int collision_with_Dir(const Data *a, const Data *b);
	}

}

namespace Shape
{
	namespace Rect
	{
		void init(Factory *f, int array_size)
		{
			f->array_size = array_size;
			f->rect = (Data*)malloc(sizeof(Data)*f->array_size);

			Spawn_Stack::init(&f->spawn_stack, array_size);
		}

		int make_Instance(Factory *f)
		{
			return Spawn_Stack::make(&f->spawn_stack);
		}

		int collision(const Data *pa, const Data *pb)
		{
			if (pa->x + pa->w < pb->x) return 0;
			if (pa->x > pb->x + pb->w) return 0;
			if (pa->y + pa->h < pb->y) return 0;
			if (pa->y > pb->y + pb->h) return 0;
			return 1;
		}

		//minkowski sum
		//if collision direction is important, use this
		int collision_with_Dir(const Data *a, const Data *b)
		{
			//                           1        2           3             4
			enum { NO_COLLISION = 0, TOP_OF_B, RIGHT_OF_B, BOTTOM_OF_B, LEFT_OF_B };

			float w = 0.5 * (a->w + b->w);
			float h = 0.5 * (a->h + b->h);
			float dx = a->x - b->x + 0.5*(a->w - b->w);
			float dy = a->y - b->y + 0.5*(a->h - b->h);

			if (dx*dx <= w * w && dy*dy <= h * h)
			{
				float wy = w * dy;
				float hx = h * dx;

				if (wy > hx)
				{
					return (wy + hx > 0) ? BOTTOM_OF_B : LEFT_OF_B;
				}
				else
				{
					return (wy + hx > 0) ? RIGHT_OF_B : TOP_OF_B;
				}
			}
			return NO_COLLISION;
		}
	}


	
}