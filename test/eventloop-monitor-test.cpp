/***********************************************************************
* eventloop-monitor-test.cpp - Event loop monitor unit tests           *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2023 Markus Gans                                           *
*                                                                      *
* FINAL CUT is free software; you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as       *
* published by the Free Software Foundation; either version 3 of       *
* the License, or (at your option) any later version.                  *
*                                                                      *
* FINAL CUT is distributed in the hope that it will be useful, but     *
* WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
* GNU Lesser General Public License for more details.                  *
*                                                                      *
* You should have received a copy of the GNU Lesser General Public     *
* License along with this program.  If not, see                        *
* <http://www.gnu.org/licenses/>.                                      *
***********************************************************************/

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include <chrono>
#include <string>

#include <final/final.h>
#define USE_FINAL_H
#include <final/eventloop/eventloop_functions.h>
#undef USE_FINAL_H

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::high_resolution_clock;

void getException()
{ throw finalcut::monitor_error{"Monitor error"}; }

void getNoException()
{ }

//----------------------------------------------------------------------
// class FWidget_protected
//----------------------------------------------------------------------

class Monitor_protected : public finalcut::Monitor
{
  public:
    // Using-declaration
    using Monitor::Monitor;

    // Destructor
    ~Monitor_protected() override;

    // Mutators
    void p_setFileDescriptor (int);
    void p_setEvents (short);
    void p_setHandler (finalcut::handler_t&&);
    void p_setUserContext (void*);
    void p_setInitialized();

    // Inquiry
    auto p_isInitialized() const -> bool;

    // Methods
    void p_trigger (short) ;
};

//----------------------------------------------------------------------
inline Monitor_protected::~Monitor_protected() noexcept = default;  // destructor

//----------------------------------------------------------------------
inline void Monitor_protected::p_setFileDescriptor (int file_descriptor)
{
  finalcut::Monitor::setFileDescriptor(file_descriptor);
}

//----------------------------------------------------------------------
inline void Monitor_protected::p_setEvents (short ev)
{
  finalcut::Monitor::setEvents(ev);
}

//----------------------------------------------------------------------
inline void Monitor_protected::p_setHandler (finalcut::handler_t&& hdl)
{
  finalcut::Monitor::setHandler(std::move(hdl));
}

//----------------------------------------------------------------------
inline void Monitor_protected::p_setUserContext (void* uc)
{
  finalcut::Monitor::setUserContext(uc);
}

//----------------------------------------------------------------------
inline void Monitor_protected::p_setInitialized()
{
  finalcut::Monitor::setInitialized();
}

//----------------------------------------------------------------------
inline auto Monitor_protected::p_isInitialized() const -> bool
{
  return finalcut::Monitor::isInitialized();
}

//----------------------------------------------------------------------
inline void Monitor_protected::p_trigger (short return_events)
{
  finalcut::Monitor::trigger(return_events);
}

namespace
{

std::function<void(int)> signal_handler;

//----------------------------------------------------------------------
void sigHandler (int num)
{
  CPPUNIT_ASSERT ( num == SIGALRM );
  std::cout << "Call sigHandler(" << num << ")\n";
  signal_handler(num);
}

}

#if !defined(USE_KQUEUE_TIMER)
  struct kevent
  {
  };
#endif

namespace test
{

//----------------------------------------------------------------------
// class FSystemTest
//----------------------------------------------------------------------

class FSystemTest : public finalcut::FSystem
{
  public:
    // Constructor
    FSystemTest();

