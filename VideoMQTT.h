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
  std::string host;
  int port;
  int QoS;
  std::string topic_cmd;
  std::string topic_status;
  DBusConnection *conn;
  std::string m_dbus_name;
  
 public:
  VideoMQTT(std::string _host, std::string _topic_cmd, std::string _topic_status, int _port, int _QoS);
  ~VideoMQTT();
  void Close();
  void Process();
  void send_MQTT_msg(std::string *mess);
  void send_MQTT_msg(std::string *mess, std::string *topic);
  void setDbusName(std::string dbus_name);
  void Sleep(unsigned int dwMilliSeconds);
 
 private:
  void on_message(const struct mosquitto_message * message);
  void send_dbus_cmd(const char * cmd);
  void send_dbus_cmd(const char * cmd, int32_t arg);
  void send_dbus_cmd(const char * cmd, int64_t arg);
  int dbus_connect();
  void dbus_disconnect();
 };
