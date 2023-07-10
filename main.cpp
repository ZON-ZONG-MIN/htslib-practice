#include <fstream>
#include <iostream>
#include "Phasing.h"
#include "htslib/vcf.h"

int main(int argc, char *argv[])
{
	htsFile *fp;
	bcf_hdr_t *hdr;
	bcf_idpair_t *ctg;
	int i;
	if(argc == 1) {
		fprintf(stderr, "Usage: print-ctg <in.vcf>\n");
		return 1;
	}
	fp = vcf_open(argv[1], "r");
	hdr = vcf_hdr_read(fp);
	ctg = hdr->id[BCF_DT_CTG];
	//ctg = hdr->id[BCF_DT_SAMPLE];
	//ctg = hdr->id[BCF_DT_ID];
	for(i = 0; i < hdr->n[BCF_DT_CTG]; ++i)
		printf("%s\t%ld\n", ctg[i].key, ctg[i].val->info[0]);
	bcf_hdr_destroy(hdr);
	vcf_close(fp);
	return 0;
}
