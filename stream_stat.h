
#ifndef _STREAM_STAT_H_
#define _STREAM_STAT_H_

#ifdef INAC_CARD_SIDE
#include <stdint.h>
#else
#ifdef INAC_HOST_DRV
#include "byteorder.h"
#ifdef INAC_LINUX
#include <linux/types.h>
#endif

#else
#ifdef INAC_LINUX
#include <stdint.h>
#endif

#endif
#endif

#ifdef INAC_WIN
#include "win_dep.h"
#endif

typedef struct stream_stat_s
{
	uint64_t	packets;
	uint64_t	bytes;
	uint64_t	dropped;
	uint64_t	dropped_bytes;
	uint64_t	free;
} stream_stat_t;

#ifdef INAC_HOST_DRV
static inline void stream_stat_ntoh(stream_stat_t *stats)
{
	stats->packets = custom_be64_to_cpu(stats->packets);		/* total packets received	*/
	stats->bytes = custom_be64_to_cpu(stats->bytes);		/* total bytes received	*/
	stats->dropped = custom_be64_to_cpu(stats->dropped);		/* total dropped	*/
	stats->dropped_bytes = custom_be64_to_cpu(stats->dropped_bytes);		/* total dropped bytes	*/

}

#endif

#endif /* _STREAM_STAT_H_ */

