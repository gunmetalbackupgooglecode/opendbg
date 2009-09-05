/*
    *
    * Copyright (c) 2009
    * vol4ok <vol4ok@highsecure.ru> PGP key ID - 0x7A1C8BB4A0F34B67
    *

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <ntifs.h>
#include "defines.h"
#include "channel.h"

#define CH_NULL     0
#define CH_SENT     1
#define CH_RECVED   2
#define CH_REPLAYED 3

channel *channel_alloc(int snd_size, int rcv_size)
{
	channel *chan;

	if (chan = mem_alloc(sizeof(channel) + snd_size + rcv_size))
	{
		KeInitializeEvent(
			&chan->recv_event, NotificationEvent, FALSE
			);

		KeInitializeEvent(
			&chan->send_event, NotificationEvent, TRUE
			);

		KeInitializeEvent(
			&chan->rcvd_event, NotificationEvent, FALSE
			);

		KeInitializeEvent(
			&chan->repl_event, NotificationEvent, FALSE
			);

		chan->state    = CH_NULL;
		chan->snd_size = snd_size;
		chan->rcv_size = rcv_size;
		chan->released    = 0;
		chan->waiters  = 0;
		chan->thread   = NULL;
	}

	return chan;
}

int channel_send_recv(channel *chan, void *send_msg, void *recv_msg)
{
	NTSTATUS status;
	int      succs = 0;

	lock_inc(&chan->waiters);

	do
	{
		/* wait for send event */
		status = KeWaitForSingleObject(
			&chan->send_event, UserRequest, UserMode,
			FALSE, NULL
			);

		if ( (status == STATUS_USER_APC) || (chan->released != 0) ) {
			break;
		}

		/* change channel state */
		if (lock_cmpxchg(&chan->state, CH_SENT, CH_NULL) != CH_NULL) {
			continue;
		}

		/* clear send event */
		KeClearEvent(&chan->send_event);

		/* copy sent data to buffer */
		memcpy(&chan->data, send_msg, chan->snd_size);

		/* indicate change state to CH_SENT */
		KeSetEvent(
			&chan->recv_event, IO_NO_INCREMENT, TRUE
			);

		/* wait for replay */
		status = KeWaitForSingleObject(
			&chan->repl_event, UserRequest, UserMode,
			FALSE, NULL
			);

		if ( (status != STATUS_USER_APC) && (chan->released == 0) ) {
			succs = 1;
		}

		/* copy recived data */
		memcpy(
			recv_msg, chan->data + chan->snd_size, chan->rcv_size
			);

		/* clear replay event */
		KeClearEvent(&chan->repl_event);

		/* set initial channel state */
		chan->thread = NULL;
		chan->state  = CH_NULL;

		/* indicate ready for next transaction */
		KeSetEvent(
			&chan->send_event, IO_NO_INCREMENT, FALSE
			);
		break;
	} while (0);

	lock_dec(&chan->waiters);

	return succs;
}

int channel_recv(channel *chan, void *msg)
{
	NTSTATUS status;
	int      succs = 0;

	lock_inc(&chan->waiters);

	do
	{
		/* wait for message in channel */
		status = KeWaitForSingleObject(
			&chan->recv_event, UserRequest, UserMode,
			FALSE, NULL
			);

		if ( (status == STATUS_USER_APC) || (chan->released != 0) ) {
			break;
		}

		/* change channel state */
		if (lock_cmpxchg(&chan->state, CH_RECVED, CH_SENT) != CH_SENT) {
			continue;
		}

		/* assign transaction channel to this thread */
		if (lock_cmpxchg_ptr(&chan->thread, PsGetCurrentThread(), NULL) != NULL) {
			continue;
		}

		/* clear recive event */
		KeClearEvent(&chan->recv_event);

		/* copy readed data to output */
		memcpy(
			msg, chan->data, chan->snd_size
			);

		/* indicate ready for send replay */
		KeSetEvent(
			&chan->rcvd_event, IO_NO_INCREMENT, FALSE
			);
		succs = 1;
		break;
	} while (1);

	lock_dec(&chan->waiters);

	return succs;
}

int channel_send(channel *chan, void *msg)
{
	NTSTATUS status;
	int      succs = 0;

	lock_inc(&chan->waiters);

	do
	{
		/* wait replay ready event */
		status = KeWaitForSingleObject(
			&chan->rcvd_event, UserRequest, UserMode,
			FALSE, NULL
			);

		if ( (status == STATUS_USER_APC) || (chan->released != 0) ) {
			break;
		}

		/* check transaction thread */
		if (chan->thread != PsGetCurrentThread()) {
			continue;
		}

		if (lock_cmpxchg(&chan->state, CH_REPLAYED, CH_RECVED) != CH_RECVED) {
			continue;
		}

		/* clear replay ready event */
		KeClearEvent(&chan->rcvd_event);

		/* copy replay data */
		memcpy(
			chan->data + chan->snd_size, msg, chan->rcv_size
			);

		/* indicate for replay sent */
		KeSetEvent(
			&chan->repl_event, IO_NO_INCREMENT, FALSE
			);
		succs = 1;
		break;
	} while (1);

	lock_dec(&chan->waiters);

	return succs;
}


void channel_release(channel *chan)
{
	chan->released = 1;
	
	DbgMsg("channel_release\n");
	
	KeSetEvent(
		&chan->send_event, IO_NO_INCREMENT, FALSE
		);

	KeSetEvent(
		&chan->recv_event, IO_NO_INCREMENT, FALSE
		);

	KeSetEvent(
		&chan->rcvd_event, IO_NO_INCREMENT, FALSE
		);

	KeSetEvent(
		&chan->repl_event, IO_NO_INCREMENT, FALSE
		);
}

void channel_free(channel *chan)
{
	LARGE_INTEGER short_time;
	KEVENT        delay_event;
	
	DbgMsg("channel_free\n");
	
	KeInitializeEvent(
		&delay_event, NotificationEvent, FALSE
		);

	short_time.QuadPart = -10000 * 10;

	while (chan->waiters != 0)
	{
		DbgMsg("channel_free wait %d\n", chan->waiters);

		KeWaitForSingleObject(
			&delay_event, Executive, KernelMode,
			FALSE, &short_time
			);
	}

	mem_free(chan);
}

