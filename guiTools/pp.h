#pragma once

// pp - power print, mini logger
// pp("width:$, height:$", w, h);
//
// $ is the placeholder for all kind of objects
// $ is type save
//
// (C) Olaf Kliche
// -----------------------------------------------------------

#include <string>
#include <cstring>
#include <sstream>
#include <mutex>
#include <iostream>
#include <iomanip>
#include <thread>
#include <stdarg.h>
#include <chrono>

#include <QString>
#include <QPoint>
#include <QRect>
#include <QImage>

#ifdef _WIN32
    #define NOMINMAX
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#elif __linux__
    #include <unistd.h>
    #include <syscall.h>
#endif

namespace okl
{
    void pp();
    void pp(const char * pStr);
    void pp(unsigned int space, const char * pStr);

    template<typename First, typename ... Rest>
    void pp(const char * pFormat, const First & first, const Rest & ... rest);

    template<typename First, typename ... Rest>
    void pp(unsigned int space, const char * pFormat, const First & first, const Rest & ... rest);

    // some string-format helpers
    std::string xprintF(const char * pFormat=nullptr);

    template<typename First, typename ... Rest>
    std::string xprintF(unsigned int space, const char * pFormat, const First & first, const Rest & ... rest);

    template<typename First, typename ... Rest>
    std::string xprintF(const char * pFormat, const First & first, const Rest & ... rest);

    std::string xprf(const char * format, ...); // format is like c's int sprintf ( char * str, const char * format, ... );

    enum class FORM_SECOND   { MICROSECOND, MILLISECOND, NO_DECIMAL_PLACES };

    // default like this "16:46:24,205523" - Microseconds
    std::string get_time_stamp(const std::string & sFormat="%H:%M:%S", FORM_SECOND formSecond = FORM_SECOND::MICROSECOND);

    void pp_set_thread_ID_main();

    void xSleep(unsigned long long milliSeconds);

    void exitMessage (int exitCode, const char * pFormat); // cout message and wait 5 sec before exit

    template<typename First, typename ... Rest>
    void exitMessage (int exitCode, const char * pFormat, const First & first, const Rest & ... rest); // same with variadics


}

// -------------------------------------------------------------------------------------------------------------------------- 
// Implementation
// -------------------------------------------------------------------------------------------------------------------------- 
namespace okl
{
    template<typename STREAM>
    void streamIt(STREAM & stream, const char * pStr);

    template<typename STREAM, typename T, typename... Targs>
    void streamIt(STREAM & ss, const char * format, T value, Targs... Fargs);


    class LogPrinter
    {
    public:
        static void log(const std::string & s); // thread safe - called by pp(..)
        static void set_main_thread_id(); // optional: if called in  main(..), "main thread id" is 000000 always!
    private:
        inline static std::mutex mutexWorker;
        inline static unsigned long long counter = 0;
        inline static unsigned int th_id_main = 0;

        static unsigned int get_current_threadID(); // want native thread id, not std::this_thread::get_id() monster id!
        static std::string trailler();
    };



    template<typename STREAM>
    void streamIt(STREAM & stream, const char * pStr)
    {
        if (pStr)
        {
            stream << pStr;
        }
    }

    template<typename STREAM, typename T, typename... Targs>
    void streamIt(STREAM & ss, const char * format, T value, Targs... Fargs)
    {
        for (; *format; format++)
        {
            if (*format == '$')
            {
                //objectToStringStream(ss, value);
                ss << value;
                streamIt(ss, format + 1, Fargs...); // recursive call

                return;
            }
            ss << *format;
        }
    }

    inline std::string xprintF(const char * pFormat)
    {
        return pFormat ? pFormat : "";
    }

    inline std::string xprf(const char * format, ...)
    {
        if (!format)
        {
            return "<<<xprf,nullpointer>>>";
        }

        if (strlen(format) >= 4 * 1024)
        {
            return "";
        }

#ifdef _WIN32
        // about _CRT_SECURE_NO_WARNINGS
        #pragma warning( push )
        #pragma warning(disable: 4996)
#endif

        char dest[4 * 1024];
        va_list argptr;
        va_start(argptr, format);
        vsprintf(dest, format, argptr);
        va_end(argptr);

#ifdef _WIN32
    #pragma warning( pop )
#endif

        return dest;
    }


    template <typename OBJECT>
    inline void write_object(std::stringstream & ss, const OBJECT & o)
    {
        ss << o;
    }


    template<typename First, typename... Rest>
    std::string xprintF(unsigned int spaces, const char * pFormat, const First & first, const Rest & ... rest)
    {
        if (!pFormat)
        {
            return "";
        }

        std::stringstream ss;

        ss << std::string(spaces, ' ');

        for (; *pFormat; pFormat++)
        {
            if (*pFormat == '$')
            {
                write_object(ss, first);

                pFormat++;
                break;
            }
            ss << *pFormat;
        }

        streamIt(ss, pFormat, rest ...);

        return ss.str();
    }

