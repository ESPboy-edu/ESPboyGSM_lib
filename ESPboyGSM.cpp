/*
	ESPboyGSM Library
	
	This library written for ESPboy GSM phone shield ESPboy project by RomanS
    ESPboy.edu@gmail.com

	based on ideas of GSMSim Library by Erdem ARSLAN and SIM800 documentation.
	erdemsaid@gmail.com
*/


#include "Arduino.h"
#include "ESPboyGSM.h"
#include <SoftwareSerial.h>


ESPboyGSM::ESPboyGSM(uint8_t rx, uint8_t tx) : SoftwareSerial(rx, tx){
	RX_PIN = rx;
	TX_PIN = tx;
}



// Start GSM
bool ESPboyGSM::init(uint32_t baud) {
	_baud = baud;
	this->begin(_baud);
	_buffer.reserve(BUFFER_RESERVE_MEMORY);
	this->print(F("AT\r"));
	_buffer = _readSerial();
		if( (_buffer.indexOf("OK") ) != -1)  {
			return true;
		}
		else {
			return false;
		}
}


//SIM800 error answer: true=numerical, false=text
bool ESPboyGSM::setSIManswerStyle(bool type = false) {
        if (type == true){
			this->print(F("ATV0\r"));
		}
		else{
			this->print(F("ATV1\r"));
		}
		_buffer = _readSerial();
		if( (_buffer.indexOf("OK") ) != -1)  {
			return true;
		}
		else {
			return false;
		}
}


//SIM800 URC mode true-on, false-off
bool ESPboyGSM::setURC(bool type = true) {
		if (type = true){
			this->print(F("AT+CLIP=1\r"));
		}
		else{
			this->print(F("AT+CLIP=0\r"));
		}
		_buffer = _readSerial();
		if( (_buffer.indexOf("OK") ) != -1)  {
			return true;
		}
		else {
			return false;
		}
}


//Error report level 0="ERROR", 1=error code, 2=error text description
bool ESPboyGSM::setErrorReport(uint8_t level = 1) {
	if(level != 0 || level != 1 || level != 1) {
		return false;
	}
	else {
		this->print(F("AT+CMEE="));
		this->print(String(level));
		this->print(F("\r"));
		_buffer = _readSerial();
		if( (_buffer.indexOf("OK") ) != -1)  {
			return true;
		}
		else {
			return false;
		}
	}
}


// ECHO true=on, false=off
bool ESPboyGSM::setEcho(bool type = false) {
    if (type = true){
    	this->print(F("ATE1\r"));
    }
    else{
		this->print(F("ATE0\r"));
	}
	_buffer = _readSerial();
	if ( (_buffer.indexOf("OK") )!=-1 ) {
   		return true;
   	}
   	else {
   		return false;
   	}
}


// set phone functionality 0=min, 1=full, 4-...
bool ESPboyGSM::setPhoneFunc(uint8_t level = 1) {
	if(level != 0 || level != 1 || level != 4) {
		return false;
	}
	else {
		this->print(F("AT+CFUN="));
		this->print(String(level));
		this->print(F("\r"));
		_buffer = _readSerial();
		if( (_buffer.indexOf("OK") ) != -1)  {
			return true;
		}
		else {
			return false;
		}
	}
}


//save param
bool ESPboyGSM::saveParam () {
	this->print(F("AT&W\r"));
	_buffer = _readSerial();
	if( (_buffer.indexOf("OK") ) != -1)  {
		return true;
	}
	else {
		return false;
	}
}


//get ringer volume [0-100]
uint8_t ESPboyGSM::ringerVolume() {
	this->print(F("AT+CRSL?\r"));
	_buffer = _readSerial();
	String veri = _buffer.substring(7, _buffer.indexOf("OK"));
	veri.trim();
	return veri.toInt();
}


