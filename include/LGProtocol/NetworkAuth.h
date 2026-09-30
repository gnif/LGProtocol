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

#ifndef LGPROTOCOL_NETWORK_AUTH_H
#define LGPROTOCOL_NETWORK_AUTH_H

#include "NetworkProtocol.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LG_NET_PASSWORD_TRANSCRIPT_VERSION 1U
#define LG_NET_PASSWORD_SHA256_LENGTH       32U

#define LG_NET_PASSWORD_CLIENT_PROOF_DOMAIN        \
  "LGProtocol network password client proof"
#define LG_NET_PASSWORD_SERVER_PROOF_DOMAIN        \
  "LGProtocol network password server proof"
#define LG_NET_PASSWORD_CLIENT_PROOF_DOMAIN_LENGTH \
  (sizeof(LG_NET_PASSWORD_CLIENT_PROOF_DOMAIN) - 1U)
#define LG_NET_PASSWORD_SERVER_PROOF_DOMAIN_LENGTH \
  (sizeof(LG_NET_PASSWORD_SERVER_PROOF_DOMAIN) - 1U)

/* Excludes the proof domain and variable-length challenge/response fields. */
#define LG_NET_PASSWORD_TRANSCRIPT_FIXED_SIZE 92U
#define LG_NET_PASSWORD_CLIENT_PROOF_TRANSCRIPT_MAX_SIZE \
  (LG_NET_PASSWORD_CLIENT_PROOF_DOMAIN_LENGTH +          \
   LG_NET_PASSWORD_TRANSCRIPT_FIXED_SIZE +               \
   LG_NET_PASSWORD_SALT_MAX_LENGTH +                     \
   LG_NET_PASSWORD_CHALLENGE_MAX_LENGTH +                \
   LG_NET_PASSWORD_NONCE_MAX_LENGTH)
#define LG_NET_PASSWORD_SERVER_PROOF_TRANSCRIPT_MAX_SIZE \
  (LG_NET_PASSWORD_SERVER_PROOF_DOMAIN_LENGTH +          \
   LG_NET_PASSWORD_TRANSCRIPT_FIXED_SIZE +               \
   LG_NET_PASSWORD_SALT_MAX_LENGTH +                     \
   LG_NET_PASSWORD_CHALLENGE_MAX_LENGTH +                \
   LG_NET_PASSWORD_NONCE_MAX_LENGTH +                    \
   LG_NET_PASSWORD_SHA256_LENGTH)

typedef struct LGNetPasswordProofContext
{
  uint16_t                       protocolVersion;
  uint64_t                       sessionEpoch;
  const uint8_t *                helloNonce;
  const uint8_t *                helloAckNonce;
  const LGNetPasswordChallenge * challenge;
  const LGNetPasswordResponse *  response;
}
LGNetPasswordProofContext;

/* helloNonce and helloAckNonce each point to LG_NET_HELLO_NONCE_LENGTH bytes.
 * The response proof length must be set to the SHA-256 length before either
 * transcript is constructed. Its bytes are omitted from the client proof
 * transcript and included in the server proof transcript. The destination
 * size passed to an encoder must exactly match the corresponding size. */
bool lgNetPasswordProofContextValid(
  const LGNetPasswordProofContext * context);
bool lgNetPasswordClientProofTranscriptSize(
  const LGNetPasswordProofContext * context, size_t * size);
bool lgNetPasswordClientProofTranscriptEncode(void * data, size_t size,
  const LGNetPasswordProofContext * context);
bool lgNetPasswordServerProofTranscriptSize(
  const LGNetPasswordProofContext * context, size_t * size);
bool lgNetPasswordServerProofTranscriptEncode(void * data, size_t size,
  const LGNetPasswordProofContext * context);

#ifdef __cplusplus
}
#endif

#endif