    // Methods
    auto inPortByte (uShort) -> uChar override;
    void outPortByte (uChar, uShort) override;
    auto isTTY (int) const -> int override;
    auto ioctl (int, uLong, ...) -> int override;
    auto pipe (int[2]) -> int override;
    auto open (const char*, int, ...) -> int override;
    auto close (int) -> int override;
    auto fopen (const char*, const char*) -> FILE* override;
    auto fputs (const char*, FILE*) -> int override;
    auto fclose (FILE*) -> int override;
    auto putchar (int) -> int override;
    auto sigaction ( int, const struct sigaction*
                   , struct sigaction*) -> int override;
    auto timer_create ( clockid_t, struct sigevent*
                      , timer_t* ) -> int override;
    auto timer_settime ( timer_t, int
                       , const struct itimerspec*
                       , struct itimerspec* ) -> int override;
    auto timer_delete (timer_t) -> int override;
    auto kqueue() -> int override;
    auto kevent ( int, const struct kevent*
                , int, struct kevent*
                , int, const struct timespec* ) -> int override;
    auto getuid() -> uid_t override;
    auto geteuid() -> uid_t override;
    auto getpwuid_r ( uid_t, struct passwd*, char*
                    , size_t, struct passwd** ) -> int override;
    auto realpath (const char*, char*) -> char* override;
    void setPipeReturnValue (int);
    void setSigactionReturnValue (int);
    void setTimerCreateReturnValue (int);
    void setTimerSettimeReturnValue (int);
    void setTimerDeleteReturnValue (int);
    void setKqueueReturnValue (int);
    void setKeventReturnValue (int);

  private:
    int pipe_ret_value{0};
    int sigaction_ret_value{0};
    int timer_create_ret_value{0};
    int timer_settime_ret_value{0};
    int timer_delete_ret_value{0};
    int kqueue_ret_value{0};
    int kevent_ret_value{0};
};


// constructors and destructor
//----------------------------------------------------------------------
FSystemTest::FSystemTest()  // constructor
{ }


// public methods of FSystemTest
//----------------------------------------------------------------------
inline auto FSystemTest::inPortByte (uShort) -> uChar
{
  return 0;
}

//----------------------------------------------------------------------
inline void FSystemTest::outPortByte (uChar, uShort)
{ }

//----------------------------------------------------------------------
inline auto FSystemTest::isTTY (int file_descriptor) const -> int
{
  std::cerr << "Call: isatty (file_descriptor=" << file_descriptor << ")\n";
  return 1;
}

//----------------------------------------------------------------------
inline auto FSystemTest::ioctl (int file_descriptor, uLong request, ...) -> int
{
  va_list args{};
  void* argp{};
  int ret_val{0};

  va_start (args, request);
  argp = va_arg (args, void*);
  va_end (args);

  std::cerr << "Call: ioctl (file_descriptor=" << file_descriptor
            << ", request=0x" << std::hex << request
            << ", argp=" << argp << std::dec << ")\n";
  return ret_val;
}

//----------------------------------------------------------------------
inline auto FSystemTest::pipe (int pipefd[2]) -> int
{
  std::cerr << "Call: pipe (pipefd={" << pipefd[0] << ", "
            << pipefd[1] << "})\n";
  return pipe_ret_value;
}

//----------------------------------------------------------------------
inline auto FSystemTest::open (const char* pathname, int flags, ...) -> int
{
  va_list args{};
  va_start (args, flags);
  auto mode = static_cast<mode_t>(va_arg (args, int));
  va_end (args);

  std::cerr << "Call: open (pathname=\"" << pathname
            << "\", flags=" << flags
            << ", mode=" << mode << ")\n";

  return 0;
}

//----------------------------------------------------------------------
inline auto FSystemTest::close (int file_descriptor) -> int
{
  std::cerr << "Call: close (file_descriptor=" << file_descriptor << ")\n";
  return 0;
}

//----------------------------------------------------------------------
inline auto FSystemTest::fopen (const char* path, const char* mode) -> FILE*
{
  std::cerr << "Call: fopen (path=" << path
            << ", mode=" << mode << ")\n";
  return nullptr;
}

//----------------------------------------------------------------------
inline auto FSystemTest::fclose (FILE* file_ptr) -> int
{
  std::cerr << "Call: fclose (file_ptr=" << file_ptr << ")\n";
  return 0;
}

//----------------------------------------------------------------------
inline auto FSystemTest::fputs (const char* str, FILE* stream) -> int
{
  return std::fputs(str, stream);
}

//----------------------------------------------------------------------
inline auto FSystemTest::putchar (int c) -> int
{
#if defined(__sun) && defined(__SVR4)
      return std::putchar(char(c));
#else
      return std::putchar(c);
#endif
}

//----------------------------------------------------------------------
inline auto FSystemTest::sigaction ( int, const struct sigaction*
                                   , struct sigaction* ) -> int
{
  return sigaction_ret_value;
}

//----------------------------------------------------------------------
inline auto FSystemTest::timer_create ( clockid_t, struct sigevent*
                                      , timer_t* ) -> int
{
  return timer_create_ret_value;
}

//----------------------------------------------------------------------
inline auto FSystemTest::timer_settime ( timer_t, int
                                       , const struct itimerspec*
                                       , struct itimerspec* ) -> int
{
  return timer_settime_ret_value;
}

//----------------------------------------------------------------------
inline auto FSystemTest::timer_delete (timer_t) -> int
{
  return timer_delete_ret_value;
}

//----------------------------------------------------------------------
inline auto FSystemTest::kqueue() -> int
{
  return kqueue_ret_value;
}

//----------------------------------------------------------------------
inline auto FSystemTest::kevent ( int, const struct ::kevent*
                                , int, struct ::kevent*
                                , int, const struct timespec*) -> int
{
  return kevent_ret_value;
}

//----------------------------------------------------------------------
inline auto FSystemTest::getuid() -> uid_t
{
  return 0;
}

//----------------------------------------------------------------------
inline auto FSystemTest::geteuid() -> uid_t
{
  return 0;
}

//----------------------------------------------------------------------
inline auto FSystemTest::getpwuid_r ( uid_t, struct passwd*, char*
                                    , size_t, struct passwd** ) -> int
{
  return 0;
}

//----------------------------------------------------------------------
inline auto FSystemTest::realpath (const char*, char*) -> char*
{
  return const_cast<char*>("");
}

//----------------------------------------------------------------------
inline void FSystemTest::setPipeReturnValue (int ret_val)
{
  pipe_ret_value = ret_val;
}

//----------------------------------------------------------------------
inline void FSystemTest::setSigactionReturnValue (int ret_val)
{
  sigaction_ret_value = ret_val;
}

//----------------------------------------------------------------------
inline void FSystemTest::setTimerCreateReturnValue (int ret_val)
{
  timer_create_ret_value = ret_val;
}

//----------------------------------------------------------------------
inline void FSystemTest::setTimerSettimeReturnValue (int ret_val)
{
  timer_settime_ret_value = ret_val;
}

//----------------------------------------------------------------------
inline void FSystemTest::setTimerDeleteReturnValue (int ret_val)
{
  timer_delete_ret_value = ret_val;
}

//----------------------------------------------------------------------
inline void FSystemTest::setKqueueReturnValue (int ret_val)
{
  kqueue_ret_value = ret_val;
}

//----------------------------------------------------------------------
inline void FSystemTest::setKeventReturnValue (int ret_val)
{
  kevent_ret_value = ret_val;
}

}  // namespace test


