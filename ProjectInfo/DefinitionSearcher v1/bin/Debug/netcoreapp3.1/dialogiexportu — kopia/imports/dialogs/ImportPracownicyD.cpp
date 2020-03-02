#include "StdAfx.h"
#include "managerdll.h"
#include "ImportPracownicyD.h"

#include "wti.h"

//Szczegolowe
#include "../excel/ExcelPracownikImport.h"
#include "dialogs/male/CSVParametryD.h"
#include "../csv/CSVPracownikImport.h"

IMPLEMENT_DYNAMIC(CImportPracownicyD, CMyDialog)
CImportPracownicyD::CImportPracownicyD()
: CMyDialog(MYIDD(IDD_PRACOWNICY_IMPORT), NULL)
{
    setRegSaveDataT(_T("Import\\Pracownicy"));

    m_i_cfiexcel.addRadio(&m_i_cfiexcel, _T("CFIEXCEL"));
    m_i_cfiexcel.addRadio(&m_i_cficsv, _T("CFICSV"));
    m_ex_pomin.addRadio(&m_ex_pomin, _T("SKIP"));
    m_ex_pomin.addRadio(&m_ex_actualize, _T("UPDATE"));
    m_fileName.setFilter(getFileMask());

    setDefVal(getDataT(), _T("_format"), _T("CFIEXCEL"));
    setDefVal(getDataT(), _T("_onexists"), _T("SKIP"));

    csvconf = CCommonCSVConfig::getCSVConfig(eUstawieniaImportCSV_Pracownik, true);
}

CImportPracownicyD::~CImportPracownicyD()
{
    saveRegSaveDataT();
}

BEGIN_MESSAGE_MAP(CImportPracownicyD, CMyDialog)
    ON_BN_CLICKED(IDC_CSV_PARAMS, OnBnCsv)
END_MESSAGE_MAP()


void CImportPracownicyD::DoDataExchange(CDataExchange* pDX)
{
    CMyDialog::DoDataExchange(pDX);
    DDX_FileEditCtrl(pDX, IDC_NAZWAPLIKU, m_fileName, FEC_FILEOPEN);
    DDX_Control(pDX, IDC_I_CFIEXCEL, m_i_cfiexcel);
    DDX_Control(pDX, IDC_I_CFICSV, m_i_cficsv);
    DDX_Control(pDX, IDC_EX_POMIN, m_ex_pomin);
    DDX_Control(pDX, IDC_EX_ACTUALIZE, m_ex_actualize);
    DDX_Control(pDX, IDC_CSV_PARAMS, m_csv_params);
    DDX_Control(pDX, IDC_C_WARTDOW, m_im_wartoscidowolnych);
    DDX_Control(pDX, IDC_C_OSTRANS, m_im_osTrans);
    DDX_Control(pDX, IDC_C_BLADPRZER, m_im_bladPrzerywa);
}

bool CImportPracownicyD::RegisterOnInit()
{
    DC_REGISTERT("_fileName", m_fileName);
    DC_REGISTERT("_format", m_i_cfiexcel);
    DC_REGISTERT("_onexists", m_ex_pomin);
    DC_REGISTERT("_im_wartoscidow", m_im_wartoscidowolnych);
    DC_REGISTERT("_im_ostrans", m_im_osTrans);
    DC_REGISTERT("_im_bladprzerywa", m_im_bladPrzerywa);
    return __super::RegisterOnInit();
}

CUInfo CImportPracownicyD::getPermission(int no)
{
    switch (no)
    {
    case 0:
        return getMainPermission();
    }
    return __super::getPermission(no - 1);
}
CUInfo CImportPracownicyD::getMainPermission()
{
    return addPrefix(getUprawnienie(UPR_IMPORT_PRACOWNICY));
}

INT_PTR CImportPracownicyD::DoModal()
{
    if (!CanDoIt(UPR_IMPORT_PRACOWNICY))
    {
        AfxMessageBox(_TX("Brak uprawnień!"));
        return 0;
    }
    return __super::DoModal();
}

void CImportPracownicyD::UkryjCoNiepotrzebne()
{
    _ENABLE_IT(m_csv_params);

    if (GET_POLE(_T("_format")) != _T("CFICSV"))
    {
        _DISABLE_IT(m_csv_params);
    }

    __super::UkryjCoNiepotrzebne();
    performEHOperations();
}

void CImportPracownicyD::executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonPracownikImport>& prm)
{
    CCommonPracownikImport* eti = prm.getParam();
    if (eti)
    {
        eti->setStatusRetHandler(sr);
        CDataT::toLog(_FX(_TX("Rozpoczęto import %s pracowników z pliku  %s."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        _ileZaimportowano = eti->execute(DTGDTGETPOLET("_fileName"));
        CDataT::toLog(_FX(_TX("Zakończono import %s z pliku %s, zaimportowano %s pracowników."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName")), CMyString(_ileZaimportowano)), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        eti->clearStatusRetHandler();
    }
}

void CImportPracownicyD::OnOK()
{
    _ileZaimportowano = 0;

    CImportKeysCacheC kc(new CImportKeysCache());
    CCommonPracownikImport* ci = NULL;

    if (m_i_cfiexcel.getValue() == _T("CFIEXCEL")) ci = new CExcelPracownikImport();
    if (m_i_cfiexcel.getValue() == _T("CFICSV"))
    {
        ci = new CCSVPracownikImport();
        ((CCSVPracownikImport*)ci)->setCSVConfig(csvconf);
    }

    if (!ci) return;
    ci->setImportWartosciDowolne(_atoi(DTGDTGETPOLET("_im_wartoscidow")), TB_PRACOWNICY);
    ci->setOsobneTransakcje(_atoi(DTGDTGETPOLET("_im_ostrans")));
    ci->setBladPrzerywa(_atoi(DTGDTGETPOLET("_im_bladprzerywa")));
    ci->setHowActualize(getHowActualize());
    ci->setKeysCache(kc);

    WTI_NDLG(*this, ci, 0);

    if (ci->isError())
    {
        AfxMultiMessageBox(ci->getErrorMessage());
        DELETE_WSK(ci);
        return;
    }

    DELETE_WSK(ci);

    AfxMessageBox(_TX("Zaimportowano ") + CMyString(_ileZaimportowano) + _T(" kartotek"));

    CMyDialog::OnOK();
}
void CImportPracownicyD::OnBnCsv()
{
    CCSVParametryD dlg(GET_POLE(_T("_fileName")), csvconf);
    dlg.DoModal();
}

CMyString CImportPracownicyD::getFileMask()
{
    return _FT("%sPliki CSV (*.csv)|*.csv|Wszystkie pliki (*.*)|*.*||",FILEDIALOG_PARAMS_XLS);
}
int CImportPracownicyD::getHowActualize()
{
    if (DTGDTGETPOLET("_onexists") == _T("UPDATE")) return ACTUALIZE_UPDATE;
    return ACTUALIZE_SKIP;
}

//
// CImportPracownicyD_DM
//
class CImportPracownicyD_DM : public CDialogManagerItem
{
public:
    bool executeForParam(const CMyString& param);

    DIALOGMANAGER_ITEMCREATE(CImportPracownicyD_DM);
};

DIALOGMANAGER_REGISTERT("ImportPracownicyD", CImportPracownicyD_DM);

bool CImportPracownicyD_DM::executeForParam(const CMyString& param)
{
    CImportPracownicyD dlg;
    dlg.DoModal();
    return (dlg.getReturnValue() != NULLSTRING);
}
