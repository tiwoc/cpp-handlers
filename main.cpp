#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <exception>
#include <stdexcept>

#include <execinfo.h>
#include <signal.h>
#include <unistd.h>

void printStacktrace()
{
    void *array[20];
    size_t size = backtrace(array, sizeof(array) / sizeof(array[0]));
    backtrace_symbols_fd(array, size, STDERR_FILENO);
}

void signalHandler(int sig)
{
    std::fprintf(stderr, "Error: signal %d\n", sig);
    printStacktrace();
    std::abort();
}

void terminateHandler()
{
    std::exception_ptr exptr = std::current_exception();
    if (exptr != 0)
    {
        // the only useful feature of std::exception_ptr is that it can be rethrown...
        try
        {
            std::rethrow_exception(exptr);
        }
        catch (std::exception &ex)
        {
            std::fprintf(stderr, "Terminated due to exception: %s\n", ex.what());
        }
        catch (...)
        {
            std::fprintf(stderr, "Terminated due to unknown exception\n");
        }
    }
    else
    {
        std::fprintf(stderr, "Terminated due to unknown reason :(\n");
    }
    printStacktrace();
    std::abort();
}

int main()
{
    signal(SIGSEGV, signalHandler);
    std::set_terminate(terminateHandler);

    srand(time(NULL));
    if (rand() % 2)
    {
        // segfault
        int *bad = NULL;
        std::fprintf(stderr, "%d", *bad);
    }
    else
    {
        // exception
        throw std::runtime_error("Hello, world!");
    }
}
