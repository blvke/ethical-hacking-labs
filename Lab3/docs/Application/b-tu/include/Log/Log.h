#ifndef __LOG_H__
#define __LOG_H__


extern "C" {

	/**
	 * Write out a Message to the System.
	 */
	void write_log(void *object, const unsigned int reason, const char *const msg);
}

#endif
