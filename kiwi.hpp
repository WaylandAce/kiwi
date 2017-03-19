//
//  kiwi.hpp
//  kiwi
//
//  Created by pavelk on 19.03.17.
//  Copyright © 2017 pavelk. All rights reserved.
//

#ifndef kiwi_hpp

#define kiwi_hpp
#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>

#ifndef O_DIRECT
#define O_DIRECT 0
#endif

#ifndef O_LARGEFILE
#define O_LARGEFILE 0
#endif

typedef struct {
	unsigned char sector1;
	unsigned char sector2;
	unsigned char sector3;
	
	unsigned char logicalSector : 6;
	unsigned char storageLayer : 1;
	unsigned char diskSide : 1;
	
	unsigned int getSector() const {
		return sector1 * 65536 + sector2 * 256 + sector3;
	}
	
	void print() const;
	
	bool isNull() const {
		return *(unsigned int *)(this) == 0xFFFFFFFF;
	}
} SectorAddr;

class SectorSource {
public:
	virtual const unsigned char *getSector(SectorAddr &addr) = 0;
};

unsigned short short2host(unsigned short);
unsigned int int2host(unsigned int);

# ifdef _MSC_VER
#	pragma pack (push, 1)
# endif

typedef struct {
	unsigned char numSec1: 7;
	unsigned char sign: 1;
	
	unsigned char numSec2;
	unsigned char numFrac: 3;
	unsigned char numSec3: 5;
	
	double getNum() const {
		return (sign ? (-1.0) : 1.0) * (numSec1 * 256 * 32 + numSec2 * 32 + numSec3 + numFrac / 8.0) / 3600.0;
	}
	
	unsigned int getSecs() const {
		return numSec1 * 256 * 32 + numSec2 * 32 + numSec3;
	}
	
	void print(bool lat) const;
} GeoNum;

typedef struct {
	GeoNum lat;
	unsigned char latReserved;
	
	unsigned int getLat() const {
		return lat.getSecs();
	}
	
	GeoNum lon;
	unsigned char lonReserved;
	
	unsigned int getLon() const {
		return lon.getSecs();
	}
	
	void print() const;
} PID;

typedef struct {
	PID llPoint;
	PID urPoint;
	
	void print() const {
		llPoint.print();
		urPoint.print();
	}
} Rect;

typedef struct {
	PID location;
	unsigned char floor;
	unsigned char reserved;
	unsigned short regDays;
	
	unsigned int getRegDays() const {
		return short2host(regDays);
	}
	
	void print() const;
} MID;

typedef struct {
	MID systemSpecific;
	char systemSpecificName[52];
	MID dataAuthor;
	char authorName[52];
	MID system;
	char systemName[20];
	char formatVersion[64];
	char dataVersion[64];
	char title[128];
	
	unsigned char res1 : 5;
	unsigned char hasIndex : 1;
	unsigned char hasRouting : 1;
	unsigned char hasMainMap : 1;
	unsigned char res2[7];
	
	char version[32];
	Rect cover;
	
	unsigned short logicalSecSize;
	//  unsigned char logicalSecSize1;
	//  unsigned char logicalSecSize2;
	
	unsigned short secSize;
	
	unsigned char res3 : 6;
	unsigned char inMapBackSee : 1;
	unsigned char outMapBackSee : 1;
	unsigned char res4;
	
	unsigned char res5[14];
	
	unsigned char levels;
	
	unsigned int getLevels() const {
		return short2host(levels);
	}
	
	unsigned int getLogicalSecSize() const {
		return short2host(logicalSecSize);
	}
	
	unsigned int getSecSize() const {
		return short2host(secSize);
	}
	
	void print() const;
} DataVolume;

typedef struct {
	SectorAddr addr;
	
	unsigned short size;
	
	char name[12];
	
	unsigned int getSize() const {
		return short2host(size);
	}
	
	void print() const;
} ManagementRecord;

typedef ManagementRecord *ManagementTable;

// Parcel Data Management Distribution Header

typedef struct {
	unsigned short size;
	unsigned char res1[2];
	unsigned char res2;
	unsigned char res3 : 7;
	unsigned char filenameDesig : 1;
	unsigned char res4[2];
	
	GeoNum latU;
	GeoNum latL;
	GeoNum lonL;
	GeoNum lonR;
	
	unsigned short LMRsize;
	unsigned short BSMRsize;
	unsigned short BMRsize;
	
	unsigned short LMRnum;
	unsigned short BSMRnum;
	
	unsigned char n[1];
	
	unsigned short getSize() const {
		return short2host(size);
	}
	
	unsigned short getLMRsize() const {
		return short2host(LMRsize);
	}
	
	unsigned short getBSMRsize() const {
		return short2host(BSMRsize);
	}
	
	unsigned short getBMRsize() const {
		return short2host(BMRsize);
	}
	
	unsigned short getLMRnum() const {
		return short2host(LMRnum);
	}
	
	unsigned short getBSMRnum() const {
		return short2host(BSMRnum);
	}
	
	void print(SectorSource &ss) const;
} PDMDHeader;

