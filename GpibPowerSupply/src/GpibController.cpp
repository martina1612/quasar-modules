/*
 * GpibController.cpp
 *
 *  Created on: Apr 30, 2020
 *      Author: ressegotti
 */

#include "GpibController.h"

namespace GpibPowerSupply {

GpibController::GpibController() {
	// TODO Auto-generated constructor stub

}

GpibController::~GpibController() {
	// TODO Auto-generated destructor stub
}

int GpibController::initialize (int board, int pad, int sad, int timeout,
							int send_eoi, int eos) {
//int GpibController::initialize (int board, int pad, int sad=NO_SAD, int timeout=T10s,
//								int send_eoi=0, int eos=1) {
	m_board = board;
	m_pad = pad;
	m_sad = sad;
	m_timeout = timeout;
	m_send_eoi = send_eoi;
	m_eos = eos;
	m_dev = ibdev(board, pad, sad, timeout, send_eoi, eos);
	
	std::string cmd("*IDN?\n");
	GpibController::sendCmdRetval(cmd, m_idn);

	//if (m_dev>0) { 	//use *IDN? to check communication, as m_dev>0 in all cases
	if (m_idn != "") {
	//	std::string cmd("*IDN?\n");
	//	GpibController::sendCmdRetval(cmd, m_idn);
		LOG(Log::INF) << "Device " << m_idn << " initialized." ;
	}
	else {
		LOG(Log::INF) << "Error: device initialization not valid." ;
		m_dev = -1; //m_dev is assigned a value >0 even if no device is found. Settings it to -1 manually.
	        m_idn = "EMPTY";
	}
	

    return m_dev;
}

int GpibController::sendCmd(std::string cmd) {
	int stat = -1;

	if (m_dev <= 0) {
	//	LOG(Log::INF) << "GPIB device not valid. Command not sent." ;
		return stat;
	}

	std::string command = cmd+"\n";
	//std::lock_guard<std::mutex> lock (m_mutex);
	//LOG(Log::INF) << "Sending command : "<< command << "\nto device " << m_dev ;
	stat = ibwrt(m_dev, (void*)command.c_str(), command.size());
	return stat;
}

int GpibController::read(std::string &retval) {
	int stat = -1;

	if (m_dev <= 0) {
	//	LOG(Log::INF) << "GPIB device not valid. Command not sent." ;
		return stat;
	}

	char buf[1000];
	int len = 1000;
	//std::lock_guard<std::mutex> lock (m_mutex);
	stat= ibrd(m_dev, buf, len);
	buf[ibcnt-1] = 0;
	retval = buf;

	if ( stat != -1 )	m_returnValue = buf;
	else	m_returnValue = "";

	return stat;
}


int GpibController::sendCommand(std::string cmd) {
	int stat = -1;

	std::lock_guard<std::mutex> lock (m_mutex);
	stat = GpibController::sendCmd(cmd);
	return stat;
}

int GpibController::sendCmdRetval(std::string cmd, std::string &retval) {
	int stat = -1;

	std::lock_guard<std::mutex> lock (m_mutex);
	int statWrite = GpibController::sendCmd(cmd);
	int statRead = GpibController::read(retval);
	stat = statWrite+statRead;

	return stat;
}

int GpibController::sendCmdFloatretval(std::string cmd, float &retval) {
	int stat = -1;

	std::lock_guard<std::mutex> lock (m_mutex);
	int statWrite = GpibController::sendCmd(cmd);
	std::string strRetval = "";
	int statRead = GpibController::read(strRetval);
	retval = -1;
	std::stringstream ss;
	ss << strRetval;
	ss >> retval;

    stat = statWrite+statRead;

	return stat;
}

int GpibController::sendCmdBoolretval(std::string cmd, bool &retval) {
	int stat = -1;

	std::lock_guard<std::mutex> lock (m_mutex);
	int statWrite = GpibController::sendCmd(cmd);
	std::string strRetval = "";
	int statRead = GpibController::read(strRetval);
	
	if (strRetval.length() > 0 )
		{
		if 	(strRetval.at(0)=='1')	retval = 1;
		else if	(strRetval.at(0)=='0')	retval = 0;
		}
	else	LOG(Log::INF) << "Cannot convert device answer into boolean or no answer received." ;

    stat = statWrite+statRead;

	return stat;
}

int GpibController::sendCmdStrinval(std::string cmd, std::string inval) {
	int stat = -1;

	std::lock_guard<std::mutex> lock (m_mutex);
	std::string command = cmd + " " + inval;
	stat = GpibController::sendCmd(command);

	return stat;
}

int GpibController::sendCmdFloatinval(std::string cmd, float inval) {
	int stat = -1;

	std::lock_guard<std::mutex> lock (m_mutex);
	std::ostringstream ss;
	ss << inval;
	std::string s(ss.str());
	std::string command = cmd+" "+s;

	stat = GpibController::sendCmd(command);

	return stat;
}

int GpibController::sendCmdBoolinval(std::string cmd, bool inval) {
	int stat = -1;

	std::lock_guard<std::mutex> lock (m_mutex);
	std::string s="OFF";
	if (inval)	s="ON";

	std::string command = cmd+" "+s;

	stat = GpibController::sendCmd(command);

	return stat;
}



// applying new settings
int GpibController::setVoltage(float val) {
	return GpibController::sendCmdFloatinval("VOLT", val);
}
int GpibController::setCurrent(float val) {
	return GpibController::sendCmdFloatinval("CURR", val);
}
int GpibController::setOnoff(bool onoff) {
	return GpibController::sendCmdBoolinval("OUTP", onoff);
}

int GpibController::setVrange(std::string lowhigh) {
	return GpibController::sendCmdStrinval("VOLT:RANG", lowhigh);
}
int GpibController::setVprotection(bool onoff) {
	return GpibController::sendCmdBoolinval("VOLT:PROT:state", onoff);
}

// getting device settings
int GpibController::readVoltage(float &Vset) {
	return GpibController::sendCmdFloatretval("VOLT?", Vset);
}
int GpibController::readCurrent(float &Iset) {
	return GpibController::sendCmdFloatretval("CURR?", Iset);
}
int GpibController::readOnoff(bool &onoff) {
	return GpibController::sendCmdBoolretval("OUTP?", onoff);
}
int GpibController::readVrange(std::string &vRange) {
	return GpibController::sendCmdRetval("VOLT:RANG?", vRange);
}
int GpibController::readVprotection(bool &vProt) {
	return GpibController::sendCmdBoolretval("VOLT:PROT:state?", vProt);
}
int GpibController::readImax(float &iMax) {
	return GpibController::sendCmdFloatretval("CURR? max", iMax);
}
int GpibController::readImin(float &iMin) {
	return GpibController::sendCmdFloatretval("CURR? min", iMin);
}
int GpibController::readVmax(float &vMax) {
	return GpibController::sendCmdFloatretval("VOLT? max", vMax);
}
int GpibController::readVmin(float &vMin) {
	return GpibController::sendCmdFloatretval("VOLT? min", vMin);
}

// actual values
int GpibController::actualIson(bool &isOn) {
	return GpibController::sendCmdBoolretval("OUTP?", isOn);
}
int GpibController::actualTrip(bool &isTrip) {
	return GpibController::sendCmdBoolretval("VOLT:PROT:TRIP?", isTrip);
}
int GpibController::actualCurrent(float &iMon) {
	return GpibController::sendCmdFloatretval("MEAS:CURR?", iMon);
}
int GpibController::actualVoltage(float &vMon) {
	return GpibController::sendCmdFloatretval("MEAS:VOLT?", vMon);
}

// commands
int GpibController::sendReset() {
	return GpibController::sendCommand("*RST");
}

int GpibController::sendClear() {
	return GpibController::sendCommand("*CLS");
}

int GpibController::clearTrip() {
	return GpibController::sendCommand("VOLT:PROT:clear");
}

// values on request
int GpibController::askError(std::string& error) {
	return GpibController::sendCmdRetval("SYST:ERR?", error);
}
} /* namespace GpibPowerSupply */
