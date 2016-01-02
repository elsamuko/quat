#ifdef __cplusplus
extern "C" {
#endif

void q_mul(point c, const point a, const point b);
void q_add(point c, const point a, const point b);
void q_sub(point c, const point a, const point b);
void q_exp(point c, const point q);
void q_log(point c, const point q);
void q_pow(point c, const point a, const point b);

#ifdef __cplusplus
}
#endif
