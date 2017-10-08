#ifndef GVERBDSP_H
#define GVERBDSP_H

#include "ext.h"
#include "z_dsp.h"	// needed for denormalization makros

// Convert a value in dB's to a coefficent
#define DB_CO(g) ((g) > -90.0f ? pow(10.0f, (g) * 0.05f) : 0.0f)
// and back to dB
#define CO_DB(g) ((g) != 0.0f ? 20.0f/log(10) * log((g)) : -90.0f)

typedef struct {
  int size;
  int idx;
  float *buf;
} ty_fixeddelay;

typedef struct {
  int size;
  float coeff;
  int idx;
  float *buf;
} ty_diffuser;

typedef struct {
  float damping;
  float delay;
} ty_damper;

extern ty_diffuser *diffuser_make(int, float);
extern void diffuser_free(ty_diffuser *);
extern void diffuser_flush(ty_diffuser *);

extern ty_damper *damper_make(float);
extern void damper_free(ty_damper *);
extern void damper_flush(ty_damper *);

extern ty_fixeddelay *fixeddelay_make(int);
extern void fixeddelay_free(ty_fixeddelay *);
extern void fixeddelay_flush(ty_fixeddelay *);

extern int isprime(int);
extern int nearest_prime(int, float);
extern int ff_round(float f);
extern int ff_trunc(float f);

static inline float diffuser_do(ty_diffuser *p, float x)
{
	float y,w;

	w = x - p->buf[p->idx]*p->coeff;
	FIX_DENORM_NAN_FLOAT(w);
	y = p->buf[p->idx] + w*p->coeff;
	p->buf[p->idx] = w;
	p->idx = (p->idx + 1) % p->size;
	return(y);
}

static inline float fixeddelay_read(ty_fixeddelay *p, int n)
{
	int i;

	i = (p->idx - n + p->size) % p->size;
	return(p->buf[i]);
}

static inline void fixeddelay_write(ty_fixeddelay *p, float x)
{
	FIX_DENORM_NAN_FLOAT(x);
	p->buf[p->idx] = x;
	p->idx = (p->idx + 1) % p->size;
}

static inline void damper_set(ty_damper *p, float damping)
{ 
	p->damping = damping;
} 
  
static inline float damper_do(ty_damper *p, float x)
{ 
	float y;

	y = x*(1.0-p->damping) + p->delay*p->damping;
	p->delay = y;
	return(y);
}

#endif
