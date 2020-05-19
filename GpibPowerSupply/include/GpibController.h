/*
 * GpibController.h
 *
 *  Created on: Apr 30, 2020
 *      Author: ressegotti
 */

#include <gpib/ib.h>
#include <string>
#include <sstream>
#include <iostream>
#include <LogIt.h>
//#include <mutex>


#ifndef GPIBPOWERSUPPLY_GPIBCONTROLLER_H_
#define GPIBPOWERSUPPLY_GPIBCONTROLLER_H_

namespace GpibPowerSupply {

class GpibController {
public:
	GpibController();
	virtual ~GpibController();

public:
	// to open the device
	int m_board = -1;
	int m_pad = -1;
	int m_sad = NO_SAD;
	int m_timeout = T10s;
	int m_send_eoi = 0;
	int m_eos = 1;
	int m_dev = -1;
	int initialize (int, int, int =NO_SAD, int =T10s,
			int =0, int =1);
	std::string m_idn = "";

	// applying new settings
	int setVoltage(float);
	int setCurrent(float);
	int setOnoff(bool);
	int setVrange(std::string);
	int setVprotection(bool);

	// getting device settings
	int readVoltage(float&);
	int readCurrent(float&);
	int readOnoff(bool&);
	int readVrange(std::string&);
	int readVprotection(bool&);
	int readImax(float&);
	int readImin(float&);
	int readVmax(float&);
	int readVmin(float&);

	// actual values
	int actualIson(bool&);
	int actualTrip(bool&);
	int actualCurrent(float&);
	int actualVoltage(float&);

	// commands
	int sendReset();
	int sendClear();
	int clearTrip();

	// values "on request"
	int askError(std::string&);


	//

private:
	//mutex
	std::mutex m_mutex;

	//auxiliary functions
	std::string m_returnValue = "";
	int sendCmd(std::string);
	int read(std::string&);
	int sendCommand(std::string);
	int sendCmdRetval(std::string, std::string&);
	int sendCmdFloatretval(std::string, float&);
	int sendCmdBoolretval(std::string, bool&);
	int sendCmdStrinval(std::string, std::string);
	int sendCmdFloatinval(std::string, float);
	int sendCmdBoolinval(std::string, bool);


};


} /* namespace GpibPowerSupply */

#endif /* GPIBPOWERSUPPLY_GPIBCONTROLLER_H_ */
