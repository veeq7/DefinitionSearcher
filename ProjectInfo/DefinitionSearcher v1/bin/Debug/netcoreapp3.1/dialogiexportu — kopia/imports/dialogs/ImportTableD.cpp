#include "StdAfx.h"
#include "managerdll.h"
#include "ImporttableD.h"
#include "imports/CommonImport.h"
#include "../excel/excelTableimport.h"
#include "../CommonTableImport.h"
#include "../csv/CSVTableImport.h"
#include "dialogs/male/CSVParametryD.h"
#include "wti.h"
//Szczegolowe


// CImportTableD dialog

IMPLEMENT_DYNAMIC(CImportTableD, CMyDialog)
CImportTableD::CImportTableD()
    : CMyDialog(CImportTableD::IDD, NULL)
{
    setRegSaveDataT(_T("Import\\Table"));

    m_i_cfiexcel.addRadio(&m_i_cfiexcel, _T("CFIEXCEL"));
    m_i_cfiexcel.addRadio(&m_i_cficsv, _T("CFICSV"));
    m_fileName.setFilter(getFileMask());

    setDefVal(getDataT(), _T("_format"), _T("CFIEXCEL"));

    csvconf = CCommonCSVConfig::getCSVConfig(eUstawieniaImportCSV_TabelaBazy, true);
}

CImportTableD::~CImportTableD()
{
    saveRegSaveDataT();
}

void CImportTableD::DoDataExchange(CDataExchange* pDX)
{
    CMyDialog::DoDataExchange(pDX);
    try { DDX_FileEditCtrl(pDX, IDC_NAZWAPLIKU, m_fileName, FEC_FILEOPEN); }
    catch (...) {}
    DDXCTRL(IDC_I_CFIEXCEL, m_i_cfiexcel);
    DDXCTRL(IDC_TABNAME, m_tablename);
    DDXCTRL(IDC_I_CFICSV, m_i_cficsv);
    DDXCTRL(IDC_CSV_PARAMS, m_csv_params);
}

bool CImportTableD::RegisterOnInit()
{
    DC_REGISTERT("_fileName", m_fileName);
    DC_REGISTERT("_format", m_i_cfiexcel);
    return __super::RegisterOnInit();
}

void CImportTableD::UkryjCoNiepotrzebne()
{
    _ENABLE_IT(m_csv_params);

    if (GET_POLE(_T("_format")) != _T("CFICSV"))
    {
        _DISABLE_IT(m_csv_params);
    }

    __super::UkryjCoNiepotrzebne();
    performEHOperations();
}


BEGIN_MESSAGE_MAP(CImportTableD, CMyDialog)
    ON_BN_CLICKED(IDC_CSV_PARAMS, OnBnCsv)
END_MESSAGE_MAP()

void CImportTableD::OnBnCsv()
{
    CCSVParametryD dlg(GET_POLE(_T("_fileName")), csvconf);
    dlg.DoModal();
}



class CImportTableD_DM : public CDialogManagerItem
{
public:
    bool executeForParam(const CMyString& param);

    DIALOGMANAGER_ITEMCREATE(CImportTableD_DM);
};


DIALOGMANAGER_REGISTERT("ImportTableD", CImportTableD_DM);


bool CImportTableD_DM::executeForParam(const CMyString& param)
{
    CImportTableD dlg;
    dlg.DoModal();
    return (dlg.getReturnValue() != NULLSTRING);
}


CMyString CImportTableD::getFileMask()
{
    return _FT("%sPliki CSV (*.csv)|*.csv|Wszystkie pliki (*.*)|*.*||", FILEDIALOG_PARAMS_XLS);
}


void CImportTableD::OnOK()
{
    _ileZaimportowano = 0;

    CCommonTableImport* ci = NULL;
    if (m_i_cfiexcel.getValue() == _T("CFIEXCEL")) ci = new CExcelTableImport();
    if (m_i_cfiexcel.getValue() == _T("CFICSV"))
    {
        ci = new CCSVTableImport();
        if (ci) ((CCSVTableImport*)ci)->setCSVConfig(csvconf);
    }

    ci->setTabName(m_tablename.getValue());

    WTI_NDLG(*this, ci, 0);

    if (ci->isError())
    {
        AfxMultiMessageBox(ci->getErrorMessage());
        DELETE_WSK(ci);
        return;
    }

    DELETE_WSK(ci);

    AfxMessageBox(_TX("Zaimportowano ") + CMyString(_ileZaimportowano) + _T(" rekordów"));

    CMyDialog::OnOK();
}

void CImportTableD::executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonTableImport>& prm)
{
    CCommonTableImport* eti = prm.getParam();
    if (eti)
    {
        eti->setStatusRetHandler(sr);
        CDataT::toLog(_FX(_TX("Rozpoczęto import %s tabeli %s z pliku  %s."), GET_POLE(_T("_format")), m_tablename.getValue(), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        _ileZaimportowano = eti->execute(DTGDTGETPOLET("_fileName"));
        CDataT::toLog(_FX(_TX("Zakończono import %s z pliku %s, zaimportowano %s rekordów."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName")), CMyString(_ileZaimportowano)), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        eti->clearStatusRetHandler();
    }
}



CUInfo CImportTableD::getPermission(int no)
{
    switch (no)
    {
    case 0:
        return getMainPermission();
    }
    return __super::getPermission(no - 1);
}

CUInfo CImportTableD::getMainPermission()
{
    return addPrefix(getUprawnienie(UPR_IMPORT_TABLE));
}

INT_PTR CImportTableD::DoModal()
{
    if (!CanDoIt(UPR_IMPORT_TABLE))
    {
        AfxMessageBox(_TX("Brak uprawnień!"));
        return 0;
    }
    return __super::DoModal();
}
