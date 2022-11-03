#include <cctype>
#include "cVOXreader.h"


void cVOX_VoxelData::ReadVoxel(std::ifstream* file)
{
	file->read((char*)&m_ucX, 1);
	file->read((char*)&m_ucY, 1);
	file->read((char*)&m_ucZ, 1);
	file->read((char*)&m_ucMaterialIndex, 1);
}

cChunkHeader::cChunkHeader()
{
	m_iChunkID = -1;

	m_iNumBytesContent = -1;
	m_iNumBytesChidData = -1;
}
cChunkHeader::~cChunkHeader()
{

}

int cChunkHeader::GetChunkID()
{
	return m_iChunkID;
}

void cChunkHeader::ReadChunkHeader(std::ifstream* file)
{
	// Read 4 Chars (Chunk Name)
	for (int CurrChar = 0; CurrChar < 4; CurrChar++)
	{
		file->read((char*)&m_ch_ChunkName[CurrChar], 1);
	}
	// Read int (4 Bytes) Num Bytes Chunk Content
	file->read((char*)&m_iNumBytesContent, 4);
	// Read int (4 Bytes) Num Bytes Child Content
	file->read((char*)&m_iNumBytesChidData, 4);
}

//int m_iChunkID; // 0 = Main, 1 = Pack, 2 = Size, 3 = XYZI, 4 = RGBA

int cChunkHeader::TranslateChunkID() // gets m_iChunkID from the chunk name
{
	m_iChunkID = -1; // default value
	// Right now we can look at first letter. . . Eventually we'll probably verify this stuff!

	char FirstLetter = toupper(m_ch_ChunkName[0]);

	switch (FirstLetter)
	{
	case 'M' : // assume 'main'
		m_iChunkID = 0;
		break;
	case 'P':  // assume 'pack'
		m_iChunkID = 1;
		break;
	case 'S': // assume 'size'
		m_iChunkID = 2;
		break;
	case 'X': // assume 'xyzi'
		m_iChunkID = 3;
		break; 
	case 'R': // assume 'rgba'
		m_iChunkID = 4;
		break;
	}


	return m_iChunkID;
}

int cChunkHeader::VerifyMainChunk()
{
	return m_iChunkID;
}
int cChunkHeader::VerifyPackChunk()
{
	return m_iChunkID;
}
int cChunkHeader::VerifySizeChunk()
{
	return m_iChunkID;
}
int cChunkHeader::VerifyXYZIChunk()
{
	return m_iChunkID;
}
int cChunkHeader::VerifyRGBAChunk()
{
	return m_iChunkID;
}




cVOXreader::cVOXreader()
{
	m_iNumModels = 1; // default value

	m_VoxelBuffer = NULL;
	m_iCurrVoxelBufferSize = 0;

	m_bUsingLocalVoxelBuffer = true;
}


cVOXreader::~cVOXreader()
{
	// if we eventually allocate voxel memory, clear it here. . .
	ClearVoxelBuffer();
}

void cVOXreader::ClearVoxelBuffer()
{
	if (m_VoxelBuffer != NULL)
	{
		delete[] m_VoxelBuffer;
		m_VoxelBuffer = NULL;
		m_iCurrVoxelBufferSize = 0;
	}
}

void cVOXreader::SetUpVoxelBuffer(int iNumVoxels)
{
	if (m_VoxelBuffer != NULL)
	{
		ClearVoxelBuffer(); // need to free up memory if we've previously set it up
	}
	m_VoxelBuffer = new stVOX_VoxelData[iNumVoxels];
	m_iCurrVoxelBufferSize = iNumVoxels;
}


void cVOXreader::Read_VOX_File(std::ifstream* file) // this is the main function of this class! Read a VOX file!!!
{
	m_iNumModels = 1; // default value

	//int Version = ReadVOXFileHeader(&file);
	ReadVOXFileHeader(file);
	if (m_iVersion >= 0)
	{
		// next should be 'main' chunk ID = 0
		m_ChunkReader.ReadChunkHeader(file);
		m_ChunkReader.TranslateChunkID();
		m_ChunkReader.m_iChunkID = m_ChunkReader.m_iChunkID;
		m_ChunkReader.m_iNumBytesContent = m_ChunkReader.m_iNumBytesContent;
		m_ChunkReader.m_iNumBytesChidData = m_ChunkReader.m_iNumBytesChidData;
		if (m_ChunkReader.m_iChunkID == 0) // 0 is 'main' meaning we are now reading main chunk
		{
			// assuming we have 'main' above, then we might (or might not) get 'pack' (which is number of models)
			// if we don't get 'pack' assume num models = 1
			m_ChunkReader.ReadChunkHeader(file);
			m_ChunkReader.TranslateChunkID();
			m_ChunkReader.m_iChunkID = m_ChunkReader.m_iChunkID;
			m_ChunkReader.m_iNumBytesContent = m_ChunkReader.m_iNumBytesContent;
			m_ChunkReader.m_iNumBytesChidData = m_ChunkReader.m_iNumBytesChidData;

			// now chunk ID should either be 1 for pack, or 2 for the  start"SIZE" of voxel array
			if (m_ChunkReader.m_iChunkID == 1)
			{
				// read number of models // need to create a multi model file to test this out. . .
				ReadNumModels(file);

			}
			else if (m_ChunkReader.m_iChunkID == 2)
			{
				// get that size data (how big is our voxel array???)
				Read_3D_Array_Dimensions(file);
			}

			// now we read the models
			ReadModels(file);

			// there should also be some RGBA data for the material pallette, but this isn't needed yet
			// so, unless something changes, we'll just close the file

		} // end if 'main' chunk
	} // end if valid version

}



