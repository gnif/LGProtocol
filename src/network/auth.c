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

#include <LGProtocol/NetworkAuth.h>

#include <limits.h>

static bool sizeAdd(size_t * total, size_t value)
{
  if (!total || value > SIZE_MAX - *total)
    return false;

  *total += value;
  return true;
}

bool lgNetPasswordProofContextValid(
    const LGNetPasswordProofContext * context)
{
  if (!context || !context->helloNonce || !context->helloAckNonce ||
      context->protocolVersion < LG_NET_PROTOCOL_VERSION_MIN ||
      context->protocolVersion > LG_NET_PROTOCOL_VERSION_MAX ||
      !context->sessionEpoch ||
      !lgNetPasswordChallengeValid(context->challenge) ||
      !lgNetPasswordResponseValid(context->response))
    return false;

  const LGNetPasswordChallenge * challenge = context->challenge;
  const LGNetPasswordResponse *  response  = context->response;

  return challenge->algorithm == response->algorithm &&
    response->proofLength == LG_NET_PASSWORD_SHA256_LENGTH;
}

static bool transcriptSize(const LGNetPasswordProofContext * context,
    size_t domainLength, bool includeClientProof, size_t * size)
{
  if (!size || !lgNetPasswordProofContextValid(context))
    return false;

  size_t required = domainLength;
  if (!sizeAdd(&required, LG_NET_PASSWORD_TRANSCRIPT_FIXED_SIZE) ||
      !sizeAdd(&required, context->challenge->saltLength) ||
      !sizeAdd(&required, context->challenge->challengeLength) ||
      !sizeAdd(&required, context->response->nonceLength) ||
      (includeClientProof &&
        !sizeAdd(&required, context->response->proofLength)))
    return false;

  *size = required;
  return true;
}

bool lgNetPasswordClientProofTranscriptSize(
    const LGNetPasswordProofContext * context, size_t * size)
{
  return transcriptSize(context,
    LG_NET_PASSWORD_CLIENT_PROOF_DOMAIN_LENGTH, false, size);
}

bool lgNetPasswordServerProofTranscriptSize(
    const LGNetPasswordProofContext * context, size_t * size)
{
  return transcriptSize(context,
    LG_NET_PASSWORD_SERVER_PROOF_DOMAIN_LENGTH, true, size);
}

static bool transcriptEncode(void * data, size_t size,
    const LGNetPasswordProofContext * context, const char * domain,
    size_t domainLength, bool includeClientProof)
{
  size_t required;
  if (!data || !domain || !transcriptSize(context, domainLength,
        includeClientProof, &required) || size != required)
    return false;

  const LGNetPasswordChallenge * challenge = context->challenge;
  const LGNetPasswordResponse *  response  = context->response;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  if (!lgNetWriterBytes(&writer, domain, domainLength)              ||
      !lgNetWriterU16(&writer, LG_NET_PASSWORD_TRANSCRIPT_VERSION)  ||
      !lgNetWriterU16(&writer, context->protocolVersion)            ||
      !lgNetWriterU16(&writer, LG_NET_AUTH_PASSWORD)                ||
      !lgNetWriterU16(&writer, challenge->algorithm)                ||
      !lgNetWriterU32(&writer, challenge->rounds)                   ||
      !lgNetWriterU64(&writer, context->sessionEpoch)               ||
      !lgNetWriterU16(&writer, challenge->saltLength)               ||
      !lgNetWriterU16(&writer, challenge->challengeLength)          ||
      !lgNetWriterU16(&writer, response->nonceLength)               ||
      !lgNetWriterU16(&writer, response->proofLength)               ||
      !lgNetWriterBytes(&writer, context->helloNonce,
        LG_NET_HELLO_NONCE_LENGTH)                                  ||
      !lgNetWriterBytes(&writer, context->helloAckNonce,
        LG_NET_HELLO_NONCE_LENGTH)                                  ||
      !lgNetWriterBytes(&writer, challenge->salt,
        challenge->saltLength)                                      ||
      !lgNetWriterBytes(&writer, challenge->challenge,
        challenge->challengeLength)                                 ||
      !lgNetWriterBytes(&writer, response->nonce,
        response->nonceLength))
    return false;

  if (includeClientProof && !lgNetWriterBytes(&writer, response->proof,
        response->proofLength))
    return false;

  return lgNetWriterValid(&writer) && lgNetWriterSize(&writer) == required;
}

bool lgNetPasswordClientProofTranscriptEncode(void * data, size_t size,
    const LGNetPasswordProofContext * context)
{
  return transcriptEncode(data, size, context,
    LG_NET_PASSWORD_CLIENT_PROOF_DOMAIN,
    LG_NET_PASSWORD_CLIENT_PROOF_DOMAIN_LENGTH, false);
}

bool lgNetPasswordServerProofTranscriptEncode(void * data, size_t size,
    const LGNetPasswordProofContext * context)
{
  return transcriptEncode(data, size, context,
    LG_NET_PASSWORD_SERVER_PROOF_DOMAIN,
    LG_NET_PASSWORD_SERVER_PROOF_DOMAIN_LENGTH, true);
}
