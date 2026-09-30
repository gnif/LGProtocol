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

#include <LGProtocol/NetworkRoute.h>
#include <LGProtocol/NetworkServices.h>

#include <stddef.h>

#define C2S LG_NET_ROUTE_CLIENT_TO_SERVER
#define S2C LG_NET_ROUTE_SERVER_TO_CLIENT
#define BI  LG_NET_ROUTE_BIDIRECTIONAL
#define REL LG_NET_ROUTE_DELIVERY_RELIABLE
#define DGM LG_NET_ROUTE_DELIVERY_DATAGRAM
#define BLK LG_NET_ROUTE_DELIVERY_BULK

#define ROUTE(service_, version_, message_, direction_, delivery_) \
  { service_, message_, version_, LG_NET_MESSAGE_VERSION_INITIAL, \
    direction_, delivery_ }
#define CORE(message_, direction_, delivery_) \
  ROUTE(LG_NET_SERVICE_CORE, LG_NET_CORE_VERSION_MIN, \
    message_, direction_, delivery_)
#define RECOVERY(message_, direction_, delivery_) \
  ROUTE(LG_NET_SERVICE_RECOVERY, LG_NET_RECOVERY_VERSION_MIN, \
    message_, direction_, delivery_)
#define VIDEO(message_, direction_, delivery_) \
  ROUTE(LG_NET_SERVICE_VIDEO, LG_NET_VIDEO_VERSION_MIN, \
    message_, direction_, delivery_)
#define CURSOR(message_, direction_, delivery_) \
  ROUTE(LG_NET_SERVICE_CURSOR, LG_NET_CURSOR_VERSION_MIN, \
    message_, direction_, delivery_)
#define INPUT(message_, direction_, delivery_) \
  ROUTE(LG_NET_SERVICE_INPUT, LG_NET_INPUT_VERSION_MIN, \
    message_, direction_, delivery_)
#define AUDIO(message_, direction_, delivery_) \
  ROUTE(LG_NET_SERVICE_AUDIO, LG_NET_AUDIO_VERSION_MIN, \
    message_, direction_, delivery_)
#define CLIPBOARD(message_, direction_, delivery_) \
  ROUTE(LG_NET_SERVICE_CLIPBOARD, LG_NET_CLIPBOARD_VERSION_MIN, \
    message_, direction_, delivery_)
#define FILE_ROUTE(message_, direction_, delivery_) \
  ROUTE(LG_NET_SERVICE_FILE, LG_NET_FILE_VERSION_MIN, \
    message_, direction_, delivery_)
#define USB(message_, direction_, delivery_) \
  ROUTE(LG_NET_SERVICE_USB, LG_NET_USB_VERSION_RESERVED, \
    message_, direction_, delivery_)
#define CONTROL(message_, direction_, delivery_) \
  ROUTE(LG_NET_SERVICE_CONTROL, LG_NET_CONTROL_VERSION_MIN, \
    message_, direction_, delivery_)

static const LGNetServiceVersionInfo SERVICE_VERSIONS[] =
{
  { LG_NET_SERVICE_CORE,      LG_NET_CORE_VERSION_MIN,
    LG_NET_CORE_VERSION_CURRENT,      LG_NET_CORE_VERSION_MAX      },
  { LG_NET_SERVICE_RECOVERY,  LG_NET_RECOVERY_VERSION_MIN,
    LG_NET_RECOVERY_VERSION_CURRENT,  LG_NET_RECOVERY_VERSION_MAX  },
  { LG_NET_SERVICE_VIDEO,     LG_NET_VIDEO_VERSION_MIN,
    LG_NET_VIDEO_VERSION_CURRENT,     LG_NET_VIDEO_VERSION_MAX     },
  { LG_NET_SERVICE_CURSOR,    LG_NET_CURSOR_VERSION_MIN,
    LG_NET_CURSOR_VERSION_CURRENT,    LG_NET_CURSOR_VERSION_MAX    },
  { LG_NET_SERVICE_INPUT,     LG_NET_INPUT_VERSION_MIN,
    LG_NET_INPUT_VERSION_CURRENT,     LG_NET_INPUT_VERSION_MAX     },
  { LG_NET_SERVICE_AUDIO,     LG_NET_AUDIO_VERSION_MIN,
    LG_NET_AUDIO_VERSION_CURRENT,     LG_NET_AUDIO_VERSION_MAX     },
  { LG_NET_SERVICE_CLIPBOARD, LG_NET_CLIPBOARD_VERSION_MIN,
    LG_NET_CLIPBOARD_VERSION_CURRENT, LG_NET_CLIPBOARD_VERSION_MAX },
  { LG_NET_SERVICE_FILE,      LG_NET_FILE_VERSION_MIN,
    LG_NET_FILE_VERSION_CURRENT,      LG_NET_FILE_VERSION_MAX      },
  { LG_NET_SERVICE_USB,       LG_NET_USB_VERSION_RESERVED,
    LG_NET_USB_VERSION_RESERVED,      LG_NET_USB_VERSION_RESERVED  },
  { LG_NET_SERVICE_CONTROL,   LG_NET_CONTROL_VERSION_MIN,
    LG_NET_CONTROL_VERSION_CURRENT,   LG_NET_CONTROL_VERSION_MAX   },
};

