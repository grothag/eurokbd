class CConfig
{
    UINT64      m_mtimeOK;          // mtime ���䨣-䠩�� ���⠭���� � ��᫥���� ࠧ
                                    // 0 - �� ��祣� �� �⠫�
                                    // 1 - ����㦥�� ���祭�� �� 㬮�砭��
    UINT64      m_mtimeError;       // mtime ���䨣-䠩�� �� �⥭�� ���ண� �뫠 �訡��
protected:
    LPWSTR      m_filename;
public:
    CConfig(LPCWSTR filename)
    {
        m_filename = _wcsdup(filename);
        m_mtimeOK = 0;
        m_mtimeError = 0;
    }

    ~CConfig()
    {
        free(m_filename);
    }

    enum {  E_OK=1,
            E_DEFAULTS_LOADED=2,
            E_UNCHANGED=3 };
    int Ensure()
    {
        int rc = E_UNCHANGED; // true, �᫨ ��-� ����������

        WIN32_FIND_DATA ffdata;
        HANDLE hFind = FindFirstFile(m_filename, &ffdata);
        if( hFind==INVALID_HANDLE_VALUE ) // ��� ⠪��� 䠩�� :(
        {
            _SetDefaults();
            m_mtimeOK = 1; // ⨯� magic �� ����㦥�� ��⠭���� �� 㬮�砭��
            rc = E_DEFAULTS_LOADED;
        } else
        {
            FindClose(hFind);

            if( *(UINT64*)&ffdata.ftLastWriteTime != m_mtimeOK &&
                *(UINT64*)&ffdata.ftLastWriteTime != m_mtimeError)
            {
                // �� �६� ����� FindFirstFile � Load 䠩� ����� ����������, �� �� �� ���譮
                // �㤥� ��-Load'��� ����� ����� �����, � �६� � m_LastWriteTime �㤥� �� ��ன
                // ��� ���� ����⠥��� �� ࠧ, ���뢠� ������ ����⭮��� �⮣� ᮡ���, �� ��ଠ�쭮
                // ????: ��� �� ����� �᪫����, ���뢠� 䠩� ��� � ���� �६� �� GetFileTime, �� �� ����� �᫮����� ����஢����
                //
                // Load ����� ������ false, ���ਬ�� �᫨ ᥩ�� �ந�室�� ������ � ���䨣 ��㣨� ����ᮬ
                // � �⮬ ��砥 ������� ���� �����
                //
                if( _Load() )
                {
                    m_mtimeOK = *(UINT64*)&ffdata.ftLastWriteTime;
                    rc = E_OK;
                } else
                {
                    m_mtimeError = *(UINT64*)&ffdata.ftLastWriteTime;
                    if (0==m_mtimeOK) // �᫨ �� �뫮 ����� ������
                    {
                        _SetDefaults();
                        m_mtimeOK = 1; // ⨯� magic �� ����㦥�� ��⠭���� �� 㬮�砭��, �⮡� � ᫥�. ࠧ �� ������ SetDefaults
                        rc = E_DEFAULTS_LOADED;
                    }
                }
            }
        }
        return rc;
    }

    virtual bool Save()
    {
        if( _Save() )
        {
            WIN32_FIND_DATA ffdata;
            HANDLE hFind = FindFirstFile(m_filename, &ffdata);
            if( hFind!=INVALID_HANDLE_VALUE )
            {
                m_mtimeOK = *(UINT64*)&ffdata.ftLastWriteTime;
                return true;
            }
        }
        return false;
    }

protected:
    virtual bool _Save() = 0;
    virtual void _SetDefaults() = 0;
    virtual bool _Load() = 0;
};
