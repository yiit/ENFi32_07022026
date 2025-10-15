#include "../Helpers/SerialWriteBuffer.h"

#include "../Globals/Logging.h"

#include "../Helpers/ESPEasy_time_calc.h"
#include "../Helpers/Memory.h"
#include "../Helpers/StringConverter.h"

void SerialWriteBuffer_t::clear()
{
  _prefix.clear();
  free_string(_message);
  _timestamp = 0;
  _readpos   = 0;
}

String SerialWriteBuffer_t::colorize(const String& str) const {
#if FEATURE_COLORIZE_CONSOLE_LOGS
  const __FlashStringHelper *format = F("%s");

  switch (_loglevel)
  {
    case LOG_LEVEL_NONE: format = F("\033[31;1m%s\033[0m");     // Red + Bold or increased intensity
      break;
    case LOG_LEVEL_INFO: format = F("\033[36m%s\033[0m");       // Cyan
      break;
    case LOG_LEVEL_ERROR: format = F("\033[31;1m%s\033[0m");    // Red + Bold or increased intensity
      break;
#ifndef BUILD_NO_DEBUG
    case LOG_LEVEL_DEBUG: format = F("\033[32m%s\033[0m");      // Green
      break;
    case LOG_LEVEL_DEBUG_MORE: format = F("\033[35m%s\033[0m"); // Purple
      break;
    case LOG_LEVEL_DEBUG_DEV: format = F("\033[33m%s\033[0m");  // Yellow
      break;
#endif // ifndef BUILD_NO_DEBUG
    default:
      return str;

  }
  return strformat(format, str.c_str());
#else
return str;
#endif
}

size_t SerialWriteBuffer_t::write(Stream& stream, size_t nrBytesToWrite)
{
  size_t bytesWritten = 0;

  while (bytesWritten < nrBytesToWrite) {

    if ((_timestamp != 0) && (timePassedSince(_timestamp) > LOG_BUFFER_EXPIRE)) {
      clear();

      // Mark with empty line we skipped the rest of the message.
      bytesWritten += stream.println(F(" ..."));
      bytesWritten += stream.println();
    }

    if (_timestamp == 0) {
      _readpos = 0;

      // Need to fetch a line
      if (!Logging.getNext(_log_destination, _timestamp, _message, _loglevel) ||
          !loglevelActiveFor(_log_destination, _loglevel)) {
        free_string(_message);
        _timestamp = 0;
        return bytesWritten;
      }

      // Prepare prefix
      _prefix = format_msec_duration(_timestamp);

      if (_loglevel == LOG_LEVEL_NONE) {
        _prefix += colorize(F(" : >  "));
      } else {
      #ifndef LIMIT_BUILD_SIZE
        _prefix += strformat(F(" : (%d) "), FreeMem());
      #else
        _prefix += ' ';
      #endif // ifndef LIMIT_BUILD_SIZE
        {
          String loglevelDisplayString = getLogLevelDisplayString(_loglevel);

          while (loglevelDisplayString.length() < LOG_LEVEL_MAX_STRING_LENGTH) {
            loglevelDisplayString += ' ';
          }
          _prefix += colorize(loglevelDisplayString);
        }
        _prefix += F(" | ");
      }
    }

    const size_t maxToWrite = _prefix.length() + _message.length();

    if (nrBytesToWrite > maxToWrite) {
      nrBytesToWrite = maxToWrite;
    }

    bool done = false;

    while (!done && bytesWritten < nrBytesToWrite) {
      if (_readpos < _prefix.length()) {
        // Write prefix
        if (1 != stream.write(_prefix[_readpos])) { return bytesWritten; }
        ++bytesWritten;
        ++_readpos;
      } else if (!_prefix.isEmpty()) {
        // Clear prefix
        _prefix.clear();
        _readpos = 0;
      } else if (_readpos < _message.length()) {
        // Write message
        if (1 != stream.write(_message[_readpos])) { return bytesWritten; }
        ++bytesWritten;
        ++_readpos;
      } else {
        if ((bytesWritten + 2) > nrBytesToWrite) { return bytesWritten; }
        bytesWritten += stream.println();

        // Done with entry, cleanup and leave
        clear();
        done = true;
      }
    }
  }
  return bytesWritten;
}
