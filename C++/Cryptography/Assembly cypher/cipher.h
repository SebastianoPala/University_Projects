extern "C" bool cifra_cesare(char*buffer,int chiave,char mode);

extern "C" bool cifra_affine(char*buffer,int a,int b,char mode);

extern "C" bool  cifra_completo(char*buffer,char*seed,char mode);