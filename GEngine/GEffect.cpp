#include "GEffect.h"

//-----------------------------------------------
//  GEffectVariable
//-----------------------------------------------

GEffectVariable::GEffectVariable()
{
  _dx_effect_var = NULL;
}

//-----------------------------------------------

GEffectVariable::GEffectVariable(ID3D10EffectVariable *dx_effect_var)
{
  _dx_effect_var = dx_effect_var;

  _shader_buffer = NULL;
  _texture_buffer = NULL;

  _buf_elem_count = 0;
  _buf_elem_size = 0;
}

//-----------------------------------------------

GEffectVariable::~GEffectVariable()
{
  _release_buffers();
}

//-----------------------------------------------

void GEffectVariable::GetRawData(void **ppdata, g_uint32 data_sz)
{
  _dx_effect_var->GetRawValue(*ppdata, 0, data_sz);
}

//-----------------------------------------------

void GEffectVariable::SetRawData(void *pdata, g_uint32 data_sz)
{
  _dx_effect_var->SetRawValue(pdata, 0, data_sz);
}

//-----------------------------------------------

void GEffectVariable::SetMatrix(const g_mat4x4 &matrix)
{
  _dx_effect_var->AsMatrix()->SetMatrix((g_float*)&matrix);
}

//-----------------------------------------------

void GEffectVariable::SetMatrixArray(g_mat4x4 *matrixarray, g_uint32 elements)
{
  _dx_effect_var->AsMatrix()->SetMatrixArray((float*)matrixarray, 0, elements);
}

//-----------------------------------------------

g_bool GEffectVariable::SetTexture(GTexture *Texture)
{
  if ((this == NULL) && (_dx_effect_var == NULL)) return false;

  ID3D10EffectShaderResourceVariable *res_var = _dx_effect_var->AsShaderResource();
  
  if (Texture != NULL)
    res_var->SetResource(Texture->Texture);
  else
	res_var->SetResource(NULL);

  return true;
}

//-----------------------------------------------

g_bool GEffectVariable::SetTextureArray(vector <GTexture*> TexArray, GD3DDevice *Device)
{
  if ((this == NULL) && (_dx_effect_var == NULL)) return false;

  ID3D10Resource *pRes = NULL;
  ID3D10Texture2D *pTex2D = NULL;
  ID3D10ShaderResourceView *pSRV = NULL;

  D3D10_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(D3D10_TEXTURE2D_DESC));

  ID3D10EffectShaderResourceVariable *TMP = _dx_effect_var->AsShaderResource();
  if (TMP != NULL)
  {
	  for (g_uint8 i = 0; i < g_uint8(TexArray.size()); i++)
	  {
        pRes = TexArray[i]->TextureRes;
		if( pRes )
        {
            ID3D10Texture2D* pTemp;
            pRes->QueryInterface( __uuidof( ID3D10Texture2D ), ( LPVOID* )&pTemp );
			if (pTemp != NULL)
              pTemp->GetDesc( &desc );

            D3D10_MAPPED_TEXTURE2D mappedTex2D;
			ZeroMemory(&mappedTex2D,sizeof(D3D10_MAPPED_TEXTURE2D));

            if( desc.MipLevels > 4 )
                desc.MipLevels -= 4;
            if( !(pTex2D) )
            {
                desc.Usage = D3D10_USAGE_DEFAULT;
                desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
                desc.CPUAccessFlags = 0;
                desc.ArraySize = TexArray.size();

                HRESULT hr = Device->D3DDevice->CreateTexture2D( &desc, NULL, &pTex2D );
				if (hr != S_OK)
				  return false;
            }

            for( UINT iMip = 0; iMip < desc.MipLevels; iMip++ )
			{
			  if (pTemp->Map( iMip, D3D10_MAP_READ, 0, &mappedTex2D ) == S_OK)
			  {
                Device->D3DDevice->UpdateSubresource(pTex2D,
                                                     D3D10CalcSubresource( iMip, i, desc.MipLevels ),
                                                     NULL,
                                                     mappedTex2D.pData,
                                                     mappedTex2D.RowPitch,
                                                     0 );

                pTemp->Unmap( iMip );
			  }
			}

			if (pTemp != NULL)
              pTemp->Release();
			pTemp = NULL;
			pRes = NULL;
        }
        else
        {
          return false;
        }
	  }

	  D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc;
      ZeroMemory( &SRVDesc, sizeof( SRVDesc ) );
      SRVDesc.Format = desc.Format;
      SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2DARRAY;
      SRVDesc.Texture2DArray.MipLevels = desc.MipLevels;
      SRVDesc.Texture2DArray.ArraySize = TexArray.size();
	  if (pTex2D != NULL)
        Device->D3DDevice->CreateShaderResourceView( pTex2D, &SRVDesc, &pSRV );

      TMP->SetResource(pSRV);
  }
  else 
	return false;
  return true;
}

//-----------------------------------------------

inline void GEffectVariable::_release_buffers()
{
  FREENULL_VAR(_shader_buffer);
  FREENULL_VAR(_texture_buffer);
  _buf_elem_count = 0;
  _buf_elem_size = 0;
}

