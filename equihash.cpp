/// System includes
#include <stdio.h>
#include <assert.h>

// CUDA runtime
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
//#include "eaglesong.h"
#include "../miner.h"
extern uint32_t c_solve_gpu(uint8_t *input, uint8_t *target, uint64_t *nonce, uint32_t gpuid);

extern "C" int scanhash_equihash(int thr_id, struct work *work, uint32_t max_nonce, unsigned long *hashes_done)
{
	uint32_t _ALIGN(64) endiandata[20];
	uint32_t *pdata = work->data;
	uint32_t *ptarget = work->target;
	for (int k=0; k < 19; k++)
		be32enc(&endiandata[k], pdata[k]);
	uint64_t nonce = ((work->data[0])<<32)+work->data[1];
	
	c_solve_gpu((uint8_t *)(pdata+2), (uint8_t *)target,&nonce, thr_id);
	work_set_target_ratio(work, nonce);
	*hashes_done = pdata[9] - nonce;

	return 0;
}
