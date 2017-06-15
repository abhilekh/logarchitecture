#include <vector>
#include <string>
#include <sstream>

#ifdef __ANDROID__
#include <jni.h> //NOLINT
#include "android/log.h" //NOLINT
#ifndef APPNAME
#define APPNAME "ReosMessage->"
#endif
#else
#include <iostream>
#endif

#define TAGME(p) {PUtil::getInst()->tag(p, __func__, __LINE__);}
#define CRASH0(message) {PUtil::getInst()->print(1, __func__, __LINE__, message, 0, 0, true);}
#define CRASH1(message, val) {PUtil::getInst()->print(1, __func__, __LINE__, message, val, 0, true);}
#define CRASH2(message, val1, val2) {PUtil::getInst()->print(1, __func__, __LINE__, message, val1, val2, true);}
#define CRASH3(message, val1, val2, val3) {PUtil::getInst()->print(1, __func__, __LINE__, message, val1, val2, val3, true);}
#define PRINT(p, message) {PUtil::getInst()->print(p, __func__, __LINE__, message);}
#define PRINT1(p, message, val) {PUtil::getInst()->print(p, __func__, __LINE__, message, val);}
#define PRINT2(p, message, val1, val2) {PUtil::getInst()->print(p, __func__, __LINE__, message, val1, val2);}
#define PRINT3(p, message, val1, val2, val3) {PUtil::getInst()->print(p, __func__, __LINE__, message, val1, val2, val3);}
#define PRINTLN(p, message) {PUtil::getInst()->print(p, __func__, __LINE__, message); PUtil::getInst()->pln(p);}
#define PRINTLN1(p, message, val) {PUtil::getInst()->print(p, __func__, __LINE__, message, val); PUtil::getInst()->pln(p);}
#define PRINTLN2(p, message, val1, val2) {PUtil::getInst()->print(p, __func__, __LINE__, message, val1, val2); PUtil::getInst()->pln(p);}
#define PRINTLN3(p, message, val1, val2, val3) {PUtil::getInst()->print(p, __func__, __LINE__, message, val1, val2, val3); PUtil::getInst()->pln(p);}
#define PPUSHLVL() {TAGME(5); PUtil::getInst()->pushlvl();}
#define PPOPLVL() {TAGME(5); PUtil::getInst()->poplvl();}
#define PLIVEON() {TAGME(5); PUtil::getInst()->setlive(true);}
#define PLIVEOFF() {TAGME(5); PUtil::getInst()->setlive(false);}

namespace{
    std::string join(const std::string & sep,
        const std::vector<std::string> & seq) {
    std::vector<std::string>::size_type seqlen = seq.size();

    if (seqlen == 0)
        return "";
    if (seqlen == 1)
        return seq[0];

    std::ostringstream os;
    std::copy(seq.begin(), seq.end() - 1,
            std::ostream_iterator<std::string>(os, sep.c_str()));
    os << *seq.rbegin();
    return os.str();
}
}

class PUtil {

    // Print functionality
    int printlevel;
    std::stringstream streambuf[5];
    int curlvl;
    bool live;
    std::stringstream crashmsg;

    inline std::string process(const char * _func) {
        std::string func(_func);
        std::string::size_type pos = func.find('(');
        if (pos != std::string::npos) {
            return func.substr(0, pos);
        } else {
            return func;
        }

    }

    void mcrash() {
        PUtil::pln(1);
        while (curlvl > -1) {
            printIn(1, streambuf[curlvl].str());
            curlvl--;
        }
#ifndef __ANDROID__
        std::cout << std::flush;
        exit(1);
#else
        crashmsg << streambuf[curlvl].str() << "\n";
#endif
    }

    PUtil() {
        printlevel = 0;
        curlvl = 0;
        live = false;
    }

    //! Prohibit copying
    PUtil(const PUtil&) = delete;
    //! Prohibit assignment
    PUtil& operator =(const PUtil&) = delete;

public:
    static PUtil* getInst() {
        // Don't return reference else I will kill you
        static PUtil inst;
        return &inst;
    }
    void setDebugLevel(int lvl) {
        printlevel = lvl;
    }

    void setlive(bool _lve) {
        live = _lve;
    }

    void printIn(int priority, std::string message) {
#ifdef __ANDROID__
        __android_log_print(ANDROID_LOG_ERROR, APPNAME, "%s", message.c_str());
#else
        if (printlevel >= priority) {
            std::cout << message << "\n";
        }
#endif
    }

    /* Print tab seperated string from list */
    void printT(int priority, const std::vector<std::string> seq) {
        std::string message = join("\t", seq);
        printIn(priority, message);
    }

    /* Print space seperated string from list */
    void printS(int priority, const std::vector<std::string> seq) {
        std::string message = join(" ", seq);
        printIn(priority, message);
    }

    void tag(int priority, const char ffunc[], int line) {
        if (printlevel >= priority) {
            streambuf[curlvl] << "Ping ::" << process(ffunc) << ":" << line << "\n";
            if (live) {
                flush();
            }
        }

    }

    template<class T1 = int, class T2 = int>
    void print(int priority, const char ffunc[], int line,
            std::string message, T1 val1 = T1(), T2 val2 = T2(), bool crash =
                    false) {
        if (printlevel >= priority) {
            streambuf[curlvl] << process(ffunc) << ":" << line << "->"
                << message << "\t" << val1 << "\t" << val2;
            if (live) {
                flush();
            }
        }
        if (crash) {
            mcrash();
        }
    }

    template<class T1 = int, class T2 = int, class T3 = int>
    void print(int priority, const char ffunc[], int line,
            std::string message, T1 val1, T2 val2, T3 val3,
            bool crash = false) {
        if (printlevel >= priority) {
            streambuf[curlvl] << process(ffunc) << ":" << line << "->"
                << message << "\t" << val1 << "\t" << val2 << "\t" << val3;
            if (live) {
                flush();
            }
        }
        if (crash) {
            mcrash();
        }
    }

    void pln(int priority) {
        if (printlevel >= priority) {
            streambuf[curlvl] << "\n";
            if (live) {
                flush();
            }
        }
    }

    void pclean() {
        streambuf[curlvl].str(std::string());
    }

    void flush() {
        PUtil::printIn(1, streambuf[curlvl].str());
        streambuf[curlvl].str(std::string());
#ifndef __ANDROID__
        std::cout << std::flush;
#endif
    }

    void pushlvl() {
        curlvl++;
        pclean();
    }

    void poplvl() {
        pclean();
        curlvl--;
    }

    void cleancrash(){
        crashmsg.str(std::string());
    }

    std::string getcrash(){
        return crashmsg.str();
    }
};

