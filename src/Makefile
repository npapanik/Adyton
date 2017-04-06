CC=g++
CFLAGS=-c -g -isystem ../ext/ -Wall -O3 -std=c++11

dsPath=./data-structures
scPath=./scheduling-policies
dpPath=./dropping-policies
dmPath=./deletion-mechanisms
ccPath=./congestion-control
rpPath=./routing
corePath=./core

SOURCES_DS=$(dsPath)/Adjacency.cc $(dsPath)/DPT.cc $(dsPath)/CentralityApproximation.cc $(dsPath)/CommunityDetection.cc $(dsPath)/Utility.cc $(dsPath)/Sociability.cc $(dsPath)/Enc.cc $(dsPath)/DestEnc.cc $(dsPath)/LTS.cc $(dsPath)/AMT.cc $(dsPath)/ASP.cc $(dsPath)/AIT.cc $(dsPath)/MEED.cc $(dsPath)/Familiarity.cc $(dsPath)/Regularity.cc $(dsPath)/SPM.cc $(dsPath)/LastContact.cc $(dsPath)/Coding.cc $(dsPath)/pktDepository.cc

SOURCES_SC=$(scPath)/SchedulingPolicy.cc $(scPath)/FIFO.cc $(scPath)/LIFO.cc $(scPath)/GRTRMax.cc $(scPath)/GRTRSort.cc $(scPath)/HNUV.cc

SOURCES_DP=$(dpPath)/DroppingPolicy.cc $(dpPath)/DropFront.cc $(dpPath)/DropTail.cc $(dpPath)/DropOldest.cc $(dpPath)/DropYoungest.cc $(dpPath)/DropFrontASP.cc $(dpPath)/DropTailASP.cc $(dpPath)/DropOldestASP.cc $(dpPath)/DropYoungestASP.cc

SOURCES_DM=$(dmPath)/DeletionMechanism.cc $(dmPath)/JustTTL.cc $(dmPath)/NoDuplicates.cc $(dmPath)/NoDupsKeepReplication.cc $(dmPath)/Vaccine.cc $(dmPath)/Cataclysm.cc

SOURCES_CC=$(ccPath)/CongestionControl.cc $(ccPath)/NoCC.cc $(ccPath)/AvoidOverflow.cc $(ccPath)/ACC.cc $(ccPath)/FairRoute.cc $(ccPath)/BSA.cc $(ccPath)/CACC.cc

SOURCES_RP=$(rpPath)/Routing.cc $(rpPath)/Direct.cc $(rpPath)/Epidemic.cc $(rpPath)/Prophet.cc $(rpPath)/SimBet.cc $(rpPath)/SimBetTS.cc $(rpPath)/BubbleRap.cc $(rpPath)/SprayWait.cc $(rpPath)/LSFSpray.cc $(rpPath)/MSFSpray.cc $(rpPath)/ProphetSpray.cc $(rpPath)/LSFSprayFocus.cc $(rpPath)/CnR.cc $(rpPath)/EBR.cc $(rpPath)/Delegation.cc $(rpPath)/Optimal.cc

SOURCES_CORE=$(corePath)/Header.cc $(corePath)/Packet.cc $(corePath)/PacketPool.cc $(corePath)/Statistics.cc $(corePath)/Connection.cc $(corePath)/ConnectionMap.cc $(corePath)/Event.cc $(corePath)/EventList.cc $(corePath)/TrafficGen.cc $(corePath)/Settings.cc $(corePath)/Results.cc $(corePath)/MAC.cc $(corePath)/God.cc $(corePath)/PacketEntry.cc $(corePath)/PacketRecord.cc $(corePath)/PacketBuffer.cc $(corePath)/Node.cc $(corePath)/Simulator.cc $(corePath)/UserInteraction.cc

SOURCES_MAIN=Adyton.cc

OBJECTS=$(SOURCES_DS:.cc=.o) $(SOURCES_SC:.cc=.o) $(SOURCES_DP:.cc=.o) $(SOURCES_DM:.cc=.o) $(SOURCES_CC:.cc=.o) $(SOURCES_RP:.cc=.o) $(SOURCES_CORE:.cc=.o) $(SOURCES_MAIN:.cc=.o)

EXECUTABLE=Adyton


all: $(SOURCES_DS) $(SOURCES_SC) $(SOURCES_DP) $(SOURCES_DM) $(SOURCES_CC) $(SOURCES_RP) $(SOURCES_CORE) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

.cc.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf ${dsPath}/*.o
	rm -rf ${scPath}/*.o
	rm -rf ${dpPath}/*.o
	rm -rf $(dmPath)/*.o
	rm -rf $(ccPath)/*.o
	rm -rf $(rpPath)/*.o
	rm -rf $(corePath)/*.o
	rm -rf ./*.o
	rm -rf ./Adyton
	rm -rf ./Makefile~
	rm -rf ./*.cc~
	rm -rf ${dsPath}/*.cc~
	rm -rf ${dsPath}/*.h~
	rm -rf ${scPath}/*.cc~
	rm -rf ${scPath}/*.h~
	rm -rf ${dpPath}/*.cc~
	rm -rf ${dpPath}/*.h~
	rm -rf $(dmPath)/*.cc~
	rm -rf $(dmPath)/*.h~
	rm -rf $(ccPath)/*.cc~
	rm -rf $(ccPath)/*.h~
	rm -rf ${rpPath}/*.cc~
	rm -rf ${rpPath}/*.h~
	rm -rf ${corePath}/*.cc~
	rm -rf ${corePath}/*.h~
