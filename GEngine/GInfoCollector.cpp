#include "GInfoCollector.h"

//-----------------------------------------------

GInfoCollector::GInfoCollector(void)
{

}

//-----------------------------------------------

GInfoCollector::~GInfoCollector(void)
{
  Release();
}

//-----------------------------------------------

void GInfoCollector::CollectInfo()
{
  // Adapters Info
  IDXGIFactory* factory;
  HRESULT hres;

  hres = CreateDXGIFactory(_uuidof(IDXGIFactory), (void**)&factory);
  if (hres == S_OK)
  {
	for (g_uint32 i = 0; ; ++i)
	{
      G_INF_ADAPTER curr_adapter;
	  hres = factory->EnumAdapters(i, &curr_adapter.pAdapter);
	  if (hres != S_OK) break;

	  curr_adapter.pAdapter->GetDesc(&curr_adapter.Desc);
	  
	  for (g_uint32 j = 0; ; ++j)
	  {
		G_INF_OUTPUT curr_output;
	    hres = curr_adapter.pAdapter->EnumOutputs(j, &curr_output.pOutput);
		if (hres != S_OK) break;

		curr_output.pOutput->GetDesc(&curr_output.Desc);

		//Enum Display Resolutions
		g_uint32 NumModes = 0;
		DXGI_MODE_DESC *pModeDesc = NULL;

		hres = curr_output.pOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
                                                       DXGI_ENUM_MODES_SCALING,
                                                       &NumModes, NULL);

		if ((hres != S_OK) || (NumModes == 0))
		  continue;

		pModeDesc = new DXGI_MODE_DESC[NumModes];

		hres = curr_output.pOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
                                                       DXGI_ENUM_MODES_SCALING,
                                                       &NumModes, pModeDesc);

		if (hres == S_OK)
		  for (g_uint32 k = 0; k < NumModes; k++)
            curr_output.Modes.push_back(pModeDesc[k]);

		delete[]pModeDesc;

		curr_adapter._outputs.push_back(curr_output);
	  }

	  _adapters.push_back(curr_adapter);
	}
  }
}

//-----------------------------------------------

void GInfoCollector::Release()
{
  _adapters.clear();
}

//-----------------------------------------------

g_bool GInfoCollector::GetAdapter(g_uint32 index, G_INF_ADAPTER& out_adapter)
{
  if (index >= _adapters.size()) return false;
  out_adapter = _adapters[index];
  return true;
}

//-----------------------------------------------

void GInfoCollector::GetShortResolutionList(g_uint32 adapter_index, g_uint32 output_index, 
											vector <DXGI_MODE_DESC> &resolution_list)
{
  if (GetAdaptersCount() > adapter_index)
  {
	G_INF_ADAPTER curr_adapt;
	GetAdapter(adapter_index, curr_adapt);

	if (curr_adapt._outputs.size() > output_index)
	  curr_adapt._outputs[output_index].GetShortResolutionList(resolution_list); 
  }
}

//-----------------------------------------------

void G_INF_OUTPUT::GetShortResolutionList(vector <DXGI_MODE_DESC> &outList)
{
  outList.clear();
  g_uint32 prev_w = 0, prev_h = 0;
  for (g_int32 i = g_int32(Modes.size()) - 1; i >= 0 ; i--)
  {
	if (((Modes[i].Height == prev_h) && (Modes[i].Width == prev_w)) ||
		((Modes[i].Height < 600) || (Modes[i].Width < 800)))
	  continue;

	if (Modes[i].RefreshRate.Denominator == 0) Modes[i].RefreshRate.Denominator = 1;

	prev_h = Modes[i].Height;
	prev_w = Modes[i].Width;

	outList.push_back(Modes[i]);
  }
}

//-----------------------------------------------
