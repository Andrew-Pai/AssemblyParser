#include <iostream>
#include <fstream>
using namespace std;

/*TODO
Implement offset 
assign all labels their directive
Check Jump Labels
Edit Jump OPCODE verifier to accomdate [label]
Output counters only if no problems
*/

char* lineRead (ifstream& file, int& counter, int& line){
	char* input = new char[100];
	bool space =false;
	char read;	
	counter=0;
	read=file.get();
	while (!file.eof()&&read!='\n'){
		if(read=='#'){
			while(read!='\n'&&!file.eof())
				read=file.get();
		
		}else if(read==' '||read=='\t'){
			if(space){
				space=false;
				input[counter] =' ';
				counter++;
			}
		}else{
			if(read==','&&input[counter-1]==' '){
				input[counter-1]=',';
				space=true;
			}else{
				input[counter]=read;
				counter++;
				space=true;
			}
		}
		if(read!='\n')
			read=file.get();					
	}
	if(counter>0&&(input[counter-1]==' '||input[counter-1]=='\t')){
		input[counter-1]='\0';
		counter--;
	}else
		input[counter]='\0';
	return input;

}

//Find location of colon 
int location(char line[], int counter){
	for (int x=0;x<counter;x++)
		if(line[x]==':')
			return x;
		
	return -1;
}

int directiveVal(char line[], int colLoc, int counter){
	int value=0;
	bool space=true;
	for(int x=colLoc+1;x<counter;x++){//Go through everything after colon
		if(isdigit(line[x])){
			space=false;
			value*=10;
			value+=(line[x]-'0');
		}else{
			if(line[x]=='\0')
				return value;
			else if(!space){//If beginning starts with space ignore it
				return -2;
			}else{
				if(line[x]!=' '&&line[x]!='\t')
					return -2;
			}
		}
	}
	return value;
}
/*
LD-0
LDi-1
SD-2
SDi-3
ADD-4
ADDi-5
SUB-6
SUBi-7
MUL-8
MULi-9
DIV-10
DIVi-11
JMP-12
JZ-13
JNZ-14
JGZ-15
JGEZ-16
JLZ-17
JLEZ-18
*/
//Compare OPCODE to figure out which command it is
bool compare (char arg1[], int codeType){
	char arg2[5];
	for(int x=0;x<5;x++)
		arg2[x]='\0';
	switch(codeType){
		case 0:
			arg2[0]='L';
			arg2[1]='D';
			break;
		case 1:
			arg2[0]='L';
			arg2[1]='D';
			arg2[2]='I';
			break;
		case 2:
			arg2[0]='S';
			arg2[1]='D';
			break;
		case 3:
			arg2[0]='S';
			arg2[1]='D';
			arg2[2]='I';
			break;
		case 4:
			arg2[0]='A';
			arg2[1]='D';
			arg2[2]='D';
			break;
		case 5:
			arg2[0]='A';
			arg2[1]='D';
			arg2[2]='D';
			arg2[3]='I';
			break;
		case 6:
			arg2[0]='S';
			arg2[1]='U';
			arg2[2]='B';
			break;
		case 7:
			arg2[0]='S';
			arg2[1]='U';
			arg2[2]='B';
			arg2[3]='I';
			break;
		case 8:
			arg2[0]='M';
			arg2[1]='U';
			arg2[2]='L';
			break;
		case 9:
			arg2[0]='M';
			arg2[1]='U';
			arg2[2]='L';
			arg2[3]='I';
			break;
		case 10:
			arg2[0]='D';
			arg2[1]='I';
			arg2[2]='V';
			break;
		case 11:
			arg2[0]='D';
			arg2[1]='I';
			arg2[2]='V';
			arg2[3]='I';
			break;
		case 12:
			arg2[0]='J';
			arg2[1]='M';
			arg2[2]='P';
			break;
		case 13:
			arg2[0]='J';
			arg2[1]='Z';
			break;
		case 14:
			arg2[0]='J';
			arg2[1]='N';
			arg2[2]='Z';
			break;
		case 15:
			arg2[0]='J';
			arg2[1]='G';
			arg2[2]='Z';
			break;
		case 16:
			arg2[0]='J';
			arg2[1]='G';
			arg2[2]='E';
			arg2[3]='Z';
			break;
		case 17:
			arg2[0]='J';
			arg2[1]='L';
			arg2[2]='Z';
			break;
		case 18:
			arg2[0]='J';
			arg2[1]='L';
			arg2[2]='E';
			arg2[3]='Z';
			break;
	}

	for(int x=0;x<5;x++){
		if (toupper(arg1[x])!=arg2[x])
			return false;
	}
	return true;
}
//Check if OPERAND is a valid register
bool validReg(int line, char argument[]){
	int x=0;
	while(argument[x]!='\0'){
		if(x>2){
			cerr<<endl<<"Error on Line("<<line<<"): Invalid Extra Data.";
			return false;
		}else if(x==0){
			if(argument[0]!='R'&&argument[0]!='r'){
				return false;
			}
		}else{
			if(!isdigit(argument[x])){
				return false;
			}
			if(x==1&&argument[x]=='0')
				return false;
		}
		x++;
	}
	if(x<2)
		return false;
	return true;
}
//Check if OPERAND is a valid number
bool validNum(int line, char argument[]){
	int x=0;
	while(argument[x]!='\0'&&x!=5){
		if(!isdigit(argument[x])){
			if(argument[x]!='-'&&x>0)
				return false;

		}
		
		x++;
	}
	if(!x)
		return false;
	return true;
}
//Check if OPERAND is a vaid address
bool validAdd(int line, char argument[]){
	int x=0;
		while(argument[x]!='\0'&&x!=5){
			if(x==4){
				cerr<<endl<<"Error on Line("<<line<<"): Invalid Extra Data.";
				return false;
			}else{
				if(!isdigit(argument[x])){
					cerr<<endl<<"Error on Line("<<line<<"): Invalid Extra Characters.";
					return false;
				}
			}
			x++;
		}
	
	return true;
}

