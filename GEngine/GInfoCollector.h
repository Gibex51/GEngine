#pragma once

#include "GStructs.h"

#ifndef GINFOCOLL_H
#define GINFOCOLL_H

struct G_INF_OUTPUT
{
  IDXGIOutput *pOutput;
  DXGI_OUTPUT_DESC Desc;
  vector <DXGI_MODE_DESC> Modes;

  void GetShortResolutionList(vector <DXGI_MODE_DESC> &outList);
};

struct G_INF_ADAPTER
{
  IDXGIAdapter *pAdapter;
  DXGI_ADAPTER_DESC Desc;

  vector <G_INF_OUTPUT> _outputs;

  G_INF_ADAPTER() 
  {
	pAdapter = NULL;
	memset(&Desc, 0, sizeof(DXGI_ADAPTER_DESC));
  }
};

class GInfoCollector
{
public:
	g_uint32 GetAdaptersCount() {return _adapters.size();};
	g_bool GetAdapter(g_uint32 index, G_INF_ADAPTER& out_adapter);

	void GetShortResolutionList(g_uint32 adapter_index, g_uint32 output_index, 
		                        vector <DXGI_MODE_DESC> &resolution_list);

	void CollectInfo();
	void Release();
	//Конструктор и деструктор
	GInfoCollector(void);
	~GInfoCollector(void);
private:
	vector <G_INF_ADAPTER> _adapters;
};

#endif //GINFOCOLL_H
