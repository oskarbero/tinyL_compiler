/*
 *********************************************
 *  314 Principles of Programming Languages  *
 *  Fall 2015                                *
 *  Author: Ulrich Kremer                    *
 *  Student Version                          *
 *********************************************
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "InstrUtils.h"
#include "Utils.h"

#define AR_SIZ 512
int main()
{
	int registers[AR_SIZ][AR_SIZ];
	Instruction *head, *tail;	//tail is for last instruction for the doubly linked list

	memset(registers, 0, sizeof(registers));

	head = ReadInstructionList(stdin);
	if (!head) {
		WARNING("No instructions\n");
		exit(EXIT_FAILURE);
	}

	// Start from last instruction
	tail = LastInstruction( head );

	/*	
	 *	1 means that we've seen the register + offset combo
	 *	-1 means we've seen the tuple but already consumed it
	 *	when we see a tuple that has reference val of 1 we consume it and set it to -1 except in LoadAI
	 */
	while(tail->prev){
		switch(tail->opcode){
			case OUTPUTAI:
				tail->critical = 1;
				registers[tail->field1][tail->field2] = 1;
				break;
			case LOADI:
				if(registers[tail->field2][0] == 1){
					tail->critical = 1;	
					registers[tail->field2][0] = -1;
				}	
				break;
			case LOADAI:
				if(registers[tail->field3][0] == 1){
					tail->critical = 1;
					registers[tail->field3][0] = -1;
					registers[tail->field1][tail->field2] = 1;
				}
				break;
			case STOREAI:
				if(registers[tail->field2][tail->field3] == 1){

					tail->critical = 1;

					registers[tail->field2][tail->field3] = -1;	
					if(registers[tail->field1][0] != -1){
						registers[tail->field1][0] = 1;
					}
				}
				break;
			case ADD:
			case SUB:
			case MUL:
			case DIV:
				if(registers[tail->field3][0] == 1){

					tail->critical = 1;
					registers[tail->field3][0] = -1;

					if(registers[tail->field1][0] != -1)
						registers[tail->field1][0] = 1;
					if(registers[tail->field2][0] != -1)
						registers[tail->field2][0] = 1;
				}
				break;
			default:
				ERROR("Unknown instruction\n");
				break;

		}		
		tail = tail->prev;
	}

	head->critical = 1;

	//go through the list free not-critical instructions
	tail = head;	
	while(tail){
		if(tail->critical == 0){
			if(tail->prev){
				// prev <-  curr -> next
				if(tail->next){

					tail->prev->next = tail->next;
					tail->next->prev = tail->prev;
				}
				else{ // prev<- cur-> NULL
					tail->prev->next = NULL;
				}
			}
			else { // NULL <- cur   aka first node
				if(tail->next) // NULL < - cur -> next
					tail->next->prev = NULL;	
			}

			Instruction *p = tail;
			tail = tail->next;
			free(p);
		}
		else{
			tail = tail->next;
		}
	}

	if(head)
		PrintInstructionList(stdout, head);

	// Could add this to the critical loop but then i wouldnt use PrintInstructionList() which we implemented
	// It would be more efficient though
	while(head){
		Instruction *tmp = head;
		head = head->next;
		free(tmp);
	}


	return EXIT_SUCCESS;
}
