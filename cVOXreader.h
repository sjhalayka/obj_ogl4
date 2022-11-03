#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <ctype.h>
//#include <vector.h>
#include <vector>
#include <list>


struct stVOX_VoxelData
{
	unsigned char m_ucX;
	unsigned char m_ucY;
	unsigned char m_ucZ;
	unsigned char m_ucMaterialIndex;
};


class cVOX_VoxelData
{
public:
	void ReadVoxel(std::ifstream* file);

	unsigned char m_ucX;
	unsigned char m_ucY;
	unsigned char m_ucZ;
	unsigned char m_ucMaterialIndex;
};

class cChunkHeader
{
public:
	cChunkHeader();
	~cChunkHeader();

	int GetChunkID(); // just returns m_iChunkID
	int TranslateChunkID(); // gets m_iChunkID from the chunk name

	int VerifyMainChunk();
	int VerifyPackChunk();
	int VerifySizeChunk();
	int VerifyXYZIChunk();
	int VerifyRGBAChunk();

	void ReadChunkHeader(std::ifstream* file);

	int m_iChunkID; // 0 = Main, 1 = Pack, 2 = Size, 3 = XYZI, 4 = RGBA
	int m_iNumBytesContent;
	int m_iNumBytesChidData;

	char m_ch_ChunkName[4]; // holds the four letters of the chunk name
};


class cVOXreader
{
public:
	cVOXreader();
	~cVOXreader();

	void ClearVoxelBuffer();
	void SetUpVoxelBuffer(int iNumVoxels);
	//void SetVoxelIntoBuffer(int iVoxelNum, cVOX_VoxelData* pVoxelData);


	void Read_VOX_File(std::ifstream* file); // this is the main function of this class! Read a VOX file!!!


	void ReSetArrayDimensionData(); // resets size data back to defaults

	int ReadVOXFileHeader(std::ifstream* file);

	int ReadNumModels(std::ifstream* file);

	void Read_3D_Array_Dimensions(std::ifstream* file); // this is what is stored in "SIZE" Chunk (ID = 2)

	void ReadModels(std::ifstream* file); // reads in number of models based on m_iNumModels
	void Read_A_Model(std::ifstream* file);

	cVOX_VoxelData m_VoxelData;

	cChunkHeader m_ChunkReader;

	int m_iVersion; // the current version of this file (should be 150 from what I know)

	int m_iNumModels;


	int m_iNumVoxels_CurrModel;

	int m_iDimX_CurrModel;
	int m_iDimY_CurrModel;
	int m_iDimZ_CurrModel;

	bool m_bUsingLocalVoxelBuffer; 

	int m_iCurrVoxelBufferSize;
	stVOX_VoxelData* m_VoxelBuffer; // needs to be allocated, holds Voxel Array Data
};

