// APN data for Netzclub
const char* GPRS_APN = "pinternet.interkom.de";  // replace your GPRS APN
const char* GPRS_LOGIN = "";                     // replace with your GPRS login
const char* GPRS_PASSWORD = "";                  // replace with your GPRS password

//WU Credentials
#define UPLOAD_WU
const char serverWU[] = "weatherstation.wunderground.com";
const char pathWU[] = "/weatherstation/updateweatherstation.php";
const char WU_ID[] = "";
const char WU_PASS[] = "";

//OWM credentials
#define UPLOAD_OWM
const String idOWM = "";
const String keyOWM = "";

//Stats Server
#define UPLOAD_STATS
const String statURL = "";

//SMS Stuff
const char* sos_number = "";
