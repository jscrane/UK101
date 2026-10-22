#pragma once

// a switchable prom: the state of the switch is checkpointable
class sprom: public prom {
public:
	sprom(const byte *mem, int bytes): prom(mem, bytes) {}
	void delegate(Checkpointable *chk) { _chk = chk; }

	void checkpoint(Checkpoint &s) override { _chk->checkpoint(s); }
	void restore(Checkpoint &s) override { _chk->restore(s); }

private:
	Checkpointable *_chk;
};

extern Memory memory;

// manages a set of proms
class promswitch: public Checkpointable {
public:
	promswitch(sprom *sproms, int n, Memory::address addr): _sproms(sproms), _n(n), _curr(0), _addr(addr) {
		for (int i = 0; i < n; i++)
			sproms[i].delegate(this);
	}

	void set(uint8_t c) { _curr=c; memory.put(_sproms[c], _addr); }
	void next() { int c=_curr; set(++c == _n? 0: c); }

	void checkpoint(Checkpoint &s) override { s.write(_curr); }
	void restore(Checkpoint &s) override { uint8_t c; if (s.read(c)) set(c); }

private:
	sprom *_sproms;
	uint8_t _n, _curr;
	Memory::address _addr;
};
