//
//  kiwi.cpp
//  kiwi
//
//  Created by pavelk on 19.03.17.
//  Copyright © 2017 pavelk. All rights reserved.
//

#include "kiwi.hpp"

unsigned short short2host(unsigned short d) {
#ifdef HAVE_BIGENDIAN
	return d;
#else
	//  return (d >> 8;
	return ((d & 0xFF) << 8) + (d >> 8);
#endif
}

unsigned int int2host(unsigned int d) {
#ifdef HAVE_BIGENDIAN
	return d;
#else
	return ((d & 0xFF) << 24) + ((d & 0xFF00) << 8) + ((d & 0xFF0000) >> 8) + ((d & 0xFF000000) >> 24);
#endif
}

void DataVolume::print() const {
	printf("%s\n", systemSpecificName);
	systemSpecific.print();
	printf("%s\n", authorName);
	dataAuthor.print();
	printf("%s\n", systemName);
	system.print();
	printf("%s\n", formatVersion);
	printf("%s\n", dataVersion);
	printf("%s\n", title);
	printf("options: %s %s %s\n", hasMainMap ? "Map" : "", hasRouting ? "Routing" : "", hasIndex ? "Index" : "");
	printf("%s\n", version);
	cover.print();
	printf("logical sector size: %d\n", getLogicalSecSize());
	printf("sector size: %d\n", getSecSize());
	printf("Use %s as background when in map\n", inMapBackSee ? "See" : "Land");
	printf("Use %s as background when out of the map\n", outMapBackSee ? "See" : "Land");
	printf("levels: %d\n", getLevels());
}

void SectorAddr::print() const {
	printf("%c%c%u:%u", diskSide == 0 ? 'A' : 'B', storageLayer == 0 ? 'S' : 'D', getSector(), logicalSector);
}

void ManagementRecord::print() const {
	if (addr.getSector() == 16777215)
		return;
	printf("Loc: ");
	addr.print();
	printf("\n");
	printf("Size: %u\n", getSize());
	printf("Name: %.12s\n", name);
}


void PDMDHeader::print(SectorSource &ss) const {
	printf("header size = %d\n", getSize());
	printf("box = (");
	latU.print(true);
	printf(", ");
	lonL.print(false);
	printf("), (");
	latL.print(true);
	printf(", ");
	lonR.print(false);
	printf(")\n");
	
	printf("LMR size = %d\n", getLMRsize());
	printf("BSMR size = %d\n", getBSMRsize());
	printf("BMR size = %d\n", getBMRsize());
	printf("LMR num = %d\n", getLMRnum());
	printf("BSMR num = %d\n", getBSMRnum());
	
	unsigned int i;
	for (i = 0; i < getLMRnum(); i++) {
		LMRecord *lmr = (LMRecord *)(&n + i * getLMRsize() * 2);
		lmr->print();
	}
	for (i = 0; i < getBSMRnum(); i++) {
		BSMRecord *bsmr = (BSMRecord *)(&n + getLMRnum() * getLMRsize() * 2 + i * getBSMRsize() * 2);
		bsmr->print();
		if (bsmr->getBMTSize() > 0) {
			if (filenameDesig == 1) {
				BMRecordFile *bmt = (BMRecordFile *)(((char *)this) + bsmr->getBMTOffset() * 2);
				for (unsigned int j = 0; j * 18 < bsmr->getBMTSize(); j++)
					bmt[j].print();
			} else {
				BMRecord *bmt = (BMRecord *)(((char *)this) + bsmr->getBMTOffset() * 2);
				for (unsigned int j = 0; j * 6 < bsmr->getBMTSize(); j++) {
					bmt[j].print();
					if (bmt[j].getPMISize() > 0) {
						PMI *pmi = (PMI *)ss.getSector(bmt[j].PMIAddr);
						pmi->print();
						MainMapPMRecord0 *mm = (MainMapPMRecord0 *)(pmi + 1);
						for (unsigned int k = 0; sizeof(PMI) + k * sizeof(MainMapPMRecord0) < bmt[j].getPMISize() * 32u; k++) {
							//              if (mm[k].getParcelSize() == 0)
							//                break;
							mm[k].print();
						}
					}
				}
			}
		}
	}
}


