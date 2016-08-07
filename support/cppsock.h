#define DEBUG

#include <iostream>
#include <stdexcept>
#include <cstdio>
#include <cstdarg>
#include <cstring>

using namespace std;

namespace _basic_glue_namespace
{
char _show_buffer[1024];
int show_start(const char* FormatString,...) throw (runtime_error)
{
    #ifdef FULLDEBUG
    cout<<"Basic Glue : show_start() called."<<endl;
    #endif // DEBUG
    va_list ap;
    va_start (ap,FormatString);
    int L=strlen(FormatString);
    int begin_i=0;
    for(int end_i=0; end_i<L; end_i++)
    {
        if(FormatString[end_i]=='%')
        {
            if(begin_i!=end_i)
            {
                memset(_show_buffer,0,1024);
                strncpy(_show_buffer,FormatString+begin_i,end_i-begin_i);
                cout<<_show_buffer;
            }
            switch(FormatString[end_i+1])
            {
            case '%':
            {
                cout<<"%";
                break;
            }
            case 'u':
            {
                unsigned int _temp=va_arg(ap,unsigned int);
                cout<<_temp;
                break;
            }
            case 'd':
            {
                int _temp=va_arg(ap,int);
                cout<<_temp;
                break;
            }
            case 's':
            {
                const char* _temp=va_arg(ap,const char*);
                cout<<_temp;
                break;
            }
            case 'p':
            {
                const void* _temp=va_arg(ap,const void*);
                cout<<_temp;
                break;
            }
            case 'f':
            {
                double _temp=va_arg(ap,double);
                cout<<_temp;
                break;
            }
            case 'l':
            {
                end_i++;
                if(FormatString[end_i+1]=='d')
                {
                    long _temp=va_arg(ap,long);
                    cout<<_temp;
                }
                else if(FormatString[end_i+1]=='u')
                {
                    unsigned long _temp=va_arg(ap,unsigned long);
                    cout<<_temp;
                }
                else if(FormatString[end_i+1]=='f')
                {
                    double _temp=va_arg(ap,double);
                    cout<<_temp;
                }
                else if(FormatString[end_i+1]=='l')
                {
                    if(FormatString[end_i+1]=='d')
                    {
                        long long _temp=va_arg(ap,long long);
                        cout<<_temp;
                    }
                    else if(FormatString[end_i+1]=='u')
                    {
                        unsigned long long _temp=va_arg(ap,unsigned long long);
                        cout<<_temp;
                    }
                    else
                    {
                        throw runtime_error("Cannot cast ll?");
                    }
                }
                else
                {
                    throw runtime_error("Cannot cast l?");
                }
                break;
            }
            default:
                throw runtime_error("Cannot cast ?");
            }
            end_i++;
            begin_i=end_i+1;
        }
    }
    if(begin_i<L)
    {
        cout<<FormatString+begin_i;
    }
    va_end(ap);
    #ifdef FULLDEBUG
    cout<<"Basic Glue : show_start() end."<<endl;
    #endif // DEBUG
    return 0;
}

}/// End of _basic_glue_namespace

/// Redirect printf to cout
#define mprintf(x,args...) _basic_glue_namespace::show_start(x,##args)
#define printf mprintf

/// Debug Output
#if defined(DEBUG) && !defined(dprintf)
#define dprintf(format,args...) printf("Debug Output: ");printf(format,##args)
#elif !defined(dprintf)
#define dprintf(format,args...)
#endif // DEBUG

#include <cpplib/cpplib#gsock>




namespace cpplib_cppsock
{

class datablock
{
public:
    char* data_ptr(){return data;}
    int data_size(){return iDatasize;}
    char* cache_ptr(){return cache;}
    int cache_size(){return iCachesize;}
    datablock(int DataAndCacheSize)
    {
        data=new char[DataAndCacheSize];
        iDatasize=iCachesize=DataAndCacheSize;
        cache=data;
    }
    datablock(int DataSize,int CacheSize)
    {
        data=new char[DataSize];
        cache=new char[CacheSize];
        iDatasize=DataSize;
        iCachesize=CacheSize;
    }
    ~datablock()
    {
        delete[] data;
        if(data!=cache)
        {
            delete[] cache;
        }
    }
    void clear()
    {
        memset(data,0,iDatasize);
        memset(cache,0,iCachesize);
    }
private:
    char* data;
    char* cache;
    int iDatasize;
    int iCachesize;
};


char _basic_sock_error_buff[256];

class sock_error : public exception
{
protected:
    int errcode;
public:
    sock_error(int iErrorCode)
    {
        errcode=iErrorCode;
    }
    virtual const char* what() const noexcept override
    {
        sprintf(_basic_sock_error_buff,"Socket Send/Recv Error. Code(%d)",errcode);
        return _basic_sock_error_buff;
    }
};

class sock_error_send : public sock_error
{
public:
    using sock_error::sock_error;
    virtual const char* what() const noexcept override
    {
        sprintf(_basic_sock_error_buff,"Socket Send Error. Code(%d)",errcode);
        return _basic_sock_error_buff;
    }
};

class sock_error_recv : public sock_error
{
public:
    using sock_error::sock_error;
    virtual const char* what() const noexcept override
    {
        sprintf(_basic_sock_error_buff,"Socket Recv Error. Code(%d)",errcode);
        return _basic_sock_error_buff;
    }
};

class cppsock : public ksock
{
public:
    cppsock& operator << (datablock& block) throw (sock_error)
    {
        int ret=send(block.data_ptr(),block.data_size());
        if(ret<0) throw sock_error_send(ret);
        return *this;
    }
    cppsock& operator >> (datablock& block) throw (sock_error)
    {
        int ret=recv(block.cache_ptr(),block.cache_size());
        if(ret<0) throw sock_error_recv(ret);
        return *this;
    }
};





}/// End of cpplib_cppsock

int main()
{
    cpplib_cppsock::cppsock s;
    cpplib_cppsock::datablock b(1024);
    s<<b;
}
