/**
 * LGProtocol - Looking Glass Communication Protocol
 * Copyright © 2017-2026 Geoffrey McRae <geoff@hostfission.com>
 * https://github.com/gnif/LGProtocol
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 59
 * Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef LGPROTOCOL_NETWORK_ROUTE_H
#define LGPROTOCOL_NETWORK_ROUTE_H

#include "NetworkProtocol.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t LGNetRouteDirection;

enum
{
  LG_NET_ROUTE_CLIENT_TO_SERVER = 1U << 0,
  LG_NET_ROUTE_SERVER_TO_CLIENT = 1U << 1,
  LG_NET_ROUTE_BIDIRECTIONAL    =
    LG_NET_ROUTE_CLIENT_TO_SERVER | LG_NET_ROUTE_SERVER_TO_CLIENT,
};

typedef uint16_t LGNetRouteDelivery;

enum
{
  LG_NET_ROUTE_DELIVERY_RELIABLE = 1U << 0,
  LG_NET_ROUTE_DELIVERY_DATAGRAM = 1U << 1,
  LG_NET_ROUTE_DELIVERY_BULK     = 1U << 2,
};

typedef struct LGNetServiceVersionInfo
{
  LGNetService service;
  uint16_t     minimum;
  uint16_t     current;
  uint16_t     maximum;
}
LGNetServiceVersionInfo;

typedef struct LGNetMessageRoute
{
  LGNetService        service;
  uint16_t            messageType;
  uint16_t            serviceVersionMin;
  uint16_t            messageVersion;
  LGNetRouteDirection direction;
  LGNetRouteDelivery  delivery;
}
LGNetMessageRoute;

/* Returned metadata has static lifetime and must not be modified. */
const LGNetServiceVersionInfo * lgNetServiceVersionInfo(
  LGNetService service);
bool lgNetServiceVersionSupported(LGNetService service, uint16_t version);
const LGNetMessageRoute * lgNetMessageRoute(
  LGNetService service, uint16_t messageType);
bool lgNetMessageRouteAllows(const LGNetMessageRoute * route,
  LGNetRole sender, LGNetRouteDelivery delivery);
bool lgNetEnvelopeRouteValid(const LGNetEnvelope * envelope,
  LGNetRole sender, LGNetRouteDelivery delivery);

#ifdef __cplusplus
}
#endif

#endif