void cVOXreader::ReSetArrayDimensionData() // resets size data back to defaults
{
	m_iDimX_CurrModel = 0;
	m_iDimY_CurrModel = 0;
	m_iDimZ_CurrModel = 0;
}

// Read the intro file header. . .
int cVOXreader::ReadVOXFileHeader(std::ifstream* file)
{
	char HeaderName[4];

	int Version = -1;

	file->read((char*)&HeaderName[0], 1);
	char ch;
	ch = toupper(HeaderName[0]);
	if (ch == 'V')
	{
		file->read((char*)&HeaderName[1], 1);
		ch = toupper(HeaderName[1]);
		if (ch == 'O')
		{
			file->read((char*)&HeaderName[2], 1);
			ch = toupper(HeaderName[2]);
			if (ch == 'X')
			{
				file->read((char*)&HeaderName[3], 1);
				ch = toupper(HeaderName[3]);
				if (ch == ' ')
				{
					file->read((char*)&Version, 4);
				}
			}
		}
	}
	m_iVersion = Version;
	return Version;
}


int cVOXreader::ReadNumModels(std::ifstream* file)
{
	file->read((char*)&m_iNumModels, 4);
	return m_iNumModels;
}

void cVOXreader::Read_3D_Array_Dimensions(std::ifstream* file)
{
	file->read((char*)&m_iDimX_CurrModel, 4);
	file->read((char*)&m_iDimY_CurrModel, 4);
	file->read((char*)&m_iDimZ_CurrModel, 4);

	m_iDimZ_CurrModel = m_iDimZ_CurrModel;
}


void cVOXreader::ReadModels(std::ifstream* file) // reads in number of models based on m_iNumModels
{
	for (int CurrModel = 0; CurrModel < m_iNumModels; CurrModel++)
	{
		Read_A_Model(file);
	}
}

void cVOXreader::Read_A_Model(std::ifstream* file)
{
	// We either open with (optional) "Pack" Chunk (ID = 1), or "Size" chunk (ID = 2)
	// if the current chunk is NOT Size Chunk ID=2 then read size data now. . .
	if (m_ChunkReader.m_iChunkID != 2)
	{
		m_ChunkReader.ReadChunkHeader(file);
		m_ChunkReader.TranslateChunkID();

		if (m_ChunkReader.m_iChunkID == 2)
		{
			Read_3D_Array_Dimensions(file);
		}
		else
		{
			m_ChunkReader.m_iChunkID = m_ChunkReader.m_iChunkID; // ERROR This shouldn't happen here!!!
		}
	}

	// try to read XYZI data chunk 
	m_ChunkReader.ReadChunkHeader(file);
	m_ChunkReader.TranslateChunkID();

	if (m_ChunkReader.m_iChunkID == 3) // this is the XYZI Chunk
	{
		// get num voxels 
		file->read((char*)&m_iNumVoxels_CurrModel, 4);
		if (m_bUsingLocalVoxelBuffer)
		{
			// time to allocate our buffer. . .
			SetUpVoxelBuffer(m_iNumVoxels_CurrModel);
		}
		// start reading voxel data
		for (int CurrVoxel = 0; CurrVoxel < m_iNumVoxels_CurrModel; CurrVoxel++)
		{
			m_VoxelData.ReadVoxel(file);
			// I can either add my voxel directly to my application data, or allocate a buffer here. . .
			// Currently we are adding the data to a Buffer inside this class
			if (m_bUsingLocalVoxelBuffer)
			{
				m_VoxelBuffer[CurrVoxel].m_ucX = m_VoxelData.m_ucX;
				m_VoxelBuffer[CurrVoxel].m_ucY = m_VoxelData.m_ucY;
				m_VoxelBuffer[CurrVoxel].m_ucZ = m_VoxelData.m_ucZ;
				m_VoxelBuffer[CurrVoxel].m_ucMaterialIndex = m_VoxelData.m_ucMaterialIndex;
			}
			// temporary test to see what is happening in the process. . .
			if (CurrVoxel == 200)
			{
				m_VoxelData.m_ucX = m_VoxelData.m_ucX;
			}
		}
	}
	else
	{
		// ERROR ! we didn't find what we expected 
		m_ChunkReader.m_iChunkID = m_ChunkReader.m_iChunkID;
	}
}


