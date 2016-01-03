int TEXTVER_ReturnVideoInfo( struct vidinfo_struct* vidinfo );
int TEXTVER_getline( unsigned char* line, int y, long xres, int whichbuf );
int TEXTVER_putline( long x1, long x2, long xres, int y, unsigned char* Buf, int whichbuf );
int TEXTVER_check_event( void );
void TEXTVER_Debug( char* s );
void TEXTVER_eol( int line );
