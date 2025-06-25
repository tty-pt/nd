#include <nd/nd.h>

void mod_install(void *arg __attribute__((unused))) {
	nd_put(HD_WTS, NULL, "peck");
	nd_put(HD_WTS, NULL, "slash");
	nd_put(HD_WTS, NULL, "bite");

	/* PHYSICAL things combined with special effects */
	nd_put(HD_WTS, NULL, "heal");
	nd_put(HD_WTS, NULL, "bleed");
	nd_put(HD_WTS, NULL, "haste");
	nd_put(HD_WTS, NULL, "stun");
	nd_put(HD_WTS, NULL, "leer");
	nd_put(HD_WTS, NULL, "focus");
	nd_put(HD_WTS, NULL, "distract");
	nd_put(HD_WTS, NULL, "evade");
	nd_put(HD_WTS, NULL, "hobble");
}
