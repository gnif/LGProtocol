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

#include <LGProtocol/NetworkSequence.h>

static uint64_t windowMask(uint8_t windowSize)
{
  if (windowSize == 64U)
    return UINT64_MAX;

  return (UINT64_C(1) << windowSize) - UINT64_C(1);
}

bool lgNetSequenceTrackerInit(LGNetSequenceTracker * tracker,
  uint64_t sessionEpoch, uint64_t componentEpoch, uint8_t windowSize,
  bool ordered)
{
  if (!tracker || !sessionEpoch || !componentEpoch || !windowSize ||
      windowSize > 64U)
    return false;

  tracker->sessionEpoch    = sessionEpoch;
  tracker->componentEpoch  = componentEpoch;
  tracker->highestSequence = 0;
  tracker->seen            = 0;
  tracker->windowSize      = windowSize;
  tracker->ordered         = ordered;
  tracker->initialized     = false;
  return true;
}

bool lgNetSequenceTrackerResetComponent(LGNetSequenceTracker * tracker,
  uint64_t componentEpoch)
{
  if (!tracker || !tracker->sessionEpoch || !tracker->windowSize ||
      tracker->windowSize > 64U || !componentEpoch)
    return false;

  tracker->componentEpoch  = componentEpoch;
  tracker->highestSequence = 0;
  tracker->seen            = 0;
  tracker->initialized     = false;
  return true;
}

LGNetSequenceResult lgNetSequenceTrackerCheck(
  LGNetSequenceTracker * tracker, uint64_t sessionEpoch,
  uint64_t componentEpoch, uint64_t sequence)
{
  if (!tracker || !tracker->sessionEpoch || !tracker->componentEpoch ||
      !tracker->windowSize || tracker->windowSize > 64U || !sessionEpoch ||
      !componentEpoch || !sequence)
    return LG_NET_SEQUENCE_INVALID;

  if (sessionEpoch != tracker->sessionEpoch)
    return LG_NET_SEQUENCE_SESSION_MISMATCH;
  if (componentEpoch != tracker->componentEpoch)
    return LG_NET_SEQUENCE_COMPONENT_MISMATCH;

  if (!tracker->initialized)
  {
    tracker->highestSequence = sequence;
    tracker->seen            = UINT64_C(1);
    tracker->initialized     = true;
    return LG_NET_SEQUENCE_ACCEPTED;
  }

  if (sequence > tracker->highestSequence)
  {
    const uint64_t distance = sequence - tracker->highestSequence;
    if (tracker->ordered && distance != UINT64_C(1))
      return LG_NET_SEQUENCE_GAP;

    if (distance >= tracker->windowSize)
      tracker->seen = UINT64_C(1);
    else
    {
      tracker->seen = ((tracker->seen << distance) | UINT64_C(1)) &
        windowMask(tracker->windowSize);
    }

    tracker->highestSequence = sequence;
    return LG_NET_SEQUENCE_ACCEPTED;
  }

  const uint64_t distance = tracker->highestSequence - sequence;
  if (!distance)
    return LG_NET_SEQUENCE_DUPLICATE;
  if (tracker->ordered || distance >= tracker->windowSize)
    return LG_NET_SEQUENCE_TOO_OLD;

  const uint64_t bit = UINT64_C(1) << distance;
  if (tracker->seen & bit)
    return LG_NET_SEQUENCE_DUPLICATE;

  tracker->seen |= bit;
  return LG_NET_SEQUENCE_ACCEPTED_REORDERED;
}

LGNetSequenceResult lgNetSequenceTrackerCheckEnvelope(
  LGNetSequenceTracker * tracker, const LGNetEnvelope * envelope)
{
  if (!envelope)
    return LG_NET_SEQUENCE_INVALID;

  return lgNetSequenceTrackerCheck(tracker, envelope->sessionEpoch,
    envelope->componentEpoch, envelope->sequence);
}

bool lgNetSequenceAccepted(LGNetSequenceResult result)
{
  return result == LG_NET_SEQUENCE_ACCEPTED ||
    result == LG_NET_SEQUENCE_ACCEPTED_REORDERED;
}
