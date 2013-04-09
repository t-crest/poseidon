/* 
 * File:   IOutput.h
 * Author: Rasmus Bo Soerensen
 *
 * Created on 6. august 2012, 11:07
 */

#ifndef IOUTPUT_H
#define	IOUTPUT_H

#include "network_t.hpp"

/** The interface of the schedule output module. */
class IOutput
{
public:
	/**
	 * A module implementing this interface should provide an output_schedule function
     * @param n The network data structure containing the generated schedule.
     * @return Returns true if the schedule is outputted.
     */
    virtual bool output_schedule(const network_t& n) =0;
};

#endif	/* IOUTPUT_H */

