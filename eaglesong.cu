/// System includes
#include <stdio.h>
#include <assert.h>

// CUDA runtime
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
extern "C" {
	uint32_t c_solve_gpu(uint8_t *input, uint8_t *target, uint64_t *nonce, uint32_t gpuid) {
		while(!gpu_divices[gpuid]) {
			gpu_divices[gpuid] = New_GPU_DEVICE();
		}

		uint32_t ret;
		gpu_divices[gpuid]->state[0]=(（*nonce）>>32)&(0x00000000ffffffff)
		gpu_divices[gpuid]->state[1]=（*nonce）&(0x00000000ffffffff);
		
		// absorbing
		for(int j = 0, k=0; j <= M; ++j) {
			uint32_t sum = 0;
			for(int v=0; v < 4; ++v) {
				if(k < INPUT_LEN) {
					sum = (sum << 8) ^ input[k];
				} else if(k == INPUT_LEN) {
					sum = (sum << 8) ^ DELIMITER;
				}
				++k;
			}
			gpu_divices[gpuid]->state[j+2] = sum;
		}
		gpu_divices[gpuid]->target = target;
		gpu_divices[gpuid]->nonce_id = 0;

		ret = gpu_hash(gpuid);

		if(gpu_divices[gpuid]->nonce_id) {
			*nonce = le32toh(htobe32(gpu_divices[gpuid]->state[1]));
			*nonce = (*nonce << 32) ^ le32toh(htobe32(((gpu_divices[gpuid]->state[0])^(gpu_divices[gpuid]->nonce_id))));
		}

		return ret;
	}
}