#ifndef _SPROM_H
#define _SPROM_H

// a switchable prom: the state of the switch is checkpointable
class sprom: public prom {
public:
	sprom(const byte *mem, int bytes): prom(mem, bytes) {}
	void delegate(Checkpointable *chk) { _chk = chk; }

	void checkpoint(Stream &s) { _chk->checkpoint(s); }
	void restore(Stream &s) { _chk->restore(s); }

private:
	Checkpointable *_chk;
};

// manages a set of proms
class promswitch: public Checkpointable {
public:
	promswitch(sprom *sproms, int n, Memory::address addr): _sproms(sproms), _n(n), _last(0), _curr(0), _addr(addr) {
		for (int i = 0; i < n; i++)
			sproms[i].delegate(this);
	}

	void set(int c) { _curr=c; memory.put(_sproms[c], _addr); }
	void next() { int c=_curr; set(++c == _last? 0: c); }

	void checkpoint(Stream &s) { s.write(_curr); }
	void restore(Stream &s) { set(s.read()); }

private:
	sprom *_sproms;
	int _n, _last, _curr;
	Memory::address _addr;
};

#endif