//-----------------------------------------------

g_bool GEffectVariable::InitTextureBuffer(GD3DDevice *Device, g_uint32 elem_count, 
										  g_uint32 elem_size, DXGI_FORMAT elem_format)
{
  _release_buffers();

  _buf_elem_count = elem_count;
  _buf_elem_size = elem_size;

  _shader_buffer = Device->BufferList->CreateBuffer(G_BT_SHADERRES, G_BF_DEFAULT, elem_size,
                                                    elem_count, NULL, 0);

  if (_shader_buffer != NULL)
  {
	_texture_buffer = new GTexture("tex_buffer", Device);
    if (_texture_buffer->CreateFromBuffer(&_shader_buffer->Buffer, elem_count, elem_format))
      return true;
  }

  _release_buffers();
  return false;
}

//-----------------------------------------------

g_bool GEffectVariable::SetTextureBuffer(GD3DDevice *Device, void *Buffer)
{
  //Update Buffer In Texture
  if ((_texture_buffer == NULL) || (Buffer == NULL)) return false;

  D3D10_BOX box = {0, 0, 0, _buf_elem_size*_buf_elem_count, 1, 1};
  Device->D3DDevice->UpdateSubresource(_texture_buffer->TextureRes, 0, &box, 
		                               Buffer, _buf_elem_size, 0);

  //Set To Shader
  if (_dx_effect_var != NULL)
    _dx_effect_var->AsShaderResource()->SetResource(_texture_buffer->Texture);

  return true;
}

//-----------------------------------------------
//  GEffectTech
//-----------------------------------------------

GEffectTech::GEffectTech()
{
  _dx_effect_tech = NULL;
}

//-----------------------------------------------

GEffectTech::GEffectTech(ID3D10EffectTechnique *dx_effect_tech)
{
  _dx_effect_tech = dx_effect_tech;

  if (_dx_effect_tech != NULL)
  {
	D3D10_TECHNIQUE_DESC techDesc;
    _dx_effect_tech->GetDesc( &techDesc );
    _pass_count = techDesc.Passes;
	_name = techDesc.Name;
  }
  else
  {
    _pass_count = 0;
	_name = "";
  }
}

//-----------------------------------------------

GEffectTech::~GEffectTech()
{
  
}

//-----------------------------------------------

void GEffectTech::ApplyPass(g_uint32 index)
{
  _dx_effect_tech->GetPassByIndex(index)->Apply(0);
}

//-----------------------------------------------

void GEffectTech::ApplyPass(const g_string& name)
{
  _dx_effect_tech->GetPassByName(name.c_str())->Apply(0);
}

//-----------------------------------------------
//  GEffect
//-----------------------------------------------

GEffect::GEffect(const g_string &Name, GD3DDevice *Device):GBaseObject(Name, Device)
{
  _init();
}

//-----------------------------------------------

GEffect::GEffect(const g_string &FileName, GEffectPool *EffectPool, const g_string &Name, GD3DDevice *Device):
	     GBaseObject(Name, Device)
{
  _init();
  LoadFromFile(FileName, EffectPool);
}

//-----------------------------------------------

GEffect::~GEffect()
{
  Release();
}

//-----------------------------------------------

void GEffect::_init()
{
  _dx_effect = NULL;
  _p_fx_pool_uid = NULL;
  SetType(G_OT_EFFECT);
}

//-----------------------------------------------

void GEffect::Release()
{
  NULLRELEASE(_dx_effect);
  {
    map<g_uint32, GEffectVariable*>::iterator it;
    for (it = _vars.begin(); it != _vars.end(); it++)
      FREENULL_VAR((*it).second);
    _vars.clear();
  }
  {
    map<g_uint32, GEffectTech*>::iterator it;
    for (it = _techs.begin(); it != _techs.end(); it++)
      FREENULL_VAR((*it).second);
    _techs.clear();
  }
}

//-----------------------------------------------

g_bool GEffect::LoadFromFile(const g_string &filename, GEffectPool *effect_pool)
{
  HRESULT hres;
  _p_fx_pool_uid = NULL;
  if (effect_pool == NULL)
  {
    hres = D3DX10CreateEffectFromFile(filename.c_str(), NULL, NULL, "fx_4_0",
                                      D3D10_SHADER_ENABLE_STRICTNESS, 0, 
									  _device->D3DDevice, NULL, 
									  NULL, &_dx_effect, NULL, NULL);
  }
  else
  {
    hres = D3DX10CreateEffectFromFile(filename.c_str(), NULL, NULL, "fx_4_0",
                                      D3D10_SHADER_ENABLE_STRICTNESS, D3D10_EFFECT_COMPILE_CHILD_EFFECT,
			                          _device->D3DDevice, effect_pool->GetDXEffectPool(), 
									  NULL, &_dx_effect, NULL, NULL);
	_p_fx_pool_uid = effect_pool;
  }

  if (hres != S_OK)
	return false;

  _filename = filename;
  _load_variables();
  return true;
}