static const LGNetMessageRoute MESSAGE_ROUTES[] =
{
  CORE(LG_NET_CORE_MESSAGE_HELLO,             C2S, REL),
  CORE(LG_NET_CORE_MESSAGE_HELLO_ACK,         S2C, REL),
  CORE(LG_NET_CORE_MESSAGE_AUTH_CHALLENGE,    S2C, REL),
  CORE(LG_NET_CORE_MESSAGE_AUTH_RESPONSE,     C2S, REL),
  CORE(LG_NET_CORE_MESSAGE_AUTH_RESULT,       S2C, REL),
  CORE(LG_NET_CORE_MESSAGE_CAPABILITY,        BI,  REL),
  CORE(LG_NET_CORE_MESSAGE_CAPABILITIES_DONE, BI,  REL),
  CORE(LG_NET_CORE_MESSAGE_SESSION_INFO,      S2C, REL),
  CORE(LG_NET_CORE_MESSAGE_STATUS,            BI,  REL),
  CORE(LG_NET_CORE_MESSAGE_PING,              BI,  REL | DGM),
  CORE(LG_NET_CORE_MESSAGE_PONG,              BI,  REL | DGM),
  CORE(LG_NET_CORE_MESSAGE_GOODBYE,           BI,  REL),
  CORE(LG_NET_CORE_MESSAGE_ERROR,             BI,  REL),

  RECOVERY(LG_NET_RECOVERY_MESSAGE_GET_INFO, C2S, REL),
  RECOVERY(LG_NET_RECOVERY_MESSAGE_INFO,     S2C, REL),
  RECOVERY(LG_NET_RECOVERY_MESSAGE_REQUEST,  C2S, REL),
  RECOVERY(LG_NET_RECOVERY_MESSAGE_STATUS,   S2C, REL),

  VIDEO(LG_NET_VIDEO_MESSAGE_SUBSCRIBE,        C2S, REL),
  VIDEO(LG_NET_VIDEO_MESSAGE_UNSUBSCRIBE,      C2S, REL),
  VIDEO(LG_NET_VIDEO_MESSAGE_STREAM_CONFIG,    S2C, REL),
  VIDEO(LG_NET_VIDEO_MESSAGE_FRAME,            S2C, REL | BLK),
  VIDEO(LG_NET_VIDEO_MESSAGE_FRAME_FRAGMENT,   S2C, REL | DGM),
  VIDEO(LG_NET_VIDEO_MESSAGE_KEYFRAME_REQUEST, C2S, REL | DGM),
  VIDEO(LG_NET_VIDEO_MESSAGE_SCHEDULE,         S2C, REL),
  VIDEO(LG_NET_VIDEO_MESSAGE_FEEDBACK,         C2S, REL | DGM),
  VIDEO(LG_NET_VIDEO_MESSAGE_STATUS,           BI,  REL),

  CURSOR(LG_NET_CURSOR_MESSAGE_STATE,     S2C, REL),
  CURSOR(LG_NET_CURSOR_MESSAGE_POSITION,  S2C, REL | DGM),
  CURSOR(LG_NET_CURSOR_MESSAGE_SHAPE,     S2C, REL),
  CURSOR(LG_NET_CURSOR_MESSAGE_TRANSFORM, S2C, REL),
  CURSOR(LG_NET_CURSOR_MESSAGE_STATUS,    BI,  REL),
  ROUTE(LG_NET_SERVICE_CURSOR,
    LG_NET_CURSOR_COLOR_TRANSFORM_INTRODUCED_SERVICE_VERSION,
    LG_NET_CURSOR_MESSAGE_COLOR_TRANSFORM, S2C, REL),

  INPUT(LG_NET_INPUT_MESSAGE_CLAIM,          C2S, REL),
  INPUT(LG_NET_INPUT_MESSAGE_CLAIM_RESULT,   S2C, REL),
  INPUT(LG_NET_INPUT_MESSAGE_KEEPALIVE,      C2S, REL | DGM),
  INPUT(LG_NET_INPUT_MESSAGE_RELEASE,        C2S, REL),
  INPUT(LG_NET_INPUT_MESSAGE_RESET,          C2S, REL),
  INPUT(LG_NET_INPUT_MESSAGE_MOUSE_RELATIVE, C2S, REL | DGM),
  INPUT(LG_NET_INPUT_MESSAGE_MOUSE_ABSOLUTE, C2S, REL | DGM),
  INPUT(LG_NET_INPUT_MESSAGE_KEYBOARD,       C2S, REL),
  INPUT(LG_NET_INPUT_MESSAGE_STATUS,         S2C, REL),
  INPUT(LG_NET_INPUT_MESSAGE_KEYBOARD_LEDS,  S2C, REL),

  AUDIO(LG_NET_AUDIO_MESSAGE_SUBSCRIBE,       C2S, REL),
  AUDIO(LG_NET_AUDIO_MESSAGE_STATUS,          BI,  REL),
  AUDIO(LG_NET_AUDIO_MESSAGE_PLAYBACK_START,  S2C, REL),
  AUDIO(LG_NET_AUDIO_MESSAGE_PLAYBACK_STOP,   S2C, REL),
  AUDIO(LG_NET_AUDIO_MESSAGE_PLAYBACK_DATA,   S2C, REL | DGM),
  AUDIO(LG_NET_AUDIO_MESSAGE_PLAYBACK_VOLUME, S2C, REL),
  AUDIO(LG_NET_AUDIO_MESSAGE_PLAYBACK_MUTE,   S2C, REL),
  AUDIO(LG_NET_AUDIO_MESSAGE_CAPTURE_START,   S2C, REL),
  AUDIO(LG_NET_AUDIO_MESSAGE_CAPTURE_STOP,    S2C, REL),
  AUDIO(LG_NET_AUDIO_MESSAGE_CAPTURE_DATA,    C2S, REL | DGM),
  AUDIO(LG_NET_AUDIO_MESSAGE_CAPTURE_VOLUME,  S2C, REL),
  AUDIO(LG_NET_AUDIO_MESSAGE_CAPTURE_MUTE,    S2C, REL),
  AUDIO(LG_NET_AUDIO_MESSAGE_CLOCK_FEEDBACK,  BI,  REL | DGM),
  AUDIO(LG_NET_AUDIO_MESSAGE_KEEPALIVE,       C2S, REL | DGM),
  AUDIO(LG_NET_AUDIO_MESSAGE_RELEASE,         C2S, REL),
  AUDIO(LG_NET_AUDIO_MESSAGE_STATE_BARRIER,   BI,  REL),
  AUDIO(LG_NET_AUDIO_MESSAGE_STATE_ACK,       BI,  REL),
  ROUTE(LG_NET_SERVICE_AUDIO,
    LG_NET_AUDIO_OWNERSHIP_INTRODUCED_SERVICE_VERSION,
    LG_NET_AUDIO_MESSAGE_SUBSCRIPTION_GRANT, S2C, REL),
  ROUTE(LG_NET_SERVICE_AUDIO,
    LG_NET_AUDIO_CLOCK_STATE_INTRODUCED_SERVICE_VERSION,
    LG_NET_AUDIO_MESSAGE_CLOCK_STATE, BI, REL | DGM),

  CLIPBOARD(LG_NET_CLIPBOARD_MESSAGE_CLAIM,        BI, REL),
  CLIPBOARD(LG_NET_CLIPBOARD_MESSAGE_CLAIM_RESULT, BI, REL),
  CLIPBOARD(LG_NET_CLIPBOARD_MESSAGE_KEEPALIVE,    BI, REL),
  CLIPBOARD(LG_NET_CLIPBOARD_MESSAGE_RELEASE,      BI, REL),
  CLIPBOARD(LG_NET_CLIPBOARD_MESSAGE_OFFER,        BI, REL),
  CLIPBOARD(LG_NET_CLIPBOARD_MESSAGE_CLEAR,        BI, REL),
  CLIPBOARD(LG_NET_CLIPBOARD_MESSAGE_REQUEST,      BI, REL),
  CLIPBOARD(LG_NET_CLIPBOARD_MESSAGE_DATA_BEGIN,   BI, REL | BLK),
  CLIPBOARD(LG_NET_CLIPBOARD_MESSAGE_DATA_CHUNK,   BI, REL | BLK),
  CLIPBOARD(LG_NET_CLIPBOARD_MESSAGE_DATA_END,     BI, REL | BLK),
  CLIPBOARD(LG_NET_CLIPBOARD_MESSAGE_DATA_READY,   BI, REL),
  CLIPBOARD(LG_NET_CLIPBOARD_MESSAGE_CANCEL,       BI, REL),
  CLIPBOARD(LG_NET_CLIPBOARD_MESSAGE_STATUS,       BI, REL),

  FILE_ROUTE(LG_NET_FILE_MESSAGE_OFFER,      BI, REL),
  FILE_ROUTE(LG_NET_FILE_MESSAGE_ACQUIRE,    BI, REL),
  FILE_ROUTE(LG_NET_FILE_MESSAGE_ACQUIRED,   BI, REL),
  FILE_ROUTE(LG_NET_FILE_MESSAGE_RELEASE,    BI, REL),
  FILE_ROUTE(LG_NET_FILE_MESSAGE_REQUEST,    BI, REL),
  FILE_ROUTE(LG_NET_FILE_MESSAGE_ENTRY,      BI, REL),
  FILE_ROUTE(LG_NET_FILE_MESSAGE_DATA_BEGIN, BI, REL | BLK),
  FILE_ROUTE(LG_NET_FILE_MESSAGE_DATA_CHUNK, BI, REL | BLK),
  FILE_ROUTE(LG_NET_FILE_MESSAGE_DATA_END,   BI, REL | BLK),
  FILE_ROUTE(LG_NET_FILE_MESSAGE_DATA_READY, BI, REL),
  FILE_ROUTE(LG_NET_FILE_MESSAGE_CANCEL,     BI, REL),
  FILE_ROUTE(LG_NET_FILE_MESSAGE_STATUS,     BI, REL),

  USB(LG_NET_USB_MESSAGE_CAPABILITIES, BI, REL),
  USB(LG_NET_USB_MESSAGE_DEVICE,       BI, REL),
  USB(LG_NET_USB_MESSAGE_CLAIM,        BI, REL),
  USB(LG_NET_USB_MESSAGE_CONTROL,      BI, REL),
  USB(LG_NET_USB_MESSAGE_ENDPOINT,     BI, REL | DGM | BLK),
  USB(LG_NET_USB_MESSAGE_CANCEL,       BI, REL),
  USB(LG_NET_USB_MESSAGE_RESET,        BI, REL),
  USB(LG_NET_USB_MESSAGE_STATUS,       BI, REL),

  CONTROL(LG_NET_CONTROL_MESSAGE_CURSOR_POSITION, C2S, REL),
  CONTROL(LG_NET_CONTROL_MESSAGE_DISPLAY_SIZE,    C2S, REL),
  CONTROL(LG_NET_CONTROL_MESSAGE_FRAME_SCHEDULE,  C2S, REL),
  CONTROL(LG_NET_CONTROL_MESSAGE_STATUS,          S2C, REL),
};

