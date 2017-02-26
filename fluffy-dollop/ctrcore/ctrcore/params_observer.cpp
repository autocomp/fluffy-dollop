#include "params_observer.h"

#include <iostream>
#include <QRegExp>
#include <execinfo.h>
#include <QTimer>

ParamsObserver::ParamsObserver(QObject *parent):
    QObject( parent )
{}

QString ParamsObserver::getFunctionFromBacktrace( const int step )
{
    void* buffer[SIZE];
    size_t size;
    size = backtrace( buffer, SIZE );
    char** strings;
    strings = backtrace_symbols( buffer, size );

    QString func = strings[step];
    QRegExp rx_left = QRegExp( "(\\W|\\w)*[(]" );
    QRegExp rx_right = QRegExp( "[+](\\W|\\w)*" );
    func = func.remove( rx_left ).remove( rx_right );

    free( strings );
    return func;
}

bool ParamsObserver::checkFunctionInMap( const QString& param, bool is_cmd )
{
    if( is_cmd )
    {
        if ( !_bts.contains( param ))
            _bts[ param ] = getFunctionFromBacktrace( DEEP );
        else
            return false;
    }
    else
    {
        if ( !_bts_xml.contains( param ))
            _bts_xml[ param ] = getFunctionFromBacktrace( DEEP );
        else
            return false;
    }

    return true;
}

void ParamsObserver::printParams( const int verbose)
{
    if( verbose )
    {
        std::cerr << std::endl;
        if( _bts.size() )
        {
            std::cerr << "List of CtrConfig CMD parameters:" << std::endl;
            printMapOfParam( _bts, verbose, true );
        }
        else
            std::cerr << "No CtrConfig CMD parameters" << std::endl;

        if( _bts_xml.size() )
        {
            std::cerr << "List of CtrConfig XML parameters:" << std::endl;
            printMapOfParam( _bts_xml, verbose );
        }
        else
            std::cerr << "No CtrConfig XML parameters" << std::endl;

        std::cerr << std::endl;
    }
}

void ParamsObserver::printMapOfParam( QMap<QString, QString> &map, const int verbose,
                                      bool using_args )
{
    // max length
    int max_length_param = 0;
    int max_length_func = 0;
    for ( auto it = map.begin(); it != map.end(); ++it )
    {
        if ( it.key().size() > max_length_param )
            max_length_param = it.key().size();
        if ( it.value().size() > max_length_func )
            max_length_func = it.value().size();
    }

    for ( auto it = map.begin(); it != map.end(); ++it )
    {
        QString param = it.key();
        QString func = it.value();

        // name of param
        QString out = " -- ";
        if ( using_args )
        {
            bool contain = _args->contains( param );
            if( contain )
                out += "   USING ";
            else
                out += "NOTUSING ";
        }
        out += "param: \"" + param + "\"";

        // function name from backtrace
        if ( verbose > 1 )
        {
            out += ",";
            for ( int i = 0; i < max_length_param - param.size(); ++i)
                out += " ";
            out += " by func ( " + func + " )";
        }

        // values of makroses
        if ( verbose > 2 )
        {
            out += ",";
            for ( int i = 0; i < max_length_func - func.size(); ++i)
                out += " ";
            out += " exactly ( " + _fds[param].func + "() in " + _fds[param].file
                    + ": " + QString::number( _fds[param].line ) + " )";
        }

        std::cerr << out.toStdString() << std::endl;
    }
}

void ParamsObserver::startTimerToPrint(const int delay)
{
    QTimer* timer = new QTimer( this );
    timer->singleShot( delay, this, SLOT( letPrint() ));

    _timer_started = true;
}

void ParamsObserver::setVerbose( const int verbose )
{
    if( verbose > 0 )
        _verbose = verbose;
    else
        _verbose = 0;
}

bool ParamsObserver::timerStarted()
{
    return _timer_started;
}

void ParamsObserver::addFuncFileLine(const QString& param, const QString& func,
                                     const QString& file, const int& line)
{
    FunctionData fd;
    fd.func = func;
    fd.file = file;
    fd.line = line;

    _fds[param] = fd;
}

void ParamsObserver::letPrint()
{
    printParams( _verbose );
}
