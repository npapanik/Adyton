/*
 *  Adyton: A Network Simulator for Opportunistic Networks
 *  Copyright (C) 2015-2018  Nikolaos Papanikos,
 *  Dimitrios-Georgios Akestoridis, and Evangelos Papapetrou
 *
 *  This file is part of Adyton.
 *
 *  Adyton is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Adyton is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Adyton.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Written by Nikolaos Papanikos and Dimitrios-Georgios Akestoridis.
 */


#define ATTR_LENGTH 50

#define APPLICATION -1

#define NOTSET -1
#define INFINITE 0
#define DEFAULT 0

//Contact Traces
#define MILANO_TR 1
#define MITREALITY_TR 2
#define INTEL_TR 3
#define HAGGLECAM_TR 4
#define INFOCOM05_TR 5
#define INFOCOM06_TR 6
#define INFOCOM06SR_TR 7
#define SASSY_TR 8
#define SIGCOMM_TR 9
#define CAMBRIDGE_TR 10
#define CAMBRIDGE_STUDENTS_TR 11
#define ROLLERNET_TR 12
#define NODOBO_TR 13
#define UCSD_TR 14
#define UCSD_3WEEKS_TR 15
#define CABSPOTTING_TR 16
#define CABSPOTTING_2DAYS_TR 17
#define LYON_TR 18
#define STANFORD_TR 19
#define DIESELNET_TR 20
#define DARTMOUTH_TR 21
#define DARTMOUTH_2WEEKS_TR 22
#define DEBUG_TR 23
#define CUSTOM_TR 24
#define LAST_ENTRY_TR 25 //this should always be the last one

//Routing Protocols
#define DIRECT_RT 1
#define EPIDEMIC_RT 2
#define PROPHET_RT 3
#define SIMBET_RT 4
#define SIMBETTS_RT 5
#define BUBBLERAP_RT 6
#define SW_RT 7
#define LSFSW_RT 8
#define MSFSW_RT 9
#define PROPHETSW_RT 10
#define LSFSF_RT 11
#define CNF_RT 12
#define CNR_RT 13
#define EBR_RT 14
#define DELEGATION_RT 15
#define COORD_RT 16
#define OPTIMAL_RT 17
#define LAST_ENTRY_RT 18 //this should always be the last one

//Congestion Control Mechanisms
#define NOCC_CC 1
#define AVOIDOVERFLOW_CC 2
#define ACC_CC 3
#define FAIRROUTE_CC 4
#define BSA_CC 5
#define CACC_CC 6
#define CCAF_CC 7
#define LAST_ENTRY_CC 8 //this should always be the last one

//Scheduling Policies
#define FIFO_SP 1
#define LIFO_SP 2
#define GRTRMAX_SP 3
#define GRTRSORT_SP 4
#define HNUV_SP 5
#define LAST_ENTRY_SP 6 //this should always be the last one

//Dropping Policies
#define DF_DP 1
#define DT_DP 2
#define DO_DP 3
#define DY_DP 4
#define DFASP_DP 5
#define DTASP_DP 6
#define DOASP_DP 7
#define DYASP_DP 8
#define LAST_ENTRY_DP 9 //this should always be the last one

//Deletion Mechanisms
#define JUSTTTL_DM 1
#define NODUPLICATES_DM 2
#define NODUPSKEEPRELICATION_DM 3
#define VACCINE_DM 4
#define CATACLYSM_DM 5
#define LAST_ENTRY_DM 6 //this should always be the last one

//Output Types
#define T1_OUT 1
#define T2_OUT 2
#define T3_OUT 3
#define T4_OUT 4
#define T5_OUT 5
#define LAST_ENTRY_OUT 6 //this should always be the last one

//Traffic Types
#define UNIFORM_TT 1
#define BURST_TT 2
#define SAMPLE_TT 3
#define PREDEFINED_TT 4
#define LAST_ENTRY_TT 5 //this should always be the last one