#undef ROUTE
#undef CORE
#undef RECOVERY
#undef VIDEO
#undef CURSOR
#undef INPUT
#undef AUDIO
#undef CLIPBOARD
#undef FILE_ROUTE
#undef USB
#undef CONTROL
#undef C2S
#undef S2C
#undef BI
#undef REL
#undef DGM
#undef BLK

const LGNetServiceVersionInfo * lgNetServiceVersionInfo(
  LGNetService service)
{
  for (size_t index = 0;
       index < sizeof(SERVICE_VERSIONS) / sizeof(SERVICE_VERSIONS[0]);
       ++index)
  {
    if (SERVICE_VERSIONS[index].service == service)
      return &SERVICE_VERSIONS[index];
  }

  return NULL;
}

bool lgNetServiceVersionSupported(LGNetService service, uint16_t version)
{
  const LGNetServiceVersionInfo * info = lgNetServiceVersionInfo(service);
  return info && version >= info->minimum && version <= info->maximum;
}

const LGNetMessageRoute * lgNetMessageRoute(
  LGNetService service, uint16_t messageType)
{
  for (size_t index = 0;
       index < sizeof(MESSAGE_ROUTES) / sizeof(MESSAGE_ROUTES[0]);
       ++index)
  {
    const LGNetMessageRoute * route = &MESSAGE_ROUTES[index];
    if (route->service == service && route->messageType == messageType)
      return route;
  }

  return NULL;
}

bool lgNetMessageRouteAllows(const LGNetMessageRoute * route,
  LGNetRole sender, LGNetRouteDelivery delivery)
{
  if (!route || !delivery || (delivery & (delivery - 1U)) ||
      !(route->delivery & delivery))
    return false;

  if (sender == LG_NET_ROLE_CLIENT)
    return (route->direction & LG_NET_ROUTE_CLIENT_TO_SERVER) != 0;
  if (sender == LG_NET_ROLE_SERVER)
    return (route->direction & LG_NET_ROUTE_SERVER_TO_CLIENT) != 0;
  return false;
}

bool lgNetEnvelopeRouteValid(const LGNetEnvelope * envelope,
  LGNetRole sender, LGNetRouteDelivery delivery)
{
  if (!envelope || !lgNetServiceVersionSupported(
        envelope->service, envelope->serviceVersion))
    return false;

  const LGNetMessageRoute * route = lgNetMessageRoute(
    envelope->service, envelope->messageType);
  return route && envelope->serviceVersion >= route->serviceVersionMin &&
    envelope->messageVersion == route->messageVersion &&
    lgNetMessageRouteAllows(route, sender, delivery);
}