//-----------------------------------------------

void GEffect::_load_variables()
{
  if (_p_fx_pool_uid != NULL)
  {
    ID3D10Effect *ep_effect = _p_fx_pool_uid->GetDXEffectPool()->AsEffect();
	D3D10_EFFECT_DESC ep_desc;

	ep_effect->GetDesc(&ep_desc);
    for (g_uint32 var_ind = 0; var_ind < ep_desc.GlobalVariables; var_ind++)
    {
      ID3D10EffectVariable *curr_var = ep_effect->GetVariableByIndex(var_ind);
	  if (!curr_var->IsValid()) continue;
	  
	  g_int32 var_type = 0;
	  g_int32 var_id = 0;

	  if (curr_var->GetAnnotationByName("vType")->IsValid())
		curr_var->GetAnnotationByName("vType")->AsScalar()->GetInt(&var_type);

	  if (curr_var->GetAnnotationByName("vID")->IsValid())
		curr_var->GetAnnotationByName("vID")->AsScalar()->GetInt(&var_id);

	  g_uint32 var_uid = abs(var_type)*65536 + abs(var_id);

	  if (_vars[var_uid] == NULL)
        _vars[var_uid] = new GEffectVariable(curr_var);
    }
  }

  if (_dx_effect != NULL)
  {
	D3D10_EFFECT_DESC fx_desc;

	_dx_effect->GetDesc(&fx_desc);
    for (g_uint32 var_ind = 0; var_ind < fx_desc.GlobalVariables; var_ind++)
    {
      ID3D10EffectVariable *curr_var = _dx_effect->GetVariableByIndex(var_ind);

	  g_uint32 var_uid = 0;

	  if (!_get_variable_uid(curr_var, var_uid))
		continue;

	  if (_vars[var_uid] == NULL)
        _vars[var_uid] = new GEffectVariable(curr_var);
    }

	for (g_uint32 tech_ind = 0; tech_ind < fx_desc.Techniques; tech_ind++)
    {
	  ID3D10EffectTechnique *curr_tech = _dx_effect->GetTechniqueByIndex(tech_ind);

	  g_uint32 tech_id = 0;
      if (!_get_tech_uid(curr_tech, tech_id))
	    continue;

	  if (_techs[tech_id] == NULL)
        _techs[tech_id] = new GEffectTech(curr_tech);
	}
  }
}

//-----------------------------------------------

GEffectVariable* GEffect::GetVar(g_int32 type, g_int32 id)
{
  return _vars[_make_var_uid(type, id)];
}

//-----------------------------------------------

GEffectVariable* GEffect::GetVarByName(const g_string &name)
{
  ID3D10EffectVariable *var = _dx_effect->GetVariableByName(name.c_str());

  g_uint32 var_uid = 0;

  if (!_get_variable_uid(var, var_uid))
	return NULL;
    
  return _vars[var_uid];
}

//-----------------------------------------------

GEffectVariable* GEffect::GetVarBySemantic(const g_string &semantic)
{
  ID3D10EffectVariable *var = _dx_effect->GetVariableBySemantic(semantic.c_str());

  g_uint32 var_uid = 0;
  if (!_get_variable_uid(var, var_uid))
	return NULL;
    
  return _vars[var_uid];
}

//-----------------------------------------------

GEffectTech* GEffect::GetTech(g_uint32 id)
{
  return _techs[id];
}

//-----------------------------------------------

GEffectTech* GEffect::GetTech(const g_string &name)
{
  ID3D10EffectTechnique *tech = _dx_effect->GetTechniqueByName(name.c_str());

  g_uint32 tech_id = 0;
  if (!_get_tech_uid(tech, tech_id))
	return NULL;

  return _techs[tech_id];
}

//-----------------------------------------------

inline g_uint32 GEffect::_make_var_uid(g_int32 type, g_int32 id)
{
  return g_uint32(abs(type)*65536 + abs(id));
}

//-----------------------------------------------

inline g_bool GEffect::_get_variable_uid(ID3D10EffectVariable *var, g_uint32 &uid)
{
  if ((!var->IsValid()) && (var->GetAnnotationByName("vType")->IsValid()))
	return false;

  g_int32 var_type = G_SVT_CUSTOM;
  g_int32 var_id = 0;

  var->GetAnnotationByName("vType")->AsScalar()->GetInt(&var_type);

  if (var->GetAnnotationByName("vID")->IsValid())
	var->GetAnnotationByName("vID")->AsScalar()->GetInt(&var_id);

  uid = _make_var_uid(var_type, var_id);

  return true;
}

//-----------------------------------------------

inline g_bool GEffect::_get_tech_uid(ID3D10EffectTechnique *tech, g_uint32 &uid)
{
  if ((!tech->IsValid()) || (!tech->GetAnnotationByName("tID")->IsValid()))
	return false;

  tech->GetAnnotationByName("tID")->AsScalar()->GetInt((g_int32*)&uid);

  return true;
}

//-----------------------------------------------
