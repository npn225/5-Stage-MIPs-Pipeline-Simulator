/*
 * Programmer: Nnamdi Nwaokorie 
 * Net-ID: npn225
 * University-School: New York University - Tandon School of Engineering 
 * Course: Computer Systems Architecture (ECE-GY 6913)
 * Instructor: Brandon Reagen
 * Date: November 05, 2020
 * 
 * Special Thanks to Course TA Jiazhen Han
 */

#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>

using namespace std;

// memory size, in reality, the memory size should be 2^32, 
// but for this lab, for the space resaon, we keep it as this large number, 
// but the memory is still 32-bit addressable.
#define MemSize 1000 

struct IFStruct {
    bitset<32>  PC;
    bool        nop;  
    
    // Constructor added by student-programmer
    IFStruct () {
    
        nop = false;
        
    }
};

struct IDStruct {
    bitset<32>  Instr;
    bool        nop;  
    
    // Constructor added by student-programmer
    IDStruct () {
    
        nop = true;
        
    }
};

struct EXStruct {
    bitset<32>  Read_data1;
    bitset<32>  Read_data2;
    bitset<16>  Imm;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        is_I_type;
    bool        rd_mem;
    bool        wrt_mem; 
    bool        alu_op;     //1 for addu, lw, sw, 0 for subu 
    bool        wrt_enable;
    bool        nop;  
    
    // Constructor added by student-programmer
    EXStruct () {
    
        is_I_type = false;
        rd_mem = false;
        wrt_mem = false;
        alu_op = true;
        wrt_enable = false;
        nop = true;
    
    }
};

struct MEMStruct {
    bitset<32>  ALUresult;
    bitset<32>  Store_data;
    bitset<5>   Rs;
    bitset<5>   Rt;    
    bitset<5>   Wrt_reg_addr;
    bool        rd_mem;
    bool        wrt_mem; 
    bool        wrt_enable;    
    bool        nop;  
    
    // Constructor added by student-programmer
    MEMStruct () {
    
        rd_mem = false;
        wrt_mem = false; 
        wrt_enable = false;
        nop = true;
        
    }
};

struct WBStruct {
    bitset<32>  Wrt_data;
    bitset<5>   Rs;
    bitset<5>   Rt;     
    bitset<5>   Wrt_reg_addr;
    bool        wrt_enable;
    bool        nop;     
    
    // Constructor added by student-programmer
    WBStruct () {
    
        wrt_enable = false;
        nop = true;
        
    }
};

struct stateStruct {
    IFStruct    IF;
    IDStruct    ID;
    EXStruct    EX;
    MEMStruct   MEM;
    WBStruct    WB;
};

class RF
{
    public: 
        bitset<32> Reg_data;
     	RF()
    	{ 
			Registers.resize(32);  
			Registers[0] = bitset<32> (0);  
        }
	
        bitset<32> readRF(bitset<5> Reg_addr)
        {   
            Reg_data = Registers[Reg_addr.to_ulong()];
            return Reg_data;
        }
    
        void writeRF(bitset<5> Reg_addr, bitset<32> Wrt_reg_data)
        {
            Registers[Reg_addr.to_ulong()] = Wrt_reg_data;
        }
		 
		void outputRF()
		{
			ofstream rfout;
			rfout.open("RFresult.txt",std::ios_base::app);
			if (rfout.is_open())
			{
				rfout<<"State of RF:\t"<<endl;
				for (int j = 0; j<32; j++)
				{        
					rfout << Registers[j]<<endl;
				}
			}
			else cout<<"Unable to open file";
			rfout.close();               
		} 
			
	private:
		vector<bitset<32> >Registers;	
};

class INSMem
{
	public:
        bitset<32> Instruction;
        INSMem()
        {       
			IMem.resize(MemSize); 
            ifstream imem;
			string line;
			int i=0;
			imem.open("imem.txt");
			if (imem.is_open())
			{
				while (getline(imem,line))
				{      
					IMem[i] = bitset<8>(line);
					i++;
				}                    
			}
            else cout<<"Unable to open file";
			imem.close();                     
		}
                  
		bitset<32> readInstr(bitset<32> ReadAddress) 
		{    
			string insmem;
			insmem.append(IMem[ReadAddress.to_ulong()].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+1].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+2].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+3].to_string());
			Instruction = bitset<32>(insmem);		//read instruction memory
			return Instruction;     
		}     
      
    private:
        vector<bitset<8> > IMem;     
};
      
class DataMem    
{
    public:
        bitset<32> ReadData;  
        DataMem()
        {
            DMem.resize(MemSize); 
            ifstream dmem;
            string line;
            int i=0;
            dmem.open("dmem.txt");
            if (dmem.is_open())
            {
                while (getline(dmem,line))
                {      
                    DMem[i] = bitset<8>(line);
                    i++;
                }
            }
            else cout<<"Unable to open file";
                dmem.close();          
        }
		
        bitset<32> readDataMem(bitset<32> Address)
        {	
			string datamem;
            datamem.append(DMem[Address.to_ulong()].to_string());
            datamem.append(DMem[Address.to_ulong()+1].to_string());
            datamem.append(DMem[Address.to_ulong()+2].to_string());
            datamem.append(DMem[Address.to_ulong()+3].to_string());
            ReadData = bitset<32>(datamem);		//read data memory
            return ReadData;               
		}
            
        void writeDataMem(bitset<32> Address, bitset<32> WriteData)            
        {
            DMem[Address.to_ulong()] = bitset<8>(WriteData.to_string().substr(0,8));
            DMem[Address.to_ulong()+1] = bitset<8>(WriteData.to_string().substr(8,8));
            DMem[Address.to_ulong()+2] = bitset<8>(WriteData.to_string().substr(16,8));
            DMem[Address.to_ulong()+3] = bitset<8>(WriteData.to_string().substr(24,8));  
        }   
                     
