#ifndef PLANT_H
#define PLANT_H

typedef struct {
	struct obj o;
	char const *pre, small, big, *post;
	coord_t tmp_min, tmp_max;
	ucoord_t rn_min, rn_max;
	unsigned char yield;
	struct obj fruit;
	unsigned y;
} plant_t;

extern struct plant plants[];
