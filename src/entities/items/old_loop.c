/*
 * Open Surge Engine
 * old_loop.c - old loop system
 * Copyright (C) 2010, 2018  Alexandre Martins <alemartf@gmail.com>
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

/* This old loop system has been removed from the engine */

#include "old_loop.h"
#include "util/itemutil.h"
#include "../../scenes/level.h"
#include "../../core/stringutil.h"
#include "../../core/util.h"
#include "../player.h"
#include "../brick.h"
#include "../item.h"
#include "../enemy.h"
#include "../actor.h"
#include "../../physics/physicsactor.h"

/* loop class */
typedef struct oldloop_t oldloop_t;
struct oldloop_t {
    item_t item; /* base class */
    char *sprite_name; /* sprite name */
    void (*on_collision)(player_t*); /* strategy pattern */
};

static item_t* loop_create(void (*strategy)(player_t*), const char *sprite_name);
static void loop_init(item_t *item);
static void loop_release(item_t* item);
static void loop_update(item_t* item, player_t** team, int team_size, brick_list_t* brick_list, item_list_t* item_list, enemy_list_t* enemy_list);
static void loop_render(item_t* item, v2d_t camera_position);

static void loopright_strategy(player_t *player);
static void looptop_strategy(player_t *player);
static void loopleft_strategy(player_t *player);
static void loopnone_strategy(player_t *player);
static void loopfloor_strategy(player_t *player);
static void loopfloornone_strategy(player_t *player);
static void loopfloortop_strategy(player_t *player);

/* public methods */
item_t* loopright_create()
{
    return loop_create(loopright_strategy, "SD_LOOPRIGHT");
}

item_t* looptop_create()
{
    return loop_create(looptop_strategy, "SD_LOOPMIDDLE");
}

item_t* loopleft_create()
{
    return loop_create(loopleft_strategy, "SD_LOOPLEFT");
}

item_t* loopnone_create()
{
    return loop_create(loopnone_strategy, "SD_LOOPNONE");
}

item_t* loopfloor_create()
{
    return loop_create(loopfloor_strategy, "SD_LOOPFLOOR");
}

item_t* loopfloornone_create()
{
    return loop_create(loopfloornone_strategy, "SD_LOOPFLOORNONE");
}

item_t* loopfloortop_create()
{
    return loop_create(loopfloortop_strategy, "SD_LOOPFLOORTOP");
}

/* private strategies */

/* Loop Right: right loop entrance */
void loopright_strategy(player_t *player)
{
    ;
}

/* Loop Top (x-axis): toggles left/right walls */
void looptop_strategy(player_t *player)
{
    ;
}

/* Loop Left: left loop entrance */
void loopleft_strategy(player_t *player)
{
    ;
}

/* Loop None: deactivate loops (x-axis), restoring the walls */
void loopnone_strategy(player_t *player)
{
    ;
}

/* Loop Floor (y-axis): bottom loop entrance */
void loopfloor_strategy(player_t *player)
{
    ;
}

/* Loop Floor None: deactivate loops (y-axis), restoring the floor */
void loopfloornone_strategy(player_t *player)
{
    ;
}

/* Loop Floor Top: activate left and right walls (x-axis) */
void loopfloortop_strategy(player_t *player)
{
    ;
}

/* private methods */
item_t* loop_create(void (*strategy)(player_t*), const char *sprite_name)
{
    item_t *item = mallocx(sizeof(oldloop_t));
    oldloop_t *me = (oldloop_t*)item;

    item->init = loop_init;
    item->release = loop_release;
    item->update = loop_update;
    item->render = loop_render;

    me->on_collision = strategy;
    me->sprite_name = str_dup(sprite_name);

    return item;
}

void loop_init(item_t *item)
{
    oldloop_t *me = (oldloop_t*)item;

    item->always_active = FALSE;
    item->obstacle = FALSE;
    item->bring_to_back = FALSE;
    item->preserve = TRUE;
    item->actor = actor_create();

    actor_change_animation(item->actor, sprite_get_animation(me->sprite_name, 0));
}

void loop_release(item_t* item)
{
    oldloop_t *me = (oldloop_t*)item;
    free(me->sprite_name);
    actor_destroy(item->actor);
}

void loop_update(item_t* item, player_t** team, int team_size, brick_list_t* brick_list, item_list_t* item_list, enemy_list_t* enemy_list)
{
    actor_t *act = item->actor;
    act->visible = level_editmode();
}

void loop_render(item_t* item, v2d_t camera_position)
{
    actor_render(item->actor, camera_position);
}