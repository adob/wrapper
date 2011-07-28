#pragma once
#include <stdio.h>
#include <QTextStream>

struct Printer
{
    QTextStream & file;
    bool has_newline;
    
    Printer(QTextStream & s, bool newline = true) : file(s), has_newline(newline) {}
    
    
    operator QString () 
    {
        QString *ptr = file.string();
        
        if (ptr == NULL) 
            return QString();
        else 
            return *ptr;
    }
};

struct PrintUnformatted : Printer
{
    
    PrintUnformatted(QTextStream & f) : Printer(f) { }
    
    template <typename T>
    PrintUnformatted& operator , (T t)
    {
        file << ' ';
        file << t;
        return *this;
    }
    
    ~PrintUnformatted() { 
        if (has_newline)
            file << '\n'; 
    }
 
};

struct PrintFormatted : Printer
{
    const char *s;
    
    PrintFormatted(const char *str, QTextStream & f) : Printer(f), s(str)
    {
        do_printf();
    }
    
    ~PrintFormatted() { file << s; }
    
    void do_printf()
    {
        while (*s)
        {
            if (*s == '%' && *(++s) != '%')
            {
                s++;
                break;
            }
            
            file << *s;
            
            s++;
        }
    }
    
    template <typename T>
    PrintFormatted& operator , (T t)
    {
        file << t;
        do_printf();
        return *this;
    }
 
};


struct PrintUndecided : Printer
{
    const char *s;
    
    PrintUndecided(const char *str, QTextStream & f) : Printer(f), s(str)
    {
        
    }
    
    template <typename T>
    PrintFormatted operator % (T t)
    {
        PrintFormatted pf(s, file);
        pf, t;
        s = 0;
        return pf;
    }
    
    template <typename T>
    PrintUnformatted operator , (T t)
    {
        file << s;
        file << ' ';
        file << t;
        s = 0;
        return PrintUnformatted(file);
    }
    
    ~PrintUndecided() { 
        if (s != 0) {
            file << s;
            
            if (has_newline)
                file << '\n';
            
            s = 0;
        }
        
    }
    
    operator QString () 
    {
        this->~PrintUndecided();
        QString *ptr = file.string();
        
        if (ptr == NULL) 
            return QString();
        else 
            return *ptr;
    }
};

struct PrintHelper
{        
    QTextStream file;
    
    PrintHelper(FILE *f) : file(f, QIODevice::WriteOnly) {}
    PrintHelper(QString & str): file(&str, QIODevice::WriteOnly) {}
    
    template <typename T>
    PrintUnformatted operator * (T const& t) 
    {
        file << t;
        return PrintUnformatted(file);
    }
    
    PrintUndecided operator * (const char *str) 
    {
        return PrintUndecided(str, file);
    }
    
    ~PrintHelper()
    {
        file.flush();
    }
    
};

struct StringPrint : PrintHelper
{
    QString s;
    StringPrint() : PrintHelper(s) {}
};

#define print PrintHelper(stdout)*
#define eprint PrintHelper(stderr)*
#define fprint(file) PrintHelper(file)*
#define sprint StringPrint()*

