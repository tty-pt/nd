#ifndef SPACETIME_H
#define SPACETIME_H

#include <limits.h>
#include <stdint.h>
#include <time.h>
#include "object.h"

// adds 2^DAYTICK_Y to day tick until it reaches DAYSIZE
#define DAYTICK_Y	10
#define NIGHT_IS	(day_n == 1)

#define Y_COORD 0
#define X_COORD 1

#define COORD_MIN SHRT_MIN
#define COORD_MAX SHRT_MAX
#define UCOORD_MAX USHRT_MAX

#define DIM 2
#define WDIM X_COORD
#define POOP register int I; for (I = 0; I < DIM; I++)
#define POOP3D register int I; for (I = 0; I < 3; I++)
#define POOP4D register int I; for (I = 0; I < 4; I++)

#define OBITS(code) (code >> 48)

#define VIEW_AROUND 3
#define VIEW_SIZE ((VIEW_AROUND<<1) + 1)
#define VIEW_M VIEW_SIZE * VIEW_SIZE
#define VIEW_BDI (VIEW_SIZE * (VIEW_SIZE - 1))
#define MORTON_READ(pos) (* (morton_t *) pos)

typedef int16_t coord_t;
typedef uint16_t ucoord_t;

typedef coord_t point_t[DIM];
typedef ucoord_t upoint_t[DIM];

typedef coord_t point3D_t[3];
typedef ucoord_t upoint3D_t[3];
typedef coord_t point4D_t[4];
typedef ucoord_t upoint4D_t[4];

typedef point4D_t pos_t;
typedef upoint4D_t upos_t;

typedef uint64_t morton_t; // 4d morton, btw
typedef int64_t smorton_t;

struct rect {
	point_t s;
	upoint_t l;
};

struct rect4D {
	point4D_t s;
	upoint4D_t l;
};

typedef struct {
	const char name[16];
	const char other[16];
	enum exit simm;
	coord_t dim, dis;
} exit_t;

struct cmd_dir {
	char dir;
	enum exit e;
	morton_t rep;
};

extern unsigned long long day_tick;
extern unsigned short day_n;
extern double tick;
extern enum exit e_map[];
extern exit_t exit_map[];

time_t get_tick();

morton_t pos_morton(pos_t);
void morton_pos(pos_t p, morton_t code);

int e_exit_can(OBJ *player, enum exit e);
int e_ground(dbref room, enum exit e);

void pos_move(pos_t d, pos_t o, enum exit e);
enum exit dir_e(const char dir);
const char e_dir(enum exit e);
enum exit e_simm(enum exit e);
const char * e_name(enum exit e);
const char * e_other(enum exit e);
morton_t point_rel_idx(point_t p, point_t s, smorton_t w);

void st_update(double dt);
int st_v(dbref player_ref, const char *dir);
int st_teleport(dbref player_ref, struct cmd_dir cd);
void st_start(dbref player_ref);

void st_init();
void st_close();
void st_sync();
void st_run(dbref player_ref, char *symbol);
void do_stchown(int fd, int argc, char *argv[]);
void do_streload(int fd, int argc, char *argv[]);

#endif
