/*
	libxbee - a C library to aid the use of Digi's XBee wireless modules
	          running in API mode.

	Copyright (C) 2009 onwards  Attie Grande (attie@attie.co.uk)

	libxbee is free software: you can redistribute it and/or modify it
	under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libxbee is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../internal.h"
#include "../../xbee_int.h"
#include "../../mode.h"
#include "../../pkt.h"
#include "../../log.h"
#include "../common.h"
#include "ota.h"

xbee_err xbee_sZB_ota_rx_func(struct xbee *xbee, void *arg, unsigned char identifier, struct xbee_tbuf *buf, struct xbee_frameInfo *frameInfo, struct xbee_conAddress *address, struct xbee_pkt **pkt) {
	struct xbee_pkt *iPkt;
	xbee_err ret;
	
	if (!xbee || !frameInfo || !buf || !address || !pkt) return XBEE_EMISSINGPARAM;
	
	if (buf->len !=  22) return XBEE_ELENGTH;
	
	if ((ret = xbee_pktAlloc(&iPkt, NULL, 2)) != XBEE_ENONE) return ret;
	
// #warning TODO - check that these are the correct addresses to be using
	address->addr64_enabled = 1;
	memcpy(address->addr64, &(buf->data[1]), 8);
	address->addr16_enabled = 1;
	memcpy(address->addr16, &(buf->data[9]), 2);
	
	iPkt->options = buf->data[11];
	
	iPkt->dataLen = 2;
	iPkt->data[0] = buf->data[12]; /* bootloader message type */
	iPkt->data[1] = buf->data[13]; /* block number */
	iPkt->data[iPkt->dataLen] = '\0';
	
	*pkt = iPkt;
	
	return XBEE_ENONE;
}

/* ######################################################################### */

struct xbee_modeDataHandlerRx xbee_sZB_ota_rx  = {
	.identifier = 0xA0,
	.func = xbee_sZB_ota_rx_func,
};
struct xbee_modeConType xbee_sZB_ota = {
	.name = "OTA Update Status",
	.allowFrameId = 0,
	.useTimeout = 0,
	.addressRules = ADDR_16OR64_NOEP,
	.save_addr16 = 1,
	.save_addr64 = 1,
	.rxHandler = &xbee_sZB_ota_rx,
	.txHandler = NULL,
};
