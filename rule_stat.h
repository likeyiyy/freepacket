
#ifndef _RULE_STAT_H_
#define _RULE_STAT_H_

#include "rule_defs.h"
#ifdef INAC_WIN
#include "win_dep.h"
#endif
typedef struct rule_stat_s
{
	int32_t		count;
	int32_t		reserved_0;
	uint64_t	hits;
	uint64_t	logs[LOG_ACTION_MAX];
	uint64_t	blocks[BLOCK_ACTION_MAX];
} rule_stat_t;

#if 0

#ifdef INAC_HOST_DRV
static inline void rule_stat_ntoh(rule_stat_t *stats)
{
	int32_t i;

	stats->count = be32_to_cpu(stats->count);		/* total rule count	*/
	stats->hits = be64_to_cpu(stats->hits);		/* total hits	*/

	for (i = 0; i < LOG_ACTION_MAX; i ++)
		stats->logs[i] = be64_to_cpu(stats->logs[i]);

	for (i = 0; i < BLOCK_ACTION_MAX; i ++)
		stats->blocks[i] = be64_to_cpu(stats->blocks[i]);		/* total dropped	*/

}

#endif

#else

#ifdef INAC_HOST_DRV
static inline void rule_stat_ntoh(rule_stat_t *stats)
{}

#endif

#endif


#endif /* _RULE_STAT_H_ */
