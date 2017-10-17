
#include "VideoMQTT.h"

VideoMQTT::VideoMQTT(const char *host, int port, int QoS, int keepalive, const char * _topic)
{
	mosqpp::lib_init();
	printf("VideoMQTT Init");
	connect(host, port, keepalive);
	subscribe(NULL,_topic,QoS);

  if (dbus_connect() < 0)
  {
    CLog::Log(LOGWARNING, "VideoMQTT: DBus connection failed");
  } 
  else 
  {
    CLog::Log(LOGDEBUG, "VideoMQTT: DBus connection succeeded");
  }

  dbus_threads_init_default();
  Create();
  m_action = -1;
}

VideoMQTT::~VideoMQTT() 
{
	mosqpp::lib_cleanup();
	Close();
}

void VideoMQTT::Close()
{
  if (ThreadHandle()) 
  {
    StopThread();
  }
  dbus_disconnect();
}


void VideoMQTT::Sleep(unsigned int dwMilliSeconds)
{
  struct timespec req;
  req.tv_sec = dwMilliSeconds / 1000;
  req.tv_nsec = (dwMilliSeconds % 1000) * 1000000;

  while ( nanosleep(&req, &req) == -1 && errno == EINTR && (req.tv_nsec > 0 || req.tv_sec > 0));
}

void VideoMQTT::Process() 
{
  while(!m_bStop)
  {
    if (conn)
      dbus_connection_read_write_dispatch(conn, 0);

	loop(10,1);
	
    Sleep(20);
  }
}

void VideoMQTT::on_message(const struct mosquitto_message *mess)
{
	std::string s = (char*)mess->payload;
	s.append(" ");
	size_t pos = 0;
	std::string cmd[5];
	uint8_t	i=0;

	fprintf(stderr,"%s\n",s.c_str());

	while( (pos=s.find(" ")) != std::string::npos)
	{
		cmd[i]=s.substr(0,pos);
		s.erase(0,pos+1);
		//fprintf(stderr,"%s ",cmd[i].c_str());
		i++;
	}
	fprintf(stderr,"\n");

/*	if(!strcmp("start",cmd[0].c_str()))
		if(i == 2) // We need one argument
			VideoControls::Start(cmd[1].c_str());
		
	if(!strcmp("setPosition",cmd[0].c_str()))
		if(i == 3) // We need two argument
			VideoControls::SetPosition(atoi(cmd[1].c_str()),atoi(cmd[2].c_str()));

	if(!strcmp("stop",cmd[0].c_str()))
		VideoControls::Stop();

	if(!strcmp("reset",cmd[0].c_str()))
		VideoControls::Reset();

	if(!strcmp("pause",cmd[0].c_str()))
		VideoControls::Toggle();
*/
	


  if(!strcmp(cmd[0].c_str(),"SetPosition"))
  {
    send_dbus_cmd(cmd[0].c_str(), (int64_t)(atoi(cmd[1].c_str())*1000000));
  	Sleep(00);
  	send_dbus_cmd("Play");
  	Sleep(1000);
	send_dbus_cmd("Pause");
  } else
  {
  	send_dbus_cmd(cmd[0].c_str());
  }
  

}

void VideoMQTT::send_dbus_cmd(const char * cmd)
{
	DBusMessage *message = NULL, *reply = NULL;
	DBusError error;
	
	if (!conn)
		return;

  dbus_error_init(&error);

  if (!(message = dbus_message_new_method_call(m_dbus_name.c_str(),
                                              OMXPLAYER_DBUS_PATH_SERVER, 
                                              OMXPLAYER_DBUS_INTERFACE_PLAYER,
                                              cmd))) 
  {
    CLog::Log(LOGWARNING, "VideoMQTT: DBus error 1");
    goto fail;
  }

  reply = dbus_connection_send_with_reply_and_block(conn, message, -1, &error);

  if (!reply || dbus_error_is_set(&error))
    goto fail;

  dbus_message_unref(message);
  dbus_message_unref(reply);

  return;

fail:
  if (dbus_error_is_set(&error)) 
  {
    printf("%s", error.message);
    dbus_error_free(&error);
  }

  if (message)
    dbus_message_unref(message);

  if (reply)
    dbus_message_unref(reply);
}

void VideoMQTT::send_dbus_cmd(const char * cmd, int32_t arg)
{
	DBusMessage *message = NULL, *reply = NULL;
	DBusError error;
	
	if (!conn)
		return;

  dbus_error_init(&error);

  if (!(message = dbus_message_new_method_call(m_dbus_name.c_str(),
                                              OMXPLAYER_DBUS_PATH_SERVER, 
                                              OMXPLAYER_DBUS_INTERFACE_PLAYER,
                                              cmd))) 
  {
    CLog::Log(LOGWARNING, "VideoMQTT: DBus error 1");
    goto fail;
  }
  
  dbus_message_append_args(message, DBUS_TYPE_INT32, &arg, DBUS_TYPE_INVALID);

  reply = dbus_connection_send_with_reply_and_block(conn, message, -1, &error);

  if (!reply || dbus_error_is_set(&error))
    goto fail;

  dbus_message_unref(message);
  dbus_message_unref(reply);

  return;

fail:
  if (dbus_error_is_set(&error)) 
  {
    printf("%s", error.message);
    dbus_error_free(&error);
  }

  if (message)
    dbus_message_unref(message);

  if (reply)
    dbus_message_unref(reply);
}

void VideoMQTT::send_dbus_cmd(const char * cmd, int64_t arg)
{
	DBusMessage *message = NULL, *reply = NULL;
	DBusError error;
	
	if (!conn)
		return;

  dbus_error_init(&error);

  if (!(message = dbus_message_new_method_call(m_dbus_name.c_str(),
                                              OMXPLAYER_DBUS_PATH_SERVER, 
                                              OMXPLAYER_DBUS_INTERFACE_PLAYER,
                                              cmd))) 
  {
    CLog::Log(LOGWARNING, "VideoMQTT: DBus error 1");
    goto fail;
  }
  
  dbus_message_append_args(message, DBUS_TYPE_INT64, &arg, DBUS_TYPE_INVALID);

  reply = dbus_connection_send_with_reply_and_block(conn, message, -1, &error);

  if (!reply || dbus_error_is_set(&error))
    goto fail;

  dbus_message_unref(message);
  dbus_message_unref(reply);

  return;

fail:
  if (dbus_error_is_set(&error)) 
  {
    printf("%s", error.message);
    dbus_error_free(&error);
  }

  if (message)
    dbus_message_unref(message);

  if (reply)
    dbus_message_unref(reply);
}

void VideoMQTT::setDbusName(std::string dbus_name)
{
  m_dbus_name = dbus_name;
}

int VideoMQTT::dbus_connect() 
{
  DBusError error;

  dbus_error_init(&error);
  if (!(conn = dbus_bus_get_private(DBUS_BUS_SESSION, &error))) 
  {
    CLog::Log(LOGWARNING, "dbus_bus_get_private(): %s", error.message);
        goto fail;
  }

  dbus_connection_set_exit_on_disconnect(conn, FALSE);

  return 0;

fail:
    if (dbus_error_is_set(&error))
        dbus_error_free(&error);

    if (conn) 
    {
        dbus_connection_close(conn);
        dbus_connection_unref(conn);
        conn = NULL;
    }

    return -1;

}

void VideoMQTT::dbus_disconnect() 
{
    if (conn) 
    {
        dbus_connection_close(conn);
        dbus_connection_unref(conn);
        conn = NULL;
    }
}