    template<typename First, typename... Rest>
    std::string xprintF(const char * pFormat, const First & first, const Rest & ... rest)
    {
        if (!pFormat)
        {
            return "";
        }

        std::stringstream ss;

        for (; *pFormat; pFormat++)
        {
            if (*pFormat == '$')
            {
                write_object(ss, first);

                pFormat++;
                break;
            }
            ss << *pFormat;
        }

        streamIt(ss, pFormat, rest ...);

        return ss.str();
    }

    inline std::string get_time_stamp(const std::string & sFormat, FORM_SECOND formSecond)
    {
        std::chrono::time_point<std::chrono::system_clock> timeNow = std::chrono::system_clock::now();

        const char * pDateTimeFormat = sFormat.c_str();

        auto duration = timeNow.time_since_epoch();

        std::chrono::system_clock::time_point tp = std::chrono::system_clock::time_point(duration);


        auto ttime_t = std::chrono::system_clock::to_time_t(tp);
        auto tp_sec = std::chrono::system_clock::from_time_t(ttime_t);


#ifdef _WIN32
        // about _CRT_SECURE_NO_WARNINGS
        #pragma warning( push )
        #pragma warning(disable: 4996)
#endif
        std::tm * ttm = localtime(&ttime_t);
#ifdef _WIN32
    #pragma warning( pop )
#endif


        char time_str[512];
        memset(time_str, 0, sizeof(time_str));

        // %H:%M:%S
        strftime(time_str, sizeof(time_str) - 1, pDateTimeFormat, ttm);

        std::string result(time_str);


        std::stringstream ssAfter;

        if (formSecond == FORM_SECOND::MICROSECOND)
        {
            result.append(",");
            std::chrono::microseconds mcc = std::chrono::duration_cast<std::chrono::microseconds>(tp - tp_sec);
            uint64_t m = mcc.count();
            ssAfter << std::setw(6) << std::setfill('0') << (long long) m;
        }
        else if (formSecond == FORM_SECOND::MILLISECOND)
        {
            result.append(",");
            std::chrono::milliseconds mcc = std::chrono::duration_cast<std::chrono::milliseconds>(tp - tp_sec);
            uint64_t m = mcc.count();
            ssAfter << std::setw(3) << std::setfill('0') << (long long) m;
        }

        result.append(ssAfter.str());

        return result.c_str();

    }

    inline void pp()
    {
        LogPrinter::log("");
    }
    inline void pp(const char * pStr)
    {
        LogPrinter::log(pStr);
    }

    inline void pp(unsigned int space, const char * pStr)
    {
        std::string s = xprintF(space, "$", pStr);
        LogPrinter::log(s);
    }

    template<typename First, typename... Rest>
    inline void pp(const char * pFormat, const First & first, const Rest & ... rest)
    {
        LogPrinter::log(xprintF(pFormat, first, rest ...));
    }

    template<typename First, typename... Rest>
    inline void pp(unsigned int space, const char * pFormat, const First & first, const Rest & ... rest)
    {
        LogPrinter::log(xprintF(space, pFormat, first, rest ...));
    }


    inline void LogPrinter::set_main_thread_id()
    {
        using SCOPE_LOCKER = std::scoped_lock<std::mutex>; // in all other situations
        SCOPE_LOCKER locker(mutexWorker);

#ifdef _WIN32
        LogPrinter::th_id_main = (unsigned int) GetCurrentThreadId();
#else
        LogPrinter::th_id_main = (unsigned int) syscall(SYS_gettid);
#endif

    }

    inline void pp_set_thread_ID_main()
    {
        LogPrinter::set_main_thread_id();
    }


    inline void xSleep(unsigned long long ms)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }


    inline unsigned int LogPrinter::get_current_threadID()
    {
        unsigned int thID = 0;
#ifdef _WIN32
        thID = (unsigned int) GetCurrentThreadId();
#else
        thID = (unsigned int) syscall(SYS_gettid);
#endif

//        if (thID =)
        return LogPrinter::th_id_main == thID ? 0 : thID;
    }

    inline std::string LogPrinter::trailler()
    {
        std::stringstream ss;

        ss << xprf("[%06d] ", counter++);
        ss << xprintF("[$] ", get_time_stamp("%H:%M:%S", FORM_SECOND::MICROSECOND));
        ss << xprf("[%06d]", get_current_threadID()); // 6 digits ar normally enough
        // ss << std::this_thread::get_id(); // I DO NOT WANT 140139344544704 !
        ss << " ";

        return ss.str();
    }

    inline void LogPrinter::log(const std::string & s)
    {
        using SCOPE_LOCKER = std::scoped_lock<std::mutex>; // in all other situations
        SCOPE_LOCKER locker(mutexWorker);

        std::cout << trailler();
        std::cout << s;
        std::cout << "\n";
    }

    inline void exitMessage (int exitCode, const char * pFormat)
    {
        std::cout << "\n";
        std::cout << "exitcode:" << exitCode << "\n";
        std::cout << "exit in 5 sec\n";
        std::cout << (pFormat ? pFormat : "");

        exit(exitCode);
    }

    template<typename First, typename ... Rest>
    inline void exitMessage (int exitCode, const char * pFormat, const First & first, const Rest & ... rest)
    {
        exitMessage(exitCode, xprintF(pFormat, first, rest ...).data());
    }


}

#include "inline/pp_inline.h"