/*
 * tape.cpp -- UK101 Tape Interface
 */
#include <SD.h>
#include "config.h"
#include "memory.h"
#include "tape.h"

static File file, dir;

void tape::start()
{
	dir = SD.open(PROGRAMS);
}

void tape::stop()
{
	file.close();
}

const char *tape::advance() {
	bool rewound = false;
	file.close();
	while (true) {
		file = dir.openNextFile();
		if (file) {
			if (file.isDirectory())
				file.close();
			else
				break;
		} else if (!rewound) {
			dir.rewindDirectory();
			rewound = true;
		} else
			return 0;
	}
	return file.name();
}

const char *tape::rewind() {
	dir.rewindDirectory();
	return advance();
}

void tape::operator=(byte b) {
	// FIXME?
}

tape::operator byte() {
	if (_acc & 1)			// read data
		return _buf[_pos++];

	if (_pos >= _len) {		// read status
		_pos = 0;
		_len = file.read(_buf, sizeof(_buf));

		if (_len == 0) {
			file.close();
			return 0;	// eof
		}
	}
	return rdrf | tdre;
}
