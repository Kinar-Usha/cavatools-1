struct lru_fsm_t cache_fsm_4way[] = {
/* Header */	{ 4,  24 }, /* Ways, Number of states */
/*  0 */	{ 0,   0*4 }, /* 0-1-2-3 */
		{ 1,   6*4 }, /* 1-0-2-3 */
		{ 2,  12*4 }, /* 2-0-1-3 */
		{ 3,  18*4 }, /* 3-0-1-2 */
/*  1 */	{ 0,   1*4 }, /* 0-1-3-2 */
		{ 1,   7*4 }, /* 1-0-3-2 */
		{ 3,  18*4 }, /* 3-0-1-2 */
		{ 2,  12*4 }, /* 2-0-1-3 */
/*  2 */	{ 0,   2*4 }, /* 0-2-1-3 */
		{ 2,  12*4 }, /* 2-0-1-3 */
		{ 1,   6*4 }, /* 1-0-2-3 */
		{ 3,  19*4 }, /* 3-0-2-1 */
/*  3 */	{ 0,   3*4 }, /* 0-2-3-1 */
		{ 2,  13*4 }, /* 2-0-3-1 */
		{ 3,  19*4 }, /* 3-0-2-1 */
		{ 1,   6*4 }, /* 1-0-2-3 */
/*  4 */	{ 0,   4*4 }, /* 0-3-1-2 */
		{ 3,  18*4 }, /* 3-0-1-2 */
		{ 1,   7*4 }, /* 1-0-3-2 */
		{ 2,  13*4 }, /* 2-0-3-1 */
/*  5 */	{ 0,   5*4 }, /* 0-3-2-1 */
		{ 3,  19*4 }, /* 3-0-2-1 */
		{ 2,  13*4 }, /* 2-0-3-1 */
		{ 1,   7*4 }, /* 1-0-3-2 */
/*  6 */	{ 1,   6*4 }, /* 1-0-2-3 */
		{ 0,   0*4 }, /* 0-1-2-3 */
		{ 2,  14*4 }, /* 2-1-0-3 */
		{ 3,  20*4 }, /* 3-1-0-2 */
/*  7 */	{ 1,   7*4 }, /* 1-0-3-2 */
		{ 0,   1*4 }, /* 0-1-3-2 */
		{ 3,  20*4 }, /* 3-1-0-2 */
		{ 2,  14*4 }, /* 2-1-0-3 */
/*  8 */	{ 1,   8*4 }, /* 1-2-0-3 */
		{ 2,  14*4 }, /* 2-1-0-3 */
		{ 0,   0*4 }, /* 0-1-2-3 */
		{ 3,  21*4 }, /* 3-1-2-0 */
/*  9 */	{ 1,   9*4 }, /* 1-2-3-0 */
		{ 2,  15*4 }, /* 2-1-3-0 */
		{ 3,  21*4 }, /* 3-1-2-0 */
		{ 0,   0*4 }, /* 0-1-2-3 */
/* 10 */	{ 1,  10*4 }, /* 1-3-0-2 */
		{ 3,  20*4 }, /* 3-1-0-2 */
		{ 0,   1*4 }, /* 0-1-3-2 */
		{ 2,  15*4 }, /* 2-1-3-0 */
/* 11 */	{ 1,  11*4 }, /* 1-3-2-0 */
		{ 3,  21*4 }, /* 3-1-2-0 */
		{ 2,  15*4 }, /* 2-1-3-0 */
		{ 0,   1*4 }, /* 0-1-3-2 */
/* 12 */	{ 2,  12*4 }, /* 2-0-1-3 */
		{ 0,   2*4 }, /* 0-2-1-3 */
		{ 1,   8*4 }, /* 1-2-0-3 */
		{ 3,  22*4 }, /* 3-2-0-1 */
/* 13 */	{ 2,  13*4 }, /* 2-0-3-1 */
		{ 0,   3*4 }, /* 0-2-3-1 */
		{ 3,  22*4 }, /* 3-2-0-1 */
		{ 1,   8*4 }, /* 1-2-0-3 */
/* 14 */	{ 2,  14*4 }, /* 2-1-0-3 */
		{ 1,   8*4 }, /* 1-2-0-3 */
		{ 0,   2*4 }, /* 0-2-1-3 */
		{ 3,  23*4 }, /* 3-2-1-0 */
/* 15 */	{ 2,  15*4 }, /* 2-1-3-0 */
		{ 1,   9*4 }, /* 1-2-3-0 */
		{ 3,  23*4 }, /* 3-2-1-0 */
		{ 0,   2*4 }, /* 0-2-1-3 */
/* 16 */	{ 2,  16*4 }, /* 2-3-0-1 */
		{ 3,  22*4 }, /* 3-2-0-1 */
		{ 0,   3*4 }, /* 0-2-3-1 */
		{ 1,   9*4 }, /* 1-2-3-0 */
/* 17 */	{ 2,  17*4 }, /* 2-3-1-0 */
		{ 3,  23*4 }, /* 3-2-1-0 */
		{ 1,   9*4 }, /* 1-2-3-0 */
		{ 0,   3*4 }, /* 0-2-3-1 */
/* 18 */	{ 3,  18*4 }, /* 3-0-1-2 */
		{ 0,   4*4 }, /* 0-3-1-2 */
		{ 1,  10*4 }, /* 1-3-0-2 */
		{ 2,  16*4 }, /* 2-3-0-1 */
/* 19 */	{ 3,  19*4 }, /* 3-0-2-1 */
		{ 0,   5*4 }, /* 0-3-2-1 */
		{ 2,  16*4 }, /* 2-3-0-1 */
		{ 1,  10*4 }, /* 1-3-0-2 */
/* 20 */	{ 3,  20*4 }, /* 3-1-0-2 */
		{ 1,  10*4 }, /* 1-3-0-2 */
		{ 0,   4*4 }, /* 0-3-1-2 */
		{ 2,  17*4 }, /* 2-3-1-0 */
/* 21 */	{ 3,  21*4 }, /* 3-1-2-0 */
		{ 1,  11*4 }, /* 1-3-2-0 */
		{ 2,  17*4 }, /* 2-3-1-0 */
		{ 0,   4*4 }, /* 0-3-1-2 */
/* 22 */	{ 3,  22*4 }, /* 3-2-0-1 */
		{ 2,  16*4 }, /* 2-3-0-1 */
		{ 0,   5*4 }, /* 0-3-2-1 */
		{ 1,  11*4 }, /* 1-3-2-0 */
/* 23 */	{ 3,  23*4 }, /* 3-2-1-0 */
		{ 2,  17*4 }, /* 2-3-1-0 */
		{ 1,  11*4 }, /* 1-3-2-0 */
		{ 0,   5*4 }, /* 0-3-2-1 */
};
