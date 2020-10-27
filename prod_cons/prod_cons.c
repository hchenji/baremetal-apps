/* Copyright (c) 2013-2015 by the author(s)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * =============================================================================
 *
 * Simple hello world example.
 *
 * Author(s):
 *   Stefan Wallentowitz <stefan.wallentowitz@tum.de>
 */


#include <stdio.h> // For printf

#include <optimsoc-mp.h>
#include <or1k-support.h>
#include <optimsoc-baremetal.h>

#include <assert.h>

#define MAX_MSGS 1
#define	MSG_DEST 4

typedef struct {
	uint32_t id;
	uint32_t seq;
} msg_t;

// The main function
int main() {

    // Initialize optimsoc library
    optimsoc_init(0);
    optimsoc_mp_initialize(0);

    // Determine tiles rank
    uint32_t rank = optimsoc_get_ctrank();

	printf("Hello World! Core %d of %d in tile %d, my absolute core id is: %d\n",
         optimsoc_get_relcoreid(), optimsoc_get_tilenumcores(),
         optimsoc_get_tileid(), optimsoc_get_abscoreid());

  	printf("There are %d compute tiles\n", optimsoc_get_numct());

    optimsoc_mp_endpoint_handle ep;
    optimsoc_mp_endpoint_create(&ep, 0, 0, OPTIMSOC_MP_EP_CONNECTIONLESS, 32, 0);

    unsigned int id = optimsoc_get_abscoreid();

    if (id == MSG_DEST) {
    	//	just receive msgs
		uint8_t buf[16];
		size_t received;
		uint16_t *count = calloc(0, 9 * sizeof(uint16_t));
		size_t max_msgs = 0;

    	while (1) {
    	    // now listen for messages
    		int err = optimsoc_mp_msg_recv(ep, (uint8_t*) buf, 16, &received);

            if (err == 0) {
	//    		printf("Received msg of size %d\n", received);
				msg_t* rmsg = (msg_t*) buf;
				count[rmsg->id]++;
				max_msgs++;
				// if ( (count[rmsg->id] & 31) == 0)
					printf("Received from %d seq %04d/%04d\n", rmsg->id, count[rmsg->id], rmsg->seq + 1);
            } else {
            	printf("error %d: %s\n", err, optimsoc_mp_error_string(err));
            }

  		if (max_msgs == 2 * MAX_MSGS)
			break;
    	}
    }

    if (id == 0 || id == 8) {
    	// just send msgs to MSG_DEST
        uint32_t dest = MSG_DEST;

        optimsoc_mp_endpoint_handle ep_remote;
        optimsoc_mp_endpoint_get(&ep_remote, dest, 0, 0);

    	msg_t msg;
    	msg.id = id;
    	msg.seq = 0;

        while (1) {
            int err = optimsoc_mp_msg_send(ep, ep_remote, (uint8_t*) &msg, sizeof(msg));
            if (err == 0) {
				if ( (msg.seq & 63) == 0)
					printf("Sent message to tile %d seq %d\n", dest, msg.seq);
            } else {
            	printf("error %d: %s\n", err, optimsoc_mp_error_string(err));
            }
          	msg.seq++;
          	if (msg.seq == MAX_MSGS)
          		break;
        }

//		template for receiving msgs
//		uint8_t buf[16];
//		size_t received;
//
//	    // now listen for messages
//		int err = optimsoc_mp_msg_recv(ep, (uint8_t*) buf, 16, &received);
//        if (err == 0) {
//    		printf("Received msg of size %d!!\n", received);
//        } else {
//        	printf("error %d: %s\n", err, optimsoc_mp_error_string(err));
//        }

    }

    return 0;
}