bool validLabel(char label[], char storedLabel[50][100], int numLabel){
	bool valid;
	int y=0;

	for (int x=0;x<numLabel;x++){
		valid=true;
		y=0;
		//&&storedLabel[x][y]!='\0'
		while(label[y]!='\0'||(storedLabel[x][y]!=':'&&storedLabel[x][y]!='\0')){
			if(label[y]!=storedLabel[x][y]){
				valid=false;
				break;
			}
			y++;
		}
		if(valid){
			return true;
		}
	}
	return valid;
}
/*
0-Register, Address
1-Address, Register
2-Register, Register
3-Number, Register
4-Number, Address
5-Register, Register, Register
6-Register, Number, Register
7-Address
*/
/*
Send in label to be compared
searches through stored labels till match is found
assigns label a directive

cases:
CODE is sent in
*/

bool labelCompare(char label[], char storedLabel[50][100],int numLabel, int offset, int code, int line){
	bool valid;
	
	for(int x=0;x<numLabel;x++){
		int y=0;
		valid=true;
		while(label[y]!='\0'||(storedLabel[x][y]!=':'&&storedLabel[x][y]!='\0')){
			/*
			Check which label matches and if defined already
			*/
			if(label[y]!=storedLabel[x][y]){
				valid=false;
				break;
			}
			y++;
		}
		if(valid){
			if(storedLabel[x][y]!='\0'){
				cerr<<endl<<"Error on line("<<line<<"): Duplicate label";
				return false;
			}else{
				storedLabel[x][y]=':';
				storedLabel[x][++y]=' ';
				y++;
				int value;
				if(offset==-1){
					value=code;
				}else
					value=(offset+code);
				int div =1;
				while((int)(value/(div*10))){
					div*=10;
				}
				while(div){					
					storedLabel[x][y]=(value/div)+'0';
					value-=(value/div)*div;
					y++;
					div/=10;
				}
				storedLabel[x][y]='\0';
				return true;
			}
		}
		
	}
	cerr<<endl<<"Error on line("<<line<<"). Invalid Missing Label";
	return valid;
}



