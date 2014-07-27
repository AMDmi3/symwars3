//
//  connection.h
//  corpstates
//
//  Created by polyf  on 2013-05-30.
//  Copyright (c) 2013 DMD 'Ware. All rights reserved.
//

#ifndef CONNECTION_H
#define CONNECTION_H

#define CONNECTION_NOCONNECTION			0
#define CONNECTION_NORTH				1
#define CONNECTION_EAST					2
#define CONNECTION_SOUTH				3
#define CONNECTION_WEST					4
#define CONNECTION_NORTHEAST			5
#define CONNECTION_NORTHSOUTH			6
#define CONNECTION_EASTSOUTH			7
#define CONNECTION_NORTHWEST			8
#define CONNECTION_EASTWEST				9
#define CONNECTION_SOUTHWEST			10
#define CONNECTION_EASTSOUTHWEST		11
#define CONNECTION_NORTHSOUTHWEST		12
#define CONNECTION_NORTHEASTWEST		13
#define CONNECTION_NORTHEASTSOUTH		14
#define CONNECTION_NORTHEASTSOUTHWEST	15
#define CONNECTION_TYPES				16

int ConnType(bool n, bool e, bool s, bool w);

#define CONSTRUCTION	0
#define FINISHED		1

#endif

