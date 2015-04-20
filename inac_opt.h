
#ifndef _INAC_OPT_H_
#define _INAC_OPT_H_

#include <stdint.h>

enum 
{
	AFFINITY_DISABLED = 0,
	AFFINITY_ENABLED,
};

void inac_opt_set_affinity(int32_t affinity);
int32_t inac_opt_get_affinity(void);

#endif /* _INAC_OPT_H_ */

