/***
 * MQTT : Option to publish to MQTT broker
 */

#if defined(OPTION_MQTT)
  #include <PubSubClient.h>

  #define MQTTprefix "36brick/"         // MQTT topic prefix for all publishes
  #define MQTT_ESSAIS_MAX 12            // MQTT tries before fail

  WiFiClient wifiClient;
  PubSubClient MQTTclient(wifiClient);

  bool MQTTok = false;                  // MQTT Connection state

  /***
   * MQTT option setup : sets the MQTT server communication
   */
  void MQTTsetup() {
      MQTTok = servOK && portOK;        // MQTT options (server and port) are defined and loaded
      if (MQTTok) 
          MQTTclient.setServer(retreivedMQTTserv.serv, atoi(retreivedMQTTport.port));
      else 
          Logln("[NFO] MQTT disabled");
  }

  /***
   * MQTT main loop : keeps MQTT connected and processes MQTT loop
   */
  void MQTTloop() {
      if (wifiOK && MQTTok) {
        if (!MQTTclient.connected()) {
          MQTTreconnect();
        }
        MQTTclient.loop();
      }
  }
  
  /***
   * Connects to the MQTT server
   */
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

  /***
   * Publishes an information to an MQTT topic
   */
  void MQTTpublish(String topic, String info ) {
      if (wifiOK && MQTTok) MQTTclient.publish(String(MQTTprefix + topic).c_str(), info.c_str(), true);
  }

#endif

