#include "acia.h"

class tape: public acia {
public:
	void operator= (byte);
	operator byte ();
        void advance();
        tape();

private:
	unsigned int _pos, _len;
	byte _buf[128];
};
