#pragma once

struct VIRTUAL_KEY {
	int vk;
	const char *name;
	const char *desc;
};

extern VIRTUAL_KEY g_vk_list[];

extern int getVkIndex (int vk);
extern const char *pGetVkName (int vk);
extern const char *pGetVkDesc (int vk);
