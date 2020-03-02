#include "StdAfx.h"
// #include "managerdll.h"
#include "ImportDanychListowPrzewozowychD.h"
#include "wti.h"
//DIALOGIEXPORTU
#include "../CommonDaneListuPrzewozowegoImport.h"
#include "dialogs/male/CSVParametryD.h"
////Szczegolowe
#include "../dialogiexportu/imports/dialogs/ImportWartosciDowD.h"
#include "../interfacesgui/modules/import/ImportInFiltr.h"
#include "../sqldll/adds/CommonCSVConfig.h"
//Resources
#include "../../resource.h"

// CImportDanychListowPrzewozowychD dialog

IMPLEMENT_DYNAMIC(CImportDanychListowPrzewozowychD, CMyDialog)
CImportDanychListowPrzewozowychD::CImportDanychListowPrzewozowychD()
: CMyDialog(MYIDD(IDD_IMPORT_DANYCH_LISTOW_PRZEWOZOWYCH), NULL)
{
    setRegSaveDataT(_T("Import\\DaneListowPrzewozowych"));

    m_map_kod.addKeysValues(GetSearchMap(TG_TRANSPORT));

    m_i_cfiexcel.addRadio(&m_i_cfiexcel, _T("CFIEXCEL"));
    m_i_cfiexcel.addRadio(&m_i_cficsv, _T("CFICSV"));
    m_fileName.setFilter(getFileMask());

    setDefVal(getDataT(), _T("_format"), _T("CFIEXCEL"));

    csvconf = CCommonCSVConfig::getCSVConfig(eUstawieniaImportCSV_DaneListowPrzewozowych, true);
}

CImportDanychListowPrzewozowychD::~CImportDanychListowPrzewozowychD()
{
    saveRegSaveDataT();
}

void CImportDanychListowPrzewozowychD::DoDataExchange(CDataExchange* pDX)
{
    CMyDialog::DoDataExchange(pDX);
    try { DDX_FileEditCtrl(pDX, IDC_NAZWAPLIKU, m_fileName, FEC_FILEOPEN); } catch (...) {}
    DDXCTRL(IDC_I_CFIEXCEL, m_i_cfiexcel);
    DDXCTRL(IDC_I_CFICSV, m_i_cficsv);
    DDXCTRL(IDC_CSV_PARAMS, m_csv_params);
    DDXCTRL(IDC_C_OSTRANS, m_im_osTrans);
    DDXCTRL(IDC_C_BLADPRZER, m_im_bladPrzerywa);
    DDXCTRL(IDC_MAP_KOD, m_map_kod);
}

bool CImportDanychListowPrzewozowychD::RegisterOnInit()
{
    DC_REGISTERT("_fileName", m_fileName);
    DC_REGISTERT("_format", m_i_cfiexcel);
    DC_REGISTERT("_im_ostrans", m_im_osTrans);
    DC_REGISTERT("_im_bladprzerywa", m_im_bladPrzerywa);
    DC_REGISTERT("_mapKod", m_map_kod);
    return __super::RegisterOnInit();
}

void CImportDanychListowPrzewozowychD::UkryjCoNiepotrzebne()
{
    m_fileName.setFilter(getFileMask());

    _ENABLE_IT(m_csv_params);

    if (GET_POLE(_T("_format")) != _T("CFICSV"))
    {
        _DISABLE_IT(m_csv_params);
    }

    __super::UkryjCoNiepotrzebne();
    performEHOperations();
}

BEGIN_MESSAGE_MAP(CImportDanychListowPrzewozowychD, CMyDialog)
    ON_BN_CLICKED(IDC_CSV_PARAMS, OnBnCsv)
END_MESSAGE_MAP()

void CImportDanychListowPrzewozowychD::OnBnCsv()
{
    CCSVParametryD dlg(GET_POLE(_T("_fileName")), csvconf);
    dlg.DoModal();
}

class CImportDanychListowPrzewozowychD_DM : public CDialogManagerItem
{
public:
    bool executeForParam(const CMyString& param);

    DIALOGMANAGER_ITEMCREATE(CImportDanychListowPrzewozowychD_DM);
};

DIALOGMANAGER_REGISTERT("ImportDanychListowPrzewozowychD", CImportDanychListowPrzewozowychD_DM);

bool CImportDanychListowPrzewozowychD_DM::executeForParam(const CMyString& param)
{
    CImportDanychListowPrzewozowychD dlg;
    dlg.DoModal();
    return (dlg.getReturnValue() != NULLSTRING);
}

CMyString CImportDanychListowPrzewozowychD::getFileMask()
{
    CMyString f = DTGDTGETPOLET("_format");

    return _FT("%sPliki CSV (*.csv)|*.csv|Wszystkie pliki (*.*)|*.*||",FILEDIALOG_PARAMS_XLS);
}

void CImportDanychListowPrzewozowychD::OnOK()
{
    _ileZaimportowano = 0;
    _ilePominieto = 0;

    CCommonDaneListuPrzewozowegoImport* ci = NULL;
    if (m_i_cfiexcel.getValue() == _T("CFIEXCEL")) ci = new CXLSDaneListuPrzewozowegoImport();
    if (m_i_cfiexcel.getValue() == _T("CFICSV"))
    {
        ci = new CCSVDaneListuPrzewozowegoImport();
        if (ci) ((CCSVDaneListuPrzewozowegoImport*)ci)->setCSVConfig(csvconf);
    }

    ci->setFileName(DTGDTGETPOLET("_fileName"));
    ci->setOsobneTransakcje(_atoi(DTGDTGETPOLET("_im_ostrans")));
    ci->setBladPrzerywa(_atoi(DTGDTGETPOLET("_im_bladprzerywa")));
    ci->setMapowanieKodu((eListPrzewozowySearchBy)_atoi(DTGDTGETPOLET("_mapKod")));

    WTI_NDLG(*this, ci, 0);

    if (ci->isError())
    {
        AfxMultiMessageBox(ci->getErrorMessage());
        delete ci;
        return;
    }

    DELETE_WSK(ci);

    AfxMessageBox(_TX("Zaimportowano ") + CMyString(_ileZaimportowano) + _T(" pozycji (pominięto ") + CMyString(_ilePominieto) + _T(")"));

    CMyDialog::OnOK();
}

void CImportDanychListowPrzewozowychD::executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonDaneListuPrzewozowegoImport>& prm)
{
    CCommonDaneListuPrzewozowegoImport* eti = prm.getParam();
    if (eti)
    {
        eti->setStatusRetHandler(sr);
        CDataT::toLog(_FX(_TX("Rozpoczęto import %s danych z pliku  %s."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        _ileZaimportowano = eti->execute();
        _ilePominieto += eti->ilePominieto();
        CDataT::toLog(_FX(_TX("Zakończono import %s z pliku %s, zaimportowano %s, pominęto %s pozycji."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName")), CMyString(_ileZaimportowano), CMyString(_ilePominieto)), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        eti->clearStatusRetHandler();
    }
}

CUInfo CImportDanychListowPrzewozowychD::getPermission(int no)
{
    switch (no)
    {
    case 0:
        return getMainPermission();
    }
    return __super::getPermission(no - 1);
}

//CUInfo CImportDanychZleceniaD::getMainPermission()
//{
// return addPrefix(getUprawnienie(UPR_IMPORT_DANEZLECENIA));
//}

INT_PTR CImportDanychListowPrzewozowychD::DoModal()
{
    // if (!CanDoIt(UPR_IMPORT_DANEZLECENIA))
    // {
    //  AfxMessageBox(_TX("Brak uprawnień!"));
    //  return 0;
    // }
    return __super::DoModal();
}
