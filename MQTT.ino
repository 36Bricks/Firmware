#if defined(OPTION_MQTT)
  #include <PubSubClient.h>

  #define MQTTprefix "36brick/"

  WiFiClient wifiClient;
  PubSubClient MQTTclient(wifiClient);

  bool MQTTok = false;

  void MQTTsetup() {
      MQTTok = servOK && portOK;
      if (MQTTok) 
          MQTTclient.setServer(retreivedMQTTserv.serv, atoi(retreivedMQTTport.port));
      else 
          Logln("[NFO] MQTT disabled");
  }

  void MQTTloop() {
      if (wifiOK && MQTTok) {
        if (!MQTTclient.connected()) {
          MQTTreconnect();
        }
        MQTTclient.loop();
      }
  }

  #define MQTT_ESSAIS_MAX 12
  
  void MQTTreconnect() {
    int nbEssaisMQTT = 0;
    while (!MQTTclient.connected()) {
      Logln(String("[NFO] Attempting MQTT connection : " + String(retreivedMQTTserv.serv)));
      if (MQTTclient.connect("36BrickClient")) {
        Logln("[NFO] MQTT connected");
      } else {
        nbEssaisMQTT++;
        if (nbEssaisMQTT >= MQTT_ESSAIS_MAX) {
            Logln("[NFO] MQTT connection failed ! MQTT disabled");
            MQTTok = false;
            return;
        } else {
            Logln("[NFO] MQTT connection failed, try again in 5 seconds");
            delay(5000);
        }
        yield();
      }
    }
  }

  void MQTTpublish(String topic, String info ) {
      if (wifiOK && MQTTok) MQTTclient.publish(String(MQTTprefix + topic).c_str(), info.c_str(), true);
  }

#endif

