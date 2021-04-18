struct lru_fsm_t cache_fsm_3way[] = {
/* Header */	{ 3,   6 }, /* Ways, Number of states */
/*  0 */	{ 0,   0*3 }, /* 0-1-2 */
		{ 1,   2*3 }, /* 1-0-2 */
		{ 2,   4*3 }, /* 2-0-1 */
/*  1 */	{ 0,   1*3 }, /* 0-2-1 */
		{ 2,   4*3 }, /* 2-0-1 */
		{ 1,   2*3 }, /* 1-0-2 */
/*  2 */	{ 1,   2*3 }, /* 1-0-2 */
		{ 0,   0*3 }, /* 0-1-2 */
		{ 2,   5*3 }, /* 2-1-0 */
/*  3 */	{ 1,   3*3 }, /* 1-2-0 */
		{ 2,   5*3 }, /* 2-1-0 */
		{ 0,   0*3 }, /* 0-1-2 */
/*  4 */	{ 2,   4*3 }, /* 2-0-1 */
		{ 0,   1*3 }, /* 0-2-1 */
		{ 1,   3*3 }, /* 1-2-0 */
/*  5 */	{ 2,   5*3 }, /* 2-1-0 */
		{ 1,   3*3 }, /* 1-2-0 */
		{ 0,   1*3 }, /* 0-2-1 */
};