//----------------------------------------------------------------------
// class EventloopMonitorTest
//----------------------------------------------------------------------

class EventloopMonitorTest : public CPPUNIT_NS::TestFixture
{
  public:
    EventloopMonitorTest() = default;

  protected:
    void classNameTest();
    void noArgumentTest();
    void eventLoopTest();
    void setMonitorTest();
    void IoMonitorTest();
    void SignalMonitorTest();
    void TimerMonitorTest();
    void exceptionTest();

  private:
    void keyboardInput (std::string);
    void drainStdin();

    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (EventloopMonitorTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (noArgumentTest);
    CPPUNIT_TEST (eventLoopTest);
    CPPUNIT_TEST (setMonitorTest);
    CPPUNIT_TEST (IoMonitorTest);
    CPPUNIT_TEST (SignalMonitorTest);
    CPPUNIT_TEST (TimerMonitorTest);
    CPPUNIT_TEST (exceptionTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};


//----------------------------------------------------------------------
void EventloopMonitorTest::classNameTest()
{
  finalcut::EventLoop eloop{};
  const finalcut::Monitor monitor(&eloop);
  const finalcut::FString& eloop_classname = eloop.getClassName();
  const finalcut::FString& monitor_classname = monitor.getClassName();
  CPPUNIT_ASSERT ( eloop_classname == "EventLoop" );
  CPPUNIT_ASSERT ( monitor_classname == "Monitor" );
}

//----------------------------------------------------------------------
void EventloopMonitorTest::noArgumentTest()
{
  finalcut::EventLoop eloop{};
  const finalcut::Monitor m(&eloop);
  CPPUNIT_ASSERT ( m.getEvents() == 0 );
  CPPUNIT_ASSERT ( m.getFileDescriptor() == -1 );  // No File Descriptor
  CPPUNIT_ASSERT ( m.getUserContext() == nullptr );
  CPPUNIT_ASSERT ( ! m.isActive() );
}

//----------------------------------------------------------------------
void EventloopMonitorTest::eventLoopTest()
{
  // Test without monitor
  finalcut::EventLoop eloop{};
  signal_handler = [&eloop] (int num)
  {
    eloop.leave();
  };
  signal(SIGALRM, sigHandler);  // Register signal handler
  std::cout << "\n";
  alarm(1);  // Schedule a alarm after 1 seconds
  CPPUNIT_ASSERT ( eloop.run() == 0 );

  // Test with one monitor
  Monitor_protected mon(&eloop);
  CPPUNIT_ASSERT ( mon.getEvents() == 0 );
  CPPUNIT_ASSERT ( mon.getFileDescriptor() == -1 );  // No File Descriptor
  CPPUNIT_ASSERT ( mon.getUserContext() == nullptr );
  CPPUNIT_ASSERT ( ! mon.isActive() );
  mon.p_setEvents (POLLIN);
  std::array<int, 2> pipe_fd{{-1, -1}};
  auto callback_handler = [&pipe_fd, &eloop] (const finalcut::Monitor*, short)
  {
    std::cout << "Callback handle";
    uint64_t buf{0};
    CPPUNIT_ASSERT ( ::read(pipe_fd[0], &buf, sizeof(buf)) == sizeof(buf) );
    CPPUNIT_ASSERT ( buf == std::numeric_limits<uint64_t>::max() );
    eloop.leave();
  };
  mon.p_setHandler(callback_handler);
  CPPUNIT_ASSERT ( ::pipe(pipe_fd.data()) == 0 );
  mon.p_setFileDescriptor(pipe_fd[0]);  // Read end of pipe
  mon.resume();
  signal_handler = [&pipe_fd] (int)
  {
    uint64_t buf{std::numeric_limits<uint64_t>::max()};
    CPPUNIT_ASSERT ( ::write (pipe_fd[1], &buf, sizeof(buf)) > 0 );
  };
  alarm(1);  // Schedule a alarm after 1 seconds
  CPPUNIT_ASSERT ( eloop.run() == 0 );
  CPPUNIT_ASSERT ( mon.getEvents() == POLLIN );
  CPPUNIT_ASSERT ( mon.getFileDescriptor() == pipe_fd[0] );
  CPPUNIT_ASSERT ( mon.getUserContext() == nullptr );
  CPPUNIT_ASSERT ( mon.isActive() );
  signal(SIGALRM, SIG_DFL);
  signal_handler = [] (int) { };  // Do nothing
}

//----------------------------------------------------------------------
void EventloopMonitorTest::setMonitorTest()
{
  finalcut::EventLoop eloop{};
  Monitor_protected m(&eloop);
  CPPUNIT_ASSERT ( m.getEvents() == 0 );
  CPPUNIT_ASSERT ( m.getFileDescriptor() == -1 );  // No File Descriptor
  CPPUNIT_ASSERT ( m.getUserContext() == nullptr );
  CPPUNIT_ASSERT ( ! m.p_isInitialized() );
  m.p_setInitialized();
  CPPUNIT_ASSERT ( m.p_isInitialized() );
  CPPUNIT_ASSERT ( ! m.isActive() );
  m.resume();
  CPPUNIT_ASSERT ( m.isActive() );
  m.suspend();
  CPPUNIT_ASSERT ( ! m.isActive() );
  m.resume();
  CPPUNIT_ASSERT ( m.isActive() );
  m.p_setFileDescriptor (0);
  CPPUNIT_ASSERT ( m.getFileDescriptor() == 0 );
  m.p_setFileDescriptor (245);
  CPPUNIT_ASSERT ( m.getFileDescriptor() == 245 );
  m.p_setEvents (2);
  CPPUNIT_ASSERT ( m.getEvents() == 2 );
  m.p_setEvents (std::numeric_limits<short>::max());
  CPPUNIT_ASSERT ( m.getEvents() == std::numeric_limits<short>::max() );
  int value = 10;
  m.p_setHandler ([&value] (const finalcut::Monitor*, short n) { value -= n; });
  CPPUNIT_ASSERT ( value == 10 );
  m.p_trigger(2);
  CPPUNIT_ASSERT ( value == 8 );
  using Function = std::function<void()>;
  Function f = [&value] () { value *= 10; };
  m.p_setUserContext (reinterpret_cast<void*>(&f));
  CPPUNIT_ASSERT ( value == 8 );
  (*reinterpret_cast<Function*>(m.getUserContext()))();
  CPPUNIT_ASSERT ( value == 80 );
}

//----------------------------------------------------------------------
void EventloopMonitorTest::IoMonitorTest()
{
  finalcut::FTermios::init();
  finalcut::FTermios::storeTTYsettings();
  drainStdin();
  auto stdin_no = finalcut::FTermios::getStdIn();
  auto stdin_status_flags = fcntl(stdin_no, F_GETFL);
  finalcut::EventLoop eloop{};
  finalcut::IoMonitor io_monitor{&eloop};
  const finalcut::FString& io_monitor_classname = io_monitor.getClassName();
  CPPUNIT_ASSERT ( io_monitor_classname == "IoMonitor" );
  auto callback_handler = [&stdin_status_flags, &stdin_no, &eloop] (const finalcut::Monitor* mon, short)
  {
    char read_character{'\0'};
    CPPUNIT_ASSERT ( read_character == '\0' );
    stdin_status_flags |= O_NONBLOCK;
    CPPUNIT_ASSERT ( fcntl(stdin_no, F_SETFL, stdin_status_flags) != -1 );
    const auto bytes = ::read(mon->getFileDescriptor(), &read_character, 1);
    CPPUNIT_ASSERT ( bytes == 1 );
    CPPUNIT_ASSERT ( read_character == 'A' );
    stdin_status_flags &= ~O_NONBLOCK;
    CPPUNIT_ASSERT ( fcntl(stdin_no, F_SETFL, stdin_status_flags) != -1 );
    eloop.leave();
    std::cout << "\nIoMonitor callback handle" << std::flush;
  };
  io_monitor.init (stdin_no, POLLIN, callback_handler, nullptr);
  std::cout << "\n" << std::flush;
  io_monitor.resume();
  // Enter 'A'
  keyboardInput("A");
  // Keyboard interval timeout 75 ms
  std::this_thread::sleep_for(std::chrono::milliseconds(75));
  CPPUNIT_ASSERT ( eloop.run() == 0 );
  finalcut::FTermios::restoreTTYsettings();
}

//----------------------------------------------------------------------
void EventloopMonitorTest::SignalMonitorTest()
{
  finalcut::EventLoop eloop{};
  signal_handler = [] (int)
  {
    std::raise(SIGABRT);  // Send abort signal
  };
  signal(SIGALRM, sigHandler);  // Register signal handler
  finalcut::SignalMonitor signal_monitor{&eloop};
  const finalcut::FString& signal_monitor_classname = signal_monitor.getClassName();
  CPPUNIT_ASSERT ( signal_monitor_classname == "SignalMonitor" );
  auto callback_handler = [&eloop] (const finalcut::Monitor*, short)
  {
    std::cout << "SignalMonitor callback handle";
    eloop.leave();
  };
  signal_monitor.init(SIGABRT, callback_handler, nullptr);
  std::cout << "\n";
  alarm(1);  // Schedule a alarm after 1 seconds
  signal_monitor.resume();
  CPPUNIT_ASSERT ( eloop.run() == 0 );
  signal(SIGALRM, SIG_DFL);
  signal_handler = [] (int) { };  // Do nothing
}

//----------------------------------------------------------------------
void EventloopMonitorTest::TimerMonitorTest()
{
  finalcut::EventLoop eloop{};
  finalcut::TimerMonitor timer_monitor{&eloop};
  const finalcut::FString& timer_monitor_classname = timer_monitor.getClassName();
  CPPUNIT_ASSERT ( timer_monitor_classname == "TimerMonitor" );
  int num{0};
  auto callback_handler = [&eloop, &num] (const finalcut::Monitor*, short)
  {
    num++;
    std::cout << "TimerMonitor callback handle (" << num << ")\n";

    if ( num == 3 )
      eloop.leave();
  };
  timer_monitor.init (callback_handler, nullptr);
  timer_monitor.setInterval ( std::chrono::nanoseconds{ 500'000'000 }
                            , std::chrono::nanoseconds{ 1'000'000'000 } );
  std::cout << "\n";
  timer_monitor.resume();
  auto start = high_resolution_clock::now();
  CPPUNIT_ASSERT ( eloop.run() == 0 );
  auto end = high_resolution_clock::now();
  auto duration_ms = int(duration_cast<milliseconds>(end - start).count());
  CPPUNIT_ASSERT ( num == 3 );
  CPPUNIT_ASSERT ( duration_ms >= 2500 );
  CPPUNIT_ASSERT ( duration_ms < 2510 );

  timer_monitor.setInterval ( std::chrono::nanoseconds{ 100'000'000 }
                            , std::chrono::nanoseconds{ 100'000'000 } );
  num = 0;
  start = high_resolution_clock::now();
  CPPUNIT_ASSERT ( eloop.run() == 0 );
  end = high_resolution_clock::now();
  duration_ms = int(duration_cast<milliseconds>(end - start).count());
  CPPUNIT_ASSERT ( num == 3 );
  CPPUNIT_ASSERT ( duration_ms >= 300 );
  CPPUNIT_ASSERT ( duration_ms < 310 );
}

//----------------------------------------------------------------------
void EventloopMonitorTest::exceptionTest()
{
  CPPUNIT_ASSERT_THROW ( getException(), finalcut::monitor_error );
  CPPUNIT_ASSERT_NO_THROW ( getNoException() );

  // Bad file descriptor
  auto max_fd = int(sysconf(_SC_OPEN_MAX));
  CPPUNIT_ASSERT_THROW ( finalcut::drainPipe(max_fd), std::system_error );
  CPPUNIT_ASSERT_THROW ( finalcut::drainPipe(-1), std::system_error);

  // Signal monitor
  //---------------

  finalcut::EventLoop eloop{};
  finalcut::SignalMonitor signal_monitor1{&eloop};
  auto callback_handler = [] (finalcut::Monitor*, short) { };

  // SIGALRM used
  CPPUNIT_ASSERT_THROW ( signal_monitor1.init(SIGALRM, callback_handler, nullptr)
                       , std::invalid_argument );

  // No pipe could be established
  std::unique_ptr<finalcut::FSystem> fsys = std::make_unique<test::FSystemTest>();
  finalcut::FSystem::getInstance().swap(fsys);
  auto fsys_ptr = static_cast<test::FSystemTest*>(finalcut::FSystem::getInstance().get());
  fsys_ptr->setPipeReturnValue(-1);
  std::cout << "\n";
  CPPUNIT_ASSERT_THROW ( signal_monitor1.init(SIGTERM, callback_handler, nullptr)
                       , finalcut::monitor_error );
  fsys_ptr->setPipeReturnValue(0);

  // Double monitor instance for one signal
  CPPUNIT_ASSERT_NO_THROW ( signal_monitor1.init(SIGTERM, callback_handler, nullptr) );
  finalcut::SignalMonitor signal_monitor2{&eloop};
  CPPUNIT_ASSERT_THROW ( signal_monitor2.init(SIGTERM, callback_handler, nullptr)
                       , std::invalid_argument );
  CPPUNIT_ASSERT_NO_THROW ( signal_monitor2.init(SIGABRT, callback_handler, nullptr) );

  // Already initialised
  CPPUNIT_ASSERT_THROW ( signal_monitor1.init(SIGINT, callback_handler, nullptr)
                       , finalcut::monitor_error );

  // Sigaction error
  fsys_ptr->setSigactionReturnValue(-1);
  finalcut::SignalMonitor signal_monitor3{&eloop};
  CPPUNIT_ASSERT_THROW ( signal_monitor3.init(SIGHUP, callback_handler, nullptr)
                       , std::system_error );
  fsys_ptr->setSigactionReturnValue(0);
  CPPUNIT_ASSERT_NO_THROW ( signal_monitor3.init(SIGHUP, callback_handler, nullptr) );

  // Posix timer monitor
  //--------------------

  // No pipe could be established
  finalcut::PosixTimer posix_timer_monitor{&eloop};
  fsys_ptr->setPipeReturnValue(-1);
  CPPUNIT_ASSERT_THROW ( posix_timer_monitor.init(callback_handler, nullptr)
                       , finalcut::monitor_error );
  fsys_ptr->setPipeReturnValue(0);

  // Posix timer cannot be created
  fsys_ptr->setTimerCreateReturnValue(-1);
  CPPUNIT_ASSERT_THROW ( posix_timer_monitor.init(callback_handler, nullptr)
                       , finalcut::monitor_error );
  fsys_ptr->setTimerCreateReturnValue(0);

  CPPUNIT_ASSERT_NO_THROW ( posix_timer_monitor.init(callback_handler, nullptr) );

  // Already initialised
  CPPUNIT_ASSERT_THROW ( posix_timer_monitor.init(callback_handler, nullptr)
                       , finalcut::monitor_error );

  // Timer interval cannot be set
  fsys_ptr->setTimerSettimeReturnValue(-1);
  auto t1 = std::chrono::nanoseconds{ 500'000'000 };
  auto t2 = std::chrono::nanoseconds{ 1'000'000'000 };
  CPPUNIT_ASSERT_THROW ( posix_timer_monitor.setInterval(t1, t2)
                       , std::system_error );
  fsys_ptr->setTimerSettimeReturnValue(0);
  CPPUNIT_ASSERT_NO_THROW ( posix_timer_monitor.setInterval(t1, t2) );

  // Kqueue timer monitor
  //---------------------

  fsys_ptr->setKqueueReturnValue(-1);
  CPPUNIT_ASSERT_THROW ( finalcut::KqueueTimer{&eloop}
                       , std::system_error );
  fsys_ptr->setKqueueReturnValue(0);
  CPPUNIT_ASSERT_NO_THROW ( finalcut::KqueueTimer{&eloop} );
  finalcut::KqueueTimer kqueue_timer_monitor{&eloop};

  CPPUNIT_ASSERT_NO_THROW ( kqueue_timer_monitor.init(callback_handler, nullptr) );

  // Already initialised
  CPPUNIT_ASSERT_THROW ( kqueue_timer_monitor.init(callback_handler, nullptr)
                       , finalcut::monitor_error );

  // Event cannot be registered
  fsys_ptr->setKeventReturnValue(-1);
  CPPUNIT_ASSERT_THROW ( kqueue_timer_monitor.setInterval(t1, t2)
                       , finalcut::monitor_error );
  fsys_ptr->setKeventReturnValue(0);
  CPPUNIT_ASSERT_NO_THROW ( kqueue_timer_monitor.setInterval(t1, t2) );

}

//----------------------------------------------------------------------
void EventloopMonitorTest::keyboardInput (std::string s)
{
  // Simulates keystrokes

  const char EOT = 0x04;  // End of Transmission
  auto stdin_no = finalcut::FTermios::getStdIn();
  fflush(stdout);

  std::string::const_iterator iter;
  iter = s.begin();

  while ( iter != s.end() )
  {
    char c = *iter;

    if ( ::ioctl (stdin_no, TIOCSTI, &c) < 0 )
      break;

    ++iter;
  }

  if ( ::ioctl (stdin_no, TIOCSTI, &EOT) < 0 )
    return;

  fflush(stdin);
}

//----------------------------------------------------------------------
void EventloopMonitorTest::drainStdin()
{
  auto stdin_no = finalcut::FTermios::getStdIn();
  auto stdin_no2 = dup(stdin_no);

  if ( stdin_no2 < 0 )
    return;

  if ( tcdrain(stdin_no2) < 0 )
    return;

  if ( tcflush(stdin_no2, TCIFLUSH) < 0 )
    return;

  if ( ::close(stdin_no2) < 0 )
    return;
}

// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (EventloopMonitorTest);

// The general unit test main part
#include <main-test.inc>