/*
Processing Labels:
	Check if> 'Code:' or 'Data:'
		if code or data>read number after to assign to directive
	Or if valid label> Only contains numbers and letters. Does not start with number
		Find associating value to label and somehow return
*/
bool lineProcess(char line[],int counter, int& offset, int lineNum, int& code,int& data,int numInstruc[], char storedLabel[50][100],int numLabel){
	int arg=0, pos=0, numArg=1;
	bool valid=true;
	int colLoc=location(line,counter);//Check if current line contains colon
	if(colLoc>=0){//Process Labels
		char label[colLoc+1];
		int value=-1;
		if(colLoc>0){
			
			for (int x=0;x<colLoc;x++){
				label[x]=line[x];
			}
			label[colLoc]='\0';
				
			if(colLoc==4){
				if (label[pos]=='c'||label[pos]=='C'){//Check if code directive
					if (label[pos+1]=='o'||label[pos+1]=='O')
						if (label[pos+2]=='d'||label[pos+2]=='D')
							if (label[pos+3]=='e'||label[pos+3]=='E'){
								//Get address
								value=directiveVal(line,colLoc,counter);
								if(value>=0){
									code=value;
									valid=labelCompare(label,storedLabel,numLabel,-1,code,lineNum);
								}else
									cerr<<endl<<"Error on line("<<lineNum<<"), invalid code directive: ";
								//lineError; //Invalid address
							}
				}else if (label[pos]=='d'||label[pos]=='D')//Check if data directive
					if (label[pos+1]=='a'||label[pos+1]=='A')
						if (label[pos+2]=='t'||label[pos+2]=='T')
							if (label[pos+3]=='a'||label[pos+3]=='A'){
								value=directiveVal(line,colLoc,counter);
								if(value>=0){
									valid=labelCompare(label,storedLabel,numLabel,-1,value,lineNum);
								}else
									cerr<<endl<<"Error on line("<<lineNum<<"), invalid data directive: "<<value;
							}
			}
			if(value<0){//If label is neither Code: nor Data:
				valid=true;
				if(colLoc+1==counter){
					for(int x=0;x<colLoc;x++){
						if(!isalnum(label[x])){
							cerr<<endl<<"Error on line("<<lineNum<<"): Invalid label. Non AlphaNumeric Character Detected.";
							valid=false;
						}
						 if(x==0&&label[x]>='0'&&label[x]<='9'){
							cerr<<endl<<"Error on line("<<lineNum<<"): Invalid label. Does not start with Letter.";
							valid=false;
						}
					}
					valid=valid&labelCompare(label,storedLabel,numLabel,offset,code,lineNum);
				}else{
					cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Label. Extra Invalid Arguments found.";
				}
			}
		}else
			cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Missing Label";
						
	}else if(line[0]!='\0'){
		char label[counter];
		bool next=false, jump=false;
		valid=true;
		int y=0;
		if(code==-1){
			code=-2;
			cerr<<endl<<"Error on line(1): Invalid Missing Code Directive";
			valid=false;
		}		

		arg=0;
		pos=0;
		char argument[5][counter];//[arg][pos]
		for(int x=0;x<5;x++)
			for (int y=0;y<counter;y++)
				argument[x][y]='\0';
		for(int x=0;x<counter;x++){
			if(arg==5){
				cerr<<endl<<"Error on line("<<lineNum<<"): Extra Invalid OPERAND found";
				valid=false;
			}else if(jump){
				if(!isalnum(line[x])){
					if(line[x]==']'){
						if(y>0&&line[x+1]=='\0'){
							jump=false;
							label[y]='\0';
							argument[arg][pos]='\0';
							arg++;
							next=true;
						}else{
							valid=false;
							label[y]='\0';
							if(line[x+1]!='\0')
								cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Extra Label.";
							else
								cerr<<endl<<"Error on line("<<lineNum<<"): Missing Invalid Label.";
						}
					}else{
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid character found";
						valid=false;
					}
				}else{
					label[y]=line[x];
					argument[arg][pos]=line[x];
					pos++;
				}
				if(y==0&&isdigit(label[y])){
					cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Extra label, starts with number.";
					valid=false;
				}
				y++;
			}else if(line[x]=='['){
				if(line[x-1]!=' '&&line[x-1]!=',')
					cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Extra Label.";
				jump=true;

			}else if(next&&(line[x]==','||line[x]==' ')){
				if(numArg==1){
					if(line[x]==' '){
						arg++;
						numArg++;
						pos=0;
						next = false;
					}else{
						cerr<<"Error on Line("<<lineNum<<"): Invalid Extra OPERANDS";
						valid=false;
					}
				}else{
					if((line[x]==',')){
						arg++;
						numArg++;
						pos=0;
						next=false;
					}
					else if(line[x]==' '){
						cerr<<"Error on Line("<<lineNum<<"): Invalid Extra OPERANDS";
						valid=false;
					}

				}
				/*
				if(line[x-1]!=','&&line[x]==' '&&numArg==1){
					arg++;
					numArg++;
					pos=0;
				}else if(line[x]==','){
					arg++;
					numArg++;
					pos=0;
				}
				if(line[x]==' ')
					next=false;
				*/
			}else {
				if(line[x]!=' '){
					argument[arg][pos]=line[x];
					next=true;
					pos++;
				}
			}
			//LD R1, R2
		}
		if(valid){
			
			//for(int x=0;x<5;x++)
			//	cout<<endl<<argument[x]<<".";
			//switch and case
			//void argumentCheck(char argument[][5], int numArg, int type, int line){
			if(compare(argument[0],0)){//LD
				if(numArg>3){
					cerr<<endl<<"Error on line("<<lineNum<<"):Extra Invalid argument";
					valid=false;
				}else if(numArg<3){
					cerr<<endl<<"Error on line("<<lineNum<<"):Invalid Missing OPERAND";
					valid=false;
				}else{
					if(!validAdd(lineNum,argument[1])&&!validReg(lineNum,argument[1])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid address or register at OPERAND 1. Found:"<<argument[1];
						valid=false;
					}
					if(!validReg(lineNum,argument[2])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Register at OPERAND 2. Found:"<<argument[2];
						valid=false;
					}
					numInstruc[0]++;
					offset++;
				}
					
			}else if(compare(argument[0],1)){//LDI
				if(numArg>3){
					cerr<<endl<<"Error on line("<<lineNum<<"): Extra Invalid OPERAND";
					valid=false;
				}else if(numArg<3){
					cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Missing OPERAND";
					valid=false;
				}else{
					if(!validNum(lineNum,argument[1])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid number at OPERAND 1. Found:"<<argument[1];
						valid=false;
					}
					if(!validReg(lineNum,argument[2])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Register at OPERAND 2. Found:"<<argument[2];
						valid=false;
					}
					if(argument[1][0]=='\0'||argument[2][0]=='\0'){
						cerr<<endl<<"Error on line ("<<lineNum<<"): Invalid Missing OPERAND.";
						valid=false;
					}
					numInstruc[0]++;
					offset++;
				}
			}else if(compare(argument[0],2)){//SD
				if(numArg>3){
					cerr<<endl<<"Error on line("<<lineNum<<"): Extra Invalid OPERAND";
					valid=false;
				}else if(numArg<3){
					cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Missing OPERAND";
					valid=false;
				}else{
					if(!validReg(lineNum,argument[1])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Register at OPERAND 1. Found:"<<argument[1];
						valid=false;
					}
					if(!validAdd(lineNum,argument[2])&&!validReg(lineNum,argument[2])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Reg or Address at OPERAND 2. Found:"<<argument[2];
						valid=false;
					}
					if(argument[1][0]=='\0'||argument[2][0]=='\0'){
						cerr<<endl<<"Error on line ("<<lineNum<<"): Invalid Missing OPERAND.";
						valid=false;
					}
					numInstruc[0]++;
					offset++;
				}
			}else if(compare(argument[0],3)){//SDi
				if(numArg>3){
					cerr<<endl<<"Error on line("<<lineNum<<"): Extra Invalid OPERAND";
					valid=false;
				}else if(numArg<3){
					cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Missing OPERAND";
					valid=false;
				}else{
					if(!validNum(lineNum,argument[1])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid number at OPERAND 1.";
						valid=false;
					}
					if(!validAdd(lineNum,argument[2])&&!validReg(lineNum,argument[2])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid address or register at OPERAND 2. Found:"<<argument[2];
						valid=false;
					}
					if(argument[1][0]=='\0'||argument[2][0]=='\0'){
						cerr<<endl<<"Error on line ("<<lineNum<<"): Invalid Missing OPERAND.";
						valid=false;
					}
					numInstruc[0]++;
					offset++;
				}
			}else if(compare(argument[0],4)){//ADD
				if(numArg>4){
					cerr<<endl<<"Error on line("<<lineNum<<"): Extra Invalid OPERAND";
					valid=false;
				}else if(numArg<4){
					cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Missing OPERAND";
					valid=false;
				}else{
					if(!validReg(lineNum,argument[1])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Register, OPERAND 1. Found:"<<argument[1];
						valid=false;
					}
					if(!validReg(lineNum,argument[2])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Register, OPERAND 2. Found:"<<argument[2];
						valid=false;
					}
					if(!validReg(lineNum,argument[3])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Register, OPERAND 3";
						valid=false;
					}
					if(argument[1][0]=='\0'||argument[2][0]=='\0'||argument[3][0]=='\0'){
						cerr<<endl<<"Error on line ("<<lineNum<<"): Invalid Missing OPERAND.";
						valid=false;
					}
					numInstruc[1]++;
					offset++;
				}
			}else if(compare(argument[0],5)){//ADDi
				if(numArg>4){
					cerr<<endl<<"Error on line("<<lineNum<<"): Extra Invalid OPERAND";
					valid=false;
				}else if(numArg<4){
					cerr<<endl<<"Error: on line("<<lineNum<<"): Invalid Missing OPERAND";
					valid=false;
				}else{
					if(!validReg(lineNum,argument[1])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Register, OPERAND 1. Found:"<<argument[1];
						valid=false;
					}
					if(!validNum(lineNum,argument[2])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Number, OPERAND 2. Found:"<<argument[2];
						valid=false;
					}
					if(!validReg(lineNum,argument[3])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Register, OPERAND 3";
						valid=false;
					}
					if(argument[1][0]=='\0'||argument[2][0]=='\0'||argument[3][0]=='\0'){
						cerr<<endl<<"Error on line ("<<lineNum<<"): Invalid Missing OPERAND.";
						valid=false;
					}
					numInstruc[1]++;
					offset++;
				}
			}else if(compare(argument[0],6)){//SUB
				if(numArg>4){
					cerr<<endl<<"Error on line("<<lineNum<<"): Extra Invalid OPERAND";
					valid=false;
				}else if(numArg<4){
					cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Missing OPERAND";
					valid=false;
				}else{
					if(!validReg(lineNum,argument[1])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Register, OPERAND 1. Found:"<<argument[1];
						valid=false;
					}
					if(!validReg(lineNum,argument[2])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Register, OPERAND 2. Found:"<<argument[2];
						valid=false;
					}
					if(!validReg(lineNum,argument[3])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Register, OPERAND 3";
						valid=false;
					}
					if(argument[1][0]=='\0'||argument[2][0]=='\0'||argument[3][0]=='\0'){
						cerr<<endl<<"Error on line ("<<lineNum<<"): Invalid Missing OPERAND.";
						valid=false;
					}
					numInstruc[1]++;
					offset++;
				}
			}else if(compare(argument[0],7)){//SUBi
				if(numArg>4){
					cerr<<endl<<"Error on line("<<lineNum<<"): Extra Invalid OPERAND";
					valid=false;
				}else if(numArg<4){
					cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Missing OPERAND";
					valid=false;
				}else{
					if(!validReg(lineNum,argument[1])){
						cerr<<endl<<"Error: Invalid Register, OPERAND 1. Found:"<<argument[1];
						valid=false;
					}
					if(!validNum(lineNum,argument[2])){
						cerr<<endl<<"Error: Invalid Number, OPERAND 2. Found:"<<argument[2];
						valid=false;
					}
					if(!validReg(lineNum,argument[3])){
						cerr<<endl<<"Error: Invalid Register, OPERAND 3";
						valid=false;
					}
					if(argument[1][0]=='\0'||argument[2][0]=='\0'||argument[3][0]=='\0'){
						cerr<<endl<<"Error on line ("<<lineNum<<"): Invalid Missing OPERAND.";
						valid=false;
					}
					numInstruc[1]++;
					offset++;
				}
			}else if(compare(argument[0],8)){//MUL
				if(numArg>4){
					cerr<<endl<<"Error on line("<<lineNum<<"): Extra Invalid OPERAND";
					valid=false;
				}else if(numArg<4){
					cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Missing OPERAND";
					valid=false;
				}else{
					if(!validReg(lineNum,argument[1])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Register, OPERAND 1. Found:"<<argument[1];
						valid=false;
					}
					if(!validReg(lineNum,argument[2])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Register, OPERAND 2. Found:"<<argument[2];
						valid=false;
					}
					if(!validReg(lineNum,argument[3])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Register, OPERAND 3";
						valid=false;
					}
					if(argument[1][0]=='\0'||argument[2][0]=='\0'||argument[3][0]=='\0'){
						cerr<<endl<<"Error on line ("<<lineNum<<"): Invalid Missing OPERAND.";
						valid=false;
					}
					numInstruc[1]++;
					offset++;
				}
			}else if(compare(argument[0],9)){//MULi
				if(numArg>4){
					cerr<<endl<<"Error on line("<<lineNum<<"): Extra Invalid OPERAND";
					valid=false;
				}else if(numArg<4){
					cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Missing OPERAND";
					valid=false;
				}else{
					if(!validReg(lineNum,argument[1])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Register, OPERAND 1. Found:"<<argument[1];
						valid=false;
					}
					if(!validNum(lineNum,argument[2])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Number, OPERAND 2. Found:"<<argument[2];
						valid=false;
					}
					if(!validReg(lineNum,argument[3])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Register, OPERAND 3";
						valid=false;
					}
					if(argument[1][0]=='\0'||argument[2][0]=='\0'||argument[3][0]=='\0'){
						cerr<<endl<<"Error on line ("<<lineNum<<"): Invalid Missing OPERAND.";
						valid=false;
					}
					numInstruc[1]++;
					offset++;
				}
			}else if(compare(argument[0],10)){//DIV
				if(numArg>4){
					cerr<<endl<<"Error on line("<<lineNum<<"): Extra Invalid OPERAND";
					valid=false;
				}else if(numArg<4){
					cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Missing OPERAND";
					valid=false;
				}else{
					if(!validReg(lineNum,argument[1])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Register, OPERAND 1. Found:"<<argument[1];
						valid=false;
					}
					if(!validReg(lineNum,argument[2])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Register, OPERAND 2. Found:"<<argument[2];
						valid=false;
					}
					if(!validReg(lineNum,argument[3])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Register, OPERAND 3";
						valid=false;
					}
					if(argument[1][0]=='\0'||argument[2][0]=='\0'||argument[3][0]=='\0'){
						cerr<<endl<<"Error on line ("<<lineNum<<"): Invalid Missing OPERAND.";
						valid=false;
					}
					numInstruc[1]++;
					offset++;
				}
			}else if(compare(argument[0],11)){//DIVi
				if(numArg>4){
					cerr<<endl<<"Error on line("<<lineNum<<"): Extra Invalid OPERAND";
					valid=false;
				}else if(numArg<4){
					cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Missing OPERAND";
					valid=false;
				}else{
					if(!validReg(lineNum,argument[1])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Register, OPERAND 1. Found:"<<argument[1];
						valid=false;
					}
					if(!validNum(lineNum,argument[2])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Number, OPERAND 2. Found:"<<argument[2];
						valid=false;
					}
					if(!validReg(lineNum,argument[3])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Register, OPERAND 3";
						valid=false;
					}
					if(argument[1][0]=='\0'||argument[2][0]=='\0'||argument[3][0]=='\0'){
						cerr<<endl<<"Error on line ("<<lineNum<<"): Invalid Missing OPERAND.";
						valid=false;
					}
					numInstruc[1]++;
					offset++;
				}
			}else if(compare(argument[0],12)){//JMP
				if(numArg>2){
					cerr<<endl<<"Error on line("<<lineNum<<"): Extra Invalid OPERAND";
					valid=false;
				}else if(numArg<2){
					cerr<<endl<<"Error: on line("<<lineNum<<"): Invalid Missing OPERAND";
					valid=false;
				}else{
					if(!validAdd(lineNum,argument[1])&&(!validLabel(label,storedLabel, numLabel)||!validLabel(argument[1],storedLabel, numLabel))){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Missing Address/Label"<<line;
						valid=false;
					}
					if(argument[1][0]=='\0'&&label[0]!='\0'){
						cerr<<endl<<"Error on line ("<<lineNum<<"): Invalid Missing OPERAND.";
						valid=false;
					}
					numInstruc[2]++;
					offset++;
				}
			}else if(compare(argument[0],13)){//JZ
				if(numArg>3){
					cerr<<endl<<"Error on line("<<lineNum<<"): Extra Invalid OPERAND";
					valid=false;
				}else if(numArg<3){
					cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Missing OPERAND";
					valid=false;
				}else{
					if(!validReg(lineNum,argument[1])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Register at OPERAND 1. Found:"<<argument[1];
						valid=false;
					}
					if(!validAdd(lineNum,argument[2])&&(!validLabel(label,storedLabel, numLabel)||!validLabel(argument[2],storedLabel, numLabel))){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Missing Address/Label"<<line;
						valid=false;
					}
					if((argument[1][0]=='\0'||argument[2][0]=='\0')&&label[0]!='\0'){
						cerr<<endl<<"Error on line ("<<lineNum<<"): Invalid Missing OPERAND.";
						valid=false;
					}
					numInstruc[2]++;
					offset++;
				}
			}else if(compare(argument[0],14)){//JNZ
				if(numArg>3){
					cerr<<endl<<"Error on line("<<lineNum<<"): Extra Invalid OPERAND";
					valid=false;
				}else if(numArg<3){
					cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Missing OPERAND";
					valid=false;
				}else{
					if(!validReg(lineNum,argument[1])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Register at OPERAND 1. Found:"<<argument[1];
						valid=false;
					}
					if(!validAdd(lineNum,argument[2])&&(!validLabel(label,storedLabel, numLabel)||!validLabel(argument[2],storedLabel, numLabel))){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Missing Address/Label"<<line;
						valid=false;
					}
					if((argument[1][0]=='\0'||argument[2][0]=='\0')&&label[0]!='\0'){
						cerr<<endl<<"Error on line ("<<lineNum<<"): Invalid Missing OPERAND.";
						valid=false;
					}
					numInstruc[2]++;
					offset++;
				}

			}else if(compare(argument[0],15)){//JGZ
				if(numArg>3){
					cerr<<endl<<"Error on line("<<lineNum<<"): Extra Invalid OPERAND";
					valid=false;
				}else if(numArg<3){
					cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Missing OPERAND";
					valid=false;
				}else{
					if(!validReg(lineNum,argument[1])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Register at OPERAND 1. Found:"<<argument[1];
						valid=false;
					}
					if(!validAdd(lineNum,argument[2])&&(!validLabel(label,storedLabel, numLabel)||!validLabel(argument[2],storedLabel, numLabel))){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Missing Address/Label."<<line;
						valid=false;
					}
					if((argument[1][0]=='\0'||argument[2][0]=='\0')&&label[0]!='\0'){
						cerr<<endl<<"Error on line ("<<lineNum<<"): Invalid Missing OPERAND.";
						valid=false;
					}
					numInstruc[2]++;
					offset++;
				}

			}else if(compare(argument[0],16)){//JGEZ
				if(numArg>3){
					cerr<<endl<<"Error on line("<<lineNum<<"): Extra Invalid OPERAND";
					valid=false;
				}else if(numArg<3){
					cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Missing OPERAND";
					valid=false;
				}else{
					if(!validReg(lineNum,argument[1])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Register at OPERAND 1. Found:"<<argument[1];
						valid=false;
					}
					if(!validAdd(lineNum,argument[2])&&(!validLabel(label,storedLabel, numLabel)||!validLabel(argument[2],storedLabel, numLabel))){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Missing Address/Label"<<line;
						valid=false;
					}
					if((argument[1][0]=='\0'||argument[2][0]=='\0')&&label[0]!='\0'){
						cerr<<endl<<"Error on line ("<<lineNum<<"): Invalid Missing OPERAND.";
						valid=false;
					}
					numInstruc[2]++;
					offset++;
				}
			}else if(compare(argument[0],17)){//JLZ
				if(numArg>3){
					cerr<<endl<<"Error on line("<<lineNum<<"): Extra Invalid OPERAND";
					valid=false;
				}else if(numArg<3){
					cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Missing OPERAND";
					valid=false;
				}else{
					if(!validReg(lineNum,argument[1])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Register at OPERAND 1. Found:"<<argument[1];
						valid=false;
					}
					if(!validAdd(lineNum,argument[2])&&(!validLabel(label,storedLabel, numLabel)||!validLabel(argument[2],storedLabel, numLabel))){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Missing Address/Label"<<line;
						valid=false;
					}
					if((argument[1][0]=='\0'||argument[2][0]=='\0')&&label[0]!='\0'){
						cerr<<endl<<"Error on line ("<<lineNum<<"): Invalid Missing OPERAND.";
						valid=false;
					}
					numInstruc[2]++;
					offset++;
				}
			}else if(compare(argument[0],18)){//JLEZ
				if(numArg>3){
					cerr<<endl<<"Error on line("<<lineNum<<"): Extra Invalid OPERAND";
					valid=false;
				}else if(numArg<3){
					cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Missing OPERAND";
					valid=false;
				}else{
					if(!validReg(lineNum,argument[1])){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Register at OPERAND 1. Found:"<<argument[1];
						valid=false;
					}
					if(!validAdd(lineNum,argument[2])&&(!validLabel(label,storedLabel, numLabel)||!validLabel(argument[2],storedLabel, numLabel))){
						cerr<<endl<<"Error on line("<<lineNum<<"): Invalid Missing Address/Label"<<line;
						valid=false;
					}
					if((argument[1][0]=='\0'||argument[2][0]=='\0')&&label[0]!='\0'){
						cerr<<endl<<"Error on line ("<<lineNum<<"): Invalid Missing OPERAND.";
						valid=false;
					}
					numInstruc[2]++;
					offset++;
				}
			}else{
				cerr<<endl<<"Error on line("<<lineNum<<"): Invalid OPCODE";
				valid=false;
			}
			
		}
	}
	return valid;
}