typedef struct {
	unsigned short res1: 2;
	unsigned short level: 6;
	unsigned short highParcelsNumCode: 4;
	unsigned short lowParcelsNumCode: 4;
	
	short getLevel() const {
		if (level > 32)
			return -(level & 0x1F);
		return level;
	}
} LMHeader;

typedef struct {
	unsigned short extDFMRMap: 4;
	unsigned short basicDFMRMap: 4;
	unsigned short extDFMRRoute: 4;
	unsigned short basicDFMRRoute: 4;
} DFMRNum;

typedef struct {
	LMHeader header;
	DFMRNum nums;
	
	unsigned int displayScale1;
	unsigned int displayScale2;
	unsigned int displayScale3;
	unsigned int displayScale4;
	unsigned int displayScale5;
	
	unsigned char latBSNum; // +1
	unsigned char lonBSNum; // +1
	
	unsigned char latBNum; // +1
	unsigned char lonBNum; // +1
	
	unsigned char latPNum; // +1
	unsigned char lonPNum; // +1
	
	unsigned char latP1Num; // +1
	unsigned char lonP1Num; // +1
	
	unsigned char latP2Num; // +1
	unsigned char lonP2Num; // +1
	
	unsigned char latP3Num; // +1
	unsigned char lonP3Num; // +1
	
	unsigned short BSMROffset;
	unsigned short nodeRecordSize;
	
	// Extended data
	unsigned short roadDisplayClassNum: 4;
	unsigned short res2: 2;
	unsigned short nameDisplayClassNum: 5;
	unsigned short bgDisplayClassNum: 5;
	
	unsigned int getDisplayScale1() const {
		return int2host(displayScale1);
	}
	
	unsigned int getDisplayScale2() const {
		return int2host(displayScale2);
	}
	
	unsigned int getDisplayScale3() const {
		return int2host(displayScale3);
	}
	
	unsigned int getDisplayScale4() const {
		return int2host(displayScale4);
	}
	
	unsigned int getDisplayScale5() const {
		return int2host(displayScale5);
	}
	
	unsigned short getBSMROffset() const {
		return short2host(BSMROffset);
	};
	
	unsigned short getNodeRecordSize() const {
		return short2host(nodeRecordSize);
	};
	
	void print() const;
} LMRecord;

typedef struct {
	unsigned char res1: 2;
	unsigned char level: 6;
	unsigned char BSNum;
	
	short getLevel() const {
		if (level > 32)
			return -(level & 0x1F);
		return level;
	}
} BSMHeader;

typedef struct {
	BSMHeader header;
	unsigned int BMTOffset;
	unsigned int BMTSize;
	
	unsigned int getBMTOffset() const {
		return int2host(BMTOffset);
	}
	
	unsigned int getBMTSize() const {
		return int2host(BMTSize);
	}
	
	void print() const;
} BSMRecord;

typedef struct {
	SectorAddr PMIAddr;
	unsigned short PMISize;
	
	unsigned short getPMISize() const {
		return short2host(PMISize);
	}
	
	void print() const;
} BMRecord;

class BMRecordFile : public BMRecord {
public:
	unsigned char name[12];
	
	void print() const;
};

typedef struct {
	unsigned char res1: 6;
	unsigned char typeNum: 2;
	unsigned char PMTypeNum;
} PMType;

typedef struct {
	PMType type;
	unsigned short RGPMLOffset;
	
	unsigned short getRGPMLOffset() const {
		return short2host(RGPMLOffset);
	}
	
	void print() const;
} PMI;

typedef struct {
	SectorAddr parcelAddr;
	unsigned short parcelSize;
	
	unsigned short getParcelSize() const {
		return short2host(parcelSize);
	}
	
	void print() const;
} MainMapPMRecord0;

typedef struct {
	SectorAddr parcelAddr;
	unsigned short parselSize1;
	unsigned short parselSize2; // Basic part size for type 1 and road part size for type 2
	
	unsigned short getParselSize1() const {
		return short2host(parselSize1);
	}
	
	unsigned short getParselSize2() const {
		return short2host(parselSize2);
	}
	
} MainMapPMRecord1;

typedef struct {
	SectorAddr parcelAddr;
	unsigned short parselSize;
	unsigned char filename[12];
	
	unsigned short getParselSize() const {
		return short2host(parselSize);
	}
	
} MainMapPMRecord100;


#endif /* kiwi_hpp */
