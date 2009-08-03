#ifndef _CHANNEL_
#define _CHANNEL_

typedef struct _channel {
    KEVENT   send_event;
	KEVENT   recv_event;
    KEVENT   rcvd_event;
	KEVENT   repl_event;
	PETHREAD thread;
	u32      state;
	u32      waiters;
	int      released;
	int      snd_size;
	int      rcv_size;
	u8       data[];
} channel;

channel *channel_alloc(int snd_size, int rcv_size);
void     channel_release(channel *chan);
void     channel_free(channel *chan);
int      channel_send_recv(channel *chan, void *send_msg, void *recv_msg);
int      channel_recv(channel *chan, void *msg);
int      channel_send(channel *chan, void *msg);



#endif