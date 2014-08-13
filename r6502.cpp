#include <stdio.h>

#include "Memory.h"
#include "cpu.h"
#include "r6502.h"

//#define DEBUG 1

void r6502::run (unsigned clocks) {
	while (clocks--) 
	{
		byte op = (*_memory)[PC];
#ifdef DEBUG
                _status("Executing %02x at %04x", op, PC);
#endif
		PC++;
		(this->*_ops[op])();
	}
}

char *r6502::status () {
  static char buf[128];
	sprintf (buf, "aa xx yy sp nv_bdizc  _pc_\n"
		 "%02x %02x %02x %02x %d%d1%d%d%d%d%d  %04x", 
		 A, X, Y, S, (N & 0x80)!=0, V!=0, P.bits.B, 
		 P.bits.D, P.bits.I, Z==0, C!=0, PC);
	return buf;
}

void r6502::raise (int level) {
	if (level < 0)
		nmi ();
	else if (!P.bits.I)
		irq ();
	else
		_irq = true;
}

void r6502::irq () {
	Memory::address ret = PC+1;
	(*_memory)[0x0100+S--] = ret >> 8;
	(*_memory)[0x0100+S--] = ret & 0xff;
	php ();
	P.bits.I = 1;
	PC = ((*_memory)[0xffff] << 8) | (*_memory)[0xfffe];
	_irq = false;
}

// php and plp are complicated by the representation
// of the processor state for efficient normal operation
void r6502::php () {
	P.bits.N = (N & 0x80);
	P.bits.V = V;
	P.bits.Z = !Z;
	P.bits.C = C;
	(*_memory)[0x0100+S--] = P.value;
}

void r6502::plp () {
	P.value = (*_memory)[++S+0x0100];
	N = P.bits.N? 0x80: 0;
	V = P.bits.V;
	Z = !P.bits.Z;
	C = P.bits.C;
}

void r6502::rts () {
	byte b = (*_memory)[++S+0x0100];
	PC = (((*_memory)[++S+0x0100] << 8) | b)+1;
}

void r6502::rti () {
	plp ();
	byte b = (*_memory)[++S+0x0100];
	PC = ((*_memory)[++S+0x0100] << 8) | b;
}

void r6502::nmi () {
	(*_memory)[0x0100+S--] = PC >> 8;
	(*_memory)[0x0100+S--] = PC & 0xff;
	php ();
	P.bits.I = 1;
	PC = ((*_memory)[0xfffb] << 8) | (*_memory)[0xfffa];
}

void r6502::brk () {
	if (!P.bits.I) {
		Memory::address ret = PC+1;
		(*_memory)[0x0100+S--] = ret >> 8;
		(*_memory)[0x0100+S--] = ret & 0xff;
		P.bits.B = 1;
		php ();
		P.bits.I = 1;
		PC = ((*_memory)[0xffff] << 8) | (*_memory)[0xfffe];
	}
}

void r6502::jsr () {
	Memory::address ret = PC+1;
	(*_memory)[0x0100+S--] = ret >> 8;
	(*_memory)[0x0100+S--] = ret & 0xff;
	PC = ((*_memory)[PC+1] << 8) | (*_memory)[PC];
}

void r6502::_adc (byte d) {
	if (P.bits.D) {
		int r = _fromBCD[A]+_fromBCD[d]+C;
		Z = N = _toBCD[r];
		C = (r > 99);
	} else {
		unsigned short r = C+(unsigned short)A+(unsigned short)d;
		Z = N = r;
		C = (r > 255);
	}
	V = (Z ^ A) & 0x80;
	A = Z;
}

void r6502::_sbc (byte d) {
	if (P.bits.D) {
		int r = _fromBCD[A] - _fromBCD[d] - !C;
		if (r < 0) r += 100;
		Z = N = _toBCD[r];
	} else {
		unsigned short r = A-d-!C;
		Z = N = r & 0xff;
	}
	C = (A >= (d+!C));
	V = (Z ^ A) & 0x80;
	A = Z;
}

void r6502::ill () {
	_status ("Illegal instruction at %04x!", PC-1);
	longjmp (*_err, 1);
}

void r6502::reset () {
	P.value = 0;
	P.bits._ = 1;
	_irq = false;
	S = 0xff;
	PC = ((*_memory)[0xfffd] << 8) | (*_memory)[0xfffc];
}

