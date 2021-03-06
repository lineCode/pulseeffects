#ifndef REALTIMEKIT_HPP
#define REALTIMEKIT_HPP

#include <dbus-1.0/dbus/dbus.h>
#include <iostream>

#define RTKIT_SERVICE_NAME "org.freedesktop.RealtimeKit1"
#define RTKIT_OBJECT_PATH "/org/freedesktop/RealtimeKit1"

class RealtimeKit {
 public:
  RealtimeKit(const std::string& tag);

  ~RealtimeKit();

  void set_priority(const std::string& source_name, const int& priority);
  void set_nice(const std::string& source_name, const int& nice_value);

 private:
  std::string log_tag;

  DBusConnection* bus;

  long long get_int_property(const char* propname);

  void make_realtime(const std::string& source_name, const int& priority);

  void make_high_priority(const std::string& source_name,
                          const int& nice_value);
};

#endif
