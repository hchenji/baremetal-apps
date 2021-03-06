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

// The main function
int main() {
    if (or1k_coreid() != 0)
        return 0;

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
    optimsoc_mp_endpoint_create(&ep, 0, 0, OPTIMSOC_MP_EP_CONNECTIONLESS, 2, 0);

    //	send a message to the neighbor whose id is 1 more than you
    uint32_t dest = (rank+1) % optimsoc_get_numct();

    optimsoc_mp_endpoint_handle ep_remote;
    optimsoc_mp_endpoint_get(&ep_remote, dest, 0, 0);
    optimsoc_mp_msg_send(ep, ep_remote, (uint8_t*) &rank, sizeof(rank));
  	printf("Sent message to tile %d\n", dest);

    // now listen for messages
	uint32_t remote;
	size_t received;
	optimsoc_mp_msg_recv(ep, (uint8_t*) &remote, 4, &received);
	printf("Received from %d\n", remote);

    return 0;
}