r6502::r6502 (Memory *m, jmp_buf *e, CPU::statfn s): CPU (m,e,s) {

	for (int i=256; i--; ) {
		_fromBCD[i] = ((i >> 4) & 0x0f)*10 + (i & 0x0f);
		_toBCD[i] = (((i % 100) / 10) << 4) | (i % 10);
	}
		
	OP *p = _ops;
	*p++=&r6502::brk; *p++=&r6502::ora_ix; *p++=&r6502::ill; *p++=&r6502::ill; 
	*p++=&r6502::nop2; *p++=&r6502::ora_z; *p++=&r6502::asl_z; *p++=&r6502::ill;
	*p++=&r6502::php; *p++=&r6502::ora_; *p++=&r6502::asl; *p++=&r6502::ill; 
	*p++=&r6502::nop3; *p++=&r6502::ora_a; *p++=&r6502::asl_a; *p++=&r6502::ill;
	*p++=&r6502::bpl; *p++=&r6502::ora_iy; *p++=&r6502::ill; *p++=&r6502::ill; 
	*p++=&r6502::nop2; *p++=&r6502::ora_zx; *p++=&r6502::asl_zx; *p++=&r6502::ill;
	*p++=&r6502::clc; *p++=&r6502::ora_ay; *p++=&r6502::nop; *p++=&r6502::ill; 
	*p++=&r6502::nop2; *p++=&r6502::ora_ax; *p++=&r6502::asl_ax; *p++=&r6502::ill;
	*p++=&r6502::jsr; *p++=&r6502::and_ix; *p++=&r6502::ill; *p++=&r6502::ill; 
	*p++=&r6502::bit_z; *p++=&r6502::and_z; *p++=&r6502::rol_z; *p++=&r6502::ill;
	*p++=&r6502::plp; *p++=&r6502::and_; *p++=&r6502::rol; *p++=&r6502::ill; 
	*p++=&r6502::bit_a; *p++=&r6502::and_a; *p++=&r6502::rol_a; *p++=&r6502::ill;
	*p++=&r6502::bmi; *p++=&r6502::and_iy; *p++=&r6502::ill; *p++=&r6502::ill; 
	*p++=&r6502::nop2; *p++=&r6502::and_zx; *p++=&r6502::rol_zx; *p++=&r6502::ill;
	*p++=&r6502::sec; *p++=&r6502::and_ay; *p++=&r6502::nop; *p++=&r6502::ill; 
	*p++=&r6502::nop3; *p++=&r6502::and_ax; *p++=&r6502::rol_ax; *p++=&r6502::ill;
	*p++=&r6502::rti; *p++=&r6502::eor_ix; *p++=&r6502::ill; *p++=&r6502::ill; 
	*p++=&r6502::nop2; *p++=&r6502::eor_z; *p++=&r6502::lsr_z; *p++=&r6502::ill;
	*p++=&r6502::pha; *p++=&r6502::eor_; *p++=&r6502::lsr_; *p++=&r6502::ill; 
	*p++=&r6502::jmp; *p++=&r6502::eor_a; *p++=&r6502::lsr_a; *p++=&r6502::ill;
	*p++=&r6502::bvc; *p++=&r6502::eor_iy; *p++=&r6502::ill; *p++=&r6502::ill; 
	*p++=&r6502::nop2; *p++=&r6502::eor_zx; *p++=&r6502::lsr_zx; *p++=&r6502::ill;
	*p++=&r6502::cli; *p++=&r6502::eor_ay; *p++=&r6502::nop; *p++=&r6502::ill; 
	*p++=&r6502::nop3; *p++=&r6502::eor_ax; *p++=&r6502::lsr_ax; *p++=&r6502::ill;
	*p++=&r6502::rts; *p++=&r6502::adc_ix; *p++=&r6502::ill; *p++=&r6502::ill; 
	*p++=&r6502::nop2; *p++=&r6502::adc_z; *p++=&r6502::ror_z; *p++=&r6502::ill;
	*p++=&r6502::pla; *p++=&r6502::adc_; *p++=&r6502::ror_; *p++=&r6502::ill; 
	*p++=&r6502::jmp_i; *p++=&r6502::adc_a; *p++=&r6502::ror_a; *p++=&r6502::ill;
	*p++=&r6502::bvs; *p++=&r6502::adc_iy; *p++=&r6502::ill; *p++=&r6502::ill; 
	*p++=&r6502::nop2; *p++=&r6502::adc_zx; *p++=&r6502::ror_zx; *p++=&r6502::ill;
	*p++=&r6502::sei; *p++=&r6502::adc_ay; *p++=&r6502::nop; *p++=&r6502::ill; 
	*p++=&r6502::nop3; *p++=&r6502::adc_ax; *p++=&r6502::ror_ax; *p++=&r6502::ill;
	*p++=&r6502::nop2; *p++=&r6502::sta_ix; *p++=&r6502::nop2; *p++=&r6502::ill; 
	*p++=&r6502::sty_z; *p++=&r6502::sta_z; *p++=&r6502::stx_z; *p++=&r6502::ill;
	*p++=&r6502::dey; *p++=&r6502::nop2; *p++=&r6502::txa; *p++=&r6502::ill; 
	*p++=&r6502::sty_a; *p++=&r6502::sta_a; *p++=&r6502::stx_a; *p++=&r6502::ill;
	*p++=&r6502::bcc; *p++=&r6502::sta_iy; *p++=&r6502::ill; *p++=&r6502::ill; 
	*p++=&r6502::sty_zx; *p++=&r6502::sta_zx; *p++=&r6502::stx_zy; *p++=&r6502::ill;
	*p++=&r6502::tya; *p++=&r6502::sta_ay; *p++=&r6502::txs; *p++=&r6502::ill; 
	*p++=&r6502::ill; *p++=&r6502::sta_ax; *p++=&r6502::ill; *p++=&r6502::ill;
	*p++=&r6502::ldy_; *p++=&r6502::lda_ix; *p++=&r6502::ldx_; *p++=&r6502::lax_ix; 
	*p++=&r6502::ldy_z; *p++=&r6502::lda_z; *p++=&r6502::ldx_z; *p++=&r6502::lax_z;
	*p++=&r6502::tay; *p++=&r6502::lda_; *p++=&r6502::tax; *p++=&r6502::ill; 
	*p++=&r6502::ldy_a; *p++=&r6502::lda_a; *p++=&r6502::ldx_a; *p++=&r6502::lax_a;
	*p++=&r6502::bcs; *p++=&r6502::lda_iy; *p++=&r6502::ill; *p++=&r6502::lax_iy; 
	*p++=&r6502::ldy_zx; *p++=&r6502::lda_zx; *p++=&r6502::ldx_zy; *p++=&r6502::lax_zy;
	*p++=&r6502::clv; *p++=&r6502::lda_ay; *p++=&r6502::tsx; *p++=&r6502::ill; 
	*p++=&r6502::ldy_ax; *p++=&r6502::lda_ax; *p++=&r6502::ldx_ay; *p++=&r6502::lax_ay;
	*p++=&r6502::cpy_; *p++=&r6502::cmp_ix; *p++=&r6502::nop2; *p++=&r6502::ill; 
	*p++=&r6502::cpy_z; *p++=&r6502::cmp_z; *p++=&r6502::dec_z; *p++=&r6502::ill;
	*p++=&r6502::iny; *p++=&r6502::cmp_; *p++=&r6502::dex; *p++=&r6502::ill; 
	*p++=&r6502::cpy_a; *p++=&r6502::cmp_a; *p++=&r6502::dec_a; *p++=&r6502::ill;
	*p++=&r6502::bne; *p++=&r6502::cmp_iy; *p++=&r6502::ill; *p++=&r6502::ill; 
	*p++=&r6502::nop2; *p++=&r6502::cmp_zx; *p++=&r6502::dec_zx; *p++=&r6502::ill;
	*p++=&r6502::cld; *p++=&r6502::cmp_ay; *p++=&r6502::nop; *p++=&r6502::ill; 
	*p++=&r6502::nop3; *p++=&r6502::cmp_ax; *p++=&r6502::dec_ax; *p++=&r6502::ill;
	*p++=&r6502::cpx_; *p++=&r6502::sbc_ix; *p++=&r6502::nop2; *p++=&r6502::ill; 
	*p++=&r6502::cpx_z; *p++=&r6502::sbc_z; *p++=&r6502::inc_z; *p++=&r6502::ill;
	*p++=&r6502::inx; *p++=&r6502::sbc_; *p++=&r6502::nop; *p++=&r6502::ill; 
	*p++=&r6502::cpx_a; *p++=&r6502::sbc_a; *p++=&r6502::inc_a; *p++=&r6502::ill;
	*p++=&r6502::beq; *p++=&r6502::sbc_iy; *p++=&r6502::ill; *p++=&r6502::ill; 
	*p++=&r6502::nop2; *p++=&r6502::sbc_zx; *p++=&r6502::inc_zx; *p++=&r6502::ill;
	*p++=&r6502::sed; *p++=&r6502::sbc_ay; *p++=&r6502::nop; *p++=&r6502::ill; 
	*p++=&r6502::nop3; *p++=&r6502::sbc_ax; *p++=&r6502::inc_ax; *p++=&r6502::ill;
}

