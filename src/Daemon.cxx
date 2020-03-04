#include <Common/Daemon.h>

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <pwd.h>

static int DaemonsShutdownRequest = 0; // global flag set to 1 to request termination, e.g. on SIGTERM/SIGQUIT signals
// signal handler to notify exit request
static void signalHandler(int)
{
  DaemonsShutdownRequest = 1;
}

// this function is a replacement of the Linux glibc daemon() function, reported deprecated on MacOS
// if flag closeFiles is set, stdin/stdout/stderr are redirected to /dev/null
// returns 0 on success, an error code otherwise
int createDaemon(int closeFiles)
{

  // fork process
  switch (fork()) {
    case -1:
      // failure
      return __LINE__;

    case 0:
      // child
      break;

    default:
      // terminate calling process immediately
      _exit(0);
  }

  // create a new session to avoid being killed e.g. on closing parent terminal
  if (setsid() == -1) {
    return __LINE__;
  }

// redirect stdout/stderr to /dev/null if requested to do so
#define REDIRECT_PATH "/dev/null"
  if (closeFiles) {
    int fd = open(REDIRECT_PATH, O_RDWR, 0);
    if (fd == -1) {
      return __LINE__;
    }
    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    if (fd > 2) {
      close(fd);
    } else {
      return __LINE__;
    }
  }
  return 0;
}

Daemon::Daemon(int argc, char* argv[], DaemonConfigParameters* dConfigParams)
{
  isInitialized = 0;

  if (dConfigParams != nullptr) {
    params = *dConfigParams;
  }

  try {
    // parse command line parameters
    int option;
    while ((option = getopt(argc, argv, "z:o:")) != -1) {
      switch (option) {

        case 'z':
          log.info("Using configuration %s", optarg);
          try {
            config.load(optarg);
          } catch (std::string err) {
            log.error("Failed to load configuration: %s", err.c_str());
            throw __LINE__;
          }
          break;

        case 'o': {
          // pass-through option in the format key=value
          // todo: create a config file tree instead?
          std::string sOptarg = optarg;
          size_t qPos = sOptarg.find_first_of('=');
          if (qPos >= sOptarg.length()) {
            log.error("Failed to parse [key=value] in option %s", optarg);
            throw __LINE__;
          }
          std::string key = sOptarg.substr(0, qPos);
          std::string value = sOptarg.substr(qPos + 1);
          if (key == "isInteractive") {
            params.isInteractive = std::stoi(value);
          } else if (key == "idleSleepTime") {
            params.idleSleepTime = std::stoi(value);
            ;
          } else if (key == "userName") {
            params.userName = value;
          } else if (key == "redirectOutput") {
            params.redirectOutput = std::stoi(value);
            ;
          } else if (key == "logFile") {
            params.logFile = value;
          } else if (key == "logRotateMaxBytes") {
            params.logRotateMaxBytes = std::stoi(value);
          } else if (key == "logRotateMaxFiles") {
            params.logRotateMaxFiles = std::stoi(value);
          } else if (key == "logRotateNow") {
            params.logRotateNow = std::stoi(value);
          } else {
            log.error("Unkown option key %s in option %s", key.c_str(), optarg);
            throw __LINE__;
          }
        } break;

        default:
          throw __LINE__;
      }
    }

    // load daemon configuration parameters from config file, if any
    // read from [daemon] section
    std::string cfgEntryPoint = "daemon";
    config.getOptionalValue<int>(cfgEntryPoint + ".isInteractive", params.isInteractive);
    config.getOptionalValue<int>(cfgEntryPoint + ".idleSleepTime", params.idleSleepTime);
    config.getOptionalValue<std::string>(cfgEntryPoint + ".userName", params.userName);
    config.getOptionalValue<int>(cfgEntryPoint + ".redirectOutput", params.redirectOutput);
    config.getOptionalValue<std::string>(cfgEntryPoint + ".logFile", params.logFile);
    config.getOptionalValue<int>(cfgEntryPoint + ".logRotateMaxBytes", params.logRotateMaxBytes);
    config.getOptionalValue<int>(cfgEntryPoint + ".logRotateMaxFiles", params.logRotateMaxFiles);
    config.getOptionalValue<int>(cfgEntryPoint + ".logRotateNow", params.logRotateNow);

    // open log file, if configured
    if (params.logFile.length() > 0) {
      log.setLogFile(params.logFile.c_str(),
                     params.logRotateMaxBytes, params.logRotateMaxFiles, params.logRotateNow);
    }

    // become a daemon, if configured to do so
    if (!params.isInteractive) {
      int err = createDaemon(params.redirectOutput);
      if (err) {
        log.error("Could not become daemon: error %d", err);
        throw __LINE__;
      }
    }
    log.info("Started PID %d", getpid());

    // set daemon user name
    if (params.userName.length() > 0) {
      int success = 0;
      struct passwd* passent;
      passent = getpwnam(params.userName.c_str());
      if (passent != NULL) {
        setuid(passent->pw_uid);
        // check setuid worked as expected
        if (getuid() == passent->pw_uid) {
          success = 1;
        }
      }
      if (!success) {
        log.error("Failed to set user = %s", params.userName.c_str());
        throw __LINE__;
      } else {
        log.info("Changed user to %s", params.userName.c_str());
      }
    }

    // configure signal handlers for clean exit
    struct sigaction signalSettings;
    bzero(&signalSettings, sizeof(signalSettings));
    signalSettings.sa_handler = signalHandler;
    sigaction(SIGTERM, &signalSettings, NULL);
    sigaction(SIGQUIT, &signalSettings, NULL);
    sigaction(SIGINT, &signalSettings, NULL);

    isInitialized = 1;
    log.info("Daemon initialized");
  } catch (int errLine) {
    log.error("Failed to initialize daemon - error %d", errLine);
  }
}

Daemon::~Daemon()
{
  log.info("Daemon exiting");
}

// This is the main daemon loop
// The doLoop() [custom] method is called continuously until one of the following conditions occur:
//   - global flag DaemonsShutdownRequest is set (e.g. by signal handler)
//   - doLoop() returns with status Error or Completed
// A short pause is done between 2 consecutive calls if status Idle is received.
// returns -1 (daemon failed to initialize), 0 (loop exits with success) or 1 (loop exists with error).
int Daemon::run()
{
  if (!isInitialized)
    return -1;

  int errCode = 0;
  for (;;) {
    LoopStatus status = doLoop();
    if (DaemonsShutdownRequest) {
      log.info("Exit requested");
      break;
    }
    if (status == LoopStatus::Idle) {
      usleep(params.idleSleepTime);
    } else if (status == LoopStatus::Error) {
      log.error("Error detected");
      errCode = 1;
      break;
    } else if (status == LoopStatus::Completed) {
      log.info("Work completed");
      break;
    }
  }
  return errCode;
}

bool Daemon::isOk()
{
  return isInitialized;
}

// dummy base doLoop() class
Daemon::LoopStatus Daemon::doLoop()
{
  printf(".");
  fflush(stdout);
  return LoopStatus::Idle;
}

// todo
// boost exceptions?
