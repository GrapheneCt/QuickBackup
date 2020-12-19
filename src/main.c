/*
 * Copyright (c) 2020 Graphene
 */

#include <stdio.h>
#include <psp2/appmgr.h>
#include <psp2/power.h>
#include <psp2/sysmodule.h>
#include <psp2/incoming_dialog.h>
#include <psp2/kernel/clib.h>
#include <psp2/kernel/modulemgr.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/iofilemgr.h>

typedef struct SceAppMgrLaunchAppOptParam {
	SceSize size; //0x40
	int launchFlags; //seen: 0x20000(NPXS10016 signin), 0x820000(NPXS10002), 0x100000, 0x30000(videostreaming:), 0x40000(prevents using someCB), 0
	int unk_08; //?
	SceUID appLaunchCB; //-1 if not used
	SceUID getParamCB; //-1 if not used
	SceUID appCloseCB; //-1 if not used
	int unk_18; //seen: 0, 2, 3
	int unk_1C;
	char reserved[0x20];
} SceAppMgrLaunchAppOptParam;

typedef struct SceAppMgrEvent { // size is 0x64
	int event;						/* Event ID */
	SceUID appId;						/* Application ID. Added when required by the event */
	char  param[56];		/* Parameters to pass with the event */
} SceAppMgrEvent;

static int done = 0;

void copycon(char* str1, const char* str2)
{
	while (*str2)
	{
		*str1 = *str2;
		str1++;
		*str1 = '\0';
		str1++;
		str2++;
	}
}

SceInt32 getParamCB(SceUID notifyId, SceInt32 notifyCount, SceInt32 notifyArg, void *pCommon)
{
	char buf[128];
	int ret = -1;
	int dialogShown = 0;
	SceAppMgrEvent appEvent;

	while (ret != 0) {
		sceKernelDelayThread(1000);
		ret = sceAppMgrGetStatusByName("NPXS10092", buf);
	}

	sceSysmoduleLoadModule(SCE_SYSMODULE_INCOMING_DIALOG);

	sceIncomingDialogInit(0);

	SceIncomingDialogParam params;
	sceClibMemset(&params, 0, sizeof(SceIncomingDialogParam));
	params.sdkVersion = SCE_PSP2_SDK_VERSION;
	sceClibStrncpy((char *)params.titleid, "GRVA00014", sizeof(params.titleid));
	params.dialogTimer = 0x7FFFFFF0;
	params.unk_BC = 1;
	copycon((char *)params.buttonRightText, "OK");
	copycon((char *)params.dialogText, "Savedata backup has been created.");
	sceIncomingDialogOpen(&params);

	while (1) {
		_sceAppMgrReceiveEvent(&appEvent);
		if (appEvent.event == 0x20000004 && dialogShown)
			sceAppMgrDestroyAppByAppId(-2);
		else if (appEvent.event == 0x20000004)
			dialogShown = 1;
		sceKernelDelayThread(10000);
	}

	return 0;
}

void _start(unsigned int args, void *argp) {
	
	sceKernelDelayThread(1000 * 1000);

	SceAppMgrLaunchAppOptParam param;
	sceClibMemset(&param, 0, sizeof(SceAppMgrLaunchAppOptParam));
	param.size = 0x40;
	param.launchFlags = 0;
	param.unk_08 = 0;
	param.appLaunchCB = SCE_UID_INVALID_UID;
	param.getParamCB = sceKernelCreateCallback("getParamCB", 0, getParamCB, NULL);;
	param.appCloseCB = SCE_UID_INVALID_UID;
	param.unk_18 = 2;

	sceAppMgrLaunchAppByName2("NPXS10079", NULL, &param);

	while (1) {
		sceKernelDelayThreadCB(1000);
	}
}

