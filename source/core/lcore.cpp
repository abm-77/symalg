
internal f32 INF_F32 (void) {
    union { f32 f; u32 u; } r;
    r.u = 0x7f800000;
    return r.f;
} 

internal f32 NEG_INF_F32 (void) {
    union { f32 f; u32 u; } r;
    r.u = 0xff800000;
    return r.f;
} 

internal f64 INF_F64 (void) {
    union { f64 f; u64 u; } r;
    r.u = 0x7ff0000000000000;
    return r.f;
} 

internal f64 NEG_INF_F64 (void) {
    union { f64 f; u64 u; } r;
    r.u = 0xfff0000000000000;
    return r.f;
} 

internal OperatingSystem OperatingSystemFromContext(void) {
    OperatingSystem result = OPERATINGSYSTEM_NULL;

    #if OS_WINDOWS
    result = OPERATINGSYSTEM_WINDOWS;
    #elif OS_MAC
    result = OPERATINGSYSTEM_MAC;
    #elif OS_LINUX
    result = OPERATINGSYSTEM_LINUX;
    #endif

    return result;
}

internal Architecture ArchitectureFromContext(void) {
    Architecture result = ARCHITECTURE_NULL;

    #if ARCH_X86
    result = ARCHITECTURE_X86;
    #elif ARCH_X64
    result = ARCHITECTURE_X64;
    #elif ARCH_ARM
    result = ARCHITECTURE_ARM;
    #elif ARCH_ARM64
    result = ARCHITECTURE_ARM64;
    #endif

    return result;
}

internal const char* StringFromOperatingSystem(OperatingSystem os) {
    switch (os) {
        case OPERATINGSYSTEM_WINDOWS: {
            return "Windows";
        } break;
        case OPERATINGSYSTEM_MAC: {
            return "Mac"; 
        } break;
        case OPERATINGSYSTEM_LINUX: {
            return "Linux";
        } break;
    }
    return "(NULL)";
}

internal const char* StringFromArchitecture(Architecture arch) {
    switch (arch) {
        case ARCHITECTURE_X86: {
            return "x86";
        } break;
        case ARCHITECTURE_X64: {
            return "x64"; 
        } break;
        case ARCHITECTURE_ARM: {
            return "ARM";
        } break;
        case ARCHITECTURE_ARM64: {
            return "ARM64";
        } break;
        defualt: {
            return "(NULL)";
        } break;
    }
}

internal const char* StringFromMonth(Month month) {
    switch (month) {
        case MONTH_JAN: {
            return "January";
        } break;
        case MONTH_FEB: {
            return "February";
        } break;
        case MONTH_MAR: {
            return "March";
        } break;
        case MONTH_APR: {
            return "April";
        } break;
        case MONTH_MAY: {
            return "May";
        } break;
        case MONTH_JUN: {
            return "June";
        } break;
        case MONTH_JUL: {
            return "July";
        } break;
        case MONTH_AUG: {
            return "August";
        } break;
        case MONTH_SEP: {
            return "September";
        } break;
        case MONTH_OCT: {
            return "October";
        } break;
        case MONTH_NOV: {
            return "November";
        } break;
        case MONTH_DEC: {
            return "December";
        } break;
    }
    return "(NULL)";
}
internal const char* StringFromWeekday(Weekday day) {
    switch (day) {
        case WEEKDAY_MON: {
            return "Monday";
        } break;
        case WEEKDAY_TUE: {
            return "Tuesday";
        } break;
        case WEEKDAY_WED: {
            return "Wednesday";
        } break;
        case WEEKDAY_THU: {
            return "Thursday";
        } break;
        case WEEKDAY_FRI: {
            return "Friday";
        } break;
        case WEEKDAY_SAT: {
            return "Saturday";
        } break;
        case WEEKDAY_SUN: {
            return "Sunday";
        } break;
    }

    return "(NULL)";
}

void _DebugLog(LogType log_type, const char* file, u32 line, const char* format, ...) {
    // Log to stdout
    {
        char* name = "";
        switch (log_type)
        {
            case LOG_INFO: {
                name = "INFO";
            } break;

            case LOG_WARNING: {
                name = "WARNING";
            } break;

            case LOG_ERROR: {
                name = "ERROR";
            } break;
        }

        fprintf(stdout, "[%s] (%s:%i): ", name, file, line);

        va_list args;                       // create a variable argument list 
        va_start(args, format);             // begin the variable argument list, taking parameters after format
        vfprintf(stdout, format, args);     // print the format string formatted with arguments from args into stdout
        va_end(args);                       // end the variable argument list
        fprintf(stdout, "%s", "\n");        // print newline
    } 

    // Log to Visual Studio Output
    {
        local_persist char string[4096] = {0};

        va_list args;
        va_start(args, format);
        vsnprintf(string, sizeof(string), format, args);     // print the format string formatted with arguments from args into string
        va_end(args);
/*
    #if BUILD_WIN32 
        OutputDebugStringA(string);
        OutputDebugStringA("\n");
    #endif
*/
    }

}

void _AssertFailure(const char* expression, const char* file, u32 line, b32 crash) {
    LogError("[Assertion Failure] Assertion of %s at %s:%u failed.", expression, file, line);
    if (crash) {
        BreakDebugger();
    }
}

void _DebugBreakInternal() {
    #if COMPILER_CL
        __debugbreak();
    #else
        *(volatile int*)0 = 0;
    #endif
}