//set ringer volume [0-100]
bool ESPboyGSM::setRingerVolume(uint8_t level = 100) {
	if(level > 100) {
		level = 100;
	}
	this->print(F("AT+CRSL="));
	this->print(level);
	this->print(F("\r"));
	_buffer = _readSerial();
	if(_buffer.indexOf("OK") != -1) {
		return true;
	} else {
		return false;
	}
}


//get speaker volume [0-100]
uint8_t ESPboyGSM::speakerVolume() {
	this->print(F("AT+CLVL?\r"));
	_buffer = _readSerial();
	String veri = _buffer.substring(7, _buffer.indexOf("OK"));
	veri.trim();
	return veri.toInt();
}


//set speaker volume [0-100]
bool ESPboyGSM::setSpeakerVolume(uint8_t level = 100) {
	if(level > 100) {
		level = 100;
	}
	this->print(F("AT+CLVL="));
	this->print(level);
	this->print(F("\r"));
	_buffer = _readSerial();
	if (_buffer.indexOf("OK") != -1) {
		return true;
	}
	else {
		return false;
	}
}


//auto answer 0=no, n=after "n" number of rings
bool ESPboyGSM::setAutoAnswer (uint8_t level = 0) {
	this->print(F("ATS0="));
	this->print(String(level));
	this->print(F("\r"));
	_buffer = _readSerial();
	if( (_buffer.indexOf("OK") ) != -1)  {
		return true;
	}
	else {
		return false;
	}
}


// signal quality - 0-31 / 99=Unknown
uint8_t ESPboyGSM::signalQuality() {
	this->print(F("AT+CSQ\r"));
	_buffer = _readSerial();
	if((_buffer.indexOf("+CSQ:")) != -1) {
		return _buffer.substring(_buffer.indexOf("+CSQ: ")+6, _buffer.indexOf(",")).toInt();
	} else {
		return 99;
	}
}


// is module connected to the operator?
bool ESPboyGSM::isRegistered() {
	this->print(F("AT+CREG?\r"));
	_buffer = _readSerial();
	if( (_buffer.indexOf("+CREG: 0,1")) != -1 || (_buffer.indexOf("+CREG: 0,5")) != -1 || (_buffer.indexOf("+CREG: 1,1")) != -1 || (_buffer.indexOf("+CREG: 1,5")) != -1) {
		return true;
	} else {
		return false;
	}
}


