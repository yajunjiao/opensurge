/*
 * Open Surge Engine
 * physics/obstaclemap.c - physics system: obstacle map
 * Copyright (C) 2011, 2018  Alexandre Martins <alemartf(at)gmail(dot)com>
 * http://opensurge2d.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "obstaclemap.h"
#include "obstacle.h"
#include "physicsactor.h"
#include "../../core/util.h"
#include "../../core/image.h"
#include "../../core/video.h"

/* linked list of obstacles */
typedef struct obstacle_list_t obstacle_list_t;
struct obstacle_list_t
{
    obstacle_t *data;
    obstacle_list_t *next;
};

/* obstaclemap class */
struct obstaclemap_t
{
    obstacle_list_t *list;
};

/* private methods */
static const obstacle_t* pick_best_obstacle(const obstacle_t *a, const obstacle_t *b, int x1, int y1, int x2, int y2, movmode_t mm);

/* public methods */
obstaclemap_t* obstaclemap_create()
{
    obstaclemap_t *o = mallocx(sizeof *o);
    o->list = NULL;
    return o;
}

obstaclemap_t* obstaclemap_destroy(obstaclemap_t *obstaclemap)
{
    obstacle_list_t *l, *next;

    for(l = obstaclemap->list; l != NULL; l = next) {
        next = l->next;
        obstacle_destroy(l->data);
        free(l);
    }

    free(obstaclemap);
    return NULL;
}

void obstaclemap_add_obstacle(obstaclemap_t *obstaclemap, obstacle_t *obstacle)
{
    obstacle_list_t *l = mallocx(sizeof *l);
    l->data = obstacle;
    l->next = obstaclemap->list;
    obstaclemap->list = l;
}

const obstacle_t* obstaclemap_get_best_obstacle_at(const obstaclemap_t *obstaclemap, int x1, int y1, int x2, int y2, movmode_t mm)
{
    obstacle_t *o = NULL;
    obstacle_list_t *l;

    for(l = obstaclemap->list; l != NULL; l = l->next) {
        /* l->data is colliding with the sensor */
        if(obstacle_got_collision(l->data, x1, y1, x2, y2)) {
            /* the C standard mandates short-circuit evaluation */
            if((o == NULL) || pick_best_obstacle(l->data, o, x1, y1, x2, y2, mm) == l->data) {
                /* l->data is better than o */
                o = l->data;
            }
        }
    }

    return o;
}

int obstaclemap_obstacle_exists(const obstaclemap_t* obstaclemap, int x, int y)
{
    obstacle_list_t *l;

    for(l = obstaclemap->list; l != NULL; l = l->next) {
        if(obstacle_got_collision(l->data, x, y, x, y))
            return TRUE;
    }

    return FALSE;
}

/* 2D raycasting */
const obstacle_t* obstaclemap_raycast(const obstaclemap_t* obstaclemap, v2d_t origin, v2d_t direction, float max_distance, v2d_t* hitpoint, float* distance)
{
    /* rays can't be larger than infty */
    const float infty = 2.0f * max(VIDEO_SCREEN_W, VIDEO_SCREEN_H);
    v2d_t p = origin;

    /* sanity checks */
    max_distance = clip(max_distance, 0.0f, infty);
    if(v2d_magnitude(direction) < EPSILON || max_distance < EPSILON)
        return NULL;

    /* DDA algorithm */
    direction = v2d_normalize(direction);

    /* 404 not found */
    return NULL;
}

/* private methods */

/* considering that a and b overlap, which one should we pick? */
const obstacle_t* pick_best_obstacle(const obstacle_t *a, const obstacle_t *b, int x1, int y1, int x2, int y2, movmode_t mm)
{
    static int (*w)(const obstacle_t*) = obstacle_get_width;
    static int (*h)(const obstacle_t*) = obstacle_get_height;
    int ha, hb, xa, xb, ya, yb, x, y;

    /* solid obstacles are better than one-way platforms */
    if(!obstacle_is_solid(a) && obstacle_is_solid(b))
        return b;

    if(!obstacle_is_solid(b) && obstacle_is_solid(a))
        return a;

    /* configuring */
    xa = (int)obstacle_get_position(a).x;
    xb = (int)obstacle_get_position(b).x;
    ya = (int)obstacle_get_position(a).y;
    yb = (int)obstacle_get_position(b).y;
    x = (x1+x2)/2; /* x1 == x2 in floor/ceiling mode */
    y = (y1+y2)/2; /* y1 == y2 in left/right wall mode */

    /* get the tallest obstacle */
    switch(mm) {
        case MM_FLOOR:
            ha = obstacle_get_height_at(a, x-xa, FROM_BOTTOM);
            hb = obstacle_get_height_at(b, x-xb, FROM_BOTTOM);
            return (ya + (h(a)-1) - ha < yb + (h(b)-1) - hb) ? a : b;

        case MM_LEFTWALL:
            ha = obstacle_get_height_at(a, y-ya, FROM_LEFT);
            hb = obstacle_get_height_at(b, y-yb, FROM_LEFT);
            return (xa + ha > xb + hb) ? a : b;

        case MM_CEILING:
            ha = obstacle_get_height_at(a, x-xa, FROM_TOP);
            hb = obstacle_get_height_at(b, x-xb, FROM_TOP);
            return (ya + ha > yb + hb) ? a : b;

        case MM_RIGHTWALL:
            ha = obstacle_get_height_at(a, y-ya, FROM_RIGHT);
            hb = obstacle_get_height_at(b, y-yb, FROM_RIGHT);
            return (xa + (w(a)-1) - ha < xb + (w(b)-1) - hb) ? a : b;
    }

    /* this shouldn't happen */
    return a;
}