/*
LD-0
LDi-1
SD-2
SDi-3
ADD-4
ADDi-5
SUB-6
SUBi-7
MUL-8
MULi-9
DIV-10
DIVi-11
JMP-12
JZ-13
JNZ-14
JGZ-15
JGEZ-16
JLZ-17
JLEZ-18
*/

	
//void lineProcess(char line[],int counter, int& offset, int lineNum, int& code,int& data,int& lsData,int& ALU, int& cjMP){
int main(int argc,char* argv[]){
	bool valid=true;
	int counter =0,offset=0, line=1, code=-1, data=0;
	int numInstruc[3];
	for(int x=0;x<3;x++)
		numInstruc[x]=0;
	char* fileInput =new char[100];
	char label[50][100];
	ifstream readLabel(argv[1]);
	if(argc<2){
		cerr<<endl<<"Error: Missing Invalid Arguments. /Parser <file name>";
	}else if(argc>2){
		cerr<<endl<<"Error: Extra Invalid arguments. Expected: /Parser <file name>";
	}else{
		if(!readLabel.good())//Check if file exists
			cerr<<endl<<"Error: FILE NOT FOUND. "<<argv[1]<<" is unable to open";
		else{
			int numLabel=0,pos=0;
			char read;
			bool save;
			bool comment;
			read=readLabel.get();
			while(!readLabel.eof()){
				save=false;
				comment=false;
				pos=0;
				
				while(read!='\n'&&!readLabel.eof()){
					if(read==':'){
						save=true;
					}else if(!save&&read=='#'){
						comment=true;

					}else if(!save&&!comment){
						if(read!=' '&&read!='\t'){
						label[numLabel][pos]=read;
						pos++;
						}
					}
					read=readLabel.get();
				}
				if(save&&!comment){
					label[numLabel][pos]='\0';
					numLabel++;
				}
				read=readLabel.get();
			}
			pos=0;
			char check[]="CODE";
			if(numLabel>0){
				while(label[0][pos]!='\0'||check[pos]!='\0'){
					
					if(toupper(label[0][pos])!=check[pos]){
						cerr<<"Error on line(1): Missing Code Directive.";
						valid=false;
						break;
					}
					pos++;
				}
			}else{
				cerr<<"Error on line(1): Missing Code Directive.";
				valid=false;
			}
			readLabel.close();	
			ifstream file(argv[1]);//Declare object file for class ifstream
			while(!file.eof()){
				fileInput=lineRead(file,counter, line);
				//Implement offset, label, labelValue, valid, lsdata, alu, cjmp
				valid=valid&lineProcess(fileInput,counter,offset,line,code,data,numInstruc, label,numLabel);
				line++;
				//cout<<fileInput<<endl;
			}
			file.close();
			if(valid){
				for(int x=0;x<numLabel;x++)
					cout<<endl<<label[x];
				//cout<<endl<<label[0];
				//cout<<endl<<label[1];
				int total =numInstruc[0]+numInstruc[1]+numInstruc[2];
				cout<<endl;
				cout<<"Total # of Assembly Instructions: "<<total<<endl;
				cout<<"# of load/store: "<<numInstruc[0]<<endl;
				cout<<"# of ALU: "<<numInstruc[1]<<endl;
				cout<<"# of compare and jump: "<< numInstruc[2]<<endl;
	
			}
		}
	}
}
