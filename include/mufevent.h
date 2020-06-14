#ifndef MUF_EVENT_H
#define MUF_EVENT_H

struct mufevent {
	struct mufevent *next;
	char *event;
	struct inst data;
};

#define MUFEVENT_ALL	-1
#define MUFEVENT_FIRST	-2
#define MUFEVENT_LAST	-3

int muf_event_dequeue(dbref prog, int sleeponly);
struct frame* muf_event_pid_frame(int pid);
int muf_event_controls(dbref player, int pid);
void muf_event_purge(struct frame *fr);
void muf_event_process(void);

#endif /* MUF_EVENT_H */

#ifdef DEFINE_HEADER_VERSIONS

#ifndef mufeventh_version
#define mufeventh_version
const char *mufevent_h_version = "$RCSfile$ $Revision: 1.13 $";
#endif
#else
extern const char *mufevent_h_version;
#endif

