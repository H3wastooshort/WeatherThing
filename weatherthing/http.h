//functions by me
template<typename T> void send_http_get_parameter(Stream& ser, const __FlashStringHelper* f_name, T val, bool first = false) {
  char name[32];
  strncpy_P(name, (const char*)f_name, sizeof(name) - 1);
  //size_t name_len = strlen(name);
  ser.print(first ? '?' : '&');
  ser.print('=');
  //TODO urlencode?
  ser.print(val);
}


//modified functions from the GSMSim Library (https://github.com/erdemarslan/GSMSim) (MIT Licensed)


// HTTP Get Method - No return web server response
String GSMSimHTTP::get_manual_begin(String url) {

  if (isConnected()) {
    // Terminate http connection, if it opened before! Otherwise method not run correctly.
    gsm.print(F("AT+HTTPTERM\r"));
    _readSerial();

    gsm.print(F("AT+HTTPINIT\r"));
    _readSerial();
    if (_buffer.indexOf(F("OK")) != -1) {
      gsm.print(F("AT+HTTPPARA=\"CID\",1\r"));
      _readSerial();
      if (_buffer.indexOf(F("OK")) != -1) {
        gsm.print(F("AT+HTTPPARA=\"URL\",\""));
        return "OK";
      } else {
        return "HTTP_PARAMETER_ERROR";
      }
    } else {
      return "HTTP_INIT_ERROR";
    }
  } else {
    return "GPRS_NOT_CONNECTED";
  }
}


String GSMSimHTTP::get_manual_end() {
  gsm.print("\"\r");
  _readSerial();

  if (_buffer.indexOf(F("OK")) != -1) {
    gsm.print(F("AT+HTTPACTION=0\r"));
    _readSerial();
    if (_buffer.indexOf(F("OK")) != -1) {
      _readSerial(10000);
      if (_buffer.indexOf(F("+HTTPACTION: 0,")) != -1) {
        String kod = _buffer.substring(_buffer.indexOf(F(",")) + 1, _buffer.lastIndexOf(F(",")));
        String uzunluk = _buffer.substring(_buffer.lastIndexOf(F(",")) + 1);

        String sonuc = "METHOD:GET|HTTPCODE:";
        sonuc += kod;
        sonuc += "|LENGTH:";
        sonuc += uzunluk;

        // Bağlantıyı kapat!
        gsm.print(F("AT+HTTPTERM\r"));
        _readSerial();

        sonuc.trim();

        return sonuc;
      } else {
        return "HTTP_ACTION_READ_ERROR";
      }
    } else {
      return "HTTP_ACTION_ERROR";
    }
  } else {
    return "HTTP_PARAMETER_ERROR";
  }
}


String GSMSimHTTP::post_manual_begin(String url, String contentType) {
  // önce internet bağlı olsun...
  if (isConnected()) {
    // Terminate http connection, if it opened before!
    gsm.print(F("AT+HTTPTERM\r"));
    _readSerial();

    // şimdi başlıyoruz...
    gsm.print(F("AT+HTTPINIT\r\n"));
    _readSerial();
    if (_buffer.indexOf(F("OK")) != -1) {
      // init tamam şimdi parametreleri girelim...
      gsm.print(F("AT+HTTPPARA=\"CID\",1\r"));
      _readSerial();
      if (_buffer.indexOf(F("OK")) != -1) {
        gsm.print(F("AT+HTTPPARA=\"URL\",\""));
        gsm.print(url);
        gsm.print(F("\"\r"));
        _readSerial();
        if (_buffer.indexOf(F("OK")) != -1) {
          gsm.print(F("AT+HTTPPARA=\"CONTENT\",\""));
          gsm.print(contentType);
          gsm.print(F("\"\r"));
          _readSerial();
          if (_buffer.indexOf(F("OK")) != -1) {
            // download modunu açalım
            // veri uzunluğunu bulalım
            unsigned int length = data.length();
            gsm.print(F("AT+HTTPDATA="));
            gsm.print(length);
            gsm.print(F(","));
            gsm.print(30000);
            gsm.print("\r");
            _readSerial(30000);
            if (_buffer.indexOf(F("DOWNLOAD")) != -1) {
            } else {
              return "ERROR:HTTP_DATA_ERROR";
            }
          } else {
            return "ERROR:HTTP_PARAMETER_ERROR";
          }
        } else {
          return "ERROR:HTTP_PARAMETER_ERROR";
        }
      } else {
        return "ERROR:HTTP_PARAMETER_ERROR";
      }
    } else {
      return "ERROR:HTTP_INIT_ERROR";
    }
  } else {
    return "ERROR:GPRS_NOT_CONNECTED";
  }
}


String GSMSimHTTP::post_manual_end() {
  gsm.print(F("\r"));
  _readSerial(30000);
  if (_buffer.indexOf(F("OK")) != -1) {
    gsm.print(F("AT+HTTPACTION=1\r"));
    _readSerial();
    if (_buffer.indexOf(F("OK")) != -1) {
      _readSerial(10000);
      if (_buffer.indexOf(F("+HTTPACTION: 1,")) != -1) {
        String kod = _buffer.substring(_buffer.indexOf(F(",")) + 1, _buffer.lastIndexOf(F(",")));
        String uzunluk = _buffer.substring(_buffer.lastIndexOf(F(",")) + 1);

        String sonuc = "METHOD:POST|HTTPCODE:";
        sonuc += kod;
        sonuc += "|LENGTH:";
        sonuc += uzunluk;

        // Bağlantıyı kapat!
        gsm.print(F("AT+HTTPTERM\r"));
        _readSerial();

        sonuc.trim();

        return sonuc;
      } else {
        return "HTTP_ACTION_READ_ERROR";
      }
    } else {
      return "ERROR:HTTP_ACTION_ERROR";
    }
  } else {
    return "ERROR:HTTP_DATADOWNLOAD_ERROR";
  }
}