// is SIM inserted?
bool ESPboyGSM::isSimInserted() {
	this->print(F("AT+CSMINS?\r"));
	_buffer = _readSerial();
	if(_buffer.indexOf(",") != -1) {
		String veri = _buffer.substring(_buffer.indexOf(","), _buffer.indexOf("OK"));
		veri.trim();
		if(veri == "1") {
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}


// operator name
String ESPboyGSM::operatorName() {
	this->print(F("AT+COPS?\r"));
	_buffer = _readSerial();
	if(_buffer.indexOf(",") == -1) {
		return "NOT CONNECTED";
	}
	else {
		 return _buffer.substring(_buffer.indexOf(",\"")+2, _buffer.lastIndexOf("\""));
	}
}

// operator name from SIM
String ESPboyGSM::operatorNameFromSim() {
	this->flush();
	this->print(F("AT+CSPN?\r"));
	_buffer = _readSerial();
	yield(250);
	_buffer = _readSerial();
	if(_buffer.indexOf("OK") != -1) {
		return _buffer.substring(_buffer.indexOf(" \"") + 2, _buffer.lastIndexOf("\""));
	}
	else {
		return "NOT CONNECTED";
	}

}


/*phone status
0 Ready (MT allows commands from TA/TE)
2 Unknown (MT is not guaranteed to respond to instructions)
3 Ringing (MT is ready for commands from TA/TE, but the ringer is active)
4 Call in progress (MT is ready for commands from TA/TE, a call is in progress)
*/
uint8_t ESPboyGSM::phoneStatus() {
	this->print(F("AT+CPAS\r"));
	_buffer = _readSerial();
	if((_buffer.indexOf("+CPAS: ")) != -1)
	{
		return _buffer.substring(_buffer.indexOf("+CPAS: ")+7,_buffer.indexOf("+CPAS: ")+9).toInt();
	}
	else {
		return 99; // not read from module
	}
}


//manufacturer identification
String ESPboyGSM::moduleManufacturer() {
	this->print(F("AT+CGMI\r"));
	_buffer = _readSerial();
	String veri = _buffer.substring(8, _buffer.indexOf("OK"));
	veri.trim();
	veri.replace("_", " ");
	return veri;
}


//module identification
String ESPboyGSM::moduleModel() {
	this->print(F("AT+CGMM\r"));
	_buffer = _readSerial();
	String veri = _buffer.substring(8, _buffer.indexOf("OK"));
	veri.trim();
	veri.replace("_", " ");
	return veri;
}


//revision identification of software release
String ESPboyGSM::moduleRevision() {
	this->print(F("AT+CGMR\r"));
	_buffer = _readSerial();
	String veri = _buffer.substring(_buffer.indexOf(":")+1 , _buffer.indexOf("OK"));
	veri.trim();
	return veri;
}


//module IMEI
String ESPboyGSM::moduleIMEI() {
	this->print(F("AT+CGSN\r"));
	_buffer = _readSerial();
	String veri = _buffer.substring(8, _buffer.indexOf("OK"));
	veri.trim();
	return veri;
}



//international mobile subscriber identity
String ESPboyGSM::moduleIMSI() {
	this->print(F("AT+CIMI\r"));
	_buffer = _readSerial();
	String veri = _buffer.substring(8, _buffer.indexOf("OK"));
	veri.trim();
	return veri;
}


//ICCID
String ESPboyGSM::moduleICCID() {
	this->print(F("AT+CCID\r"));
	_buffer = _readSerial();
	String veri = _buffer.substring(8, _buffer.indexOf("OK"));
	veri.trim();
	return veri;
}


String ESPboyGSM::moduleDebug() {
	this->print(F("AT&V\r"));
	return _readSerial();
}


//////////
// CALL //
//////////

// call
// 0 - failed / 1 - ATD OK no COLP / 2 - BUSY / 3 - NO DIAL TONE / 4 - NO CARRIER / 5 - OK answered
uint8_t ESPboyGSM::call(char* phone_number) {
	bool colp = callIsCOLPActive();
	_buffer = _readSerial();
	yield(100);
	this->print(F("ATD"));
	this->print(phone_number);
	this->print(";\r");
	_buffer = _readSerial();
	if (colp) {
		if (_buffer.indexOf("BUSY") != -1){
			return 2;
		}
		if (_buffer.indexOf("NO DIAL TONE") != -1){
			return 3;
		}
		if (_buffer.indexOf("NO CARRIER") != -1){
			return 4;
		}
		if (_buffer.indexOf("OK") != -1){
			return 5;
		}	
	}
	else {
		if (_buffer.indexOf("OK") != -1){
			return 1;
		}
		else {
			return 0;
		}
	}
}


// answer
bool ESPboyGSM::callAnswer() {
	this->print(F("ATA\r"));
	_buffer = _readSerial();
	if (_buffer.indexOf("OK") != -1){
		return true;
	}
	else {
		false;
	}
}


// hang off
bool ESPboyGSM::callHangoff() {
	this->print(F("ATH\r"));
	_buffer = _readSerial();
	if(_buffer.indexOf("OK") != -1){
		return true;
	} 
	else {
		false;
	}
}


/* call status
	0 - Ready (MT allows commands from TA/TE)
	2 - Unknown (MT is not guaranteed to respond to tructions)
	3 - Ringing (MT is ready for commands from TA/TE, but the ringer is active)
	4 - Call in progress */
uint8_t ESPboyGSM::callStatus() {
	this->print(F("AT+CPAS\r"));
	_buffer = _readSerial();
	return _buffer.substring(_buffer.indexOf("+CPAS: ") + 7, _buffer.indexOf("+CPAS: ") + 9).toInt();
}


// COLP=false, module returns OK just after ATD command
// COLP=true, return BUSY / NO DIAL TONE / NO CARRIER / OK - after taking call other side
bool ESPboyGSM::callSetCOLP(bool active = 1) {
	this->print(F("AT+COLP="));
	this->print((uint8_t)active);
	this->print(F("\r"));
	_buffer = _readSerial();
	if (_buffer.indexOf("OK") != -1){
		return true;
	}
	else {
		false;
	}
}


// is COLP active?
bool ESPboyGSM::callIsCOLPActive() {
	this->print(F("AT+COLP?\r"));
	_buffer = _readSerial();
	if (_buffer.indexOf("+COLP: 1") != -1) {
		return true;
	}
	else {
		false;
	}
}



/////////
// SMS //
/////////

// SMS text mode on/off
bool ESPboyGSM::smsTextMode(bool textModeON = true) {
	if (textModeON == true) {
		this->print(F("AT+CMGF=1\r"));
	}
	else {
		this->print(F("AT+CMGF=0\r"));
	}
	_buffer = _readSerial();
	if (_buffer.indexOf("OK") != -1) {
		return true;
	}
	else{
		return false;
	}
}



// send SMS
bool ESPboyGSM::smsSend(char* number, char* message) {
	this->print(F("AT+CMGS=\"")); 
	this->print(number);
	this->print(F("\"\r"));
	_buffer = _readSerial();
	this->print(message);
	this->print(F("\r"));
	_buffer += _readSerial();
	this->print((char)26);
	_buffer += _readSerial();
	if (((_buffer.indexOf("+CMGS:")) != -1)) {
		if (((_buffer.indexOf("OK")) != -1)){
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}


// return unread SMS
String ESPboyGSM::smsListUnread() {
	this->print(F("AT+CMGL=\"REC UNREAD\",1\r"));
	_buffer = _readSerial();
	String donus = "";
	if (_buffer.indexOf("ERROR") != -1) {
		donus = "ERROR";
	}
	if (_buffer.indexOf("+CMGL:") != -1) {
		String veri = _buffer;
		bool islem = false;
		donus = "";
		while (!islem) {
			if (veri.indexOf("+CMGL:") == -1) {
				islem = true;
				continue;
			}
			veri = veri.substring(veri.indexOf("+CMGL: ") + 7);
			String metin = veri.substring(0, veri.indexOf(","));
			metin.trim();
			if (donus == "") {
				donus += "SMSIndexNo:";
				donus += metin;
			}
			else {
				donus += ",";
				donus += metin;
			}
		}
	}
	else {
		if (donus != "ERROR") {
			donus = "NO_SMS";
		}
	}
	return donus;
}


// return SMS no = index
String ESPboyGSM::smsRead(uint8_t index, bool markRead) {
	this->print(F("AT+CMGR="));
	this->print(index);
	this->print(",");
	if (markRead == true) {
		this->print("0");
	}
	else {
		this->print("1");
	}
	this->print(F("\r"));
	_buffer = _readSerial();
	String durum = "INDEX_NO_ERROR";
	if (_buffer.indexOf("+CMGR:") != -1) {
		String klasor, okundumu, telno, zaman, mesaj;
		klasor = "UNKNOWN";
		okundumu = "UNKNOWN";
		if (_buffer.indexOf("REC UNREAD") != -1) {
			klasor = "INCOMING";
			okundumu = "UNREAD";
		}
		if (_buffer.indexOf("REC READ") != -1) {
			klasor = "INCOMING";
			okundumu = "READ";
		}
		if (_buffer.indexOf("STO UNSENT") != -1) {
			klasor = "OUTGOING";
			okundumu = "UNSENT";
		}
		if (_buffer.indexOf("STO SENT") != -1) {
			klasor = "OUTGOING";
			okundumu = "SENT";
		}
		String telno_bol1 = _buffer.substring(_buffer.indexOf("\",\"") + 3);
		telno = telno_bol1.substring(0, telno_bol1.indexOf("\",\""));
		String tarih_bol = telno_bol1.substring(telno_bol1.lastIndexOf("\",\"") + 3);
		zaman = tarih_bol.substring(0, tarih_bol.indexOf("\"")); 
		mesaj = tarih_bol.substring(tarih_bol.indexOf("\"")+1, tarih_bol.lastIndexOf("OK"));
		mesaj.trim();
		durum = "FOLDER:";
		durum += klasor;
		durum += "|STATUS:";
		durum += okundumu;
		durum += "|PHONENO:";
		durum += telno;
		durum += "|DATETIME:";
		durum += zaman;
		durum += "|MESSAGE:";
		durum += mesaj;
	}
	return durum;
}



// delete SMS no = index
bool ESPboyGSM::smsDeleteOne(uint8_t index) {
	this->print(F("AT+CMGD="));
	this->print(index);
	this->print(F(",0\r"));
	_buffer = _readSerial();
	if (_buffer.indexOf("OK") != -1) {
		return true;
	}
	else {
		return false;
	}
}


// delete all SMS been read
bool ESPboyGSM::smsDeleteAllRead() {
	this->print(F("AT+CMGD=1,1\r"));
	_buffer = _readSerial();
	if (_buffer.indexOf("OK") != -1) {
		return true;
	}
	else {
		return false;
	}
}



// delete all SMS
bool ESPboyGSM::smsDeleteAll() {
	this->print(F("AT+CMGD=1,4\r"));
	_buffer = _readSerial();
	if (_buffer.indexOf("OK") != -1) {
		return true;
	}
	else {
		return false;
	}
}



///////////////////
//	USSD SECTION //
///////////////////

bool ESPboyGSM::ussdSend(char* code) {
	this->print(F("AT+CUSD=1,\""));
	this->print(code);
	this->print(F("\"\r"));
	_buffer = _readSerial();
	if (_buffer.indexOf("OK") != -1) {
		return true;
	}
	else {
		return false;
	}
}


String ESPboyGSM::ussdRead(String serialRaw) {
	if (serialRaw.indexOf("+CUSD:") != -1) {
		String metin = serialRaw.substring(serialRaw.indexOf(",\"") + 2, serialRaw.indexOf("\","));
		return metin;
	}
	else {
		return "NOT_USSD_RAW";
	}
}



////////////////////
//	TIME SECTION  //
////////////////////

bool ESPboyGSM::setTimeFromOperator (bool optime = true){}
		if (optime = true){
			this->print(F("AT+CLTS=1\r"));
		}
		else{
			this->print(F("AT+CLTS=0\r"));
		}
		_buffer = _readSerial();
		if( (_buffer.indexOf("OK") ) != -1)  {
			return true;
		}
		else {
			return false;
		}
}



bool ESPboyGSM::timeGet(uint8_t *day, uint8_t *month, uint16_t *year, uint8_t *hour, uint8_t *minute, uint8_t *second) {
	this->print(F("AT+CCLK?\r"));
	_buffer = _readSerial();
	if (_buffer.indexOf("OK") != -1) {
		_buffer = _buffer.substring(_buffer.indexOf("\"") + 1, _buffer.lastIndexOf("\"") - 1);
		*year = (_buffer.substring(0, 2).toInt()) + 2000;
		*month = _buffer.substring(3, 5).toInt();
		*day = _buffer.substring(6, 8).toInt();
		*hour = _buffer.substring(9, 11).toInt();
		*minute = _buffer.substring(12, 14).toInt();
		*second = _buffer.substring(15, 17).toInt();
		return true;
	}
	else {
		return false}
}



/////////////
// PRIVATE //
/////////////

String ESPboyGSM::_readSerial() {
	uint64_t timeOld = millis();
	while (!this->available() && !(millis() > timeOld + TIMEOUT_READ_SERIAL)){
		yield(10);
	}
	String str = "";
	if (this->available()){
		str = this->readString();
	}
	str.trim();
	return str;
}


