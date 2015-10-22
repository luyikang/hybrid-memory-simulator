/*******************************************************************************
* Copyright (c) 2012-2014, The Microsystems Design Labratory (MDL)
* Department of Computer Science and Engineering, The Pennsylvania State University
* All rights reserved.
* 
* This source code is part of NVMain - A cycle accurate timing, bit accurate
* energy simulator for both volatile (e.g., DRAM) and non-volatile memory
* (e.g., PCRAM). The source code is free and you can redistribute and/or
* modify it by providing that the following conditions are met:
* 
*  1) Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
* 
*  2) Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* 
* Author list: 
*   Matt Poremba    ( Email: mrp5060 at psu dot edu 
*                     Website: http://www.cse.psu.edu/~poremba/ )
*******************************************************************************/

#ifndef __NVMAIN_UTILS_STATICMIGRATOR_H__
#define __NVMAIN_UTILS_STATICMIGRATOR_H__

#include "sys/mman.h"
#include "fcntl.h"
#include "unistd.h"
#include "sys/ipc.h"
#include "sys/shm.h"
#include <iterator>
#include "src/NVMObject.h"
#include "src/Params.h"
#include "include/NVMainRequest.h"
#include "include/Exception.h"
#include "Decoders/Migrator/Migrator.h"
#include "vector"
#include <algorithm>
#include <string>
#include <cctype>

namespace NVM {

#define MIG_READ_TAG GetTagGenerator( )->CreateTag("MIGREAD")
#define MIG_WRITE_TAG GetTagGenerator( )->CreateTag("MIGWRITE")

class Migrator;

struct command
{
	 uint64_t start_addr;
	 uint64_t optype;
	 //uint64_t migration_len;
};

enum _op_type{
	PCM_MEM = 0x01,
	DRAM_MEM = 0x02
};


class StaticMigrator : public NVMObject
{
  public:
    StaticMigrator( );
    virtual ~StaticMigrator( );

    void Init( Config *config );

    bool IssueAtomic( NVMainRequest *request );
    bool IssueCommand( NVMainRequest *request );
    bool RequestComplete( NVMainRequest *request );

    void Cycle( ncycle_t steps );
	//call first
	bool ParseRequest( NVMainRequest* req);
  private:
    bool promoBuffered, demoBuffered; 
    NVMAddress demotee, promotee; 
    NVMainRequest *promoRequest;
    NVMainRequest *demoRequest;

	char *signal_file;
	bool signal_inited;
	uint64_t signal;
    ncounter_t numCols;
    bool queriedMemory;
	command* cmd;
    ncycle_t bufferReadLatency;
    Params *promotionChannelParams;
	uint64_t migraAddress;
	bool can_migrate;
	uint64_t start_addr , op;

	char *migration_done;
	int migration_done_len;
	uint64_t channels;
	
	std::vector<uint64_t> dram_channel;
	std::vector<uint64_t> nvm_channel;
	uint64_t dram_num;
	uint64_t nvm_num;
	uint64_t cur_dram;
	uint64_t cur_nvm;

    ncounter_t totalPromotionPages;
    ncounter_t currentPromotionPage;
    ncounter_t promotionChannel;

    ncounter_t migrationCount;
    ncounter_t queueWaits;
    ncounter_t bufferedReads;
    bool CheckIssuable( NVMAddress address, OpType type );
    bool TryMigration( NVMainRequest *request, bool atomic );
    void ChooseVictim( Migrator *at, NVMAddress& promo, NVMAddress& victim );
	bool GetSignal();
	void UpdateCurChannel( uint64_t &cur_channel , uint64_t channel_num);
	
	
	bool ReadCommand( command* cmd );
	bool CopyChars(char* &dest_chars , const char* source_chars);
	void WriteDoneFlag();
};

};

#endif