        void outputDataMem()
        {
            ofstream dmemout;
            dmemout.open("dmemresult.txt");
            if (dmemout.is_open())
            {
                for (int j = 0; j< 1000; j++)
                {     
                    dmemout << DMem[j]<<endl;
                }
                     
            }
            else cout<<"Unable to open file";
            dmemout.close();               
        }             
      
    private:
		vector<bitset<8> > DMem;      
};  

void printState(stateStruct state, int cycle)
{
    ofstream printstate;
    printstate.open("stateresult.txt", std::ios_base::app);
    if (printstate.is_open())
    {
        printstate<<"State after executing cycle:\t"<<cycle<<endl; 
        
        printstate<<"IF.PC:\t"<<state.IF.PC.to_ulong()<<endl;        
        printstate<<"IF.nop:\t"<<state.IF.nop<<endl; 
        
        printstate<<"ID.Instr:\t"<<state.ID.Instr<<endl; 
        printstate<<"ID.nop:\t"<<state.ID.nop<<endl;
        
        printstate<<"EX.Read_data1:\t"<<state.EX.Read_data1<<endl;
        printstate<<"EX.Read_data2:\t"<<state.EX.Read_data2<<endl;
        printstate<<"EX.Imm:\t"<<state.EX.Imm<<endl; 
        printstate<<"EX.Rs:\t"<<state.EX.Rs<<endl;
        printstate<<"EX.Rt:\t"<<state.EX.Rt<<endl;
        printstate<<"EX.Wrt_reg_addr:\t"<<state.EX.Wrt_reg_addr<<endl;
        printstate<<"EX.is_I_type:\t"<<state.EX.is_I_type<<endl; 
        printstate<<"EX.rd_mem:\t"<<state.EX.rd_mem<<endl;
        printstate<<"EX.wrt_mem:\t"<<state.EX.wrt_mem<<endl;        
        printstate<<"EX.alu_op:\t"<<state.EX.alu_op<<endl;
        printstate<<"EX.wrt_enable:\t"<<state.EX.wrt_enable<<endl;
        printstate<<"EX.nop:\t"<<state.EX.nop<<endl;        

        printstate<<"MEM.ALUresult:\t"<<state.MEM.ALUresult<<endl;
        printstate<<"MEM.Store_data:\t"<<state.MEM.Store_data<<endl; 
        printstate<<"MEM.Rs:\t"<<state.MEM.Rs<<endl;
        printstate<<"MEM.Rt:\t"<<state.MEM.Rt<<endl;   
        printstate<<"MEM.Wrt_reg_addr:\t"<<state.MEM.Wrt_reg_addr<<endl;              
        printstate<<"MEM.rd_mem:\t"<<state.MEM.rd_mem<<endl;
        printstate<<"MEM.wrt_mem:\t"<<state.MEM.wrt_mem<<endl; 
        printstate<<"MEM.wrt_enable:\t"<<state.MEM.wrt_enable<<endl;         
        printstate<<"MEM.nop:\t"<<state.MEM.nop<<endl;        

        printstate<<"WB.Wrt_data:\t"<<state.WB.Wrt_data<<endl;
        printstate<<"WB.Rs:\t"<<state.WB.Rs<<endl;
        printstate<<"WB.Rt:\t"<<state.WB.Rt<<endl;        
        printstate<<"WB.Wrt_reg_addr:\t"<<state.WB.Wrt_reg_addr<<endl;
        printstate<<"WB.wrt_enable:\t"<<state.WB.wrt_enable<<endl;        
        printstate<<"WB.nop:\t"<<state.WB.nop<<endl; 
    }
    else cout<<"Unable to open file";
    printstate.close();
}
 

