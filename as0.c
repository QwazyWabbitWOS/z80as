/*
 * Z-80 assembler.
 * Command line processing
 * and main driver.
 */
#include	"as.h"

FILE	*ifp;
FILE	*ofp;
FILE	*lfp;
char	cb[NCODE];
char	eb[NERR];
char	ib[NINPUT];
char	*cp;
char	*ep;
char	*ip;
int	lflag;
VALUE	laddr;
int	lmode;
VALUE	dot;
SYM	*phash[NHASH];
SYM	*uhash[NHASH];
int	pass;
int	line;
jmp_buf	env;
unsigned int err_count;

int main(int argc, char *argv[])
{
	register char	*ifn;
	register char	*p;
	register int	i;
	register int	c;
	char		fn[NFNAME];

	printf("Mark Williams Co. Z80 Cross-Assembler Open Source Version 3.0\n");

	ifn = NULL;
	for (i=1; i<argc; ++i) {
		p = argv[i];
		if (*p == '-') {
			while ((c = *++p) != 0) {
				switch (c) {
				case 'l':
					++lflag;
					break;

				default:
					fprintf(stderr, "Bad option %c\n", c);
					return BAD;
				}
			}
		} else if (ifn == NULL)
			ifn = p;
		else {
			fprintf(stderr, "Too many source files\n");
			return BAD;
		}
	}
	if (ifn == NULL) {
		fprintf(stderr, "No source file\n");
		return BAD;
	}
	if ((ifp=fopen(ifn, "r")) == NULL) {
		fprintf(stderr, "%s: cannot open\n", ifn);
		return BAD;
	}
	mkname(fn, ifn, "hex");
	if ((ofp=fopen(fn, "w")) == NULL) {
		fprintf(stderr, "%s: cannot create\n", fn);
		return BAD;
	}
	if (lflag != 0) {
		mkname(fn, ifn, "lis");
		if ((lfp=fopen(fn, "w")) == NULL) {
			fprintf(stderr, "%s: cannot create\n", fn);
			return BAD;
		}
	}
	err_count = 0;
	printf("Assembling %s\n", ifn);
	if (lfp)
		printf("Listing file is %s\n", fn);

	syminit();
	for (pass=0; pass<2; ++pass) {
		line = 0;
		dot  = 0;
		fseek(ifp, 0L, 0);
		while (fgets(ib, NINPUT, ifp) != NULL) {
			++line;
			cp = &cb[0];
			ep = &eb[0];
			ip = &ib[0];
			if (setjmp(env) == 0)
				asmline();
			if (pass != 0)
				list();
		}
	}
	outeof();
	printf("%u Error(s)\n", err_count);
	if (err_count)
		return BAD;
	else
		return GOOD;	// no errors
}

/*
 * Make up a file name.
 * The "sfn" is the source file
 * name and "dft" is the desired file
 * type. The finished name is copied
 * into the "dfn" buffer.
 */
void mkname(char *dfn, char *sfn, char *dft)
{
	register char	*p1;
	register char	*p2;
	register int	c;

	p1 = sfn;
	while (*p1 != 0)
		++p1;
#ifdef	vax
	while (p1!=sfn && p1[-1]!=':' && p1[-1]!=']')
		--p1;
#else
	while (p1!=sfn && p1[-1]!='/')
		--p1;
#endif
	p2 = dfn;
	while ((c = *p1++)!=0 && c!='.')
		*p2++ = (char)c;
	*p2++ = '.';
	p1 = dft;
	while ((*p2++ = *p1++))
		;
}
