/* 
 * File:   IOutput.h
 * Author: Rasmus Bo Soerensen
 *
 * Created on 6. august 2012, 11:07
 */

#ifndef IOUTPUT_H
#define	IOUTPUT_H

#include "schedule.hpp"

class IOutput
{
public:
    virtual bool output_schedule(const network_t& n) =0;
};

#endif	/* IOUTPUT_H */

