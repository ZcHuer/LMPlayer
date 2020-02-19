#pragma once

#define _USE_ASYNCTASK_

#define PIPE_NAME(x)						(L"\\\\.\\pipe\\" x)

#ifndef PIPE_DEBUG
#define PIPE_DEBUG	
#endif//#ifndef PIPE_DEBUG

#define PIPEMSG_INVALID						0
#define PIPEMSG_SYSTEM_RESULT				1	//CPipeMessageResult, Param1 = PIPEMSG_SYSTEM_RESULT_*
#define PIPEMSG_SYSTEM_HEART				2	//CPipeMessageHeart
#define PIPEMSG_SYSTEM_OPERATOR				3	//CPipeMessageOperator, Param1 = PIPEMSG_SYSTEM_OP_*
#define PIPEMSG_BASE						1000

#define PIPE_ERROR_OK						0
#define PIPE_ERROR_FAIL						-1
#define PIPE_ERROR_INVALIDARG				-2
#define PIPE_ERROR_MARSHAL					-3
#define PIPE_ERROR_UNMARSHAL				-4
#define PIPE_ERROR_CREATEPIPE				-5
#define PIPE_ERROR_CREATEMSG				-6
#define PIPE_ERROR_READ						-7
#define PIPE_ERROR_WRITE					-8
#define PIPE_ERROR_POST_CREATEPOSTER		-9
#define PIPE_ERROR_POST_ADDTASK				-10
#define PIPE_ERROR_POST_UNMARSHAL			-11
#define PIPE_ERROR_POST_CANCEL				-12
#define PIPE_ERROR_BROADCAST_PARTIAL		-13

#define PIPEMSG_SYSTEM_RESULT_OK			-1000
#define PIPEMSG_SYSTEM_RESULT_FAIL			-1001
#define PIPEMSG_SYSTEM_RESULT_READMSG		-1002
#define PIPEMSG_SYSTEM_RESULT_NOHANDLER		-1003
#define PIPEMSG_SYSTEM_RESULT_UNMARSHALMSG	-1004

/*

Param1 : OP
Param2 : Client Name (Sender)
*/
#define PIPEMSG_SYSTEM_OP_ADDCLIENT			1
#define PIPEMSG_SYSTEM_OP_REMOVECLIENT		2
#define PIPEMSG_SYSTEM_OP_ISRUNNNING		3
#define PIPEMSG_SYSTEM_OP_QUIT				4

