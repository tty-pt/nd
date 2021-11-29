#ifndef _SPEECH_H_
#define _SPEECH_H_

void
notify_except(dbref first, dbref exception, const char *msg, dbref who)
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