int main()
{
    // Variables assigned by instructor 
    RF myRF;
    INSMem myInsMem;
    DataMem myDataMem;
    
    // Variables below assigned by student-programmer
    
    // Needed Structs 
    IFStruct if_stage;
    IDStruct id_stage;
    EXStruct ex_stage;
    MEMStruct mem_stage;
    WBStruct wb_stage;
    stateStruct newState, state;
    
    // Needed Values
    bitset<32> PC, instruction, alu_result, data_memory;
    string opcode, rs, rt, rd, shamt, funct, immed;
    int PC_num, cycle;
    bool is_stalled = false, at_halt = false, ex_written = false, mem_written = false, wb_written = false, has_branched = false, from_branch = false;
		
    cycle = 0;
             
    while ( 1 ) {

        // Output the Current Clock Cycle Number: 
        cout << endl << "___________________________________________________" \
                 "___________________________________________________" << endl;
        cout << "Current Cycle: " << cycle << endl;
        
        
        /* --------------------- WB stage --------------------- */
        
        // If there is an operation on the WriteBack Stage ...
        if( !wb_stage.nop ){
            
            cout << "Write-Back Stage: Is Operating " << endl << endl;
            
            // Write back to the Register File when there is something to be written
            if ( wb_stage.wrt_enable ) {
            
                myRF.writeRF( wb_stage.Wrt_reg_addr, wb_stage.Wrt_data );
            
            }
          
                        
        } else {
        
            cout << "Write-Back Stage: NOT Operating (No-Op) " << endl << endl;

        }


        /* --------------------- MEM stage --------------------- */
        
        // If there is an operation on the Memory Stage ...
        if( !mem_stage.nop ){
            
            cout << "Memory Stage: Is Operating " << endl;
            
            // If the current instruction is R-type . . . 
            if ( !mem_stage.rd_mem && !mem_stage.wrt_mem && !from_branch ) {
                
                cout << "Current Instruction is R-type so Memory Stage just updates WB flip-flops! " << endl << endl;
                
                // Set the flip-flops for the Write-Back stage
                // Set the Read Register Address Flip-flops
                wb_stage.Rs = mem_stage.Rs;
                wb_stage.Rt = mem_stage.Rt;
                
                // Set the Write Data Flip-Flop 
                wb_stage.Wrt_data = mem_stage.ALUresult;
                
                // Set the Write Register Address Flip-Flop
                wb_stage.Wrt_reg_addr = mem_stage.Wrt_reg_addr;
                
                // Set the Write Enable to true since R-types update the Register File
                wb_stage.wrt_enable = true;
                
                // Set up the Write-Back Stage for Operation 
                wb_stage.nop = false; 
                
                
            // If the current instruction is Load Word (LW) . . . 
            } else if ( mem_stage.rd_mem && !mem_stage.wrt_mem ) {
            
                cout << "Current Instruction is I-type Load Word, so Memory Stage reads from data memory! " << endl << endl;
                                               
                // Set the flip-flops for the Write-Back stage
                // Set the Read Register Address Flip-flop
                // NOTE: Load Word (LW) doesn't use the Rt_register variable in this code!
                //       This is here just to keep this program's results congruent with the professor's! 
                wb_stage.Rs = mem_stage.Rs;
                wb_stage.Rt = mem_stage.Rt;
                
                // Read Data from Memory, using the the memory address stored in 
                // ALUresult and Set it to the Write Data Flip-Flop 
                wb_stage.Wrt_data = myDataMem.readDataMem( mem_stage.ALUresult );
                
                // Set the Write Register Address Flip-Flop
                wb_stage.Wrt_reg_addr = mem_stage.Wrt_reg_addr;
                
                // Set the Write Enable to true since LW updates the Register File
                wb_stage.wrt_enable = true;
                
                // Set up the Write-Back Stage for Operation 
                wb_stage.nop = false; 
                
            // If the current instruction is Store Word (SW) . . . 
            } else if ( !mem_stage.rd_mem && mem_stage.wrt_mem ) {
            
                cout << "Current Instruction is I-type Store Word, so Memory Stage stores data to memory! " << endl << endl;
                
                // Set the flip-flops for the Write-Back stage
                // Set the Read Register Address Flip-flops
                // NOTE: Store Word (SW) doesn't use the Write_Register_Address variable in this code!
                //       This is here just to keep this program's results congruent with the professor's! 
                wb_stage.Rs = mem_stage.Rs;
                wb_stage.Rt = wb_stage.Wrt_reg_addr = mem_stage.Rt;
                
                // Write Data to Memory
                myDataMem.writeDataMem( mem_stage.ALUresult, mem_stage.Store_data );
                
                // Set the Write Enable to true since SW does not update the Register File
                wb_stage.wrt_enable = false;
                
                // Set up the Write-Back Stage for Operation 
                wb_stage.nop = false; 
                
            } 
            // If the current instruction is Branch-If-Equal (BEQ) . . . 
            else if ( !mem_stage.rd_mem && !mem_stage.wrt_mem && from_branch ) {
            
                // NOTE: This is used so that the ADDU and BEQ instructions
                //       can be distinguished in the Memory Stage
                // Set from_branch variable back to false
                from_branch = false;
                
                // Set the Write Enable to true since BEQ does not update the Register File
                wb_stage.wrt_enable = false;
                
                // Set up the Write-Back Stage for Operation 
                wb_stage.nop = false; 
                
            }
        
            // Set to true now that the Write-Back Stage has been written to
            wb_written = true;
            
            // Update the newState struct
            newState.WB = wb_stage;
            
        } else {
        
            cout << "Memory Stage: NOT Operating (No-Op) " << endl << endl;
            
            // Propagate the NO-Op to the Write-Back stage . . . 
            wb_stage.nop = true;
            
            // Update the newState struct 
            newState.WB = wb_stage;
            
        }


        /* --------------------- EX stage --------------------- */
        
        // If there is an operation on the Execution Stage ...
        if( !ex_stage.nop ){
            
            cout << "Execution Stage: Is Operating " << endl;
            
            // If current instruction is I-type
            if ( ex_stage.is_I_type ) {
                
                // Define the immediate and sign extend it to 32 bits 
                immed = ex_stage.Imm.to_string();
                immed.insert( 0, 16, immed[0] );

                // Compute the Memory Address and set the ALUresult value of 
                // the Memory Stage to this new address
                mem_stage.ALUresult = bitset<32>( ex_stage.Read_data1.to_ulong() + bitset<32>( immed ).to_ulong() );
                
                // If current instruction is Load Word (LW) . . .
                if ( ex_stage.rd_mem && !ex_stage.wrt_mem ) {
                    
                    cout << "Executing I-type Instruction: Load Word (LW)" << endl << endl;
                    
                    // Set the Flip-Flops for the Memory Stage 
                    // Set the Read Register Address Flip-Flops
                    // NOTE: Load Word (LW) doesn't use the Rt_register variable in this code!
                    //       This is here just to keep this program's results congruent with the professor's! 
                    mem_stage.Rs = ex_stage.Rs;
                    mem_stage.Rt = ex_stage.Rt;
                    
                    // Set the Write Register Address Flip-Flop
                    mem_stage.Wrt_reg_addr = ex_stage.Wrt_reg_addr;
                    
                    // Set the Write Enable to true since LW updates the Register File
                    mem_stage.wrt_enable = true;
                    
                    // Set the Read_Mem and Write_Mem Bits 
                    mem_stage.rd_mem = true; 
                    mem_stage.wrt_mem = false; 
                    
                    // Set up the Memory Stage for Operation 
                    mem_stage.nop = false; 
                } 
                
                // If current instruction is Store Word (SW) . . . 
                else if ( !ex_stage.rd_mem && ex_stage.wrt_mem ) {
                    
                    cout << "Executing I-type Instruction: Store Word (SW)" << endl;
                    
                    // Check to see if the Memory Stage Write Address is the same as the Execution Rt Address
                    // NOTE: "mem_written" is used to ensure that the Memory Stage has actually been written to
                    bool is_rt_mem_depen = mem_written && ( mem_stage.Wrt_reg_addr.to_ulong() == ex_stage.Rt.to_ulong() );
                    
                    // Set the flip-flops for the Memory Stage
                    // Set the Two Read Register Address Flip-Flops 
                    // NOTE: Store Word (SW) doesn't use the Write_Register_Address variable in this code!
                    //       This is here just to keep this program's results congruent with the professor's! 
                    mem_stage.Rs = ex_stage.Rs;
                    mem_stage.Rt = mem_stage.Wrt_reg_addr = ex_stage.Rt;
                    
                    // Set the Store Data Flip-Flop
                    // If there is an MEM-MEM Dependency for the Rt Register of Store Word (SW) Instruction . . . 
                    if ( is_rt_mem_depen ) {
                        
                        cout << "(ALU/LW)-(SW) Raw Hazard Detected for Store Word Rt-Register: Use MEM-MEM Forwarding" << endl << endl;
                        
                        // Forward the Wrt-data from the MEM Stage that is ahead
                        // NOTE: Since (due to the arrangement of this code) the MEM stage
                        //       has already written to WB_Stage, we take the WB_stage value 
                        mem_stage.Store_data = wb_stage.Wrt_data;
                        
                    } else {
                    
                        cout << "No-Forwarding: Store Word (SW) Store Data is set normally" << endl << endl;
                    
                        // Proceed as normal - Set MEM_stage Store_data to the value 
                        // read_data2 value of the ex_stage
                        mem_stage.Store_data = ex_stage.Read_data2;
                    
                    }
                    
                    // Set the Write Enable to false since SW does not update the Register File
                    mem_stage.wrt_enable = false;
                    
                    // Set the Read_Mem and Write_Mem Bits for SW
                    mem_stage.rd_mem = false; 
                    mem_stage.wrt_mem = true; 
                    
                    // Set up the Memory Stage for Operation 
                    mem_stage.nop = false; 
                    
                } 
                
                // If current instruction is Branch-If-Equal (BEQ) . . . 
                else if ( !ex_stage.rd_mem && !ex_stage.wrt_mem ) {
                
                    // Set the Write Enable to false since BEQ does not update the Register File
                    mem_stage.wrt_enable = false;
                    
                    // Set the Read_Mem and Write_Mem Bits for BEQ
                    mem_stage.rd_mem = false; 
                    mem_stage.wrt_mem = false; 
                    
                    // Set up the Memory Stage for Operation 
                    mem_stage.nop = false; 
                    
                    // Set the from_branch bool to true
                    // NOTE: This is used so that the ADDU and BEQ instructions
                    //       can be distinguished in the Memory Stage
                    from_branch = true;
                }
                
                
            // If current instruction is R-type
            } else {
                
                // Set the flip-flops for the Memory Stage
                // Set the Two Read Register Address Flip-Flops 
                mem_stage.Rs = ex_stage.Rs;
                mem_stage.Rt = ex_stage.Rt;
                
                // Set the Write Register Address Flip-Flop
                mem_stage.Wrt_reg_addr = ex_stage.Wrt_reg_addr;
                
                // Set the Write Enable to true since R-types update the Register File
                mem_stage.wrt_enable = true;

                // Set the Read_Mem and Write_Mem Bits 
                mem_stage.rd_mem = false; 
                mem_stage.wrt_mem = false; 

                // Set up the Memory Stage for Operation 
                mem_stage.nop = false; 
                
                // If current instruction is ADDU . . . 
                if ( ex_stage.alu_op ) {
                    
                    cout << "Executing R-type Instruction: ADDU" << endl << endl;
                
                    // Compute the ALU_result of ADDU command 
                    mem_stage.ALUresult = bitset<32>( ex_stage.Read_data1.to_ulong() + ex_stage.Read_data2.to_ulong() );
                
                // If current instruction is SUBU . . . 
                } else {
                
                    cout << "Executing R-type Instruction: SUBU" << endl << endl;
                    
                    // Compute the ALU_result of SUBU command 
                    mem_stage.ALUresult = bitset<32>( ex_stage.Read_data1.to_ulong() - ex_stage.Read_data2.to_ulong() );
                    
                }
            }
           
            // Set to true now that the mem_stage has been written to
            mem_written = true;
            
            // Update the newState struct
            newState.MEM = mem_stage;;
            
        } else {
            
            cout << "Execution Stage: NOT Operating (No-Op) " << endl << endl;
            
            // Propagate the NO-Op to the Execution stage . . . 
            mem_stage.nop = true;
            
            // Update the newState struct 
            newState.MEM = mem_stage;;
            
        }
          

        /* --------------------- ID stage --------------------- */
        
        // If there is an operation on the Instruction Decode Stage 
        if ( !id_stage.nop ) {

            // Set up the register address values and opcode value
            opcode = id_stage.Instr.to_string().substr(0, 6);
            rs = id_stage.Instr.to_string().substr(6, 5);
            rt = id_stage.Instr.to_string().substr(11, 5);
            
        }
        
        // If there is an operation on the Instruction Decode Stage
        // and if the Previous Instruction is a Load Word (LW) Instruction 
        // and if the Execution Stage hasn't already been stalled . . .
        if ( !id_stage.nop && ex_stage.rd_mem && !is_stalled ){
            
            // NOTE: There is no stalling when a Load Word precedes an I-type and
            // the dependency is between the Load Word's Wrt_Register and the 
            // I-type instruction's Rt register
            // Check for this special I-type Instruction case
            // (1) - Check if the current instruction is a load word (LW)
            // (2) - Check if the current instruction is a store word (SW)
            // (3) - Check if the current instruction is either LW or SW 
            // (4) - Check if the dependent register is Rt
            // (5) - Check for the special I-type instruction case
            bool is_load_word = ( bitset<6>( opcode ).to_ulong() == 35 );                         // (1) 
            bool is_store_word = ( bitset<6>( opcode ).to_ulong() == 43 );                        // (2)
            bool is_i_type = ( is_load_word || is_store_word );                                   // (3)
            bool is_rt_depend = ( ex_stage.Wrt_reg_addr.to_ulong() == bitset<5>(rt).to_ulong() ); // (4)
            bool is_i_special = ( is_i_type && is_rt_depend );                                    // (5)
            
            // Check for Load Word Dependency and Need for Stalling
            // (1) - Check if the dependent register is Rs
            // (2) - Check if the dependent register is Rs or Rt
            // (3) - Check if the next instruction is a LW, there is a 
            //       dependency, and this isn't the special I-type case
            is_stalled = ( ex_stage.Wrt_reg_addr.to_ulong() == bitset<5>(rs).to_ulong() );                   // (1)
            is_stalled = ( is_stalled || ( ex_stage.Wrt_reg_addr.to_ulong() == bitset<5>(rt).to_ulong() ) ); // (2)
            is_stalled = ( ex_stage.rd_mem && is_stalled && !is_i_special );                                 // (3)
                                
        // Set is_stalled back to false if it is already true
        } else if ( is_stalled ) {
            
            // Set is_stalled back to false
            is_stalled = false;
            
        }
        
        // If there is an operation on the Instruction Decode Stage 
        // and if the Execution Stage hasn't already been stalled . . .
        if( !id_stage.nop && !is_stalled ){
            
            cout << "Instruction Decode Stage: Is Operating " << endl;
            
            // Check for Forwarding Dependencies
            // NOTE: "_written" is used to make sure that the stage has actually been written to
            // (1) - Check to see if the Execution Write Address is the same as the Decode Rs Address
            // (2) - Check to see if the Execution Write Address is the same as the Decode Rt Address
            // (3) - Check to see if there is an ALU instruction (ADDU or SUBU) at the Execution Stage
            // (4) - Check to see if the Write-Back Write Address is the same as the Decode Rs Address
            // (5) - Check to see if the Write-Back Write Address is the same as the Decode Rt Address
            bool is_rs_ex_depen = ex_written && ( ex_stage.Wrt_reg_addr.to_ulong() == bitset<5>( rs ).to_ulong() );  // (1)
            bool is_rt_ex_depen = ex_written && ( ex_stage.Wrt_reg_addr.to_ulong() == bitset<5>( rt ).to_ulong() );  // (2)
            bool alu_at_ex = ex_written && ( ( ex_stage.rd_mem == 0 ) && ( ex_stage.wrt_mem == 0 ) );                // (3) 
            bool is_rs_mem_depen = wb_written && ( wb_stage.Wrt_reg_addr.to_ulong() == bitset<5>( rs ).to_ulong() ); // (4) 
            bool is_rt_mem_depen = wb_written && ( wb_stage.Wrt_reg_addr.to_ulong() == bitset<5>( rt ).to_ulong() ); // (5) 
            
            // Check the instruction's type
            // If the instruction is an R-type . . .
            if ( bitset<6>( opcode ).none() ) {
                
                // Set the flip-flops for the Execution Stage
                // Set the Two Read Register Address Flip-Flops 
                ex_stage.Rs = bitset<5>( rs );
                ex_stage.Rt = bitset<5>( rt );
                
                // Set the Write Register Address Flip-Flop
                ex_stage.Wrt_reg_addr = bitset<5>( id_stage.Instr.to_string().substr(16, 5) );
                
                // Get the Immem 
                // NOTE: ALU does NOT use Immed. This is in place just to be congruent with Instructor's Results
                immed = id_stage.Instr.to_string().substr(16, 16);
                
                // Get the Shift Amount and the Function Code
                shamt = id_stage.Instr.to_string().substr(21, 5);
                funct = id_stage.Instr.to_string().substr(26, 6);
                
                
                // Set the Two Read-Data Flip-Flops: (01) and (02)
                
                // (01)________________________________________________________________________________________________________________
                // If the prior instruction is an R-type instruction and 
                // If there is an EX-EX dependency for Register Rs . . . 
                if ( alu_at_ex && is_rs_ex_depen ) {
                    
                    cout << " ALU-ALU RAW Hazard Detected for R-type Rs-Register: Use EX-EX Forwarding" << endl;
                    
                    // Forward the ALU-Result from the EX Stage that is ahead
                    // NOTE: Since (due to the arrangement of this code) the EX stage
                    //       has already written to MEM_Stage, we take the MEM_stage value
                    ex_stage.Read_data1 = mem_stage.ALUresult;
                
                // If there is an Mem-Ex dependency for Register Rs . . . 
                } else if ( is_rs_mem_depen ) {
                    
                    // Check to see if this is either LW-(EX-EX) Forwarding or (ALU/LW)-(MEM-EX) forwarding
                    if ( ex_stage.nop ) {
                    
                        cout << "LW-ALU Raw Hazard Detected for R-type Rs-Register: Use EX-EX Forwarding" << endl;
                        
                    } else {
                    
                        cout << "(ALU/LW)-ALU Raw Hazard Detected for R-type Rs-Register: Use MEM-EX Forwarding" << endl;
                        
                    }
                    
                    // Forward the Data to be written from the Mem Stage that is ahead
                    // NOTE: This also handles EX-EX for Load-ALU dependencies due to logic of code for stalling
                    // NOTE: Since (due to the arrangement of this code) the MEM stage
                    //       has already written to WB_Stage, we take the WB_stage value
                    ex_stage.Read_data1 = wb_stage.Wrt_data;
                  
                // If there are no Rs dependencies and no need for forwarding . . .
                } else {
                
                    cout << "No-Forwarding: R-type Rs-Register set normally" << endl;
                    
                    // Proceed as normal - Set EX_stage read_data to the value 
                    // read from the register using the given read_address
                    ex_stage.Read_data1 = myRF.readRF( ex_stage.Rs );
                    
                }
                // (01)________________________________________________________________________________________________________________
                
                
                // (02)________________________________________________________________________________________________________________
                // If the prior instruction is an R-type instruction and 
                // If there is an EX-EX dependency for Register Rt . . . 
                if ( alu_at_ex && is_rt_ex_depen ) {
                    
                    cout << " ALU-ALU RAW Hazard Detected for R-type Rt-Register: Use EX-EX Forwarding" << endl;
                    
                    // Forward the ALU-Result from the EX Stage that is ahead
                    // NOTE: Since (due to the arrangement of this code) the EX stage
                    //       has already written to MEM_Stage, we take the MEM_stage value
                    ex_stage.Read_data2 = mem_stage.ALUresult;
                
                // If there is an Mem-Ex dependency for Register Rt . . . 
                } else if ( is_rt_mem_depen ) {
                    
                    // Check to see if this is either LW(EX-EX) Forwarding or MEM-EX forwarding
                    if ( ex_stage.nop ) {
                    
                        cout << "LW-ALU Raw Hazard Detected for R-type Rt-Register: Use EX-EX Forwarding" << endl;
                        
                    } else {
                    
                        cout << "(ALU/LW)-ALU Raw Hazard Detected for R-type Rt-Register: Use MEM-EX Forwarding" << endl;
                        
                    }
                    
                    // Forward the Data to be written from the Mem Stage that is ahead
                    // NOTE: This also handles EX-EX for Load-ALU dependencies due to logic of code for stalling
                    // NOTE: Since (due to the arrangement of this code) the MEM stage
                    //       has already written to WB_Stage, we take the WB_stage value
                    ex_stage.Read_data2 = wb_stage.Wrt_data;
                  
                // If there are no Rt dependencies and no need for forwarding . . .
                } else {
                
                    cout << "No-Forwarding: R-type Rt-Register set normally" << endl;
                    
                    // Proceed as normal - Set EX_stage read_data to the value 
                    // read from the register using the given read_address
                    ex_stage.Read_data2 = myRF.readRF( ex_stage.Rt );
                    
                }
                // (02)________________________________________________________________________________________________________________
                
                // Set the Boolean Flip-Flops 
                ex_stage.is_I_type = false;
                ex_stage.wrt_enable = true; // Set up for writing to the Register File
                ex_stage.rd_mem = false;
                ex_stage.wrt_mem = false;
                ex_stage.nop = false; // Set up the Execution Stage for operation
                
                // NOTE: ALU does NOT use Immed. This is in place just to be congruent with Instructor's Results
                ex_stage.Imm = bitset<16>( immed );
                
                // If the current R-type Instruction is SUBU, set the alu_op to false
                if ( bitset<6>( funct ).to_ulong() == 35 ) {
                    
                    cout << "Current Instruction: R-type - SUBU" << endl << endl;
                    ex_stage.alu_op = false;
                    
                // Else, the current R-type Instruction is ADDU and alu-op is set to trues
                } else {
                    
                    cout << "Current Instruction: R-type - ADDU" << endl << endl;
                    ex_stage.alu_op = true;
                }
                
            } 
            
            // Else, If at the Halt Instruction . . .
            else if ( bitset<6>( opcode ).all() ) {
                
                cout << "Current Instruction: Custom-Type - Halt" << endl << endl;
                
            }
            
            // If the instruction is an I-type . . .
            else {
                
                // Set the immediate value of this I-type Instruction
                immed = id_stage.Instr.to_string().substr(16, 16);
                
                // Set the Rs Read-Data Flip-Flop: (1)
                
                // (1)____________________________________________________________________________________________________________
                // If the prior instruction is an R-type instruction and 
                // If there is an EX-EX dependency for Register Rs . . . 
                if ( alu_at_ex && is_rs_ex_depen ) {
                    
                    cout << " ALU-(LW/SW) RAW Hazard Detected for I-type Rs-Register: Use EX-EX Forwarding" << endl;
                    
                    // Forward the ALU-Result from the EX Stage that is ahead
                    // NOTE: Since (due to the arrangement of this code) the EX stage
                    //       has already written to MEM_Stage, we take the MEM_stage value
                    ex_stage.Read_data1 = mem_stage.ALUresult;
                
                // If there is an Mem-Ex dependency for Register Rs . . . 
                } else if ( is_rs_mem_depen ) {
                    
                    // Check to see if this is either LW(EX-EX) Forwarding or MEM-EX forwarding
                    if ( ex_stage.nop ) {
                    
                        cout << "LW-(LW/SW) Raw Hazard Detected for I-type Rs-Register: Use EX-EX Forwarding" << endl;
                        
                    } else {
                    
                        cout << "(ALU/LW)-(LW/SW) Raw Hazard Detected for I-type Rs-Register: Use MEM-EX Forwarding" << endl;
                        
                    }
                    
                    // Forward the Data to be written from the Mem Stage that is ahead
                    // NOTE: This also handles EX-EX for Load-ALU dependencies due to logic of code for stalling
                    // NOTE: Since (due to the arrangement of this code) the MEM stage
                    //       has already written to WB_Stage, we take the WB_stage value
                    ex_stage.Read_data1 = wb_stage.Wrt_data;
                  
                // If there are no Rs dependencies and no need for forwarding . . .
                } else {
                
                    cout << "No-Forwarding: I-type Rs-Register set normally" << endl;
                    
                    // Proceed as normal - Set EX_stage Register_Rs address and then set 
                    // read_data1 to the value read from the register using the given read_address
                    ex_stage.Rs = bitset<5>( rs );
                    ex_stage.Read_data1 = myRF.readRF( ex_stage.Rs );
                                        
                }
                // (1)____________________________________________________________________________________________________________
                
                
                // Test to see what specific I-type Instruction is present
                switch ( bitset<6>( opcode ).to_ulong() ){
                
                    // If Current Instruction is Load Word (LW) . . . 
                    case 35 : 
                        
                        cout << "Current Instruction: I-type - Load Word (LW)" << endl << endl;
                        
                        // Set the Write Register Address Flip-Flop
                        // NOTE: Load Word (LW) doesn't use Register_Rt variable in this code! 
                        //       This is here to keep the results congruent with the professor's!
                        ex_stage.Wrt_reg_addr = ex_stage.Rt = bitset<5>( rt );
                        
                        // Set the Immediate Memory Address Flip-Flop
                        ex_stage.Imm = bitset<16>( immed );
                        
                        // Set the Boolean Flip-Flops 
                        ex_stage.is_I_type = true;
                        ex_stage.wrt_enable = true; // Set up for writing to the Register File
                        ex_stage.rd_mem = true; // LW reads from memory 
                        ex_stage.wrt_mem = false;
                        ex_stage.nop = false; // Set up the Execution Stage for operation
                        ex_stage.alu_op = true; // For addition to compute memory address in Execution Stage 
                        
                        break;
                        
                    // If Current Instruction is Store Word (SW) . . . 
                    case 43 : 
                        
                        cout << "Current Instruction: I-type - Store Word (SW)" << endl << endl;
                        
                        // Set the Rt Read Register Address Flip-Flop 
                        // NOTE: Store Word (SW) doesn't use the Write_Register_Address variable in this code!
                        //       This is here just to keep this program's results congruent with the professor's! 
                        ex_stage.Rt = ex_stage.Wrt_reg_addr = bitset<5>( rt );
                        
                        // Set the Immediate Memory Address Flip-Flop
                        ex_stage.Imm = bitset<16>( immed );
                        
                        // Set the Two Read-Data Flip-Flops
                        ex_stage.Read_data2 = myRF.readRF( ex_stage.Rt );
                        
                        // Set the Boolean Flip-Flops 
                        ex_stage.is_I_type = true;
                        ex_stage.wrt_enable = false; // Store Word does not write to Register File
                        ex_stage.rd_mem = false;
                        ex_stage.wrt_mem = true; // SW writes to memory
                        ex_stage.nop = false; // Set up the Execution Stage for operation
                        ex_stage.alu_op = true; // For addition to compute memory address in Execution Stage 
                        
                        break;
                        
                    // If Current Instruction is Branch-If-Equal (BEQ) . . . 
                    case 4 : 
                        // IMPORTANT NOTE: BEQ is implemented to operate act 
                        // like Branch-If-NOT-Equal (BNE) 
                        
                        cout << "Current Instruction: I-type - Branch-If-Equal (BEQ)" << endl;
                        
                        // Set the Two Read Register Address Flip-Flops 
                        ex_stage.Rs = bitset<5>( rs );
                        ex_stage.Rt = bitset<5>( rt );
                        
                        // Set the Immediate Memory Address Flip-Flop
                        ex_stage.Imm = bitset<16>( immed );
                        
                        // Set the Two Read-Data Flip-Flops
                        ex_stage.Read_data1 = myRF.readRF( ex_stage.Rs );
                        ex_stage.Read_data2 = myRF.readRF( ex_stage.Rt );

                        // Set the Boolean Flip-Flops 
                        ex_stage.is_I_type = true;
                        ex_stage.wrt_enable = false; // BEQ does not write to Register File
                        ex_stage.rd_mem = false;
                        ex_stage.wrt_mem = false; 
                        
                        ex_stage.alu_op = false; // BEQ usually uses subtraction in EX stage for comparison check 
                        
                        // NOTE: The Branch Predictor always assumes branches are NOT Taken
                        // If the register values are NOT equal - If Branch is Taken . . . 
                        if ( ex_stage.Read_data1.to_ulong() != ex_stage.Read_data2.to_ulong() ) {
                        
                            cout << "Branch is Taken" << endl << endl;
                            
                            // Set the has_branched bool to true
                            has_branched = true;
                            
                            // Sign extend the immediate to 30 bits and append two zeros 
                            immed.insert( 0, 14, immed[0] );
                            immed += "00";
                            
                            // If the immediate value is a negative . . .
                            if ( immed.at(0) == '1' ) {
                                
                                // Perform 2's Complement Operation 
                                // (1) - Flip the bits of the Sign-Extended Immediate Value
                                // (2) - Add 1 to the now flipped value of the Sign-Extended Immediate 
                                // (3) - Compute the new Address using SUBTRACTION and set the PC value of the Instruction Fetch Stage to this new address
                                PC = bitset<32>( immed ).flip();                                          // (1) 
                                PC = bitset<32>( PC.to_ulong() + 1 );                                     // (2) 
                                PC = bitset<32>( if_stage.PC.to_ulong() - PC.to_ulong() );       // (3)

                                cout << "Am Using Two's Complement: " << if_stage.PC.to_ulong() << endl; 
                                
                            } else {
                            
                            // Compute the new Address using ADDITION and set the PC value of the Instruction Fetch Stage to this new address
                            PC = bitset<32>( if_stage.PC.to_ulong() + bitset<32>(immed).to_ulong() );
                            
                            cout << "NOT Using Two's Complement: " << if_stage.PC.to_ulong() << endl;
                            
                            }
                                                       
                                                        
                        } else { // If the register values are equal - If Branch is NOT Taken
                        
                            cout << "Branch is NOT Taken" << endl << endl;
                            
                            // Since the branch is NOT taken and assumed to be NOT taken, 
                            // set up the Execution Stage for operation
                            ex_stage.nop = false; 
                            
                        }
                        
                        break;
                
                }
                
            }
            
            // Set to true now that the ex_stage has been written to
            ex_written = true;
            
            // Update the newState struct 
            newState.EX = ex_stage;
        
        // Check for Load Word Dependency Stalling
        } else if ( is_stalled ) {
            
                cout << "Instruction Decode Stage Has Detected RAW Load Word Dependency!" << endl << endl;

                // Propagate the NO-Op to the Execution stage . . . 
                ex_stage.nop = true;
                
                // Update the newState struct 
                newState.EX = ex_stage;
            
        // Check for Simple No-Op    
        } else {
        
            cout << "Instruction Decode Stage: NOT Operating (No-Op) " << endl << endl;
            
            // Propagate the NO-Op to the Execution stage . . . 
            ex_stage.nop = true;
            
            // Update the newState struct 
            newState.EX = ex_stage;
        }

        
        /* --------------------- IF stage --------------------- */
        
        // If there is an operation on the Instruction Fetch Stage 
        // and the program hasn't yet reached the halt instruction. . . 
        if( !if_stage.nop && !at_halt ){
             
            // If the Execution Stage has been stalled . . .
            if ( is_stalled ) {
            
                cout << "Instruction Fetch Stage: Is Operating " << endl;
                cout << "Momentarily pausing Instruction Fetch due to Load Word Dependency Stalling " << endl << endl;
                
                // Set up the id_stage for operation
                id_stage.nop = false;

                // Update the newState struct 
                newState.IF = if_stage;
                newState.ID = id_stage;
                
            } else {
                cout << "Instruction Fetch Stage: Is Operating " << endl;
                cout << "Current Instruction Address: " << if_stage.PC.to_ulong() << endl;

                // Pass the instruction to the Instruction Decode Stage    
                id_stage.Instr = myInsMem.readInstr( if_stage.PC );

                cout << "Current Instruction: " << id_stage.Instr.to_string() << endl << endl;

                // Check to see if the Instruction Fetch Stage has reached the Halt Instruction
                if ( id_stage.Instr.all() ) {

                    // Set the at_halt bool
                    at_halt = true;

                    // Set the Instruction Fetch stage (the current stage) for NO-operation
                    // Set the Instruction Decode stage (the current stage) for NO-operation
                    if_stage.nop = true;
                    id_stage.nop = true;
                    
                    // Update the newState struct 
                    newState.IF = if_stage;
                    newState.ID = id_stage;
                    
                                        
                // If not at halt, perform usual tasks
                } else {

                    // Set up the id_stage for operation
                    id_stage.nop = false;
                    
                    // Update the Program Counter
                    // If there was a branch . . . 
                    if ( has_branched ) {
                        
                        // Set the PC to the new Branch Address
                        if_stage.PC = PC;
                        
                        // Propagate a No-Op to the Instruction Decode Stage
                        id_stage.nop = true;
                        
                        // Set has_branched bool back to false
                        has_branched = false;
                        
                    } else {
                        
                        // Just increment the program coutner 
                        PC_num = if_stage.PC.to_ulong();
                        if_stage.PC = bitset<32>( PC_num += 4 );
                    
                    }
                    
                    // Update the newState struct 
                    newState.IF = if_stage;
                    newState.ID = id_stage;
                    
                }
                
            }
            
        // Else if there is a NO-Operation on the Instruction Fetch stage 
        // and the program has not yet reached the halt instruction . . . 
        } else if ( if_stage.nop && !at_halt ) {
            
            cout << "Instruction Fetch Stage: NOT Operating (No-Op) " << endl << endl;
            
            // Propagate the NO-Op to the Instruction Decode stage . . . 
            id_stage.nop = true;
            
            // Set up the Instruction Fetch stage (the current stage) for future operation
            if_stage.nop = false;
            
            // Update the newState struct 
            newState.IF = if_stage;
            newState.ID = id_stage;
        
        // Else if there is a NO-Operation on the Instruction Fetch stage 
        // and the program has reached the halt instruction . . . 
        } else if ( if_stage.nop && at_halt ) {
        
            cout << "Instruction Fetch Stage: NOT Operating (due to HALT) " << endl << endl;

            // Pass the Halt Instruction to the Instruction Decode stage
            id_stage.Instr = myInsMem.readInstr( if_stage.PC );
                    
            // Update the newState struct 
            newState.IF = if_stage;
            newState.ID = id_stage;
          
        } 

        
        if (state.IF.nop && state.ID.nop && state.EX.nop && state.MEM.nop && state.WB.nop)
            break;
        
        printState(newState, cycle); //print states after executing cycle 0, cycle 1, cycle 2 ... 
       
        state = newState; /*The end of the cycle and updates the current state with the values calculated in this cycle */ 

        // Increment the Clock Cycle 
        cycle++;
       	
    }
    
    myRF.outputRF(); // dump RF;	
	myDataMem.outputDataMem(); // dump data mem 
	
	return 0;
}
