#include "libdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rijndael_instance {
	int Nk,Nb,Nr;
	word8 fi[24],ri[24];
	word32 fkey[120];
	word32 rkey[120];
} RI;

void _mcrypt_rijndael_set_key(RI* rinst, int nb,int nk,word8 *key);
  /* blocksize=32*nb bits. Key=32*nk bits */
  /* currently nb,bk = 4, 6 or 8          */
  /* key comes as 4*Nk bytes              */
  /* Key Scheduler. Create expanded encryption key */
  
void _mcrypt_rijndael_encrypt(RI* rinst, word8 *buff);
void _mcrypt_rijndael_decrypt(RI* rinst, word8 *buff);
void _mcrypt_rijndael_gentables(void);

#ifdef __cplusplus
}
#endif
