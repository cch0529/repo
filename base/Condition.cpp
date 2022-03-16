#include "Condition.h"
#include <errno.h>
bool Condition::waitForSeconds(int seconds){
	struct timespec targetTime;
	clock_gettime(CLOCK_REALTIME,&targetTime);
	targetTime.tv_sec+=static_cast<time_t>(seconds);
	return ETIMEDOUT==pthread_cond_timedwait(&cond,mutex_.getMutex(),&targetTime);
}