void LMRecord::print() const {
	printf("header.level = %d\n", header.getLevel());
	if (getDisplayScale1() != 0xFFFFFFFF)
		printf("1. %u, ", getDisplayScale1());
	if (getDisplayScale2() != 0xFFFFFFFF)
		printf("2. %u, ", getDisplayScale2());
	if (getDisplayScale3() != 0xFFFFFFFF)
		printf("3. %u, ", getDisplayScale3());
	if (getDisplayScale4() != 0xFFFFFFFF)
		printf("4. %u, ", getDisplayScale4());
	if (getDisplayScale5() != 0xFFFFFFFF)
		printf("5. %u, ", getDisplayScale5());
	printf("\n");
}

void BSMRecord::print() const {
	if (getBMTSize() > 0)
		printf("Level = %d, BSNum = %d, BMTOffset = %d, BMTSize = %d\n",
      header.getLevel(), header.BSNum, getBMTOffset(), getBMTSize());
}

void BMRecord::print() const {
	if (getPMISize() == 0)
		return;
	printf("  Addr: ");
	PMIAddr.print();
	printf(", size: %d\n", getPMISize());
}

void BMRecordFile::print() const {
	if (getPMISize() == 0)
		return;
	printf("  Addr: ");
	PMIAddr.print();
	printf(", size: %d, file = %s\n", getPMISize(), name);
}

void MainMapPMRecord0::print() const {
	if (parcelAddr.isNull())
		return;
	printf("      Addr: ");
	parcelAddr.print();
	printf(", size = %d\n", getParcelSize());
}

void GeoNum::print(bool lat) const {
	if (lat)
		printf("%c%u'%u\"%u %u/8", sign == 1? 'S' : 'N', getSecs() / 3600, (getSecs() / 60) % 60, getSecs() % 60, numFrac);
	else
		printf("%c%u'%u\"%u %u/8", sign ? 'W' : 'E', getSecs() / 3600, (getSecs() / 60) % 60, getSecs() % 60, numFrac);
}

void PID::print() const {
	lat.print(true);
	printf(" ");
	lon.print(false);
	printf("\n");
}

void PMI::print() const {
	printf("    typeNum = %d, PMTypeNum = %d, RGPMLOffset = %d\n",
		   type.typeNum, type.PMTypeNum, getRGPMLOffset());
}

void MID::print() const {
	location.print();
	printf("floor: %u\n", floor);
	printf("Days after 01.01.1997: %u\n", getRegDays());
	printf("sizeof(MID) = %lu\n", sizeof(MID));
	struct tm tm;
	tm.tm_year = 97;
	tm.tm_mon = 1;
	tm.tm_mday = 1;
	tm.tm_hour = 0;
	tm.tm_min = 0;
	tm.tm_sec= 0;
	tm.tm_isdst = 0;
	time_t t = mktime(&tm);
	t += (getRegDays() * 60 * 60 * 24);
	struct tm *tmo = localtime(&t);
	printf("Register date: %04u-%02d-%02d %02d:%02d:%02d\n", tmo->tm_year + 1900, tmo->tm_mon, tmo->tm_mday, tmo->tm_hour, tmo->tm_min, tmo->tm_sec);
}

class FileSectorSource : public SectorSource {
public:
	FileSectorSource(const char *file) {
		fd = open(file, O_DIRECT | O_LARGEFILE);
	}
	~FileSectorSource() {
		close(fd);
	}
	
	
	int isValid() {
		return fcntl(fd, F_GETFL) != -1 || errno != EBADF;
	}
	
	const unsigned char *getStart() {
		return pFileData;
	}
	
	virtual const unsigned char *getSector(SectorAddr &addr) {
		return pFileData + addr.getSector() * 2048 + addr.logicalSector * 32;
	}
	
	int fd;
	const unsigned char *pFileData;
};


int main()
{
	FileSectorSource ss("alldata.kwi");
	if (!ss.isValid()) {
		printf("File alldata.kwi not found\n");
		return 0;
	}
	
	DataVolume *d = (DataVolume *)ss.getStart();
	d->print();
	
	SectorAddr sa;
	memset(&sa, 0, sizeof(sa));
	sa.sector3 = 1;
	ManagementTable mt = (ManagementTable)ss.getSector(sa);
	for (int i=0; i < 34; i++) {
		printf("Record %d\n", i + 1);
		mt[i].print();
	}
	
	PDMDHeader *ph = (PDMDHeader *)ss.getSector(mt[0].addr);
	ph->print(ss);
    
    return 1;
}
