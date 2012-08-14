//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) 2003-2010 Jason Zink 
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
#include "PCH.h"
#include "ImmediateGeometryDX11.h"
#include "VertexBufferDX11.h"
#include "BufferConfigDX11.h"
#include "Log.h"
#include "GlyphString.h"
#include "PipelineManagerDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ImmediateGeometryDX11::ImmediateGeometryDX11( )
{
	m_uiMaxSize = 0;
	m_uiVertexCount = 0;
	m_Color = Vector4f( 0.0f, 1.0f, 0.0f, 1.0f );
	m_ePrimType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pVertexArray = nullptr;

	// Initialize our buffer to a reasonable size
	SetMaxSize( 1024 );


	// Fill in the vertex element descriptions based on each element of our format

	D3D11_INPUT_ELEMENT_DESC e1;
	D3D11_INPUT_ELEMENT_DESC e2;
	D3D11_INPUT_ELEMENT_DESC e3;

	e1.SemanticName = "POSITION";
	e1.SemanticIndex = 0;
	e1.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	e1.InputSlot = 0;
	e1.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	e1.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	e1.InstanceDataStepRate = 0;

	e2.SemanticName = "COLOR";
	e2.SemanticIndex = 0;
	e2.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	e2.InputSlot = 0;
	e2.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	e2.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	e2.InstanceDataStepRate = 0;

	e3.SemanticName = "TEXCOORD";
	e3.SemanticIndex = 0;
	e3.Format = DXGI_FORMAT_R32G32_FLOAT;
	e3.InputSlot = 0;
	e3.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	e3.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	e3.InstanceDataStepRate = 0;

	m_elements.add( e1 );
	m_elements.add( e2 );
	m_elements.add( e3 );
}
//--------------------------------------------------------------------------------
ImmediateGeometryDX11::~ImmediateGeometryDX11()
{
	SAFE_DELETE_ARRAY( m_pVertexArray );
}
//--------------------------------------------------------------------------------
void ImmediateGeometryDX11::Execute( PipelineManagerDX11* pPipeline, IParameterManager* pParamManager )
{
	if ( m_uiVertexCount > 0 ) {

		UploadData( pPipeline );
	
		pPipeline->InputAssemblerStage.ClearDesiredState();

		// Set the Input Assembler state, then perform the draw call.
		int layout = GetInputLayout( pPipeline->ShaderStages[VERTEX_SHADER]->DesiredState.GetShaderProgram() );
		pPipeline->InputAssemblerStage.DesiredState.SetInputLayout( layout );
		pPipeline->InputAssemblerStage.DesiredState.SetPrimitiveTopology( m_ePrimType );
		pPipeline->InputAssemblerStage.DesiredState.SetVertexBuffer( 0, m_VB->m_iResource, 0, sizeof( ImmediateVertexDX11 ) );
	
		pPipeline->ApplyInputResources();

		pPipeline->Draw( m_uiVertexCount, 0 );
	}
}
//--------------------------------------------------------------------------------
void ImmediateGeometryDX11::ResetGeometry()
{
	// Reset the vertex count here to prepare for the next drawing pass.
	m_uiVertexCount = 0;
}
//--------------------------------------------------------------------------------
void ImmediateGeometryDX11::UploadData( PipelineManagerDX11* pPipeline )
{

	if ( m_uiVertexCount > 0 ) {
		// Map the vertex buffer for writing

		D3D11_MAPPED_SUBRESOURCE resource = 
			pPipeline->MapResource( m_VB, 0, D3D11_MAP_WRITE_DISCARD, 0 );

		// Only copy as much of the data as you actually have filled up
	
		memcpy( resource.pData, m_pVertexArray, m_uiVertexCount * sizeof( ImmediateVertexDX11 ) );

		// Unmap the vertex buffer

		pPipeline->UnMapResource( m_VB, 0 );
	}
}
//--------------------------------------------------------------------------------
void ImmediateGeometryDX11::AddVertex( const ImmediateVertexDX11& vertex )
{
	if ( m_uiVertexCount < m_uiMaxSize ) {
		m_pVertexArray[m_uiVertexCount] = vertex;
		m_uiVertexCount++;
	}
}
//--------------------------------------------------------------------------------
void ImmediateGeometryDX11::AddVertex( const Vector3f& position )
{
	if ( m_uiVertexCount < m_uiMaxSize ) {
		m_pVertexArray[m_uiVertexCount].position = position;
		m_pVertexArray[m_uiVertexCount].color = m_Color;
		m_pVertexArray[m_uiVertexCount].texcoords = Vector2f( 0.0f, 0.0f );
		m_uiVertexCount++;
	}
}
//--------------------------------------------------------------------------------
void ImmediateGeometryDX11::AddVertex( const Vector3f& position, const Vector4f& color )
{
	if ( m_uiVertexCount < m_uiMaxSize ) {
		m_pVertexArray[m_uiVertexCount].position = position;
		m_pVertexArray[m_uiVertexCount].color = color;
		m_pVertexArray[m_uiVertexCount].texcoords = Vector2f( 0.0f, 0.0f );
		m_uiVertexCount++;
	}
}
//--------------------------------------------------------------------------------
void ImmediateGeometryDX11::AddVertex( const Vector3f& position, const Vector2f& texcoords )
{
	if ( m_uiVertexCount < m_uiMaxSize ) {
		m_pVertexArray[m_uiVertexCount].position = position;
		m_pVertexArray[m_uiVertexCount].color = m_Color;
		m_pVertexArray[m_uiVertexCount].texcoords = texcoords;
		m_uiVertexCount++;
	}
}
//--------------------------------------------------------------------------------
void ImmediateGeometryDX11::AddVertex( const Vector3f& position, const Vector4f& color, const Vector2f& texcoords )
{
	if ( m_uiVertexCount < m_uiMaxSize ) {
		m_pVertexArray[m_uiVertexCount].position = position;
		m_pVertexArray[m_uiVertexCount].color = color;
		m_pVertexArray[m_uiVertexCount].texcoords = texcoords;
		m_uiVertexCount++;
	}
}
//--------------------------------------------------------------------------------
D3D11_PRIMITIVE_TOPOLOGY ImmediateGeometryDX11::GetPrimitiveType()
{
	return( m_ePrimType );
}
//--------------------------------------------------------------------------------
void ImmediateGeometryDX11::SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY type )
{
	m_ePrimType = type;
}
//--------------------------------------------------------------------------------
void ImmediateGeometryDX11::SetColor( const Vector4f& color )
{
	m_Color = color;
}
//--------------------------------------------------------------------------------
Vector4f ImmediateGeometryDX11::GetColor( )
{
	return( m_Color );
}
//--------------------------------------------------------------------------------
void ImmediateGeometryDX11::SetMaxSize( unsigned int maxVertices )
{
	// if this is different than the current size, then release the existing
	// local array, plus resize the resource.

	if ( maxVertices != m_uiMaxSize ) {

		// Remember the maximum number of vertices to allow
		m_uiMaxSize = maxVertices;
		m_uiVertexCount = 0;

		// Release system memory for the old array so that we can create a new one
		SAFE_DELETE_ARRAY( m_pVertexArray );
		m_pVertexArray = new ImmediateVertexDX11[m_uiMaxSize];

		// Delete the existing resource if it already existed
		if ( nullptr != m_VB ) {
			RendererDX11::Get()->DeleteResource( m_VB );
			m_VB = nullptr;
		}

		// Create the new vertex buffer, with the dynamic flag set to true
		BufferConfigDX11 vbuffer;
		vbuffer.SetDefaultVertexBuffer( m_uiMaxSize * sizeof( ImmediateVertexDX11 ), true );
		m_VB = RendererDX11::Get()->CreateVertexBuffer( &vbuffer, nullptr );
	}
}
//--------------------------------------------------------------------------------
unsigned int ImmediateGeometryDX11::GetMaxSize()
{
	return( m_uiMaxSize );
}
//--------------------------------------------------------------------------------
unsigned int ImmediateGeometryDX11::GetVertexCount()
{
	return( m_uiVertexCount );
}
//--------------------------------------------------------------------------------
unsigned int ImmediateGeometryDX11::GetPrimitiveCount()
{
	unsigned int count = 0;
	unsigned int indices = m_uiVertexCount;

	switch ( m_ePrimType )
	{
	case D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED:
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
		count = indices; break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
		count = indices / 2; break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP:
		count = indices - 1; break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		count = indices / 3; break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		count = indices - 2; break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
		count = indices / 4; break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
		count = indices - 3; break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
		count = indices / 6; break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
		count = ( indices - 3 ) / 2; break;
	case D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST:
		count = indices; break;
	case D3D11_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST:
		count = indices / 2; break;
	case D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST:
		count = indices / 3; break;
	case D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST:
		count = indices / 4; break;
	case D3D11_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST:
		count = indices / 5; break;
	case D3D11_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST:
		count = indices / 6; break;
	case D3D11_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST:
		count = indices / 7; break;
	case D3D11_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST:
		count = indices / 8; break;
	case D3D11_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST:
		count = indices / 9; break;
	case D3D11_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST:
		count = indices / 10; break;
	case D3D11_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST:
		count = indices / 11; break;
	case D3D11_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST:
		count = indices / 12; break;
	case D3D11_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST:
		count = indices / 13; break;
	case D3D11_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST:
		count = indices / 14; break;
	case D3D11_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST:
		count = indices / 15; break;
	case D3D11_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST:
		count = indices / 16; break;
	case D3D11_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST:
		count = indices / 17; break;
	case D3D11_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST:
		count = indices / 18; break;
	case D3D11_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST:
		count = indices / 19; break;
	case D3D11_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST:
		count = indices / 20; break;
	case D3D11_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST:
		count = indices / 21; break;
	case D3D11_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST:
		count = indices / 22; break;
	case D3D11_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST:
		count = indices / 23; break;
	case D3D11_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST:
		count = indices / 24; break;
	case D3D11_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST:
		count = indices / 25; break;
	case D3D11_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST:
		count = indices / 26; break;
	case D3D11_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST:
		count = indices / 27; break;
	case D3D11_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST:
		count = indices / 28; break;
	case D3D11_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST:
		count = indices / 29; break;
	case D3D11_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST:
		count = indices / 30; break;
	case D3D11_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST:
		count = indices / 31; break;
	case D3D11_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST:
		count = indices / 32; break;
	}

	return( count );
}
//--------------------------------------------------------------------------------
void ImmediateGeometryDX11::GenerateInputLayout( int ShaderID )
{
	// Create the input layout for the given shader index

	RendererDX11* pRenderer = RendererDX11::Get();
	if ( m_InputLayouts[ShaderID] == 0 )
	{
		InputLayoutKey* pKey = new InputLayoutKey();
		pKey->shader = ShaderID;
		pKey->layout = pRenderer->CreateInputLayout( m_elements, ShaderID );
		m_InputLayouts[ShaderID] = pKey;
	}
}
//--------------------------------------------------------------------------------