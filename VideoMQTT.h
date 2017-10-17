#define OMXPLAYER_DBUS_PATH_SERVER "/org/mpris/MediaPlayer2"  
#define OMXPLAYER_DBUS_INTERFACE_ROOT "org.mpris.MediaPlayer2"
#define OMXPLAYER_DBUS_INTERFACE_PLAYER "org.mpris.MediaPlayer2.Player"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dbus/dbus.h>
#include <errno.h>

#include "utils/log.h"

#include <mosquittopp.h>
#include "OMXThread.h"

 class VideoMQTT : public OMXThread , public mosqpp::mosquittopp
 {
 protected:
  int m_action;
  DBusConnection *conn;
  std::string m_dbus_name;
  
 public:
  VideoMQTT(const char *host, int port, int QoS, int keepalive, const char * _topic);
  ~VideoMQTT();
  void Close();
  void Process();
  void setDbusName(std::string dbus_name);
  void Sleep(unsigned int dwMilliSeconds);
  //int getEvent();
 
 private:
  void on_message(const struct mosquitto_message *mess);
  void send_dbus_cmd(const char * cmd);
  void send_dbus_cmd(const char * cmd, int32_t arg);
  void send_dbus_cmd(const char * cmd, int64_t arg);
  int dbus_connect();
  void dbus_disconnect();
 };
