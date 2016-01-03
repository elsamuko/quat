#ifndef CREPLACEMENTS_H
#define CREPLACEMENTS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>

#ifndef NO_NAMESPACE
using namespace std;
#endif

class pathname: public string {
    public:
        pathname() {}
        pathname( const char* );
        pathname( const string& );
        void uppercase();
        string path() const;
        string file() const;
        void file( const char* );
        string filename() const;
        string ext() const;
        void ext( const char* );
        bool exists() const;
        void seperate( string& path, string& filename, string& ext ) const;
        string next_name( int digits = 3 ) const;
        void auto_name( int digits = 3 );
    private:
        string _convert_slash( const char* ) const;
};

#endif

