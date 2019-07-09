#include "cpuClass.h"
#include "prsClass.h"

CPU cpu;
PRS pr(100);

#define PRIFERR() if( pr.error() ) {printf("Line=%d, error=%d\r\n", __LINE__, pr.error() ); exit(0); }

void setup()
{
    cpu.init();
    
    char *test[] = {
                        " { \"This_Line\" : \"10\", \"More\": 30 }",
                        " { \"This_Line\" : \"\",   \"More\": 30 }",
                        " { \"This Line\" : \"20\"  \"More\": 30 }",  // missing COMMA
                        " { \"This Line\" : \"20\", \"More\"  30 }",  // missing COLON
                        " { \"This Line\" : \"20\", \"Test\": 30 }"   // wrong key2
                    };
                        

    struct x_t
    {
        char *key1; 
        char *val1;
        char *key2; 
        int val2;
    } x;

    for( int i=0; i<5; i++ )
    {
        INIT( test[i] );
    
        LCURL; 
        x.key1 = COPYS;
        COLON; 
        x.val1 = COPYS;
        
        COMMA; 
        EXPECT( x.key2 = "More" );
        COLON;  
        x.val2 = atoi( COPYD );
        RCURL;

        printf("Decoding %s. Parse Error=%d. {key1=%s, value1=%s, key2=%s, val2=%d}\r\n", test[i], pr.error(), x.key1, x.val1, x.key2, x.val2 );
    }
}
void loop()
{
    ;
}
