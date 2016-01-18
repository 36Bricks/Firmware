void setupWifi() {
  MDNS.begin(host);
  if (ssidOK && passOK) {
    wifiOK = true;
    Logln("[NFO] Connecting WIFI to " + String(retreivedSSID.ssid) + " / " + String(retreivedPASS.pass));

    WiFi.begin(retreivedSSID.ssid, retreivedPASS.pass);
    int nbRetries = 0;
    while (WiFi.status() != WL_CONNECTED) {
      nbRetries++;
      delay(250);
      //Log(".");
      if (nbRetries > 120) {
        wifiOK = false;
        break;
      }
    }

    if (wifiOK) {
      Logln("[NFO] WiFi connected ! ");
      Logln("[NFO] IP : " + String(WiFi.localIP()));
      Logln("[NFO] HTTP Server started ... begin loop");
    } else {
      Logln("");
      Logln("[ERR] WiFi not connected, starting AP !");
      setupWiFiAP(); 
    }
  }
}


