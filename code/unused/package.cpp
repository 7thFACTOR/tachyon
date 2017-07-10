#if defined(E_USE_ZLIB)
class E_API ZipFile : public File
{
public:
	friend class Package;
	ZipFile();
	~ZipFile();

	u64 size() const;
	u64 position() const;
	bool open(const char* pFilename, u32 aModeFlags);
	bool close();
	bool read(void* pBuffer, u64 aLen);
	bool write(const void* pBuffer, u64 aLen);
	bool seek(FileSeekMode aMode, u64 aOffset);
	bool eof() const;

protected:
	Package* m_pPackage;
	unzFile m_pZipFile;
	char* m_pZipFileData;
	u64 m_zipFileSize, m_zipFilePos;
};

ZipFile::ZipFile() : File()
{
	m_pZipFile = nullptr;
	m_zipFileSize = 0;
	m_zipFilePos = 0;
	m_pZipFileData = nullptr;
}

ZipFile::~ZipFile()
{
}

u64 ZipFile::size() const
{
	if (m_pZipFile)
	{
		return m_zipFileSize;
	}

	return 0;
}

u64 ZipFile::position() const
{
	if (m_pZipFile)
	{
		return m_zipFilePos;
	}

	return 0;
}

bool ZipFile::open(const char* pFilename, u32 aModeFlags)
{
	if (!pFilename)
		return false;

	m_fileName = pFilename;

	if (UNZ_OK == unzLocateFile(m_pZipFile, pFilename, 0))
	{
		i32 result = 0;

		if (m_pPackage->password() != "")
		{
			result = unzOpenCurrentFilePassword(
				m_pZipFile, m_pPackage->password().c_str());
		}
		else
		{
			result = unzOpenCurrentFile(m_pZipFile);
		}

		if (UNZ_OK == result)
		{
			unz_file_info fi;

			if (UNZ_OK == unzGetCurrentFileInfo(m_pZipFile,
							&fi, nullptr, 0, nullptr, 0, nullptr, 0))
			{
				if (fi.uncompressed_size)
					m_pZipFileData = new char[fi.uncompressed_size];

				B_ASSERT(m_pZipFileData);

				if (fi.uncompressed_size == unzReadCurrentFile(
												m_pZipFile,
												m_pZipFileData,
												fi.uncompressed_size))
				{
					unzCloseCurrentFile(m_pZipFile);
					m_zipFilePos = 0;
					m_zipFileSize = fi.uncompressed_size;

					return true;
				}

				B_SAFE_DELETE(m_pZipFileData);
			}
		}
	}

	return false;
}

bool ZipFile::close()
{
	if (m_pZipFile)
	{
		m_zipFileSize = 0;
		m_zipFilePos = 0;
		B_SAFE_DELETE_ARRAY(m_pZipFileData);

		return true;
	}

	return true;
}

bool ZipFile::read(void* pBuffer, u64 aLen)
{
	B_ASSERT(pBuffer);

	if (m_pZipFile)
	{
		if (nullptr != m_pZipFileData)
		{
			if (m_zipFilePos >= m_zipFileSize)
				return false;

			if (aLen > m_zipFileSize - m_zipFilePos)
				aLen = m_zipFileSize - m_zipFilePos;

			memcpy(pBuffer, m_pZipFileData + m_zipFilePos, aLen);
			m_zipFilePos += aLen;

			return true;
		}
	}

	return false;
}

bool ZipFile::write(const void* pBuffer, u64 aLen)
{
	if (m_pZipFile)
	{
		//TODO: write file into ZIP ? :), nopes :D
	}

	return false;
}

bool ZipFile::seek(ESeek aMode, u64 aOffset)
{
	if (m_pZipFile)
	{
		switch (aMode)
		{
		case File::eSeek_Set:
			m_zipFilePos = aOffset;
			break;
		case File::eSeek_Current:
			m_zipFilePos += aOffset;
			break;
		case File::eSeek_End:
			m_zipFilePos = m_zipFileSize + aOffset - 1;
			break;
		default:
			break;
		}

		if (m_zipFilePos < 0)
		{
			m_zipFilePos = 0;

			return false;
		}

		if (m_zipFilePos >= m_zipFileSize)
		{
			m_zipFilePos = m_zipFileSize - 1;

			return false;
		}

		return true;
	}

	return false;
}

bool ZipFile::eof() const
{
	if (m_pZipFile)
	{
		if (m_zipFilePos >= m_zipFileSize)
		{
			return true;
		}
	}

	return false;
}

//---

Package::Package()
{
}

Package::~Package()
{
}

File* Package::openFile(const char* pFilename, u32 aModeFlags)
{
	ZipFile* pFile = new ZipFile();

	B_ASSERT(pFile);

	if (!pFile)
		return nullptr;

	pFile->m_pPackage = this;
	pFile->m_pZipFile = m_hZip;

	if (!pFile->open(pFilename, aModeFlags))
	{
		delete pFile;
		return nullptr;
	}

	return pFile;
}

bool Package::fileExists(const char* pFile) const
{
	if (UNZ_OK == unzLocateFile(m_hZip, pFile, 2))
	{
		return true;
	}

	return false;
}

bool Package::folderExists(const char* pFolder) const
{
	if (UNZ_OK == unzLocateFile(m_hZip, pFolder, 2))
	{
		return true;
	}

	return false;
}

bool Package::open(const char* pPackageFilename, const char* pPassword)
{
	m_password = pPassword;
	m_hZip = unzOpen(pPackageFilename);

	if (!m_hZip)
		return false;

	return true;
}

bool Package::close()
{
	if (m_hZip)
	{
		if (UNZ_OK != unzClose(m_hZip))
			return false;
	}
	
	m_hZip = 0;

	return true;
}

bool Package::childFoldersOf(const char* pFolder, Array<String>& rChildFolders) const
{
	if (UNZ_OK != unzGoToFirstFile(m_hZip))
		return false;

	uLong i;
	unz_global_info gi;

	if (UNZ_OK != unzGetGlobalInfo(m_hZip, &gi))
		return false;

	unz_file_info info;
	char pFilename[1024] = "";

	for (i = 0; i < gi.number_entry; ++i)
	{
		if (UNZ_OK != unzGetCurrentFileInfo(m_hZip, &info, pFilename, 1024, nullptr, 0, nullptr, 0))
		{
			return false;
		}

		if ((i + 1) < gi.number_entry)
		{
			if (UNZ_OK != unzGoToNextFile(m_hZip))
			{
				break;
			}
		}
	}

	return true;
}

bool Package::filesOf(const char* pFolder, Array<String>& rChildFiles) const
{
	//TODO
	return true;
}
const String& Package::password() const
{
	return m_password;
}

#endif
