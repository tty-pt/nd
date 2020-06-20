#ifndef _SPEECH_H_
#define _SPEECH_H_

void
notify_listeners(dbref who, dbref xprog, dbref obj, dbref room, const char *msg, int isprivate)
;

void
notify_except(dbref first, dbref exception, const char *msg, dbref who)
;

void
parse_omessage(command_t *cmd, dbref dest, dbref exit, const char *msg,
			   const char *prefix, const char *whatcalled, int mpiflags)
;


int
blank(const char *s)
;

#endif /* !defined _SPEECH_H_ */

#ifdef DEFINE_HEADER_VERSIONS

#ifndef speechh_version
#define speechh_version
const char *speech_h_version = "$RCSfile$ $Revision: 1.4 $";
#endif
#else
extern const char *speech_h_version;
#endif

